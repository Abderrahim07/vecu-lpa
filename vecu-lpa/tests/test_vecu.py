import docker
import boto3
import pytest
import time

ECR_IMAGE = "624577704118.dkr.ecr.us-east-1.amazonaws.com/vecu-dev:latest"
REGION    = "us-east-1"

class TestVECU:

    def setup_method(self):
        """Initialiser le client Docker"""
        self.client = docker.from_env()

    # ── Test 1 ────────────────────────────────────────
    def test_container_starts(self):
        """Vérifier que le container démarre correctement"""
        container = self.client.containers.run(
            ECR_IMAGE,
            detach=True,
            remove=False
        )
        time.sleep(3)
        container.reload()

        assert container.status in ["running", "exited"], \
            f"Container status inattendu: {container.status}"

        container.stop()
        container.remove()
        print("✅ Test 1 PASSED: container démarre")

    # ── Test 2 ────────────────────────────────────────
    def test_firmware_initializes(self):
        """Vérifier que le firmware Simulink s'initialise"""
        container = self.client.containers.run(
            ECR_IMAGE,
            detach=True,
            remove=False
        )
        time.sleep(3)

        logs = container.logs().decode("utf-8")

        assert "Starting Simulink ECU simulation" in logs, \
            f"Firmware pas initialisé. Logs: {logs}"

        container.stop()
        container.remove()
        print("✅ Test 2 PASSED: firmware initialisé")

    # ── Test 3 ────────────────────────────────────────
    def test_simulink_model_loaded(self):
        """Vérifier que le modèle LPA_EQUI_VCodeGen est chargé"""
        container = self.client.containers.run(
            ECR_IMAGE,
            detach=True,
            remove=False
        )
        time.sleep(3)

        logs = container.logs().decode("utf-8")

        assert "LPA_EQUI_VCodeGen" in logs, \
            f"Modèle Simulink pas chargé. Logs: {logs}"

        container.stop()
        container.remove()
        print("✅ Test 3 PASSED: modèle Simulink chargé")

    # ── Test 4 ────────────────────────────────────────
    def test_no_crash(self):
        """Vérifier que le firmware ne crash pas"""
        container = self.client.containers.run(
            ECR_IMAGE,
            detach=True,
            remove=False
        )
        time.sleep(5)
        container.reload()

        assert container.status != "exited" or \
            container.attrs["State"]["ExitCode"] == 0, \
            f"Firmware crashé! Exit code: {container.attrs['State']['ExitCode']}"

        container.stop()
        container.remove()
        print("✅ Test 4 PASSED: pas de crash")

    # ── Test 5 ────────────────────────────────────────
    def test_ecr_image_exists(self):
        """Vérifier que l'image existe dans ECR"""
        ecr = boto3.client("ecr", region_name=REGION)

        response = ecr.list_images(
            repositoryName="vecu-dev"
        )

        assert len(response["imageIds"]) > 0, \
            "Aucune image dans ECR !"

        print(f"✅ Test 5 PASSED: {len(response['imageIds'])} image(s) dans ECR")