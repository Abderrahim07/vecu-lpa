import docker, pytest, time, json, boto3

ECR_IMAGE = "624577704118.dkr.ecr.us-east-1.amazonaws.com/vecu-dev:latest"
REGION    = "us-east-1"
S3_BUCKET = "vecu-sim-results-dev"

class TestLPA:

    def setup_method(self):
        self.client = docker.from_env()
        self.s3     = boto3.client("s3", region_name=REGION)

    def get_result(self, scenario):
        """Récupérer résultat depuis S3"""
        obj = self.s3.get_object(
            Bucket=S3_BUCKET,
            Key=f"outputs/{scenario}_result.json"
        )
        return json.loads(obj["Body"].read())

    # ── Test 1 ───────────────────────────────────
    def test_lpa_active_normal(self):
        """
        Vidéo 1, Speed=54, LPA=1
        → S_Cause=5 (LPA Active)
        → S_LPA_Active=true
        → Pas de warning
        → confidence >= 70
        → lignes détectées
        """
        result = self.get_result("normal")

        assert result["S_LPA_Active"]        == True,  \
            f"❌ LPA pas actif! S_Cause={result['S_Cause']}"
        assert result["S_Cause"]             == 5,     \
            f"❌ S_Cause={result['S_Cause']} attendu=5 (LPA Active)"
        assert result["S_Warning_Left"]      == False, \
            "❌ Fausse alerte gauche!"
        assert result["S_Warning_Right"]     == False, \
            "❌ Fausse alerte droite!"
        assert result["confidence"]          >= 70,    \
            f"❌ Confidence trop faible: {result['confidence']}"
        assert result["leftType"]            in [1,2], \
            f"❌ Ligne gauche non détectée: {result['leftType']}"
        assert result["rightType"]           in [1,2], \
            f"❌ Ligne droite non détectée: {result['rightType']}"

        print(f"✅ Test 1 PASSED: LPA Active")
        print(f"   S_Cause={result['S_Cause']} (LPA Active)")
        print(f"   confidence={result['confidence']}")
        print(f"   leftType={result['leftType']} rightType={result['rightType']}")

    # ── Test 2 ───────────────────────────────────
    def test_vitesse_trop_faible(self):
        """
        Vidéo 2, Speed=8 < 10 km/h
        → S_Cause=1 (Speed too low)
        → S_LPA_Active=false
        → Pas de warning
        """
        result = self.get_result("vitesse_faible")

        assert result["S_LPA_Active"]    == False, \
            "❌ LPA devrait être désactivé!"
        assert result["S_Cause"]         == 1,     \
            f"❌ S_Cause={result['S_Cause']} attendu=1 (Speed too low)"
        assert result["S_Warning_Left"]  == False, \
            "❌ Warning gauche inattendu!"
        assert result["S_Warning_Right"] == False, \
            "❌ Warning droite inattendu!"

        print(f"✅ Test 2 PASSED: Speed too low")
        print(f"   S_Cause={result['S_Cause']} (Speed too low)")
        print(f"   S_LPA_Active={result['S_LPA_Active']}")

    # ── Test 3 ───────────────────────────────────
    def test_cause_valide(self):
        """
        S_Cause doit toujours être entre 0 et 5
        """
        for scenario in ["normal", "vitesse_faible"]:
            result = self.get_result(scenario)
            assert 0 <= result["S_Cause"] <= 5, \
                f"❌ S_Cause invalide: {result['S_Cause']}"
        print("✅ Test 3 PASSED: S_Cause toujours valide (0-5)")

    # ── Test 4 ───────────────────────────────────
    def test_type_lignes_valide(self):
        """
        leftType et rightType = 0, 1 ou 2
        """
        result = self.get_result("normal")
        assert result["leftType"]  in [0, 1, 2], \
            f"❌ leftType invalide: {result['leftType']}"
        assert result["rightType"] in [0, 1, 2], \
            f"❌ rightType invalide: {result['rightType']}"
        print(f"✅ Test 4 PASSED: types lignes valides")
        print(f"   leftType={result['leftType']}  (1=Dashed 2=Solid)")
        print(f"   rightType={result['rightType']} (1=Dashed 2=Solid)")

    # ── Test 5 ───────────────────────────────────
    def test_confidence_plage(self):
        """
        confidence doit être entre 0 et 100
        """
        result = self.get_result("normal")
        assert 0 <= result["confidence"] <= 100, \
            f"❌ confidence hors plage: {result['confidence']}"
        print(f"✅ Test 5 PASSED: confidence={result['confidence']}/100")