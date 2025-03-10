/*
 * controller.h
 *
 *  Created on: Feb 17, 2025
 *      Author: seyon
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_
#include "main.h"

void pressureController(float *pressure, float *target_pressure, float *min_pressure, handState_t state);
#endif /* INC_CONTROLLER_H_ */
