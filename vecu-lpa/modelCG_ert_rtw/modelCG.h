/*
 * File: modelCG.h
 *
 * Code generated for Simulink model 'modelCG'.
 *
 * Model version                  : 1.3
 * Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
 * C/C++ source code generated on : Thu Mar 12 10:31:23 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef modelCG_h_
#define modelCG_h_
#ifndef modelCG_COMMON_INCLUDES_
#define modelCG_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* modelCG_COMMON_INCLUDES_ */

#include "modelCG_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals (default storage) */
typedef struct {
  real32_T bevImg[230400];             /* '<S3>/MATLAB Function3' */
  real32_T ColorSpaceConversion[76800];
  uint8_T binaryImg[76800];            /* '<S3>/MATLAB Function4' */
  uint8_T maskedImg[76800];            /* '<S3>/MATLAB Function' */
  real_T confidence;                   /* '<S3>/MATLAB Function5' */
  boolean_T drift_left;                /* '<S3>/MATLAB Function2' */
  boolean_T drift_right;               /* '<S3>/MATLAB Function2' */
  boolean_T isValid;                   /* '<S3>/MATLAB Function1' */
} B_modelCG_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T frameCount_DSTATE;            /* '<S3>/frameCount ' */
  real_T prevOffset_DSTATE;            /* '<S3>/prevOffset' */
  real_T CONF_MIN;                     /* '<Root>/Chart' */
  real32_T ColorSpaceConversion_DWORK1[230400];/* '<S3>/Color Space  Conversion' */
  uint8_T warningLeftCnt;              /* '<Root>/Chart' */
  uint8_T warningRightCnt;             /* '<Root>/Chart' */
  uint8_T is_active_c4_modelCG;        /* '<Root>/Chart' */
  uint8_T is_c4_modelCG;               /* '<Root>/Chart' */
  boolean_T EnabledSubsystem_MODE;     /* '<Root>/Enabled Subsystem' */
} DW_modelCG_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T E_speed;                      /* '<Root>/E_speed' */
  real32_T E_Image[230400];            /* '<Root>/E_Image' */
  boolean_T E_LPA_Act;                 /* '<Root>/E_LPA_Act' */
  boolean_T E_turnSignal;              /* '<Root>/E_turnSignal' */
} ExtU_modelCG_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  boolean_T S_LPA_Active;              /* '<Root>/S_LPA_Active' */
  boolean_T S_Warning_Left;            /* '<Root>/S_Warning_Left' */
  boolean_T S_Warning_Right;           /* '<Root>/S_Warning_Right' */
  real_T S_Cause;                      /* '<Root>/S_Cause' */
  real_T S_leftType;                   /* '<Root>/S_leftType' */
  real_T S_rightType;                  /* '<Root>/S_rightType' */
} ExtY_modelCG_T;

/* Real-time Model Data Structure */
struct tag_RTM_modelCG_T {
  const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern B_modelCG_T modelCG_B;

/* Block states (default storage) */
extern DW_modelCG_T modelCG_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_modelCG_T modelCG_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_modelCG_T modelCG_Y;

/* Model entry point functions */
extern void modelCG_initialize(void);
extern void modelCG_step(void);
extern void modelCG_terminate(void);

/* Real-time Model object */
extern RT_MODEL_modelCG_T *const modelCG_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S3>/Display' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'modelCG'
 * '<S1>'   : 'modelCG/Chart'
 * '<S2>'   : 'modelCG/Compare To Constant'
 * '<S3>'   : 'modelCG/Enabled Subsystem'
 * '<S4>'   : 'modelCG/MATLAB Function3'
 * '<S5>'   : 'modelCG/Enabled Subsystem/MATLAB Function'
 * '<S6>'   : 'modelCG/Enabled Subsystem/MATLAB Function1'
 * '<S7>'   : 'modelCG/Enabled Subsystem/MATLAB Function2'
 * '<S8>'   : 'modelCG/Enabled Subsystem/MATLAB Function3'
 * '<S9>'   : 'modelCG/Enabled Subsystem/MATLAB Function4'
 * '<S10>'  : 'modelCG/Enabled Subsystem/MATLAB Function5'
 */
#endif                                 /* modelCG_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
