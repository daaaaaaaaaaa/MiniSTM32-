#include "oled.h"
#include "ultrasonic.h"
#include "timer.h"

float distance;
u16 pwmval=  9000;//����PWM��ռ�ձ�

int main(void)
{
    delay_init();	    	 //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    OLED_Clear();
	OLED_Init();
    ultrasonic_init();
    TIM3_PWM_Init(17999,0);//����ARR��ֵΪ18000��72M/18000=4000������PWM��Ƶ��Ϊ4000��������Դ��������������2k~5k��

    while(1)
    {
        distance = calculate_distance();//��ȡ����ֵ
        OLED_ShowNum(50,3,distance,4,16);//��ʾ����ֵ
        delay_ms(100);
        if (distance < 10)
            TIM_SetCompare2(TIM3,pwmval);
        else
            TIM_SetCompare2(TIM3,0);

    }

}

