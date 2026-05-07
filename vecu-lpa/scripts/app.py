"""
app.py — vECU LPA Dashboard Server
Flask + SocketIO + Prometheus metrics for Grafana integration
"""
import eventlet
eventlet.monkey_patch()
import os
import json
import threading
import queue
import subprocess
import time
import numpy as np
import cv2

from flask import Flask, render_template, request, jsonify, Response
from flask_socketio import SocketIO, emit

app = Flask(__name__, template_folder="../templates", static_folder="../static")
app.config["SECRET_KEY"] = "vecu-lpa-secret"
socketio = SocketIO(
    app,
    cors_allowed_origins="*",
    async_mode="eventlet",
    ping_timeout=60,
    ping_interval=25,
    logger=False,
    engineio_logger=False
)

# ── Optional S3 ──────────────────────────────────────────────────
try:
    import boto3
    BOTO3_OK = True
except ImportError:
    BOTO3_OK = False

BUCKET     = os.environ.get("S3_BUCKET", "vecu-sim-results-dev")
REGION     = os.environ.get("AWS_DEFAULT_REGION", "us-east-1")
S3_ENABLED = BOTO3_OK and os.environ.get("S3_ENABLED", "0") == "1"

# ── Prometheus metrics ───────────────────────────────────────────
_metrics_lock = threading.Lock()
_metrics = {
    "lpa_active":         0,
    "warning_left":       0,
    "warning_right":      0,
    "system_state":       0,
    "frames_processed":   0,
    "conf":               0.0,
    "offset_percent":     0.0,
    "left_type":          1.0,
    "right_type":         1.0,
    "is_valid":           0,
    "speed":              0.0,
    "simulation_running": 0,
    "warn_left_count":    0,
    "warn_right_count":   0,
    "active_count":       0,
}

def _update_metrics(result: dict, speed: float):
    with _metrics_lock:
        _metrics["lpa_active"]       = 1 if result.get("active") else 0
        _metrics["warning_left"]     = 1 if result.get("warn_L") else 0
        _metrics["warning_right"]    = 1 if result.get("warn_R") else 0
        _metrics["system_state"]     = int(result.get("cause", 0))
        _metrics["frames_processed"] = int(result.get("frame", 0))
        _metrics["conf"]             = float(result.get("conf", 0.0))
        _metrics["left_type"]        = float(result.get("leftT", 1.0))
        _metrics["right_type"]       = float(result.get("rightT", 1.0))
        _metrics["is_valid"]         = 1 if result.get("valid") else 0
        _metrics["speed"]            = speed
        if result.get("warn_L"):
            _metrics["warn_left_count"]  += 1
        if result.get("warn_R"):
            _metrics["warn_right_count"] += 1
        if result.get("active"):
            _metrics["active_count"]     += 1

def _reset_session_metrics():
    with _metrics_lock:
        _metrics["warn_left_count"]  = 0
        _metrics["warn_right_count"] = 0
        _metrics["active_count"]     = 0
        _metrics["frames_processed"] = 0

# ── Session state ────────────────────────────────────────────────
session_state = {
    "running":     False,
    "proc":        None,
    "thread":      None,
    "frame_count": 0,
    "last_result": None,
    "speed":       54.0,
}


# ════════════════════════════════════════════════════════════════
# HTTP Routes
# ════════════════════════════════════════════════════════════════

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/videos")
def list_videos():
    video_dir = os.environ.get("VIDEO_DIR", "inputs/videos")
    videos = []
    if os.path.exists(video_dir):
        for f in sorted(os.listdir(video_dir)):
            if f.lower().endswith((".mp4", ".avi", ".mov", ".mkv")):
                videos.append(f)
    return jsonify({"videos": videos})

@app.route("/api/status")
def status():
    with _metrics_lock:
        m = dict(_metrics)
    return jsonify({
        "running":     session_state["running"],
        "frame_count": session_state["frame_count"],
        "last_result": session_state["last_result"],
        "metrics":     m,
    })

@app.route("/api/stop", methods=["POST"])
def api_stop():
    _stop_simulation()
    return jsonify({"ok": True})

