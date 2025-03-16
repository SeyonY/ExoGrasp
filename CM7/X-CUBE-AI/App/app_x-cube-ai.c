
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\[user_name]\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "network.h"
#include "network_data.h"

/* USER CODE BEGIN includes */
/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/

#if !defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_in_1[AI_NETWORK_IN_1_SIZE_BYTES];
ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
data_in_1
};
#else
ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
NULL
};
#endif

#if !defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_out_1[AI_NETWORK_OUT_1_SIZE_BYTES];
AI_ALIGNED(4) ai_i8 data_out_2[AI_NETWORK_OUT_2_SIZE_BYTES];
ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
data_out_1,
data_out_2
};
#else
ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
NULL,
NULL
};
#endif

/* Activations buffers -------------------------------------------------------*/

AI_ALIGNED(32)
static uint8_t pool0[AI_NETWORK_DATA_ACTIVATION_1_SIZE];

ai_handle data_activations0[] = {pool0};

/* AI objects ----------------------------------------------------------------*/

static ai_handle network = AI_HANDLE_NULL;

static ai_buffer* ai_input;
static ai_buffer* ai_output;

static void ai_log_err(const ai_error err, const char *fct)
{
  /* USER CODE BEGIN log */
  if (fct)
    printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
        err.type, err.code);
  else
    printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);

  do {} while (1);
  /* USER CODE END log */
}

static int ai_boostrap(ai_handle *act_addr)
{
  ai_error err;

  /* Create and initialize an instance of the model */
  err = ai_network_create_and_init(&network, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    ai_log_err(err, "ai_network_create_and_init");
    return -1;
  }

  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);

#if defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-inputs" option is used, memory buffer can be
   *  used from the activations buffer. This is not mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
	data_ins[idx] = ai_input[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
	  ai_input[idx].data = data_ins[idx];
  }
#endif

#if defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-outputs" option is used, memory buffer can be
   *  used from the activations buffer. This is no mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
	data_outs[idx] = ai_output[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
	ai_output[idx].data = data_outs[idx];
  }
#endif

  return 0;
}

static int ai_run(void)
{
  ai_i32 batch;

  batch = ai_network_run(network, ai_input, ai_output);
  if (batch != 1) {
    ai_log_err(ai_network_get_error(network),
        "ai_network_run");
    return -1;
  }

  return 0;
}

/* USER CODE BEGIN 2 */
/* --- Feature Extraction Helper Functions --- */
static double compute_mean(uint32_t *data, int n) {
  double sum = 0.0;
  for (int i = 0; i < n; i++) {
    sum += data[i];
  }
  return sum / n;
}

static double compute_variance(uint32_t *data, int n, double mean) {
  double sum_sq = 0.0;
  for (int i = 0; i < n; i++) {
    double diff = data[i] - mean;
    sum_sq += diff * diff;
  }
  return sum_sq / n;
}

static double compute_rms(uint32_t *data, int n) {
  double sum_sq = 0.0;
  for (int i = 0; i < n; i++) {
    sum_sq += data[i] * data[i];
  }
  return sqrt(sum_sq / n);
}

static double compute_waveform_length(uint32_t *data, int n) {
  double wl = 0.0;
  for (int i = 1; i < n; i++) {
    wl += fabs((double)data[i] - (double)data[i-1]);
  }
  return wl;
}

static int compute_slope_sign_changes(uint32_t *data, int n) {
  int count = 0;
  if (n < 3)
    return 0;
  int d_prev = (int)data[1] - (int)data[0];
  for (int i = 1; i < n - 1; i++) {
    int d = (int)data[i+1] - (int)data[i];
    if (d_prev * d < 0)
      count++;
    d_prev = d;
  }
  return count;
}

static double compute_mean_absolute_value(uint32_t *data, int n) {
  double sum = 0.0;
  for (int i = 0; i < n; i++) {
    sum += fabs((double)data[i]);
  }
  return sum / n;
}

