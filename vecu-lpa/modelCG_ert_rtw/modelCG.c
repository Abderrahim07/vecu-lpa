/*
 * File: modelCG.c
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

#include "modelCG.h"
#include "rtwtypes.h"
#include <string.h>
#include "modelCG_private.h"
#include <math.h>

/* Named constants for Chart: '<Root>/Chart' */
#define modelCG_IN_ACTIVE              ((uint8_T)1U)
#define modelCG_IN_INACTIVE            ((uint8_T)2U)
#define modelCG_IN_WARNING_LEFT        ((uint8_T)3U)
#define modelCG_IN_WARNING_RIGHT       ((uint8_T)4U)
#define modelCG_SPEED_MIN              (10.0)
#define modelCG_VALIDATION_FRAMES      (5.0)
#define modelCG_WARNING_THRESHOLD      (30.0)

/* Block signals (default storage) */
B_modelCG_T modelCG_B;

/* Block states (default storage) */
DW_modelCG_T modelCG_DW;

/* External inputs (root inport signals with default storage) */
ExtU_modelCG_T modelCG_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_modelCG_T modelCG_Y;

/* Real-time model */
static RT_MODEL_modelCG_T modelCG_M_;
RT_MODEL_modelCG_T *const modelCG_M = &modelCG_M_;
real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/* Model step function */
void modelCG_step(void)
{
  real_T offsetRaw;
  real_T sumVal;
  real_T wRaw;
  int32_T histogram[320];
  int32_T leftPositions[5];
  int32_T rightPositions[5];
  int32_T cntL;
  int32_T curRightX;
  int32_T d_c;
  int32_T i;
  int32_T leftMax;
  int32_T scanY;
  int32_T startRow;
  int32_T sumL;
  int32_T winL;
  int32_T winR;
  int32_T xblockoffset;
  real32_T bsum;
  real32_T x;
  uint8_T prevPixelL;
  uint8_T rtb_maskedImg_0;
  boolean_T rtb_cameraOK;
  boolean_T tmp;
  boolean_T tmp_0;

  /* MATLAB Function: '<Root>/MATLAB Function3' incorporates:
   *  Inport: '<Root>/E_Image'
   */
  x = modelCG_U.E_Image[0];
  for (leftMax = 0; leftMax < 1023; leftMax++) {
    x += modelCG_U.E_Image[leftMax + 1];
  }

  for (leftMax = 0; leftMax < 224; leftMax++) {
    xblockoffset = (leftMax + 1) << 10;
    bsum = modelCG_U.E_Image[xblockoffset];
    for (startRow = 0; startRow < 1023; startRow++) {
      bsum += modelCG_U.E_Image[(xblockoffset + startRow) + 1];
    }

    x += bsum;
  }

  rtb_cameraOK = !(x / 230400.0F < 0.05);

  /* End of MATLAB Function: '<Root>/MATLAB Function3' */

  /* Outputs for Enabled SubSystem: '<Root>/Enabled Subsystem' incorporates:
   *  EnablePort: '<S3>/Enable'
   */
  /* Logic: '<Root>/Logical Operator' incorporates:
   *  Constant: '<S2>/Constant'
   *  Inport: '<Root>/E_LPA_Act'
   *  Inport: '<Root>/E_speed'
   *  RelationalOperator: '<S2>/Compare'
   */
  if (rtb_cameraOK && modelCG_U.E_LPA_Act && (modelCG_U.E_speed >= 10.0)) {
    if (!modelCG_DW.EnabledSubsystem_MODE) {
      /* InitializeConditions for UnitDelay: '<S3>/frameCount ' */
      modelCG_DW.frameCount_DSTATE = 0.0;

      /* InitializeConditions for UnitDelay: '<S3>/prevOffset' */
      modelCG_DW.prevOffset_DSTATE = 0.0;
      modelCG_DW.EnabledSubsystem_MODE = true;
    }

    /* MATLAB Function: '<S3>/MATLAB Function3' incorporates:
     *  Inport: '<Root>/E_Image'
     */
    memset(&modelCG_B.bevImg[0], 0, 230400U * sizeof(real32_T));
    for (leftMax = 0; leftMax < 240; leftMax++) {
      for (xblockoffset = 0; xblockoffset < 320; xblockoffset++) {
        wRaw = (((real_T)xblockoffset + 1.0) * -0.0005 + ((real_T)leftMax + 1.0)
                * 0.0192) - 5.7848;
        startRow = (int32_T)rt_roundd_snf(((((real_T)xblockoffset + 1.0) *
          -1.3665 + ((real_T)leftMax + 1.0) * 3.5153) - 832.7234) / wRaw);
        curRightX = (int32_T)rt_roundd_snf(((((real_T)xblockoffset + 1.0) *
          -0.1202 + ((real_T)leftMax + 1.0) * 2.7747) - 964.448) / wRaw);
        if ((startRow >= 1) && (startRow <= 320) && (curRightX <= 240)) {
          startRow = (startRow - 1) * 240 + curRightX;
          curRightX = 240 * xblockoffset + leftMax;
          modelCG_B.bevImg[curRightX] = modelCG_U.E_Image[startRow - 1];
          modelCG_B.bevImg[curRightX + 76800] = modelCG_U.E_Image[startRow +
            76799];
          modelCG_B.bevImg[curRightX + 153600] = modelCG_U.E_Image[startRow +
            153599];
        }
      }
    }

    /* End of MATLAB Function: '<S3>/MATLAB Function3' */

    /* MATLAB Function: '<S3>/MATLAB Function4' */
    sumVal = 0.0;
    for (i = 0; i < 76800; i++) {
      /* S-Function (svipcolorconv): '<S3>/Color Space  Conversion' */
      x = (modelCG_B.bevImg[i + 76800] * 0.587F + modelCG_B.bevImg[i] * 0.299F)
        + modelCG_B.bevImg[i + 153600] * 0.114F;

      /* S-Function (svipcolorconv): '<S3>/Color Space  Conversion' */
      modelCG_B.ColorSpaceConversion[i] = x;

      /* S-Function (svipcolorconv): '<S3>/Color Space  Conversion' */
      if (x > 1.0F) {
        x = 1.0F;
        modelCG_B.ColorSpaceConversion[i] = 1.0F;
      } else if (x < 0.0F) {
        x = 0.0F;
        modelCG_B.ColorSpaceConversion[i] = 0.0F;
      }

      /* MATLAB Function: '<S3>/MATLAB Function4' */
      modelCG_B.binaryImg[i] = 0U;
      wRaw = x;
      if (x > 1.0F) {
        wRaw = x / 255.0;
      }

      sumVal += wRaw;
    }

    /* MATLAB Function: '<S3>/MATLAB Function4' incorporates:
     *  S-Function (svipcolorconv): '<S3>/Color Space  Conversion'
     */
    sumVal = sumVal / 76800.0 + 0.2;
    if (sumVal > 0.85) {
      sumVal = 0.85;
    }

    if (sumVal < 0.55) {
      sumVal = 0.55;
    }

    for (i = 0; i < 76800; i++) {
      x = modelCG_B.ColorSpaceConversion[i];
      wRaw = x;
      if (x > 1.0F) {
        wRaw = x / 255.0;
      }

      if (wRaw >= sumVal) {
        modelCG_B.binaryImg[i] = MAX_uint8_T;
      }

      /* MATLAB Function: '<S3>/MATLAB Function' incorporates:
       *  S-Function (svipcolorconv): '<S3>/Color Space  Conversion'
       */
      modelCG_B.maskedImg[i] = 0U;
    }

    /* MATLAB Function: '<S3>/MATLAB Function' */
    if (modelCG_U.E_speed < 15.0) {
      sumVal = 0.55;
    } else if (modelCG_U.E_speed < 30.0) {
      sumVal = 0.65;
    } else if (modelCG_U.E_speed < 70.0) {
      sumVal = 0.68;
    } else if (modelCG_U.E_speed < 100.0) {
      sumVal = 0.6;
    } else {
      sumVal = 0.52;
    }

    startRow = (int32_T)rt_roundd_snf(240.0 * sumVal);
    for (leftMax = startRow; leftMax < 241; leftMax++) {
      for (xblockoffset = 0; xblockoffset < 320; xblockoffset++) {
        curRightX = (240 * xblockoffset + leftMax) - 1;
        modelCG_B.maskedImg[curRightX] = modelCG_B.binaryImg[curRightX];
      }
    }

    /* MATLAB Function: '<S3>/MATLAB Function1' */
    memset(&histogram[0], 0, 320U * sizeof(int32_T));
    for (leftMax = 0; leftMax < 97; leftMax++) {
      for (xblockoffset = 0; xblockoffset < 320; xblockoffset++) {
        if (modelCG_B.maskedImg[(240 * xblockoffset + leftMax) + 143] > 0) {
          startRow = histogram[xblockoffset];
          if (startRow > 2147483646) {
            histogram[xblockoffset] = MAX_int32_T;
          } else {
            histogram[xblockoffset] = startRow + 1;
          }
        }
      }
    }

    startRow = 40;
    leftMax = 0;
    curRightX = 200;
    xblockoffset = 0;
    for (i = 0; i < 160; i++) {
      scanY = histogram[i];
      if (scanY > leftMax) {
        leftMax = scanY;
        startRow = i - 39;
      }

      scanY = histogram[i + 160];
      if (scanY > xblockoffset) {
        xblockoffset = scanY;
        curRightX = i + 121;
      }
    }

    for (leftMax = 0; leftMax < 5; leftMax++) {
      leftPositions[leftMax] = 0;
      rightPositions[leftMax] = 0;
    }

    leftMax = 0;
    xblockoffset = 0;
    for (i = 0; i < 5; i++) {
      scanY = (int32_T)rt_roundd_snf((0.95 - (((real_T)i + 1.0) - 1.0) * 0.08) *
        240.0) - 1;
      winL = startRow;
      winR = startRow;
      if (startRow < 1) {
        winL = 1;
      }

      if (startRow + 80 > 320) {
        winR = 240;
      }

      sumL = 0;
      cntL = 0;
      for (d_c = winL; d_c <= winR + 80; d_c++) {
        if (modelCG_B.maskedImg[(d_c - 1) * 240 + scanY] > 0) {
          if ((sumL < 0) && (d_c < MIN_int32_T - sumL)) {
            sumL = MIN_int32_T;
          } else if ((sumL > 0) && (d_c > MAX_int32_T - sumL)) {
            sumL = MAX_int32_T;
          } else {
            sumL += d_c;
          }

          if (cntL <= 2147483646) {
            cntL++;
          }
        }
      }

      if (cntL > 2) {
        startRow = (int32_T)rt_roundd_snf((real_T)sumL / (real_T)cntL) - 40;
        if (leftMax > 2147483646) {
          leftMax = MAX_int32_T;
        } else {
          leftMax++;
        }

        leftPositions[leftMax - 1] = startRow + 40;
      }

      winL = curRightX;
      winR = curRightX;
      if (curRightX < 1) {
        winL = 1;
      }

      if (curRightX + 80 > 320) {
        winR = 240;
      }

      sumL = 0;
      cntL = 0;
      for (d_c = winL; d_c <= winR + 80; d_c++) {
        if (modelCG_B.maskedImg[(d_c - 1) * 240 + scanY] > 0) {
          if ((sumL < 0) && (d_c < MIN_int32_T - sumL)) {
            sumL = MIN_int32_T;
          } else if ((sumL > 0) && (d_c > MAX_int32_T - sumL)) {
            sumL = MAX_int32_T;
          } else {
            sumL += d_c;
          }

          if (cntL <= 2147483646) {
            cntL++;
          }
        }
      }

      if (cntL > 2) {
        curRightX = (int32_T)rt_roundd_snf((real_T)sumL / (real_T)cntL) - 40;
        if (xblockoffset > 2147483646) {
          xblockoffset = MAX_int32_T;
        } else {
          xblockoffset++;
        }

        rightPositions[xblockoffset - 1] = curRightX + 40;
      }
    }

    if ((leftMax == 0) || (xblockoffset == 0)) {
      startRow = 0;
      curRightX = 0;
      wRaw = 0.0;
      modelCG_B.isValid = false;

      /* Outport: '<Root>/S_leftType' */
      modelCG_Y.S_leftType = 0.0;

      /* Outport: '<Root>/S_rightType' */
      modelCG_Y.S_rightType = 0.0;
      leftMax = 0;
      xblockoffset = 0;
    } else {
      startRow = 0;
      for (i = 0; i < leftMax; i++) {
        curRightX = leftPositions[i];
        if ((startRow < 0) && (curRightX < MIN_int32_T - startRow)) {
          startRow = MIN_int32_T;
        } else if ((startRow > 0) && (curRightX > MAX_int32_T - startRow)) {
          startRow = MAX_int32_T;
        } else {
          startRow += curRightX;
        }
      }

      startRow = (int32_T)rt_roundd_snf((real_T)startRow / (real_T)leftMax);
      i = 0;
      for (scanY = 0; scanY < xblockoffset; scanY++) {
        curRightX = rightPositions[scanY];
        if ((i < 0) && (curRightX < MIN_int32_T - i)) {
          i = MIN_int32_T;
        } else if ((i > 0) && (curRightX > MAX_int32_T - i)) {
          i = MAX_int32_T;
        } else {
          i += curRightX;
        }
      }

      curRightX = (int32_T)rt_roundd_snf((real_T)i / (real_T)xblockoffset);
      wRaw = (real_T)curRightX - (real_T)startRow;
      if ((wRaw > 60.0) && (wRaw < 290.0)) {
        modelCG_B.isValid = true;
      } else {
        modelCG_B.isValid = false;
        startRow = 0;
        curRightX = 0;
        wRaw = 0.0;
      }

      i = 0;
      scanY = 0;
      winL = startRow;
      if (startRow < 1) {
        winL = 1;
      }

      if (winL > 320) {
        winL = 320;
      }

      winL = (winL - 1) * 240;
      prevPixelL = modelCG_B.maskedImg[winL + 131];
      for (winR = 0; winR < 109; winR++) {
        rtb_maskedImg_0 = modelCG_B.maskedImg[(winL + winR) + 131];
        if (rtb_maskedImg_0 != prevPixelL) {
          i++;
        }

        prevPixelL = rtb_maskedImg_0;
      }

      winL = curRightX;
      if (curRightX < 1) {
        winL = 1;
      }

      if (winL > 320) {
        winL = 320;
      }

      winL = (winL - 1) * 240;
      prevPixelL = modelCG_B.maskedImg[winL + 131];
      for (winR = 0; winR < 109; winR++) {
        rtb_maskedImg_0 = modelCG_B.maskedImg[(winL + winR) + 131];
        if (rtb_maskedImg_0 != prevPixelL) {
          scanY++;
        }

        prevPixelL = rtb_maskedImg_0;
      }

      if (i <= 2) {
        /* Outport: '<Root>/S_leftType' */
        modelCG_Y.S_leftType = 2.0;
      } else {
        /* Outport: '<Root>/S_leftType' */
        modelCG_Y.S_leftType = (i <= 8);
      }

      if (scanY <= 2) {
        /* Outport: '<Root>/S_rightType' */
        modelCG_Y.S_rightType = 2.0;
      } else {
        /* Outport: '<Root>/S_rightType' */
        modelCG_Y.S_rightType = (scanY <= 8);
      }
    }

    /* Sum: '<S3>/Add' incorporates:
     *  Constant: '<S3>/Constant'
     *  UnitDelay: '<S3>/frameCount '
     */
    modelCG_DW.frameCount_DSTATE++;

    /* MATLAB Function: '<S3>/MATLAB Function2' incorporates:
     *  MATLAB Function: '<S3>/MATLAB Function1'
     *  UnitDelay: '<S3>/frameCount '
     *  UnitDelay: '<S3>/prevOffset'
     */
    modelCG_B.drift_left = false;
    modelCG_B.drift_right = false;
    if ((startRow > 0) && (curRightX > 0) && (!(wRaw < 60.0))) {
      offsetRaw = ((real_T)((uint32_T)startRow + (uint32_T)curRightX) / 2.0 -
                   160.0) / (wRaw / 2.0) * 100.0;
      if (offsetRaw > 100.0) {
        offsetRaw = 100.0;
      }

      if (offsetRaw < -100.0) {
        offsetRaw = -100.0;
      }

      if (modelCG_DW.frameCount_DSTATE > 5.0) {
        offsetRaw = 0.3 * offsetRaw + 0.7 * modelCG_DW.prevOffset_DSTATE;
      }

      sumVal = fabs(offsetRaw);
      if (sumVal < 5.0) {
        offsetRaw = 0.0;
      } else if (sumVal < 10.0) {
        if (offsetRaw < 0.0) {
          offsetRaw = -(sumVal - 5.0);
        } else {
          offsetRaw -= 5.0;
        }
      }

      if (fabs(offsetRaw - modelCG_DW.prevOffset_DSTATE) < 2.0) {
        offsetRaw = modelCG_DW.prevOffset_DSTATE;
      }

      modelCG_B.drift_left = (offsetRaw < -30.0);
      modelCG_B.drift_right = (offsetRaw > 30.0);
      modelCG_DW.prevOffset_DSTATE = offsetRaw;
    }

    /* End of MATLAB Function: '<S3>/MATLAB Function2' */

    /* MATLAB Function: '<S3>/MATLAB Function5' incorporates:
     *  MATLAB Function: '<S3>/MATLAB Function1'
     */
    modelCG_B.confidence = 0.0;
    if ((leftMax >= 2) && (xblockoffset >= 2)) {
      modelCG_B.confidence = 30.0;
    }

    modelCG_B.confidence += ((real_T)leftMax / 5.0 + (real_T)xblockoffset / 5.0)
      / 2.0 * 30.0;
    if ((wRaw > 80.0) && (wRaw < 250.0)) {
      modelCG_B.confidence += 25.0;
    } else if (wRaw > 60.0) {
      modelCG_B.confidence += 12.0;
    }

    if ((leftMax > 0) && (xblockoffset > 0)) {
      modelCG_B.confidence += 15.0;
    }

    /* End of MATLAB Function: '<S3>/MATLAB Function5' */
  } else {
    modelCG_DW.EnabledSubsystem_MODE = false;
  }

  /* End of Logic: '<Root>/Logical Operator' */
  /* End of Outputs for SubSystem: '<Root>/Enabled Subsystem' */

  /* Chart: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/E_LPA_Act'
   *  Inport: '<Root>/E_speed'
   *  Inport: '<Root>/E_turnSignal'
   *  UnitDelay: '<S3>/prevOffset'
   */
  if (modelCG_DW.is_active_c4_modelCG == 0U) {
    modelCG_DW.is_active_c4_modelCG = 1U;
    modelCG_DW.is_c4_modelCG = modelCG_IN_INACTIVE;

    /* Outport: '<Root>/S_LPA_Active' */
    modelCG_Y.S_LPA_Active = false;

    /* Outport: '<Root>/S_Warning_Left' */
    modelCG_Y.S_Warning_Left = false;

    /* Outport: '<Root>/S_Warning_Right' */
    modelCG_Y.S_Warning_Right = false;
    modelCG_DW.warningLeftCnt = 0U;
    modelCG_DW.warningRightCnt = 0U;
  } else {
    switch (modelCG_DW.is_c4_modelCG) {
     case modelCG_IN_ACTIVE:
      tmp = !modelCG_U.E_turnSignal;
      tmp_0 = (modelCG_B.drift_left && tmp);
      if (tmp_0 && (modelCG_DW.warningLeftCnt >= (int32_T)
                    modelCG_VALIDATION_FRAMES)) {
        modelCG_DW.is_c4_modelCG = modelCG_IN_WARNING_LEFT;

        /* Outport: '<Root>/S_LPA_Active' */
        modelCG_Y.S_LPA_Active = true;

        /* Outport: '<Root>/S_Warning_Left' */
        modelCG_Y.S_Warning_Left = true;

        /* Outport: '<Root>/S_Warning_Right' */
        modelCG_Y.S_Warning_Right = false;

        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 5.0;
      } else {
        tmp = (modelCG_B.drift_right && tmp);
        if (tmp && (modelCG_DW.warningRightCnt >= (int32_T)
                    modelCG_VALIDATION_FRAMES)) {
          modelCG_DW.is_c4_modelCG = modelCG_IN_WARNING_RIGHT;

          /* Outport: '<Root>/S_LPA_Active' */
          modelCG_Y.S_LPA_Active = true;

          /* Outport: '<Root>/S_Warning_Left' */
          modelCG_Y.S_Warning_Left = false;

          /* Outport: '<Root>/S_Warning_Right' */
          modelCG_Y.S_Warning_Right = true;

          /* Outport: '<Root>/S_Cause' */
          modelCG_Y.S_Cause = 5.0;
        } else if ((!rtb_cameraOK) || (!modelCG_B.isValid) ||
                   (modelCG_B.confidence < modelCG_DW.CONF_MIN) ||
                   (modelCG_U.E_speed <= modelCG_SPEED_MIN) ||
                   (!modelCG_U.E_LPA_Act)) {
          modelCG_DW.is_c4_modelCG = modelCG_IN_INACTIVE;

          /* Outport: '<Root>/S_LPA_Active' */
          modelCG_Y.S_LPA_Active = false;

          /* Outport: '<Root>/S_Warning_Left' */
          modelCG_Y.S_Warning_Left = false;

          /* Outport: '<Root>/S_Warning_Right' */
          modelCG_Y.S_Warning_Right = false;
          modelCG_DW.warningLeftCnt = 0U;
          modelCG_DW.warningRightCnt = 0U;
        } else {
          /* Outport: '<Root>/S_LPA_Active' */
          modelCG_Y.S_LPA_Active = true;

          /* Outport: '<Root>/S_Cause' */
          modelCG_Y.S_Cause = 5.0;
          if (tmp_0) {
            if (modelCG_DW.warningLeftCnt < 255) {
              modelCG_DW.warningLeftCnt++;
            }
          } else {
            modelCG_DW.warningLeftCnt = 0U;
          }

          if (tmp) {
            if (modelCG_DW.warningRightCnt < 255) {
              modelCG_DW.warningRightCnt++;
            }
          } else {
            modelCG_DW.warningRightCnt = 0U;
          }
        }
      }
      break;

     case modelCG_IN_INACTIVE:
      /* Outport: '<Root>/S_Warning_Left' */
      modelCG_Y.S_Warning_Left = false;

      /* Outport: '<Root>/S_Warning_Right' */
      modelCG_Y.S_Warning_Right = false;
      if (modelCG_U.E_LPA_Act && (modelCG_U.E_speed > modelCG_SPEED_MIN) &&
          rtb_cameraOK && modelCG_B.isValid && (modelCG_B.confidence >=
           modelCG_DW.CONF_MIN)) {
        modelCG_DW.is_c4_modelCG = modelCG_IN_ACTIVE;

        /* Outport: '<Root>/S_LPA_Active' */
        modelCG_Y.S_LPA_Active = true;

        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 5.0;
      } else if (!rtb_cameraOK) {
        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 2.0;
      } else if (!modelCG_U.E_LPA_Act) {
        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 0.0;
      } else if (modelCG_U.E_speed <= modelCG_SPEED_MIN) {
        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 1.0;
      } else if (modelCG_B.confidence < modelCG_DW.CONF_MIN) {
        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 4.0;
      } else if (!modelCG_B.isValid) {
        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 3.0;
      } else {
        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 6.0;
      }
      break;

     case modelCG_IN_WARNING_LEFT:
      /* Outport: '<Root>/S_Warning_Left' */
      modelCG_Y.S_Warning_Left = true;

      /* Outport: '<Root>/S_Warning_Right' */
      modelCG_Y.S_Warning_Right = false;
      if ((modelCG_DW.prevOffset_DSTATE >= -30.0) || modelCG_U.E_turnSignal) {
        modelCG_DW.is_c4_modelCG = modelCG_IN_ACTIVE;

        /* Outport: '<Root>/S_LPA_Active' */
        modelCG_Y.S_LPA_Active = true;

        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 5.0;
      } else if ((!rtb_cameraOK) || (!modelCG_B.isValid) || (modelCG_U.E_speed <=
                  modelCG_SPEED_MIN) || (!modelCG_U.E_LPA_Act) ||
                 (modelCG_B.confidence < modelCG_DW.CONF_MIN)) {
        modelCG_DW.is_c4_modelCG = modelCG_IN_INACTIVE;

        /* Outport: '<Root>/S_LPA_Active' */
        modelCG_Y.S_LPA_Active = false;

        /* Outport: '<Root>/S_Warning_Left' */
        modelCG_Y.S_Warning_Left = false;
        modelCG_DW.warningLeftCnt = 0U;
        modelCG_DW.warningRightCnt = 0U;
      }
      break;

     default:
      /* Outport: '<Root>/S_Warning_Left' */
      /* case IN_WARNING_RIGHT: */
      modelCG_Y.S_Warning_Left = false;

      /* Outport: '<Root>/S_Warning_Right' */
      modelCG_Y.S_Warning_Right = true;
      if ((!rtb_cameraOK) || (!modelCG_B.isValid) || (modelCG_U.E_speed <=
           modelCG_SPEED_MIN) || (!modelCG_U.E_LPA_Act) || (modelCG_B.confidence
           < modelCG_DW.CONF_MIN)) {
        modelCG_DW.is_c4_modelCG = modelCG_IN_INACTIVE;

        /* Outport: '<Root>/S_LPA_Active' */
        modelCG_Y.S_LPA_Active = false;

        /* Outport: '<Root>/S_Warning_Right' */
        modelCG_Y.S_Warning_Right = false;
        modelCG_DW.warningLeftCnt = 0U;
        modelCG_DW.warningRightCnt = 0U;
      } else if ((modelCG_DW.prevOffset_DSTATE <= modelCG_WARNING_THRESHOLD) ||
                 modelCG_U.E_turnSignal) {
        modelCG_DW.is_c4_modelCG = modelCG_IN_ACTIVE;

        /* Outport: '<Root>/S_LPA_Active' */
        modelCG_Y.S_LPA_Active = true;

        /* Outport: '<Root>/S_Cause' */
        modelCG_Y.S_Cause = 5.0;
      }
      break;
    }
  }

  /* End of Chart: '<Root>/Chart' */
}

/* Model initialize function */
void modelCG_initialize(void)
{
  /* SystemInitialize for Chart: '<Root>/Chart' */
  modelCG_DW.CONF_MIN = 40.0;
}

/* Model terminate function */
void modelCG_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
