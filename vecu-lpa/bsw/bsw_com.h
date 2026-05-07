#ifndef BSW_COM_H
#define BSW_COM_H

#include "LPA_EQUI_VCodeGen.h"
#include <stdint.h>

/* Frame buffer for one image */
#define BSW_IMAGE_SIZE 3

/* COM init */
void BSW_COM_Init(double speed, int lpa_act, int turn_signal);

/* Read inputs from stdin (video frame) into firmware struct */
int  BSW_COM_ReadInputs(ExtU_LPA_EQUI_VCodeGen_T *inputs);

/* Write outputs to stdout as JSON */
void BSW_COM_WriteOutputs(const ExtY_LPA_EQUI_VCodeGen_T *outputs,
                          const B_LPA_EQUI_VCodeGen_T    *signals,
                          int frame_num);

#endif