@app.route("/metrics")
def prometheus_metrics():
    with _metrics_lock:
        m = dict(_metrics)
    lines = [
        "# HELP lpa_active LPA system active",
        "# TYPE lpa_active gauge",
        f"lpa_active {m['lpa_active']}",
        "# HELP lpa_warning_left Left lane warning",
        "# TYPE lpa_warning_left gauge",
        f"lpa_warning_left {m['warning_left']}",
        "# HELP lpa_warning_right Right lane warning",
        "# TYPE lpa_warning_right gauge",
        f"lpa_warning_right {m['warning_right']}",
        "# HELP lpa_system_state System state",
        "# TYPE lpa_system_state gauge",
        f"lpa_system_state {m['system_state']}",
        "# HELP lpa_frames_total Frames processed",
        "# TYPE lpa_frames_total counter",
        f"lpa_frames_total {m['frames_processed']}",
        "# HELP lpa_confidence Confidence score",
        "# TYPE lpa_confidence gauge",
        f"lpa_confidence {m['conf']:.2f}",
        "# HELP lpa_vehicle_speed Vehicle speed",
        "# TYPE lpa_vehicle_speed gauge",
        f"lpa_vehicle_speed {m['speed']:.1f}",
        "# HELP lpa_simulation_running Simulation running",
        "# TYPE lpa_simulation_running gauge",
        f"lpa_simulation_running {m['simulation_running']}",
        "# HELP lpa_active_frames_total Active frames",
        "# TYPE lpa_active_frames_total counter",
        f"lpa_active_frames_total {m['active_count']}",
        "# HELP lpa_warn_left_total Warn left events",
        "# TYPE lpa_warn_left_total counter",
        f"lpa_warn_left_total {m['warn_left_count']}",
        "# HELP lpa_warn_right_total Warn right events",
        "# TYPE lpa_warn_right_total counter",
        f"lpa_warn_right_total {m['warn_right_count']}",
    ]
    return Response("\n".join(lines) + "\n", mimetype="text/plain; version=0.0.4")


# ════════════════════════════════════════════════════════════════
# WebSocket Events
# ════════════════════════════════════════════════════════════════

@socketio.on("connect")
def on_connect():
    emit("server_ready", {"msg": "vECU LPA server connected"})
    with _metrics_lock:
        emit("metrics_update", dict(_metrics))

@socketio.on("start_simulation")
def on_start(data):
    if session_state["running"]:
        emit("error", {"msg": "Simulation already running"})
        return

    video_name  = data.get("video", "")
    video_dir   = os.environ.get("VIDEO_DIR", "inputs/videos")
    video_path  = os.path.join(video_dir, video_name)
    speed       = float(data.get("speed", 54.0))
    lpa_act     = int(data.get("lpa_act", 1))
    turn_signal = int(data.get("turn_signal", 0))

    if not os.path.exists(video_path):
        emit("error", {"msg": f"Video not found: {video_path}"})
        return

    _reset_session_metrics()
    with _metrics_lock:
        _metrics["simulation_running"] = 1
        _metrics["speed"] = speed

    session_state["speed"] = speed
    t = threading.Thread(
        target=_run_simulation,
        args=(video_path, speed, lpa_act, turn_signal),
        daemon=True,
    )
    session_state.update({"running": True, "frame_count": 0, "thread": t})
    t.start()
    emit("simulation_started", {"video": video_name, "speed": speed, "lpa_act": lpa_act})

@socketio.on("stop_simulation")
def on_stop():
    _stop_simulation()
    emit("simulation_stopped", {"msg": "Stopped by user"})

@socketio.on("update_speed")
def on_speed(data):
    speed = float(data.get("speed", session_state["speed"]))
    session_state["speed"] = speed
    with _metrics_lock:
        _metrics["speed"] = speed
    emit("speed_updated", {"speed": speed})


# ════════════════════════════════════════════════════════════════
# Simulation Engine — avec queue pour ne pas bloquer SocketIO
# ════════════════════════════════════════════════════════════════

