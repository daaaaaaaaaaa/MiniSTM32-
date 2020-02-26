#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "stm32f10x_tim.h"
#include "delay.h"
#include "sys.h"


void ultrasonic_init(void);
float calculate_distance(void);
void TIM2_IRQHandler(void);

#endif


