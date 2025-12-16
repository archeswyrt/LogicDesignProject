/*
 * hc_sr04.c
 *
 *  Created on: Dec 15, 2025
 *      Author: Nam Truong
 */



#include "sensor/hc_sr04.h"

extern TIM_HandleTypeDef htim8;

static object_state_t obj_state = OBJECT_ABSENT;
static object_state_t last_obj_state = OBJECT_ABSENT;

static uint8_t last_distance = 0;

Sensor_t sensors[N_sensors] = {
	{GPIOA, GPIO_PIN_1,  &htim8, TIM_CHANNEL_1}
};

// ======================= CALLBACK ===========================
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // 1) Map htim->Channel (HAL_TIM_ACTIVE_CHANNEL_x) → TIM_CHANNEL_x
    uint32_t active_channel = 0;

    switch(htim->Channel)
    {
        case HAL_TIM_ACTIVE_CHANNEL_1: active_channel = TIM_CHANNEL_1; break;
        case HAL_TIM_ACTIVE_CHANNEL_2: active_channel = TIM_CHANNEL_2; break;
        case HAL_TIM_ACTIVE_CHANNEL_3: active_channel = TIM_CHANNEL_3; break;
        case HAL_TIM_ACTIVE_CHANNEL_4: active_channel = TIM_CHANNEL_4; break;
        default: return;    // Không khớp thì thoát
    }

    // 2) Tìm sensor tương ứng
    for(int i = 0; i < 1; i++)
    {
        Sensor_t *s = &sensors[i];

        // so sánh timer và channel
        if(htim == s->htim && active_channel == s->TIM_Channel)
        {
            if(s->Is_First_Captured == 0)
            {
                s->IC_Value1 = HAL_TIM_ReadCapturedValue(htim, s->TIM_Channel);
                s->Is_First_Captured = 1;

                // đổi sang bắt FALLING
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, s->TIM_Channel, TIM_INPUTCHANNELPOLARITY_FALLING);
            }
            else
            {
                s->IC_Value2 = HAL_TIM_ReadCapturedValue(htim, s->TIM_Channel);
                __HAL_TIM_SET_COUNTER(htim, 0);

                if(s->IC_Value2 >= s->IC_Value1)
                    s->Difference = s->IC_Value2 - s->IC_Value1;
                else
                    s->Difference = (0xFFFF - s->IC_Value1) + s->IC_Value2;

                s->Distance = s->Difference * 0.034f / 2.0f;
                s->Is_First_Captured = 0;

                // Reset polarity về RISING để chuẩn bị cho lần đo mới
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, s->TIM_Channel, TIM_INPUTCHANNELPOLARITY_RISING);

                // disable interrupt (logic gốc của bạn)
                uint32_t it = 0;
                switch(s->TIM_Channel)
                {
                    case TIM_CHANNEL_1: it = TIM_IT_CC1; break;
                    case TIM_CHANNEL_2: it = TIM_IT_CC2; break;
                    case TIM_CHANNEL_3: it = TIM_IT_CC3; break;
                    case TIM_CHANNEL_4: it = TIM_IT_CC4; break;
                }
                __HAL_TIM_DISABLE_IT(htim, it);
            }
        }
    }
}




void delay(uint16_t time){
	__HAL_TIM_SET_COUNTER(&htim8, 0);
	while(__HAL_TIM_GET_COUNTER(&htim8) < time);
}

uint8_t HCSR04_GetDis(uint8_t id)
{
	//__HAL_TIM_SET_COUNTER(&htim1, 0);
	Sensor_t *s = &sensors[id];

	HAL_GPIO_WritePin(s->TRIG_Port, s->TRIG_Pin, GPIO_PIN_SET); // pull trig pin high
	delay(10); //delay
	HAL_GPIO_WritePin(s->TRIG_Port, s->TRIG_Pin, GPIO_PIN_RESET); // pull trig pin low

	//__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
	//HAL_Delay(60);
	uint32_t it = 0;
	if (s->TIM_Channel == TIM_CHANNEL_1) it = TIM_IT_CC1;
	if (s->TIM_Channel == TIM_CHANNEL_2) it = TIM_IT_CC2;
	if (s->TIM_Channel == TIM_CHANNEL_3) it = TIM_IT_CC3;
	if (s->TIM_Channel == TIM_CHANNEL_4) it = TIM_IT_CC4;

	__HAL_TIM_ENABLE_IT(s->htim, it);
	return (uint8_t)s->Distance;
}

uint8_t fsm_hcsr04_reading()
{
    uint8_t dis = HCSR04_GetDis(0);

    last_distance = dis;

    /* Update current object state with hysteresis */
    if (dis < ENTER_DIST_CM)
    {
        obj_state = OBJECT_PRESENT;
    }
    else if (dis > EXIT_DIST_CM)
    {
        obj_state = OBJECT_ABSENT;
    }
    /* else: keep previous state */
    return dis;
}


int is_object_arrived(void)
{
    int arrived = 0;

    if (obj_state == OBJECT_PRESENT &&
        last_obj_state == OBJECT_ABSENT)
    {
        arrived = 1;
    }

    /* Latch state AFTER checking */
    last_obj_state = obj_state;

    return arrived;
}

int is_object_present(void)
{
	return (obj_state == OBJECT_PRESENT);
}
