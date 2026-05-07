/*
 * bsw_os.c — Virtual Basic Software OS
 *
 * Replaces AUTOSAR OS for vECU cloud execution.
 * Calls rt_OneStep() every 200ms cycle (same as ert_main base rate).
 * Does NOT modify ert_main.c — BSW runs as a separate process.
 */

#include "bsw_os.h"
#include "bsw_com.h"
#include "bsw_nvm.h"
#include "LPA_EQUI_VCodeGen.h"
#include <stdio.h>
#include <unistd.h>

/* ── Internal state ── */
static int s_frame_num   = 0;
static int s_running     = 1;

/* Forward declaration of rt_OneStep from ert_main */
extern void rt_OneStep(void);

/* ─────────────────────────────────────────
 * BSW_Initialize
 * Equivalent to AUTOSAR EcucInit()
 * ───────────────────────────────────────── */
BSW_Status_T BSW_Initialize(void) {
    /* 1. Init firmware model (like AUTOSAR OS init) */
    LPA_EQUI_VCodeGen_initialize();

    /* 2. Init COM layer (CAN bus equivalent) */
    /* Args injected from video_runner.py via argv */
    /* Already done in main — BSW_COM_Init called before BSW_Initialize */

    /* 3. Init NvM layer (Flash storage equivalent) */
    BSW_NVM_Init();

    fprintf(stderr, "[BSW] Initialized — cycle=200ms\n");
    return BSW_OK;
}

/* ─────────────────────────────────────────
 * BSW_Task_200ms
 * Equivalent to AUTOSAR OS AlarmCallback()
 * Called every 200ms by the scheduler
 * ───────────────────────────────────────── */
void BSW_Task_200ms(void) {
    /* 1. COM — Read inputs (equivalent to CAN RX interrupt) */
    int has_frame = BSW_COM_ReadInputs(&LPA_EQUI_VCodeGen_U);
    if (!has_frame) {
        s_running = 0;
        return;
    }

    /* 2. FIRMWARE — Execute one step (equivalent to timer interrupt) */
    rt_OneStep();
    s_frame_num++;

    /* 3. COM — Write outputs (equivalent to CAN TX) */
    BSW_COM_WriteOutputs(&LPA_EQUI_VCodeGen_Y,
                         &LPA_EQUI_VCodeGen_B,
                         s_frame_num);

    /* 4. NvM — Log result (equivalent to DEM/NvM write) */
    BSW_NVM_LogFrame(&LPA_EQUI_VCodeGen_Y,
                     &LPA_EQUI_VCodeGen_B,
                     s_frame_num);
}

/* ─────────────────────────────────────────
 * BSW_Run
 * Main scheduler loop
 * Equivalent to AUTOSAR OS StartOS()
 * ───────────────────────────────────────── */
BSW_Status_T BSW_Run(void) {
    fprintf(stderr, "[BSW] Scheduler started\n");

    while (s_running &&
           rtmGetErrorStatus(LPA_EQUI_VCodeGen_M) == (NULL)) {

        /* Execute 200ms task */
        BSW_Task_200ms();

        /* In cloud mode: no real sleep needed (data-driven) */
        /* In real-time mode: usleep(BSW_CYCLE_US); */
    }

    return BSW_OK;
}

/* ─────────────────────────────────────────
 * BSW_Terminate
 * Equivalent to AUTOSAR OS ShutdownOS()
 * ───────────────────────────────────────── */
void BSW_Terminate(void) {
    /* Flush NvM to stderr (Python uploads to S3) */
    BSW_NVM_Flush();

    /* Terminate firmware model */
    LPA_EQUI_VCodeGen_terminate();

    fprintf(stderr, "[BSW] Terminated — %d frames processed\n", s_frame_num);
}