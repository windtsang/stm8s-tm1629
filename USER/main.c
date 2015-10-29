
#include "stm8s.h" 
//#include "iostm8s003f3.h"
#include "main.h"
unsigned char boardid;
unsigned char uart_state=0;
unsigned char shown;
unsigned char showdigit;
unsigned char showdata[20];
/////////////////////main start////////////////////////
int main(void)
{
  unsigned char i,j;
  sysclk_ini(); 
  sysio_ini();    
  Uart_Init();  
  tmi1_ini();  
  __enable_interrupt();
  led_on(0);
  led_on(1);
  led_on(2);
  led_on(3);
  led_on(4);
  boardid = GPIO_ReadInputData(GPIOD)&0x18;
  boardid = 3-(boardid>>3);
  for(j=0;j<10;j++)
  {      
    for(i=0;i<20;i++)
    {
      xianshi(i%4,lednest[j],i/4); 
      showdata[i]=10;
    }
    while(systime<150);
    systime=0;
  }
  
  for(i=0;i<20;i++)
  {
    xianshi(i%4,0,i/4);     
  }
  while (1)
  {      
    if(shown)
    {
      for(i=0;i<20;i++)
      {
         xianshi(i%4,lednest[showdata[i]],i/4);
      }
      --shown;
    }
  }  
}

////////////////////////main end///////////////////
void sysclk_ini(void)
{
  CLK_HSECmd(ENABLE);//外部时钟开??
  CLK_LSICmd(ENABLE);//内部低频RC开??
  CLK_HSICmd(ENABLE);//内部高频RC开????
  CLK_ClockSwitchCmd(ENABLE);//切换使能??
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO,CLK_SOURCE_HSE,DISABLE,CLK_CURRENTCLOCKSTATE_DISABLE);//切换配置到外部时钟，关闭原来时钟
  
}
//*********************************//
void  sysio_ini(void)
{
  GPIO_DeInit(GPIOD);
  GPIO_Init(GPIOD,GPIO_PIN_2 ,GPIO_MODE_OUT_PP_HIGH_FAST); //tm1650  scl
  GPIO_Init(GPIOD,GPIO_PIN_3 ,GPIO_MODE_IN_PU_NO_IT); //Id0
  GPIO_Init(GPIOD,GPIO_PIN_4 ,GPIO_MODE_IN_PU_NO_IT); //Id1
  GPIO_WriteHigh(GPIOD,GPIO_PIN_2);
  
  
  GPIO_DeInit(GPIOC);
  GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_OUT_PP_HIGH_FAST);//TM1650   SDA
  GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);// 
  GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);// 
  GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_HIGH_FAST);// 
  GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_FAST);// 
  
  
  GPIO_DeInit(GPIOB);
  GPIO_Init(GPIOB,GPIO_PIN_4 ,GPIO_MODE_OUT_OD_HIZ_SLOW); // 
  GPIO_Init(GPIOB,GPIO_PIN_5 ,GPIO_MODE_OUT_OD_HIZ_SLOW); // 
  
}
//*********************************//2ms
void tmi1_ini(void)
{
  TIM1_TimeBaseInit(8,TIM1_COUNTERMODE_UP,2000,0);//设置分频数，设置定时周期
  TIM1_ARRPreloadConfig(ENABLE);//使能预装载
  TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);//使能中断
  TIM1_Cmd(ENABLE); //开启定时器
}
/**********************************************
UART1  configured as follow:
- BaudRate = 115200 baud  
- Word Length = 8 Bits
- One Stop Bit
- No parity
- Receive and transmit enabled
-  Receive interrupt
- UART1 Clock disabled
*********************************************/
void Uart_Init(void)
{
  UART1_DeInit();
  UART1_Init((u32)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, \
    UART1_PARITY_NO , UART1_SYNCMODE_CLOCK_DISABLE , UART1_MODE_TXRX_ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR,ENABLE  ); 
  UART1_Cmd(ENABLE );
  
}
///////////////////////////
//void uart_send_str (unsigned char *SendData)
//{ 
//  uart_send=SendData;
//  uart_send_length=uart_send_length_bk ;                    // Enable USCI_A0 TX interrupt
//  UART1_SendData8(*uart_send);
//  UART1_ITConfig(UART1_IT_TC,ENABLE  );
//}
////////////////////////////////////////////////
unsigned char sum_verify(unsigned char *datal,unsigned char datan )//计算累加校验
{
  unsigned char i;
  unsigned  int sumhe=0;
  for(i =0; i<datan; i++)  sumhe +=*(datal+i);
  return  (sumhe&0xff);
}
/////////////////////////////
#pragma vector=0x0D
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
  ++systime; 
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
}
////************************************
#pragma vector=0x13
__interrupt void UART1_TX_IRQHandler(void)
{
  if(UART1->SR&BIT6)
  {
    // if(--uart_send_length>0)   UART1_SendData8(*(++uart_send));
    // else                     
    {
      UART1_ITConfig(UART1_IT_TC,DISABLE);                       // Disable USCI_A0 TX interrupt  
      //   uart_busy=0;
    }
  }
}
//***********************************
#pragma vector=0x14
__interrupt void UART1_RX_IRQHandler(void)
{ 
  unsigned char rxdbuft,i; 
  if(UART1_GetITStatus(UART1_IT_RXNE )!= RESET)  //*接收中断(
  {
    rxdbuft=UART1_ReceiveData8();//*(USART1->DR);读取接收到的数据,当读完数据后自动取消RXNE的中断标志位     
    switch (uart_state)
    {
    case 0:
      if(rxdbuft==0xf0)             uart_state=1;
      break;       
    case 1:   
      if(rxdbuft==0xf1)         
      {
        showdigit= 32;// 广播   清零  特殊命令   
        uart_state=2;
      }
      else if((((rxdbuft-8)/20)==boardid)&&(rxdbuft<89))//显示
      {
        uart_state=2;
        showdigit=(rxdbuft-8)%20;
      }                 
      else                            uart_state=0;
      break;
    case 2:
      if(showdigit<20)
      {
        if((rxdbuft==0)||(rxdbuft>9))
           showdata[showdigit]=10;
        else
           showdata[showdigit]=rxdbuft;
        shown++;                 
      }
      else if(showdigit==32)
      {
        for(i=0;i<20;i++)
        {
          showdata[i]=10;                   
        } 
        shown++;     
      }
      uart_state=0;
      break;
    default: break;   
    }     
    
    
  }  
}

