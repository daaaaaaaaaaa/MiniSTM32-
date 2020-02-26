#include "oled.h"
#include "ultrasonic.h"
#include "timer.h"

float distance;
u16 pwmval=  9000;//设置PWM的占空比

int main(void)
{
    delay_init();	    	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    OLED_Clear();
	OLED_Init();
    ultrasonic_init();
    TIM3_PWM_Init(17999,0);//设置ARR的值为18000，72M/18000=4000，即是PWM的频率为4000，符合无源蜂鸣器的条件（2k~5k）

    while(1)
    {
        distance = calculate_distance();//获取距离值
        OLED_ShowNum(50,3,distance,4,16);//显示距离值
        delay_ms(100);
        if (distance < 10)
            TIM_SetCompare2(TIM3,pwmval);
        else
            TIM_SetCompare2(TIM3,0);

    }

}

