import pytest, boto3, json, subprocess, os, time

BUCKET     = "vecu-sim-results-dev"
SPEED_NORM = 54.0
SPEED_LOW  = 8.0

@pytest.fixture(scope="session")
def run_vecu_scenario(tmp_path_factory):
    """Lance firmware_bsw avec une vidéo et retourne les résultats S3"""
    def _run(video, speed, lpa_act=1, turn=0):
        proc = subprocess.run(
            ["python3", "video_runner.py",
             "--video", f"inputs/videos/{video}",
             "--speed", str(speed),
             "--lpa",   str(lpa_act),
             "--turn",  str(turn)],
            capture_output=True, text=True, timeout=120
        )
        # Lire le dernier résultat depuis S3
        s3  = boto3.client("s3")
        obj = s3.list_objects_v2(Bucket=BUCKET, Prefix="results/")
        key = sorted([o["Key"] for o in obj["Contents"]])[-1]
        return json.loads(s3.get_object(Bucket=BUCKET, Key=key)
                            ["Body"].read())
    return _run

class TestScenarioNormal:
    def test_lpa_active_above_speed_min(self, run_vecu_scenario):
        r = run_vecu_scenario("test_vid.mp4", SPEED_NORM)
        assert any(f["LPA_Active"] for f in r["frames"]), \
               "LPA doit s'activer à 54 km/h"

    def test_no_warning_at_cruise(self, run_vecu_scenario):
        r = run_vecu_scenario("test_vid.mp4", SPEED_NORM)
        warns = [f for f in r["frames"]
                 if f["Warning_Left"] or f["Warning_Right"]]
        assert len(warns) == 0, f"{len(warns)} warnings inattendus"

class TestScenarioVitesseFaible:
    def test_lpa_inactive_below_speed_min(self, run_vecu_scenario):
        r = run_vecu_scenario("SHORT_TEST.mp4", SPEED_LOW)
        assert all(not f["LPA_Active"] for f in r["frames"]), \
               "LPA doit rester inactif sous 10 km/h"

    def test_system_state_speed_low(self, run_vecu_scenario):
        r = run_vecu_scenario("SHORT_TEST.mp4", SPEED_LOW)
        assert all(f["System_State"] == 0 for f in r["frames"])

class TestScenarioClignotant:
    def test_no_warning_with_turn_signal(self, run_vecu_scenario):
        r = run_vecu_scenario("test_vid.mp4", SPEED_NORM, turn=1)
        warns = [f for f in r["frames"]
                 if f["Warning_Left"] or f["Warning_Right"]]
        assert len(warns) == 0, \
               "Pas de warning si clignotant activé"