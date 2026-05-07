#include "bsw_nvm.h"
#include <stdio.h>

/* Simple in-memory log — flushed at end */
#define NVM_MAX_FRAMES 1000

static int s_frame_count = 0;
static int s_active_count = 0;
static int s_warn_left_count = 0;
static int s_warn_right_count = 0;

void BSW_NVM_Init(void) {
    s_frame_count      = 0;
    s_active_count     = 0;
    s_warn_left_count  = 0;
    s_warn_right_count = 0;
}

void BSW_NVM_LogFrame(const ExtY_LPA_EQUI_VCodeGen_T *outputs,
                      const B_LPA_EQUI_VCodeGen_T    *signals,
                      int frame_num) {
    s_frame_count++;
    if (outputs->LPA_Active)    s_active_count++;
    if (outputs->Warning_Left)  s_warn_left_count++;
    if (outputs->Warning_Right) s_warn_right_count++;
}

void BSW_NVM_Flush(void) {
    /* Print summary to stderr — Python reads this for S3 upload */
    fprintf(stderr,
        "[NVM] frames=%d active=%d warn_L=%d warn_R=%d\n",
        s_frame_count,
        s_active_count,
        s_warn_left_count,
        s_warn_right_count);
}