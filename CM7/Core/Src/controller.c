/*
 * controller.c
 *
 *  Created on: Feb 17, 2025
 *      Author: seyon
 */

#include "controller.h"


void pressureController(float *pressure, float *target_pressure, float* min_pressure, handState_t state) {
	switch (state) {
		case OPEN:
			HAL_GPIO_WritePin(THUMB_GPIO_Port, THUMB_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(INDEX_GPIO_Port, INDEX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(OTHER_SOL_GPIO_Port, OTHER_SOL_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, GPIO_PIN_RESET);
			break;
		case CLOSED:
			HAL_GPIO_WritePin(THUMB_GPIO_Port, THUMB_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(INDEX_GPIO_Port, INDEX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(OTHER_SOL_GPIO_Port, OTHER_SOL_Pin, GPIO_PIN_SET);

			if (*pressure > *target_pressure)
				HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, GPIO_PIN_RESET);
			else if (*pressure < *min_pressure)
				HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, GPIO_PIN_SET);
			break;
		default:
			break;
	}
}