def _run_simulation(video_path, speed, lpa_act, turn_signal):

    # Queue d'événements — thread séparé pour les emits
    ev_queue = queue.Queue(maxsize=200)

    def _emit_worker():
        while True:
            item = ev_queue.get()
            if item is None:
                break
            ev, data = item
            try:
                socketio.emit(ev, data)
            except Exception:
                pass
            ev_queue.task_done()

    emit_thread = threading.Thread(target=_emit_worker, daemon=True)
    emit_thread.start()

    def safe_emit(ev, data):
        try:
            ev_queue.put_nowait((ev, data))
        except queue.Full:
            pass

    # Ouvrir la vidéo
    cap   = cv2.VideoCapture(video_path)
    fps   = cap.get(cv2.CAP_PROP_FPS) or 25.0
    total = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    fw_binary = os.environ.get("FIRMWARE_BIN", "./firmware_bsw")

    try:
        proc = subprocess.Popen(
            [fw_binary, str(speed), str(lpa_act), str(turn_signal)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except FileNotFoundError:
        safe_emit("error", {"msg": f"Firmware not found: {fw_binary}"})
        session_state["running"] = False
        with _metrics_lock:
            _metrics["simulation_running"] = 0
        cap.release()
        ev_queue.put(None)
        return

    session_state["proc"] = proc
    frame_count = 0
    last_result = None

    safe_emit("log", {
        "msg":   f"Video loaded: {total} frames @ {fps:.0f} fps",
        "level": "info"
    })

    while session_state["running"]:
        ret, frame = cap.read()
        if not ret:
            break

        frame_count += 1
        session_state["frame_count"] = frame_count

        # Préparer pixel moyen pour firmware
        frame_fw  = cv2.resize(frame, (320, 240))
        frame_rgb = frame_fw[:, :, ::-1].astype(np.float64) / 255.0
        r_mean = float(frame_rgb[:, :, 0].mean())
        g_mean = float(frame_rgb[:, :, 1].mean())
        b_mean = float(frame_rgb[:, :, 2].mean())
        flat   = np.array([r_mean, g_mean, b_mean], dtype=np.float64)

        # Envoyer au firmware
        try:
            proc.stdin.write(flat.tobytes())
            proc.stdin.flush()
        except BrokenPipeError:
            safe_emit("log", {"msg": "Firmware pipe closed", "level": "error"})
            break

        # Lire résultat
        line = proc.stdout.readline()
        if line:
            try:
                last_result = json.loads(line.decode().strip())
                session_state["last_result"] = last_result
                _update_metrics(last_result, session_state["speed"])

                safe_emit("frame_result", {
                    "frame":    frame_count,
                    "total":    total,
                    "progress": round(frame_count / total * 100, 1),
                    "result":   last_result,
                })

                if frame_count % 25 == 0:
                    state_name = {
                        0: "INACTIVE",
                        1: "ACTIVE",
                        2: "WARN_L",
                        3: "WARN_R"
                    }.get(last_result.get("cause", 0), "?")
                    safe_emit("log", {
                        "msg": (
                            f"Frame {frame_count:03d}/{total} | "
                            f"{state_name} | "
                            f"conf={last_result['conf']:.1f}"
                        ),
                        "level": "ok" if last_result["active"] else "warn",
                    })

            except (json.JSONDecodeError, ValueError):
                pass

        # Céder le contrôle au scheduler — clé pour garder SocketIO vivant
        socketio.sleep(0)

    # Cleanup
    cap.release()
    try:
        proc.stdin.close()
        proc.wait(timeout=3)
    except Exception:
        proc.kill()

    session_state["running"] = False
    session_state["proc"]    = None
    with _metrics_lock:
        _metrics["simulation_running"] = 0

    if S3_ENABLED and last_result:
        _upload_results(last_result, video_path, speed, frame_count, fps)

    safe_emit("simulation_done", {
        "frame_count": frame_count,
        "last_result": last_result,
    })
    safe_emit("log", {
        "msg":   f"Done — {frame_count} frames processed",
        "level": "ok"
    })

    # Arrêter le worker d'emit
    ev_queue.put(None)
    emit_thread.join(timeout=5)


def _stop_simulation():
    session_state["running"] = False
    proc = session_state.get("proc")
    if proc:
        try:
            proc.stdin.close()
            proc.terminate()
            proc.wait(timeout=3)
        except Exception:
            try:
                proc.kill()
            except Exception:
                pass
    session_state["proc"] = None
    with _metrics_lock:
        _metrics["simulation_running"] = 0


def _upload_results(last_result, orig_video, speed, frames, fps):
    if not BOTO3_OK:
        return
    try:
        s3       = boto3.client("s3", region_name=REGION)
        scenario = os.path.splitext(os.path.basename(orig_video))[0]
        result_json = {
            "scenario":        scenario,
            "speed":           speed,
            "total_frames":    frames,
            "fps":             fps,
            "S_LPA_Active":    bool(last_result["active"]),
            "S_Warning_Left":  bool(last_result["warn_L"]),
            "S_Warning_Right": bool(last_result["warn_R"]),
            "S_Cause":         last_result["cause"],
            "confidence":      last_result["conf"],
        }
        s3.put_object(
            Bucket=BUCKET,
            Key=f"outputs/{scenario}_result.json",
            Body=json.dumps(result_json, indent=2),
        )
        socketio.emit("log", {
            "msg":   f"Results uploaded → s3://{BUCKET}/outputs/{scenario}_result.json",
            "level": "ok"
        })
    except Exception as e:
        socketio.emit("log", {"msg": f"S3 error: {e}", "level": "error"})


if __name__ == "__main__":
    port  = int(os.environ.get("PORT", 5000))
    debug = os.environ.get("DEBUG", "0") == "1"
    print(f"[vECU] Starting on http://0.0.0.0:{port}")
    print(f"[vECU] Prometheus metrics: http://0.0.0.0:{port}/metrics")
    socketio.run(app, host="0.0.0.0", port=port, debug=debug, allow_unsafe_werkzeug=True)
