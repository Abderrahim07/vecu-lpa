/*
 * File: LPA_EQUI_VCodeGen.c
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

#include "LPA_EQUI_VCodeGen.h"
#include "rtwtypes.h"

/* Named constants for Chart: '<Root>/Chart' */
#define LPA_EQUI_VCod_VALIDATION_FRAMES (5.0)
#define LPA_EQUI_VCod_WARNING_THRESHOLD (45.0)
#define LPA_EQUI_VCodeG_IN_WARNING_LEFT ((uint8_T)3U)
#define LPA_EQUI_VCodeGen_IN_ACTIVE    ((uint8_T)1U)
#define LPA_EQUI_VCodeGen_IN_INACTIVE  ((uint8_T)2U)
#define LPA_EQUI_VCodeGen_SPEED_MIN    (10.0)
#define LPA_EQUI_VCode_IN_WARNING_RIGHT ((uint8_T)4U)

/* Block signals (default storage) */
B_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_B;

/* Block states (default storage) */
DW_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_DW;

/* External inputs (root inport signals with default storage) */
ExtU_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_Y;

/* Real-time model */
static RT_MODEL_LPA_EQUI_VCodeGen_T LPA_EQUI_VCodeGen_M_;
RT_MODEL_LPA_EQUI_VCodeGen_T *const LPA_EQUI_VCodeGen_M = &LPA_EQUI_VCodeGen_M_;