/* --- Acquire ADC Data, Extract Features, and Fill the Input Buffer --- */
int acquire_and_process_data(ai_i8* data[])
{
  /* fill the inputs of the c-model
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++ )
  {
      data[idx] = ....
  }

  */

// Extract ADC values into two channels from adc_buffer (size 800).
  // According to:
  //  - adc_buffer[0], adc_buffer[4], adc_buffer[8], ... -> channel 1 (200 samples)
  //  - adc_buffer[1], adc_buffer[5], adc_buffer[9], ... -> channel 2 (200 samples)
  uint32_t adc_channel1[200];
  uint32_t adc_channel2[200];
  int idx1 = 0, idx2 = 0;
  for (int i = 0; i < 800; i++) {
	if ((i % 4) == 0 && idx1 < 200) {
	  adc_channel1[idx1++] = adc_buffer[i];
	} else if ((i % 4) == 1 && idx2 < 200) {
	  adc_channel2[idx2++] = adc_buffer[i];
	}

  }
  // is butterworth filter needed?

  // Compute six time-domain features for channel 1.
  double mean_ch1 = compute_mean(adc_channel1, 200);
  double var_ch1  = compute_variance(adc_channel1, 200, mean_ch1);
  double rms_ch1  = compute_rms(adc_channel1, 200);
  double wl_ch1   = compute_waveform_length(adc_channel1, 200);
  int    ssc_ch1  = compute_slope_sign_changes(adc_channel1, 200);
  double mav_ch1  = compute_mean_absolute_value(adc_channel1, 200);

  // Compute six time-domain features for channel 2.
  double mean_ch2 = compute_mean(adc_channel2, 200);
  double var_ch2  = compute_variance(adc_channel2, 200, mean_ch2);
  double rms_ch2  = compute_rms(adc_channel2, 200);
  double wl_ch2   = compute_waveform_length(adc_channel2, 200);
  int    ssc_ch2  = compute_slope_sign_changes(adc_channel2, 200);
  double mav_ch2  = compute_mean_absolute_value(adc_channel2, 200);

  // Create a 12-dimensional feature vector.
  double feature_vector[12];
  feature_vector[0]  = mean_ch1;
  feature_vector[1]  = var_ch1;
  feature_vector[2]  = rms_ch1;
  feature_vector[3]  = wl_ch1;
  feature_vector[4]  = (double)ssc_ch1;
  feature_vector[5]  = mav_ch1;
  feature_vector[6]  = mean_ch2;
  feature_vector[7]  = var_ch2;
  feature_vector[8]  = rms_ch2;
  feature_vector[9]  = wl_ch2;
  feature_vector[10] = (double)ssc_ch2;
  feature_vector[11] = mav_ch2;

  double feature_mean[12] = {14083.489580, 1181289.672021, 14109.204677, 4821.640164, 12.109226, 14083.489580, 12961.410092, 1726933.886649, 12991.066218, 6410.065705, 13.928869, 12961.410092};
  double feature_std[12] = {14487.636073, 6659637.014818, 14503.375841, 4027.220441, 3.414121, 14487.636073, 16950.591105, 19153639.885626, 16978.805106, 4678.975665, 2.954818, 16950.591105};

  // Scaling factor to convert standardized values into the int8 range.
  // For example, if the standardized features lie roughly in [-1, 1],
  // multiplying by 128 maps them to the int8 range (-128 to 127).
  double scale_factor = 128.0;

  // Apply standardization and quantization. Then form 12-input vector.
  for (int j = 0; j < 12; j++) {
    double standardized = (feature_vector[j] - feature_mean[j]) / feature_std[j];
    int8_t quantized = (int8_t)(standardized * scale_factor);
    ((int8_t*)data_ins[0])[j] = quantized;
  }
  return 0;
}

int post_process(ai_i8* data[])
{
  /* process the predictions
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++ )
  {
      data[idx] = ....
  }

  */
  // When the network was converted from ONNX using STM32Cube.AI,
  // it created two outputs: "output_label" and "output_probability."
  // In the generated code, these outputs are assigned to data_outs[0] and data_outs[1] respectively.
  // The label is stored in data_outs[0] as an int8_t.
  int8_t predicted_label = *((int8_t*)data_outs[0]);

  // Map the predicted label to the corresponding state.
  if (predicted_label == 0) {
	printf("Predicted State: open\r\n");
	state = OPEN;
  } else if (predicted_label == 1) {
	printf("Predicted State: closed\r\n");
	state = CLOSED;
  } else {
	printf("Predicted State: Unknown (label %d)\r\n", predicted_label);
  }

  return 0;
}
/* USER CODE END 2 */

/* Entry points --------------------------------------------------------------*/

void MX_X_CUBE_AI_Init(void)
{
    /* USER CODE BEGIN 5 */
  printf("\r\nTEMPLATE - initialization\r\n");

  ai_boostrap(data_activations0);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
  int res = -1;

  printf("TEMPLATE - run - main loop\r\n");

  if (network) {

    do {
      /* 1 - acquire and pre-process input data */
      res = acquire_and_process_data(data_ins);
      /* 2 - process the data - call inference engine */
      if (res == 0)
        res = ai_run();
      /* 3- post-process the predictions */
      if (res == 0)
        res = post_process(data_outs);
    } while (res==0);
  }

  if (res) {
    ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
    ai_log_err(err, "Process has FAILED");
  }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
