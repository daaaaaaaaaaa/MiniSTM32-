//HCSR04������ģ��
//TRIG��PB.8
//Echo��PB.9
//VCC��5V
//GND�ӵ�
#include "stm32f10x.h"
#include "ultrasonic.h"

u16 overflow = 0, value;//��¼��ʱ����������ͼ�������ֵ


//��ʼ����ʱ��TIM2�Լ����GPIO����
void ultrasonic_init(void)
{
  GPIO_InitTypeDef GPIO_IS;//�����ṹ�����
  TIM_TimeBaseInitTypeDef TIM_IS;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ������ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  //����TRIG��������
  GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_IS.GPIO_Mode  = GPIO_Mode_Out_PP;//�������ģʽ
  GPIO_IS.GPIO_Pin   = GPIO_Pin_8;
  GPIO_Init(GPIOB,&GPIO_IS);//��ʼ��PB.8
  
  //����Echo�����źŵ�����
  GPIO_IS.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_IS.GPIO_Pin  = GPIO_Pin_9;
  GPIO_Init(GPIOB,&GPIO_IS);//����PB.9
  
  //��ʼ����ʱ��2
  TIM_DeInit(TIM2);
  TIM_IS.TIM_Period = 999;//��ʱ����Ϊ1000��
  TIM_IS.TIM_Prescaler = 71;//��Ƶϵ��72����Ƶ��Ϊ1Mhz����ʱ����������Ϊ1us
  TIM_IS.TIM_ClockDivision = TIM_CKD_DIV1;//����Ƶ
  TIM_IS.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2,&TIM_IS);
  
  TIM_ClearFlag(TIM2,TIM_FLAG_Update);//������������жϣ�����һ�򿪶�ʱ�������ж�
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//TIM2���������жϣ�1000*(1us)=1ms,Ҳ���Ƕ�ʱ�������ж�һ�ε�ʱ��Ϊ1ms
  
  //����TIM2�ж����ȼ���
  NVIC_InitTypeDef NVIC_IS;//�����ṹ�����
  
 // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ������2
  
  NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;//��ֵԽС���ȼ�Խ��
  NVIC_IS.NVIC_IRQChannelSubPriority = 0;
  NVIC_IS.NVIC_IRQChannel = TIM2_IRQn;//ͨ��TIM2
  NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_IS);//��ʼ������Ĳ���
  
  TIM_Cmd(TIM2,DISABLE);//�رն�ʱ��ʹ�ܣ������⵽�ź�ʱ�ٿ�
}

void TIM2_IRQHandler(void)//�������źŹ��������������жϣ�����ֵ������ظ���������overflow�������������
{
  if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//����жϱ�־
	overflow++;//�жϴ���+1
  }
}

float distance1;
float calculate_distance(void)
{
    PBout(8) = 1;//���ߵ�ƽ����ΪTRIG�����ź�
    delay_us(11); //�ߵ�ƽ�ź�ʱ�䳬��10us
    PBout(8) = 0;
    //�ȴ�Echo����
    while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == RESET);//ע������ֺţ�
   
    TIM_Cmd(TIM2, ENABLE);//���ܴ������źţ�����TIM2����
  
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == SET);//�����ź���ʧ��ͬ��ע������ֺţ�
	TIM_Cmd(TIM2,DISABLE); // �رն�ʱ��
	value = TIM_GetCounter(TIM2); //��ȡTIM2���Ĵ����ļ���ֵ
	
	distance1 = (value + overflow*1000) * 0.017 ;  //����һ�ξ���
    TIM2->CNT = 0;//��TIM2����Ĵ����ļ���ֵ��0
	overflow = 0;//�ж���������0
  return distance1; 
}