/* Model step function */
void LPA_EQUI_VCodeGen_step(void)
{
  int32_T tmp;
  boolean_T tmp_0;

  /* Outputs for Enabled SubSystem: '<Root>/Enabled Subsystem' incorporates:
   *  EnablePort: '<S2>/Enable'
   */
  /* Logic: '<Root>/Logical Operator' incorporates:
   *  Inport: '<Root>/LPA_ct'
   *  Inport: '<Root>/image'
   *  MATLAB Function: '<Root>/MATLAB Function3'
   */
  if ((!(((LPA_EQUI_VCodeGen_U.image[0] + LPA_EQUI_VCodeGen_U.image[1]) +
          LPA_EQUI_VCodeGen_U.image[2]) / 3.0 < 0.05)) &&
      LPA_EQUI_VCodeGen_U.LPA_ct) {
    if (!LPA_EQUI_VCodeGen_DW.EnabledSubsystem_MODE) {
      /* SystemReset for MATLAB Function: '<S2>/MATLAB Function2' */
      LPA_EQUI_VCodeGen_DW.prevOffset = 0.0;
      LPA_EQUI_VCodeGen_DW.frameCount = 0.0;
      LPA_EQUI_VCodeGen_DW.EnabledSubsystem_MODE = true;
    }

    /* MATLAB Function: '<S2>/MATLAB Function1' */
    LPA_EQUI_VCodeGen_B.isValid = false;
    LPA_EQUI_VCodeGen_B.leftType = 1.0;
    LPA_EQUI_VCodeGen_B.rightType = 1.0;

    /* MATLAB Function: '<S2>/MATLAB Function2' */
    LPA_EQUI_VCodeGen_DW.frameCount++;
    LPA_EQUI_VCodeGen_B.offsetPercent = LPA_EQUI_VCodeGen_DW.prevOffset;
  } else {
    LPA_EQUI_VCodeGen_DW.EnabledSubsystem_MODE = false;
  }

  /* End of Logic: '<Root>/Logical Operator' */
  /* End of Outputs for SubSystem: '<Root>/Enabled Subsystem' */

  /* Chart: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/speed'
   *  Inport: '<Root>/turnSignal'
   */
  if (LPA_EQUI_VCodeGen_DW.is_active_c4_LPA_EQUI_VCodeGen == 0U) {
    LPA_EQUI_VCodeGen_DW.is_active_c4_LPA_EQUI_VCodeGen = 1U;
    LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen = LPA_EQUI_VCodeGen_IN_INACTIVE;

    /* Outport: '<Root>/LPA_Active' */
    LPA_EQUI_VCodeGen_Y.LPA_Active = false;

    /* Outport: '<Root>/Warning_Left' */
    LPA_EQUI_VCodeGen_Y.Warning_Left = false;

    /* Outport: '<Root>/Warning_Right' */
    LPA_EQUI_VCodeGen_Y.Warning_Right = false;

    /* Outport: '<Root>/System_State' */
    LPA_EQUI_VCodeGen_Y.System_State = 0U;
  } else {
    switch (LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen) {
     case LPA_EQUI_VCodeGen_IN_ACTIVE:
      /* Outport: '<Root>/LPA_Active' */
      LPA_EQUI_VCodeGen_Y.LPA_Active = true;

      /* Outport: '<Root>/Warning_Left' */
      LPA_EQUI_VCodeGen_Y.Warning_Left = false;

      /* Outport: '<Root>/Warning_Right' */
      LPA_EQUI_VCodeGen_Y.Warning_Right = false;

      /* Outport: '<Root>/System_State' */
      LPA_EQUI_VCodeGen_Y.System_State = 1U;
      if ((LPA_EQUI_VCodeGen_DW.warningLeftCounter >= (int32_T)
           LPA_EQUI_VCod_VALIDATION_FRAMES) && (LPA_EQUI_VCodeGen_B.leftType ==
           0.0)) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeG_IN_WARNING_LEFT;

        /* Outport: '<Root>/Warning_Left' */
        LPA_EQUI_VCodeGen_Y.Warning_Left = true;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 2U;
      } else if ((LPA_EQUI_VCodeGen_DW.warningRightCounter >= (int32_T)
                  LPA_EQUI_VCod_VALIDATION_FRAMES) &&
                 (LPA_EQUI_VCodeGen_B.rightType == 0.0)) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCode_IN_WARNING_RIGHT;

        /* Outport: '<Root>/Warning_Right' */
        LPA_EQUI_VCodeGen_Y.Warning_Right = true;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 3U;
      } else if ((!LPA_EQUI_VCodeGen_B.isValid) || (LPA_EQUI_VCodeGen_U.speed <=
                  LPA_EQUI_VCodeGen_SPEED_MIN)) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeGen_IN_INACTIVE;

        /* Outport: '<Root>/LPA_Active' */
        LPA_EQUI_VCodeGen_Y.LPA_Active = false;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 0U;
      } else {
        tmp_0 = !LPA_EQUI_VCodeGen_U.turnSignal;
        tmp = ((LPA_EQUI_VCodeGen_B.offsetPercent < -45.0) && tmp_0 &&
               (LPA_EQUI_VCodeGen_B.leftType == 0.0)) *
          (LPA_EQUI_VCodeGen_DW.warningLeftCounter + 1);
        if (tmp > 255) {
          tmp = 255;
        }

        LPA_EQUI_VCodeGen_DW.warningLeftCounter = (uint8_T)tmp;
        tmp = ((LPA_EQUI_VCodeGen_B.offsetPercent >
                LPA_EQUI_VCod_WARNING_THRESHOLD) && tmp_0 &&
               (LPA_EQUI_VCodeGen_B.rightType == 0.0)) *
          (LPA_EQUI_VCodeGen_DW.warningRightCounter + 1);
        if (tmp > 255) {
          tmp = 255;
        }

        LPA_EQUI_VCodeGen_DW.warningRightCounter = (uint8_T)tmp;
      }
      break;

     case LPA_EQUI_VCodeGen_IN_INACTIVE:
      /* Outport: '<Root>/LPA_Active' */
      LPA_EQUI_VCodeGen_Y.LPA_Active = false;

      /* Outport: '<Root>/Warning_Left' */
      LPA_EQUI_VCodeGen_Y.Warning_Left = false;

      /* Outport: '<Root>/Warning_Right' */
      LPA_EQUI_VCodeGen_Y.Warning_Right = false;

      /* Outport: '<Root>/System_State' */
      LPA_EQUI_VCodeGen_Y.System_State = 0U;
      tmp_0 = ((LPA_EQUI_VCodeGen_U.speed > LPA_EQUI_VCodeGen_SPEED_MIN) &&
               LPA_EQUI_VCodeGen_B.isValid);
      if (tmp_0 && (LPA_EQUI_VCodeGen_DW.activeStableCounter >= (int32_T)
                    LPA_EQUI_VCod_VALIDATION_FRAMES)) {
        LPA_EQUI_VCodeGen_DW.activeStableCounter = 0U;
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeGen_IN_ACTIVE;

        /* Outport: '<Root>/LPA_Active' */
        LPA_EQUI_VCodeGen_Y.LPA_Active = true;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 1U;
        LPA_EQUI_VCodeGen_DW.warningLeftCounter = 0U;
        LPA_EQUI_VCodeGen_DW.warningRightCounter = 0U;
      } else {
        tmp = (LPA_EQUI_VCodeGen_DW.activeStableCounter + 1) * tmp_0;
        if (tmp > 255) {
          tmp = 255;
        }

        LPA_EQUI_VCodeGen_DW.activeStableCounter = (uint8_T)tmp;
      }
      break;

     case LPA_EQUI_VCodeG_IN_WARNING_LEFT:
      /* Outport: '<Root>/LPA_Active' */
      LPA_EQUI_VCodeGen_Y.LPA_Active = true;

      /* Outport: '<Root>/Warning_Left' */
      LPA_EQUI_VCodeGen_Y.Warning_Left = true;

      /* Outport: '<Root>/Warning_Right' */
      LPA_EQUI_VCodeGen_Y.Warning_Right = false;

      /* Outport: '<Root>/System_State' */
      LPA_EQUI_VCodeGen_Y.System_State = 2U;
      if (LPA_EQUI_VCodeGen_B.offsetPercent >= -45.0) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeGen_IN_ACTIVE;

        /* Outport: '<Root>/Warning_Left' */
        LPA_EQUI_VCodeGen_Y.Warning_Left = false;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 1U;
        LPA_EQUI_VCodeGen_DW.warningLeftCounter = 0U;
        LPA_EQUI_VCodeGen_DW.warningRightCounter = 0U;
      } else if ((!LPA_EQUI_VCodeGen_B.isValid) || (LPA_EQUI_VCodeGen_U.speed <=
                  LPA_EQUI_VCodeGen_SPEED_MIN)) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeGen_IN_INACTIVE;

        /* Outport: '<Root>/LPA_Active' */
        LPA_EQUI_VCodeGen_Y.LPA_Active = false;

        /* Outport: '<Root>/Warning_Left' */
        LPA_EQUI_VCodeGen_Y.Warning_Left = false;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 0U;
      } else {
        LPA_EQUI_VCodeGen_DW.warningLeftCounter = (uint8_T)
          (((LPA_EQUI_VCodeGen_B.offsetPercent < -45.0) &&
            (!LPA_EQUI_VCodeGen_U.turnSignal)) * (int32_T)
           LPA_EQUI_VCod_VALIDATION_FRAMES);
      }
      break;

     default:
      /* Outport: '<Root>/LPA_Active' */
      /* case IN_WARNING_RIGHT: */
      LPA_EQUI_VCodeGen_Y.LPA_Active = true;

      /* Outport: '<Root>/Warning_Left' */
      LPA_EQUI_VCodeGen_Y.Warning_Left = false;

      /* Outport: '<Root>/Warning_Right' */
      LPA_EQUI_VCodeGen_Y.Warning_Right = true;

      /* Outport: '<Root>/System_State' */
      LPA_EQUI_VCodeGen_Y.System_State = 3U;
      if ((!LPA_EQUI_VCodeGen_B.isValid) || (LPA_EQUI_VCodeGen_U.speed <=
           LPA_EQUI_VCodeGen_SPEED_MIN)) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeGen_IN_INACTIVE;

        /* Outport: '<Root>/LPA_Active' */
        LPA_EQUI_VCodeGen_Y.LPA_Active = false;

        /* Outport: '<Root>/Warning_Right' */
        LPA_EQUI_VCodeGen_Y.Warning_Right = false;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 0U;
      } else if (LPA_EQUI_VCodeGen_B.offsetPercent <=
                 LPA_EQUI_VCod_WARNING_THRESHOLD) {
        LPA_EQUI_VCodeGen_DW.is_c4_LPA_EQUI_VCodeGen =
          LPA_EQUI_VCodeGen_IN_ACTIVE;

        /* Outport: '<Root>/Warning_Right' */
        LPA_EQUI_VCodeGen_Y.Warning_Right = false;

        /* Outport: '<Root>/System_State' */
        LPA_EQUI_VCodeGen_Y.System_State = 1U;
        LPA_EQUI_VCodeGen_DW.warningLeftCounter = 0U;
        LPA_EQUI_VCodeGen_DW.warningRightCounter = 0U;
      } else {
        LPA_EQUI_VCodeGen_DW.warningRightCounter = (uint8_T)
          (((LPA_EQUI_VCodeGen_B.offsetPercent > LPA_EQUI_VCod_WARNING_THRESHOLD)
            && (!LPA_EQUI_VCodeGen_U.turnSignal)) * (int32_T)
           LPA_EQUI_VCod_VALIDATION_FRAMES);
      }
      break;
    }
  }

  /* End of Chart: '<Root>/Chart' */
}

/* Model initialize function */
void LPA_EQUI_VCodeGen_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void LPA_EQUI_VCodeGen_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
