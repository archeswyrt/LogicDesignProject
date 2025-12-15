/*
 * sensor.c
 *
 *  Created on: Nov 29, 2025
 *      Author: DELL
 */

#include "sensor.h"

//extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;

//uint32_t IC_Val1 = 0;
//uint32_t IC_Val2 = 0;
//uint32_t Difference = 0;
//uint8_t Is_First_Captured = 0;  // is the first value captured ?
////uint8_t Distance  = 0;
//float Distance  = 0;
//
//#define TRIG_PIN GPIO_PIN_7
//#define TRIG_PORT GPIOE
//
//
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
//	{
//		if (Is_First_Captured==0) // if the first value is not captured
//		{
//			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
//			Is_First_Captured = 1;  // set the first captured as true
//			// Now change the polarity to falling edge
//			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
//		}
//		else if (Is_First_Captured==1)   // if the first is already captured
//		{
//			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
//			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
//
//			if (IC_Val2 > IC_Val1)
//			{
//				Difference = IC_Val2-IC_Val1;
//			}
//			else if (IC_Val1 > IC_Val2)
//			{
//				Difference = (0xffff - IC_Val1) + IC_Val2;
//			}
//			Distance = Difference * .034 / 2;
//			Is_First_Captured = 0; // set it back to false
//			// set polarity to rising edge
//			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
//			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
//		}
//	}
//}
//
//void delay(uint16_t time){
//	__HAL_TIM_SET_COUNTER(&htim1, 0);
//	while(__HAL_TIM_GET_COUNTER(&htim1) < time);
//}
//
//uint8_t HCSR04_GetDis (void)
//{
//	//__HAL_TIM_SET_COUNTER(&htim1, 0);
//
//	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
//	delay(10);  // wait for 10 us
//	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low
//
//	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
//	//HAL_Delay(60);
//	return Distance;
//}

Sensor_t sensors[4] =
{
    //{GPIOE, GPIO_PIN_7,  &htim1, TIM_CHANNEL_1},
	{GPIOA, GPIO_PIN_1,  &htim8, TIM_CHANNEL_1}
//    {GPIOE, GPIO_PIN_8,  &htim1, TIM_CHANNEL_2},
//    {GPIOE, GPIO_PIN_10, &htim1, TIM_CHANNEL_3},
//    {GPIOE, GPIO_PIN_12, &htim1, TIM_CHANNEL_4}
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
    for(int i = 0; i < 4; i++)
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
