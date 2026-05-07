#include "bsw_com.h"
#include <stdio.h>
#include <string.h>

static double  s_speed      = 0.0;
static int     s_lpa_act    = 0;
static int     s_turn_signal = 0;

void BSW_COM_Init(double speed, int lpa_act, int turn_signal) {
    s_speed       = speed;
    s_lpa_act     = lpa_act;
    s_turn_signal = turn_signal;
}

int BSW_COM_ReadInputs(ExtU_LPA_EQUI_VCodeGen_T *inputs) {
    /* Read one frame (3 RGB values) from stdin */
    double frame[BSW_IMAGE_SIZE];
    size_t n = fread(frame, sizeof(double), BSW_IMAGE_SIZE, stdin);
    if (n != BSW_IMAGE_SIZE) {
        return 0; /* EOF or error */
    }

    /* Inject into firmware inputs */
    inputs->image[0]    = frame[0];
    inputs->image[1]    = frame[1];
    inputs->image[2]    = frame[2];
    inputs->speed       = s_speed;
    inputs->LPA_ct      = (boolean_T)s_lpa_act;
    inputs->turnSignal  = (boolean_T)s_turn_signal;

    return 1; /* success */
}

void BSW_COM_WriteOutputs(const ExtY_LPA_EQUI_VCodeGen_T *outputs,
                          const B_LPA_EQUI_VCodeGen_T    *signals,
                          int frame_num) {
    /* Calcul cause : System_State sert de cause code */
    int cause = (int)outputs->System_State;
    /* Conf : offsetPercent converti 0-100, inversé (0% offset = 100 conf) */
    float conf = 100.0f - (float)(signals->offsetPercent < 0 ?
                 -signals->offsetPercent : signals->offsetPercent);
    if (conf < 0.0f) conf = 0.0f;

    printf("{\"frame\":%d,"
           "\"active\":%d,"
           "\"warn_L\":%d,"
           "\"warn_R\":%d,"
           "\"cause\":%d,"
           "\"conf\":%.1f,"
           "\"valid\":%d,"
           "\"leftT\":%.0f,"
           "\"rightT\":%.0f,"
           "\"drift_L\":0,"
           "\"drift_R\":0}\n",
           frame_num,
           outputs->LPA_Active    ? 1 : 0,
           outputs->Warning_Left  ? 1 : 0,
           outputs->Warning_Right ? 1 : 0,
           cause,
           conf,
           signals->isValid       ? 1 : 0,
           signals->leftType,
           signals->rightType);
    fflush(stdout);
}
