/*
 * hc_sr04.h
 *
 *  Created on: Dec 15, 2025
 *      Author: Nam Truong
 */

#ifndef MODULES_SENSOR_HC_SR04_H_
#define MODULES_SENSOR_HC_SR04_H_


#include "stm32f4xx_hal.h"

#define N_sensors

#define ENTER_DIST_CM   5
#define EXIT_DIST_CM    7   // must be > ENTER_DIST_CM

#define OBJECT_ARRIVED is_object_arrived()

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

typedef enum {
    OBJECT_ABSENT = 0,
    OBJECT_PRESENT
} object_state_t;


extern Sensor_t sensors[1];


uint8_t HCSR04_GetDis(uint8_t id);

uint8_t fsm_hcsr04_reading();

int is_object_arrived(void);
int is_object_present(void);
#endif /* MODULES_SENSOR_HC_SR04_H_ */
