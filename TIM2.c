//HCSR04超声波模块
//TRIG接PB.8
//Echo接PB.9
//VCC接5V
//GND接地
#include "stm32f10x.h"
#include "ultrasonic.h"

u16 overflow = 0, value;//记录定时器溢出次数和计数器的值


//初始化定时器TIM2以及相关GPIO设置
void ultrasonic_init(void)
{
  GPIO_InitTypeDef GPIO_IS;//声明结构体变量
  TIM_TimeBaseInitTypeDef TIM_IS;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能外设时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  //设置TRIG触发引脚
  GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_IS.GPIO_Mode  = GPIO_Mode_Out_PP;//推挽输出模式
  GPIO_IS.GPIO_Pin   = GPIO_Pin_8;
  GPIO_Init(GPIOB,&GPIO_IS);//初始化PB.8
  
  //接受Echo回响信号的引脚
  GPIO_IS.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_IS.GPIO_Pin  = GPIO_Pin_9;
  GPIO_Init(GPIOB,&GPIO_IS);//设置PB.9
  
  //初始化定时器2
  TIM_DeInit(TIM2);
  TIM_IS.TIM_Period = 999;//定时周期为1000；
  TIM_IS.TIM_Prescaler = 71;//分频系数72，则频率为1Mhz，定时器计数基数为1us
  TIM_IS.TIM_ClockDivision = TIM_CKD_DIV1;//不分频
  TIM_IS.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
  TIM_TimeBaseInit(TIM2,&TIM_IS);
  
  TIM_ClearFlag(TIM2,TIM_FLAG_Update);//事先清除更新中断，以免一打开定时器即打开中断
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//TIM2开启更新中断，1000*(1us)=1ms,也就是定时器更新中断一次的时间为1ms
  
  //设置TIM2中断优先级等
  NVIC_InitTypeDef NVIC_IS;//声明结构体变量
  
 // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组分组2
  
  NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;//数值越小优先级越高
  NVIC_IS.NVIC_IRQChannelSubPriority = 0;
  NVIC_IS.NVIC_IRQChannel = TIM2_IRQn;//通道TIM2
  NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_IS);//初始化上面的参数
  
  TIM_Cmd(TIM2,DISABLE);//关闭定时器使能，后面检测到信号时再开
}

void TIM2_IRQHandler(void)//当回响信号过长，触发更新中断，计数值溢出后重复计数，用overflow来保存溢出次数
{
  if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除中断标志
	overflow++;//中断次数+1
  }
}

float distance1;
float calculate_distance(void)
{
    PBout(8) = 1;//拉高电平，作为TRIG触发信号
    delay_us(11); //高电平信号时间超过10us
    PBout(8) = 0;
    //等待Echo回响
    while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == RESET);//注意这个分号；
   
    TIM_Cmd(TIM2, ENABLE);//接受带回响信号，开启TIM2计数
  
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == SET);//回响信号消失，同样注意这个分号；
	TIM_Cmd(TIM2,DISABLE); // 关闭定时器
	value = TIM_GetCounter(TIM2); //获取TIM2数寄存器的计数值
	
	distance1 = (value + overflow*1000) * 0.017 ;  //计算一次距离
    TIM2->CNT = 0;//将TIM2计算寄存器的计数值清0
	overflow = 0;//中断溢出溢出清0
  return distance1; 
}
