#!/usr/bin/env python3
"""
video_runner.py — Cloud version
- Génère vidéo .mp4 annotée → S3
- Envoie métriques frame par frame → CloudWatch
"""
import cv2
import subprocess
import json
import boto3
import numpy as np
import os
import time

BUCKET = "vecu-sim-results-dev"
REGION = "us-east-1"

cw = boto3.client('cloudwatch', region_name=REGION)
s3 = boto3.client('s3',         region_name=REGION)


def put_metric(namespace, metric_name, value, unit, dimensions):
    """Envoyer une métrique à CloudWatch"""
    try:
        cw.put_metric_data(
            Namespace=namespace,
            MetricData=[{
                'MetricName': metric_name,
                'Value':      value,
                'Unit':       unit,
                'Dimensions': dimensions
            }]
        )
    except Exception as e:
        print(f"[CW] Erreur métrique {metric_name}: {e}")


def draw_overlay(frame, result, frame_num):
    """Dessiner les annotations LPA sur la frame"""
    h, w = frame.shape[:2]

    # Couleur selon état
    if result["warn_L"] or result["warn_R"]:
        color_bg = (0, 0, 200)    # Rouge = Warning
    elif result["active"]:
        color_bg = (0, 180, 0)    # Vert = Actif
    else:
        color_bg = (80, 80, 80)   # Gris = Inactif

    # Bande noire en haut
    cv2.rectangle(frame, (0, 0), (w, 90), (20, 20, 20), -1)

    # Bande état en bas
    cv2.rectangle(frame, (0, h-35), (w, h), color_bg, -1)

    # Cause labels
    cause_labels = {
        0: "Driver Disabled",
        1: "Speed Too Low",
        2: "Camera Fault",
        3: "Detection Invalid",
        4: "Low Confidence",
        5: "LPA Active"
    }
    cause_str = cause_labels.get(int(result["cause"]), "Unknown")

    # Textes haut
    cv2.putText(frame, f"Frame: {frame_num:03d}",
        (10, 22), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (200,200,200), 1)
    cv2.putText(frame, f"Cause: {cause_str}",
        (10, 45), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255,255,255), 1)
    cv2.putText(frame, f"Conf: {result['conf']:.1f}/100  Valid: {'YES' if result['valid'] else 'NO'}",
        (10, 68), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (200,200,200), 1)

    # Texte bas
    status = "● LPA ACTIVE" if result["active"] else "○ LPA INACTIVE"
    cv2.putText(frame, status,
        (10, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,255,255), 2)

    # Warnings
    if result["warn_L"]:
        cv2.putText(frame, "WARNING LEFT",
            (w//2 - 70, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,255), 2)
        cv2.arrowedLine(frame, (60, h//2), (10, h//2), (0,0,255), 3)
    if result["warn_R"]:
        cv2.putText(frame, "WARNING RIGHT",
            (w//2 - 70, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,255), 2)
        cv2.arrowedLine(frame, (w-60, h//2), (w-10, h//2), (0,0,255), 3)

    # Line types
    left_labels  = {0: "None", 1: "Dashed", 2: "Solid"}
    right_labels = {0: "None", 1: "Dashed", 2: "Solid"}
    cv2.putText(frame,
        f"L:{left_labels.get(int(result['leftT']),'?')}  "
        f"R:{right_labels.get(int(result['rightT']),'?')}",
        (w-160, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255,255,255), 1)

    return frame


def process_video_cloud(video_path, speed, lpa_act, turn_signal, scenario_name):

    print(f"\n[LPA] === Scénario: {scenario_name} ===")
    print(f"[LPA] Vidéo: {video_path}")
    print(f"[LPA] Speed={speed}  LPA={lpa_act}  Signal={turn_signal}")

    cap = cv2.VideoCapture(video_path)
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    fps          = cap.get(cv2.CAP_PROP_FPS) or 25.0
    print(f"[LPA] {total_frames} frames @ {fps} fps")

    # Préparer le writer vidéo annotée
    output_video_path = f"/tmp/{scenario_name}_annotated.mp4"
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    writer = cv2.VideoWriter(output_video_path, fourcc, fps, (640, 480))

    # Lancer firmware en mode streaming
    proc = subprocess.Popen(
        ['./firmware_bsw', str(speed), str(lpa_act), str(turn_signal)],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL
    )

    dimensions = [{'Name': 'Scenario', 'Value': scenario_name}]

    frame_count  = 0
    last_result  = None
    metrics_batch = []   # batch CloudWatch (max 20 par appel)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        frame_count += 1

        # Frame pour affichage (640x480)
        frame_display = cv2.resize(frame, (640, 480))

        # Frame pour firmware (320x240)
        frame_fw  = cv2.resize(frame, (320, 240))
        frame_rgb = frame_fw[:, :, ::-1].astype(np.float64) / 255.0
        r_mean = float(frame_rgb[:, :, 0].mean())
        g_mean = float(frame_rgb[:, :, 1].mean())
        b_mean = float(frame_rgb[:, :, 2].mean())
        flat = np.array([r_mean, g_mean, b_mean], dtype=np.float64)

        # Envoyer au firmware
        try:
            proc.stdin.write(flat.tobytes())
            proc.stdin.flush()
        except BrokenPipeError:
            print("[LPA] Firmware terminé")
            break

        # Lire résultat de cette frame
        line = proc.stdout.readline()
        if line:
            try:
                last_result = json.loads(line.decode().strip())
            except:
                pass

        # Dessiner overlay + écrire dans vidéo
        if last_result:
            frame_annotated = draw_overlay(frame_display.copy(), last_result, frame_count)
            writer.write(frame_annotated)

            # Accumuler métriques CloudWatch
            metrics_batch.append({
                'MetricName': 'Confidence',
                'Value':      last_result['conf'],
                'Unit':       'None',
                'Dimensions': dimensions
            })
            metrics_batch.append({
                'MetricName': 'LPA_Active',
                'Value':      float(last_result['active']),
                'Unit':       'None',
                'Dimensions': dimensions
            })
            metrics_batch.append({
                'MetricName': 'S_Cause',
                'Value':      last_result['cause'],
                'Unit':       'None',
                'Dimensions': dimensions
            })
            metrics_batch.append({
                'MetricName': 'Warning_Left',
                'Value':      float(last_result['warn_L']),
                'Unit':       'None',
                'Dimensions': dimensions
            })
            metrics_batch.append({
                'MetricName': 'Warning_Right',
                'Value':      float(last_result['warn_R']),
                'Unit':       'None',
                'Dimensions': dimensions
            })

            # Envoyer batch de 20 max à CloudWatch
            if len(metrics_batch) >= 20:
                try:
                    cw.put_metric_data(
                        Namespace='LPA/vECU',
                        MetricData=metrics_batch[:20]
                    )
                except Exception as e:
                    print(f"[CW] Erreur batch: {e}")
                metrics_batch = metrics_batch[20:]

        # Log toutes les 50 frames
        if frame_count % 50 == 0 and last_result:
            print(f"[LPA] Frame {frame_count:03d} | "
                  f"active={last_result['active']} | "
                  f"cause={last_result['cause']:.0f} | "
                  f"conf={last_result['conf']:.1f} | "
                  f"valid={last_result['valid']}")

    # Envoyer métriques restantes
    if metrics_batch:
        try:
            for i in range(0, len(metrics_batch), 20):
                cw.put_metric_data(
                    Namespace='LPA/vECU',
                    MetricData=metrics_batch[i:i+20]
                )
        except Exception as e:
            print(f"[CW] Erreur flush: {e}")

    cap.release()
    writer.release()
    proc.stdin.close()
    proc.wait()

    # Upload vidéo annotée vers S3
    video_s3_key = f"outputs/videos/{scenario_name}_annotated.mp4"
    print(f"\n[LPA] Upload vidéo annotée → S3...")
    s3.upload_file(output_video_path, BUCKET, video_s3_key)
    print(f"[LPA] ✅ Vidéo : s3://{BUCKET}/{video_s3_key}")

    # Résultat JSON final
    result_json = {
        "scenario":      scenario_name,
        "video":         os.path.basename(video_path),
        "speed":         speed,
        "total_frames":  total_frames,
        "fps":           fps,
        "annotated_video": f"s3://{BUCKET}/{video_s3_key}"
    }

    if last_result:
        result_json.update({
            "S_LPA_Active":   bool(last_result["active"]),
            "S_Warning_Left": bool(last_result["warn_L"]),
            "S_Warning_Right":bool(last_result["warn_R"]),
            "S_Cause":        last_result["cause"],
            "confidence":     last_result["conf"],
            "S_leftType":     last_result["leftT"],
            "S_rightType":    last_result["rightT"],
            "drift_left":     bool(last_result["drift_L"]),
            "drift_right":    bool(last_result["drift_R"])
        })

    # Upload JSON résultat
    s3.put_object(
        Bucket=BUCKET,
        Key=f"outputs/{scenario_name}_result.json",
        Body=json.dumps(result_json, indent=2)
    )
    print(f"[LPA] ✅ JSON  : s3://{BUCKET}/outputs/{scenario_name}_result.json")

    # Résumé final
    print(f"\n[LPA] ── Résultats finaux ──────────────────")
    print(f"[LPA] S_LPA_Active    = {result_json.get('S_LPA_Active')}")
    print(f"[LPA] S_Warning_Left  = {result_json.get('S_Warning_Left')}")
    print(f"[LPA] S_Warning_Right = {result_json.get('S_Warning_Right')}")
    print(f"[LPA] S_Cause         = {result_json.get('S_Cause')}")
    print(f"[LPA] confidence      = {result_json.get('confidence', 0):.1f}")
    print(f"[LPA] ────────────────────────────────────────")

    return result_json


if __name__ == "__main__":

    scenarios = [
        {
            "name":        "normal",
            "video":       "/vecu/inputs/videos/test_vid.mp4",
            "speed":       54.0,
            "lpa_act":     1,
            "turn_signal": 0
        },
        {
            "name":        "vitesse_faible",
            "video":       "/vecu/inputs/videos/SHORT_TEST.mp4",
            "speed":       8.0,
            "lpa_act":     1,
            "turn_signal": 0
        }
    ]

    for s in scenarios:
        process_video_cloud(
            s["video"],
            s["speed"],
            s["lpa_act"],
            s["turn_signal"],
            s["name"]
        )

    print("\n[LPA] ✅ Tous les scénarios terminés !")