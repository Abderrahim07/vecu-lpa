#ifndef BSW_OS_H
#define BSW_OS_H

#include <stdint.h>

/* Cycle time in microseconds — 200ms like ert_main base rate */
#define BSW_CYCLE_US  200000

/* Task status */
typedef enum {
    BSW_OK    = 0,
    BSW_ERROR = 1
} BSW_Status_T;

/* Init + run */
BSW_Status_T BSW_Initialize(void);
BSW_Status_T BSW_Run(void);
void         BSW_Terminate(void);

/* Called by OS scheduler every cycle */
void BSW_Task_200ms(void);

#endif