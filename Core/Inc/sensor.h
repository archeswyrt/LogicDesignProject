/*
 * sensor.h
 *
 *  Created on: Nov 29, 2025
 *      Author: DELL
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "stm32f4xx_hal.h"

typedef struct {
    GPIO_TypeDef *TRIG_Port;
    uint16_t TRIG_Pin;

    TIM_HandleTypeDef *htim;
    uint32_t TIM_Channel;

    uint32_t IC_Value1;
    uint32_t IC_Value2;
    uint32_t Difference;
    uint8_t Is_First_Captured;

    float Distance;
} Sensor_t;

extern Sensor_t sensors[4];

uint8_t HCSR04_GetDis(uint8_t id);

#endif /* INC_SENSOR_H_ */
