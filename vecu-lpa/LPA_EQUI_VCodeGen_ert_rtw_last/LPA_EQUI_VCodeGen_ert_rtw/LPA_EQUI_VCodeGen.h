/*
 * File: LPA_EQUI_VCodeGen.h
 *
 * Code generated for Simulink model 'LPA_EQUI_VCodeGen'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
 * C/C++ source code generated on : Wed Feb 25 10:47:01 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef LPA_EQUI_VCodeGen_h_
#define LPA_EQUI_VCodeGen_h_
#ifndef LPA_EQUI_VCodeGen_COMMON_INCLUDES_
#define LPA_EQUI_VCodeGen_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* LPA_EQUI_VCodeGen_COMMON_INCLUDES_ */

#include "LPA_EQUI_VCodeGen_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals (default storage) */
typedef struct {
  real_T offsetPercent;                /* '<S2>/MATLAB Function2' */
  real_T leftType;                     /* '<S2>/MATLAB Function1' */
  real_T rightType;                    /* '<S2>/MATLAB Function1' */
  boolean_T isValid;                   /* '<S2>/MATLAB Function1' */
} B_LPA_EQUI_VCodeGen_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T ColorSpaceConversion_DWORK1[3];/* '<S2>/Color Space  Conversion' */
  real_T prevOffset;                   /* '<S2>/MATLAB Function2' */
  real_T frameCount;                   /* '<S2>/MATLAB Function2' */
  uint8_T warningLeftCounter;          /* '<Root>/Chart' */
  uint8_T warningRightCounter;         /* '<Root>/Chart' */
  uint8_T activeStableCounter;         /* '<Root>/Chart' */
  uint8_T is_active_c4_LPA_EQUI_VCodeGen;/* '<Root>/Chart' */
  uint8_T is_c4_LPA_EQUI_VCodeGen;     /* '<Root>/Chart' */
  boolean_T EnabledSubsystem_MODE;     /* '<Root>/Enabled Subsystem' */
} DW_LPA_EQUI_VCodeGen_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T speed;                        /* '<Root>/speed' */
  real_T image[3];                     /* '<Root>/image' */
  boolean_T LPA_ct;                    /* '<Root>/LPA_ct' */
  boolean_T turnSignal;                /* '<Root>/turnSignal' */
} ExtU_LPA_EQUI_VCodeGen_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  boolean_T LPA_Active;                /* '<Root>/LPA_Active' */
  boolean_T Warning_Left;              /* '<Root>/Warning_Left' */
  boolean_T Warning_Right;             /* '<Root>/Warning_Right' */
  uint8_T System_State;                /* '<Root>/System_State' */
} ExtY_LPA_EQUI_VCodeGen_T;

/* Real-time Model Data Structure */
struct tag_RTM_LPA_EQUI_VCodeGen_T {
  const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern B_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_B;

/* Block states (default storage) */
extern DW_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_Y;

/* Model entry point functions */
extern void LPA_EQUI_VCodeGen_initialize(void);
extern void LPA_EQUI_VCodeGen_step(void);
extern void LPA_EQUI_VCodeGen_terminate(void);

/* Real-time Model object */
extern RT_MODEL_LPA_EQUI_VCodeGen_T *const LPA_EQUI_VCodeGen_M;

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
 * '<Root>' : 'LPA_EQUI_VCodeGen'
 * '<S1>'   : 'LPA_EQUI_VCodeGen/Chart'
 * '<S2>'   : 'LPA_EQUI_VCodeGen/Enabled Subsystem'
 * '<S3>'   : 'LPA_EQUI_VCodeGen/MATLAB Function3'
 * '<S4>'   : 'LPA_EQUI_VCodeGen/Enabled Subsystem/Compare To Constant'
 * '<S5>'   : 'LPA_EQUI_VCodeGen/Enabled Subsystem/MATLAB Function'
 * '<S6>'   : 'LPA_EQUI_VCodeGen/Enabled Subsystem/MATLAB Function1'
 * '<S7>'   : 'LPA_EQUI_VCodeGen/Enabled Subsystem/MATLAB Function2'
 */
#endif                                 /* LPA_EQUI_VCodeGen_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
