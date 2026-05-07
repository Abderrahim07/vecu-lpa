/*
 * bsw_main.c — vECU Entry Point
 * Defines rt_OneStep() required by bsw_os.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "bsw_os.h"
#include "bsw_com.h"
#include "LPA_EQUI_VCodeGen.h"

/* ─────────────────────────────────────────────────────
 * rt_OneStep — appelée par bsw_os.c toutes les 200ms
 * DOIT être définie ici — sans static
 * ───────────────────────────────────────────────────── */
void rt_OneStep(void) {
    static boolean_T OverrunFlag = false;
    if (OverrunFlag) {
        rtmSetErrorStatus(LPA_EQUI_VCodeGen_M, "Overrun");
        return;
    }
    OverrunFlag = true;
    LPA_EQUI_VCodeGen_step();
    OverrunFlag = false;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: firmware_bsw <speed> <lpa_act> <turn_signal>\n");
        return 1;
    }

    double speed       = atof(argv[1]);
    int    lpa_act     = atoi(argv[2]);
    int    turn_signal = atoi(argv[3]);

    BSW_COM_Init(speed, lpa_act, turn_signal);

    if (BSW_Initialize() != BSW_OK) {
        fprintf(stderr, "[BSW] Init failed\n");
        return 1;
    }

    BSW_Run();
    BSW_Terminate();

    return 0;
}