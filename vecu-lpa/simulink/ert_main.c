#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LPA_EQUI_VCodeGen.h"

int_T main(int_T argc, const char *argv[])
{
    /* Arguments : speed lpa_ct turn_signal R G B frames */
    if (argc < 7) {
        printf("Usage: firmware <speed> <lpa_ct> <turn_signal> <R> <G> <B> [frames]\n");
        return 1;
    }

    double speed      = atof(argv[1]);
    int    lpa_ct     = atoi(argv[2]);
    int    turn_signal= atoi(argv[3]);
    double R          = atof(argv[4]);
    double G          = atof(argv[5]);
    double B          = atof(argv[6]);
    int    frames     = (argc > 7) ? atoi(argv[7]) : 30;

    /* Initialiser le modèle */
    LPA_EQUI_VCodeGen_initialize();

    /* Variables pour stocker les derniers outputs */
    int    lpa_active    = 0;
    int    warning_left  = 0;
    int    warning_right = 0;
    int    system_state  = 0;

    /* Lancer le modèle pour N frames */
    for (int i = 0; i < frames; i++) {

        /* Injecter les inputs */
        LPA_EQUI_VCodeGen_U.speed      = speed;
        LPA_EQUI_VCodeGen_U.LPA_ct     = (boolean_T)lpa_ct;
        LPA_EQUI_VCodeGen_U.turnSignal = (boolean_T)turn_signal;
        LPA_EQUI_VCodeGen_U.image[0]   = R;
        LPA_EQUI_VCodeGen_U.image[1]   = G;
        LPA_EQUI_VCodeGen_U.image[2]   = B;

        /* Exécuter un step du modèle */
        LPA_EQUI_VCodeGen_step();

        /* Récupérer les outputs */
        lpa_active    = LPA_EQUI_VCodeGen_Y.LPA_Active;
        warning_left  = LPA_EQUI_VCodeGen_Y.Warning_Left;
        warning_right = LPA_EQUI_VCodeGen_Y.Warning_Right;
        system_state  = LPA_EQUI_VCodeGen_Y.System_State;
    }

    /* Afficher résultats en JSON */
    printf("{\n");
    printf("  \"S_LPA_Active\": %s,\n",    lpa_active    ? "true" : "false");
    printf("  \"S_Warning_Left\": %s,\n",  warning_left  ? "true" : "false");
    printf("  \"S_Warning_Right\": %s,\n", warning_right ? "true" : "false");
    printf("  \"S_Cause\": %d,\n",         system_state);
    printf("  \"leftType\": %.1f,\n",      LPA_EQUI_VCodeGen_B.leftType);
    printf("  \"rightType\": %.1f,\n",     LPA_EQUI_VCodeGen_B.rightType);
    printf("  \"confidence\": %.1f\n",     LPA_EQUI_VCodeGen_B.offsetPercent);
    printf("}\n");

    /* Terminer le modèle */
    LPA_EQUI_VCodeGen_terminate();
    return 0;
}