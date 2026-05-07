#!/usr/bin/env python3
import cv2
import subprocess
import json
import numpy as np
import os

def draw_overlay(frame, result, frame_num):
    """Dessiner les annotations LPA sur la frame"""
    h, w = frame.shape[:2]

    # Couleur selon état
    if result["warn_L"] or result["warn_R"]:
        color_bg = (0, 0, 200)      # Rouge = Warning
    elif result["active"]:
        color_bg = (0, 180, 0)      # Vert = Actif
    else:
        color_bg = (80, 80, 80)     # Gris = Inactif

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
        cv2.putText(frame, "⚠ WARNING LEFT",
            (w//2 - 80, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,255), 2)
    if result["warn_R"]:
        cv2.putText(frame, "⚠ WARNING RIGHT",
            (w//2 - 80, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,255), 2)

    # Line type
    left_labels  = {0: "None", 1: "Dashed", 2: "Solid"}
    right_labels = {0: "None", 1: "Dashed", 2: "Solid"}
    cv2.putText(frame,
        f"L:{left_labels.get(int(result['leftT']),'?')}  R:{right_labels.get(int(result['rightT']),'?')}",
        (w - 160, h-10), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255,255,255), 1)

    # Drift indicators
    if result["drift_L"]:
        cv2.arrowedLine(frame, (60, h//2), (10, h//2), (0,0,255), 3)
    if result["drift_R"]:
        cv2.arrowedLine(frame, (w-60, h//2), (w-10, h//2), (0,0,255), 3)

    return frame


def process_video_live(video_path, speed, lpa_act, turn_signal, scenario_name):

    print(f"\n[LPA] === Scénario: {scenario_name} ===")
    print(f"[LPA] Vidéo: {video_path}")
    print(f"[LPA] Speed={speed}  LPA={lpa_act}  Signal={turn_signal}")
    print(f"[LPA] Appuie sur 'q' pour quitter, ESPACE pour pause")

    cap = cv2.VideoCapture(video_path)
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    print(f"[LPA] {total_frames} frames @ {fps} fps")

    # Lancer firmware en mode streaming
    proc = subprocess.Popen(
        ['./firmware', str(speed), str(lpa_act), str(turn_signal)],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL
    )

    cv2.namedWindow("LPA vECU — Live Debug", cv2.WINDOW_NORMAL)

    last_result = None
    frame_num   = 0
    paused      = False

    while True:
        if not paused:
            ret, frame = cap.read()
            if not ret:
                break

            frame_num += 1

            # Redimensionner
            frame_display = cv2.resize(frame, (640, 480))
            frame_fw      = cv2.resize(frame, (320, 240))

            # Préparer pour firmware
            frame_rgb = frame_fw[:, :, ::-1].astype(np.float32) / 255.0
            R = frame_rgb[:, :, 0].flatten(order='F')
            G = frame_rgb[:, :, 1].flatten(order='F')
            B = frame_rgb[:, :, 2].flatten(order='F')
            flat = np.concatenate([R, G, B]).astype(np.float32)

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

            # Dessiner overlay
            if last_result:
                frame_display = draw_overlay(frame_display, last_result, frame_num)

            cv2.imshow("LPA vECU — Live Debug", frame_display)

        # Contrôles clavier
        key = cv2.waitKey(30) & 0xFF
        if key == ord('q'):
            break
        elif key == ord(' '):
            paused = not paused
            print(f"[LPA] {'⏸ Pause' if paused else '▶ Reprise'}")

    cap.release()
    proc.stdin.close()
    proc.wait()
    cv2.destroyAllWindows()

    print(f"\n[LPA] {frame_num} frames traitées")
    if last_result:
        print(f"[LPA] Dernier état → Cause={last_result['cause']} "
              f"Conf={last_result['conf']:.1f} "
              f"Active={last_result['active']}")


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
            "speed":       80.0,
            "lpa_act":     1,
            "turn_signal": 0
        }
    ]

    for s in scenarios:
        process_video_live(
            s["video"],
            s["speed"],
            s["lpa_act"],
            s["turn_signal"],
            s["name"]
        )
