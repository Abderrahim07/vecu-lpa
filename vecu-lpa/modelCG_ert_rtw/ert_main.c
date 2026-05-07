#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modelCG.h"

int_T main(int_T argc, const char *argv[])
{
    if (argc < 4) {
        printf("Usage: firmware <speed> <lpa_act> <turn_signal>\n");
        return 1;
    }

    double speed    = atof(argv[1]);
    int    lpa_act  = atoi(argv[2]);
    int    turn_sig = atoi(argv[3]);
    int    frame_num = 0;

    modelCG_initialize();

    modelCG_U.E_speed      = speed;
    modelCG_U.E_LPA_Act    = (boolean_T)lpa_act;
    modelCG_U.E_turnSignal = (boolean_T)turn_sig;

    /* Mode streaming : une frame à la fois */
    while (fread(modelCG_U.E_Image, sizeof(real32_T), 230400, stdin) == 230400) {
        modelCG_step();
        frame_num++;

        /* Envoyer résultat de cette frame sur stdout — une ligne JSON */
        printf("{\"frame\":%d,\"active\":%d,\"cause\":%.0f,"
               "\"conf\":%.1f,\"valid\":%d,"
               "\"drift_L\":%d,\"drift_R\":%d,"
               "\"leftT\":%.0f,\"rightT\":%.0f,"
               "\"warn_L\":%d,\"warn_R\":%d}\n",
               frame_num,
               modelCG_Y.S_LPA_Active   ? 1 : 0,
               modelCG_Y.S_Cause,
               modelCG_B.confidence,
               modelCG_B.isValid        ? 1 : 0,
               modelCG_B.drift_left     ? 1 : 0,
               modelCG_B.drift_right    ? 1 : 0,
               modelCG_Y.S_leftType,
               modelCG_Y.S_rightType,
               modelCG_Y.S_Warning_Left  ? 1 : 0,
               modelCG_Y.S_Warning_Right ? 1 : 0
        );

        /* Flush immédiat pour que Python lise ligne par ligne */
        fflush(stdout);
    }

    modelCG_terminate();
    return 0;
}