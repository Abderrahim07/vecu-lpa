#ifndef BSW_NVM_H
#define BSW_NVM_H

#include "LPA_EQUI_VCodeGen.h"

void BSW_NVM_Init(void);
void BSW_NVM_LogFrame(const ExtY_LPA_EQUI_VCodeGen_T *outputs,
                      const B_LPA_EQUI_VCodeGen_T    *signals,
                      int frame_num);
void BSW_NVM_Flush(void);

#endif