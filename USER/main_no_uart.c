
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h" 
//#include "iostm8s003f3.h"
#include "main.h"

struct COMMCHAR
{
  unsigned char  comheard;
  unsigned char comlong;      
  unsigned char comdata[20];
  unsigned char compra; 
  unsigned char comtime;  
  unsigned char cominlong;  
  unsigned char comcrc;
  
}comrecive1; 

#define   Highlist(x)    ((x-1)*17)

#define   H165_SH     GPIO_PIN_4
#define   H165_CLK    GPIO_PIN_3
#define   H165_DATA   GPIO_PIN_2
/////////////////////main start////////////////////////
int main(void)
{
  sysclk_ini(); 
  sysio_ini();    
 // Uart_Init();
  Light(led_channel);
  tmi1_ini();
 // xianshi(show_led_char); 
  if(unit&0x02) count=con_count;
  else          count=0;
  __enable_interrupt();
  while (1)
  {   
    if(work_state>0x10)  sys1ms=0;
    if(sys1ms)
    {
      --sys1ms;
      Read165(sensor_chk);
      if(led_channel==6) next_channel=0;
      else               next_channel=led_channel+1;
      Light(next_channel);     
      for(unsigned char j=0;j<4;j++)
      {
        if((sensor_chk[j]&(2<<led_channel))==0) 
        {
          temp_have_led[temp_led_n]=7-led_channel+7*j;
          if(++temp_led_n>7)  temp_led_n=7 ;
        }          
      }
      ++led_channel;       
      if(led_channel>6)//||(temp_led_n))
      {        
//        if(++comrecive1.comtime>20)
//        {
//          comrecive1.comtime=0;
//          comrecive1.compra=0;
//        }
          
        if(temp_have_led[0]!=sensor_bk) 
        {
          if(temp_led_n==1)  temp_led_ok=temp_have_led[0];
          else               temp_led_ok=0; 
          norun=0;
        }
        else
        {
          if(++norun>42857)  //10分钟没动作，关显示
          {
            work_state=5;
            led_off();
          }
          temp_led_ok=0; 
        }
        sensor_bk=temp_have_led[0];
        led_channel=0;      
        if(work_state==2)//检测速度
        {
          unsigned int temp_jul;
          if(temp_led_ok) 
          {
            move_stop_time=0;
            if(rev_nn<10) ++rev_nn;
            else       rev_weight=0;    
            now_high=temp_led_ok;//weizhilist[temp_led_ok-1];          
            if((now_high>pro_high)&&(f_sign==0))
            {             
              f_sign=1;    
              julihigh=temp_led_ok;
              pro_time=systime; 
            }
            else  if((now_high<pro_high)&&f_sign)
            {
              
              if(now_time>pro_time) temp_jul=now_time-pro_time;
              else                  temp_jul=65536-pro_time+now_time;              
              speed= (Highlist(pro_high)-Highlist(julihigh))*10/temp_jul;           
              speed=speed%40;
              f_sign =0;
              
              if(unit&0x01)  ++count;
              else         
              {
               if(count>1) --count;
               else        count=con_count;
              }  
            }
            now_time=systime;
            if(pro_high==weight)  treadmil_set=1;//上次回归到了重量设置起始处，判断换稍子用
            else                  treadmil_set=0;
            pro_high=now_high;           
            
            disp_all();              
            
        //   uart_event=1;
            //uart_send_time=500;//uart_send_str (uart_send_buf);  
            nosockt=0;
          }            
          else
          {
            if(temp_led_n==0)
            {
              if((rev_weight)&&(now_high==weight))//2S z之内没有插销，且插销消失前位于移动的最低端
              {
                if(++rev_weight==0)   rev_weight=101;///=2;
              }
              //  else             rev_weight=0;
              move_stop_time=0;
              rev_nn=0;
              if(++nosockt>40000) work_state=0;
            } 
            else if(temp_led_n==1)
            {
              nosockt=0;
              ++move_stop_time;
              if(move_stop_time>698)//在上次重量位置处保持2秒---698---1.1秒--400
              {
                if( weight==temp_have_led[0]) rev_weight=1;// 回到移动的最低端
                else
                {
                  if((rev_weight>100) &&(rev_nn<2)) //没有插销的时间超过0.3秒，且是第一次出现插销
                  {
                    f_sign=0;
                    weight=temp_have_led[0]%25; 
                    speed = 0;
                    
                    disp_all();   
                    
                  }
                  rev_weight=0;
                }
              }
              else if(move_stop_time>9)//30ms 去抖
              {
                unsigned char tempf=0;
                if(temp_have_led[0]<weight)  tempf=1; //如果出现更重的重量，表明刚换了插销
                else 
                {
                  if(treadmil_set&&((temp_have_led[0]-weight)>1)) tempf=1;
                  //如果上次出现在设置的重量处，这次出现在间隔的地方也表明刚换了插销
                }
                if(tempf)
                {
                  weight=temp_have_led[0]%25;                  
                  speed = 0;
                  
                  disp_all();                      
                   
                  rev_weight=0;
                  move_stop_time=0;
                }
              }  
            }
          }
        }
        else  if(work_state==0)  //上电自检传感器
        {
          if((systime%50)==0)
          {
            //unsigned char i;
            if(temp_led_n>1)//出错情况
            {
              errlednum=temp_led_n;              
              dig_16_10(temp_have_led[0],temp_have_led[1]);
              show_led_char[0]=0x79;              
              xianshi(show_led_char);
            }
            else if(temp_led_n==0)//无插销情况
            {
              errlednum=100;               
              show_led_char[0]=0x71;  
              show_led_char[1]=0x40;  
              show_led_char[2]=0x40; 
              show_led_char[3]=0x40; 
              show_led_char[4]=0x40; 
              show_led_char[5]=0x40; 
              show_led_char[5]=unit;
              xianshi(show_led_char); 
            }
            else             
            {
              errlednum=0;     
             // dig_16_10(0,0);
             // xianshi(show_led_char); 
            }
           }
          if((++testtime>200)&&(errlednum==0))//检测正常
          {
            testtime=0;
            weight=temp_have_led[0]%20;//weizhilist[temp_have_led[0]-1];
            now_high=weight;
            work_state=2;  
            
            disp_all();
                           
          }
        } 
               
       temp_led_n=0;
      }
      
    }
    
    if((systime%10)==0)
    {
      key_buf = read( );
      if(key_buf!=0xff)//有按键
      {
        if(key_buf!=key_pro)//新按键
        {
            if(work_state==5)//结束显示休眠
              {
                 work_state=0;
                 led_on();
                 norun=0;
               }
            else
            {
              if(work_state>0x10) //设置状态
              {
                if(key_buf==KEY_CLR_D) //确认键
                {
                  if(work_state==0x11)
                  {
                    unit=set_vol;
                    //show_led_char[5]=unit;
                    flash_flag=2;
                    work_state=0x12; 
                  }
                  else if(work_state==0x12)
                  {
                    unit=set_vol;
                   // show_led_char[5]=unit;
                     if(unit&0x02)//倒计模式，设置倒计数值
                     {
                       set_vol=con_count;
                       flash_flag=3;
                       work_state=0x13;                        
                     }
                     else//正计数，结束设置
                     {
                       flash_flag=0;
                       work_state=0;
                       disp_all();
                     } 
                  }
                  else if(work_state==0x13)//结束设置
                  {
                    con_count=set_vol;
                    flash_flag=0;
                    work_state=0;
                    disp_all();
                  }
                 
                }
                else if(key_buf==KEY_SET) //设置键
                {
                  if(work_state==0x11)
                  {
                    if(set_vol&0x04)  set_vol=(set_vol&0x03)|0x08;//8  千克
                    else              set_vol=(set_vol&0x03)|0x04; //4 磅
                    unit=set_vol; 
                  }
                  else if(work_state==0x12)
                  {
                     if(set_vol&0x01)  set_vol=(set_vol&0x0c)|0x02;//2    倒计模式
                     else              set_vol=(set_vol&0x0c)|0x01; //1   正计模式
                     unit=set_vol; 
                  }
                  else if(work_state==0x13)//设置设置计数
                  {
                    if(set_vol>0) --set_vol;
                   
                  }
                }
              }
              else  //清楚计数
              {
                if(key_buf==KEY_CLR_D)//清楚键
                {
                  if(unit&0x01)  count=0;                                                                                                                                                                                                                                              
                  else           count=con_count;
                  disp_all();                  
                }
              }              
            }
           
           key_time=0;
         }
        else  //长按键  判断
        {
           ++key_time;
           if(key_buf==KEY_SET)//设置键
           {
              if((key_time>200)&&(work_state<0x10))
              {
                work_state=0x11;
                set_vol=unit;
                flash_flag=1;   
                flash_time=0;
              } 
              else if((key_time>100)&&(work_state==0x13))
              {
                if(set_vol>10) set_vol=set_vol-10;
                else           set_vol=95;
                key_time=0;
              }
           } 
        }
      }
     key_pro = key_buf; 
     if(flash_flag)
     {
       ++flash_time;
       show_led_char[0]=0;
       show_led_char[1]=0; 
       show_led_char[2]=0;
       if(flash_time==1)
       {
         if(work_state==0x13)
         {
         show_led_char[5]=unit;
         show_led_char[4]=0;
         show_led_char[3]=0;        
         }
         else 
         {
           if(work_state==0x11)  show_led_char[5]=unit&0x03;
           else                  show_led_char[5]=unit&0x0c;
         }
          xianshi(show_led_char); 
       }
       else if(flash_time==50)
       {
         if(work_state==0x13)
         {
           show_led_char[4]=lednest[set_vol%10];
           show_led_char[3]=lednest[set_vol/10];  
         } 
         else                
         {
           show_led_char[4]=lednest[count%10];
           show_led_char[3]=lednest[count/10];  
         } 
         show_led_char[5]=unit;
         xianshi(show_led_char); 
         
       }
       if(flash_time>100)  flash_time=0;       
       
     }
    }
  }
  
}

/////////按设置格式显示///
//磅，正计数 5      //磅，到计数 6//千克，正计数 9 //千克，到计数 10
void  disp_all(void)//bit4千克  bit3磅    bit2倒计   bit1正计
{
  if(unit<0x07)      dig_16_10((21-weight)*10,count);  //磅
  else                                                //千克                  
   { 
     dig_16_10(weighlist[weight],count); 
     if(weight==20) show_led_char[1]|=0x80;
    }
  show_led_char[5]=unit;
  xianshi(show_led_char);
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
  GPIO_Init(GPIOD,GPIO_PIN_2 ,GPIO_MODE_IN_PU_NO_IT); //data165
  GPIO_Init(GPIOD,GPIO_PIN_3 ,GPIO_MODE_OUT_PP_HIGH_FAST); //clk165
  GPIO_Init(GPIOD,GPIO_PIN_4 ,GPIO_MODE_OUT_PP_HIGH_FAST); //sh165
  GPIO_WriteHigh(GPIOD,H165_SH);
  GPIO_WriteHigh(GPIOD,H165_CLK);
  
  GPIO_DeInit(GPIOC);
  GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);//led
  GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);//c138
  GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_HIGH_FAST);//b138
  GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_FAST);//a138    
  GPIO_WriteLow(GPIOC,GPIO_PIN_4);
  
  GPIO_DeInit(GPIOB);
  GPIO_Init(GPIOB,GPIO_PIN_4 ,GPIO_MODE_OUT_OD_HIZ_SLOW); //clk1637
  GPIO_Init(GPIOB,GPIO_PIN_5 ,GPIO_MODE_OUT_OD_HIZ_SLOW); //sh1637
  GPIO_WriteHigh(GPIOB,T1637_CLK);
  GPIO_WriteHigh(GPIOB,T1637_DATA);
   
   
   
}
//*********************************//2ms
void tmi1_ini(void)
{
  TIM1_TimeBaseInit(8,TIM1_COUNTERMODE_UP,2000,0);//设置分频数，设置定时周期
  TIM1_ARRPreloadConfig(ENABLE);//使能预装载
  TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);//使能中断
  TIM1_Cmd(ENABLE); //开启定时器
}
//*******************************
void dig_16_10(unsigned int k1,unsigned char k2)
{
  unsigned char n;
  n=k1%1000;
  show_led_char[0]=n/100;
  if(show_led_char[0]!=0) show_led_char[0]=lednest[show_led_char[0]];
  n=k1%100;
  show_led_char[1]=lednest[n/10];
  show_led_char[2]=lednest[k1%10];
  
  n=k2%100;
  show_led_char[3]=n/10;
  if(show_led_char[3]!=0) show_led_char[3]=lednest[show_led_char[3]];  
  show_led_char[4]=lednest[k2%10];  
  
}
//*********************
//
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
void uart_send_str (unsigned char *SendData)
{ 
  uart_send=SendData;
  uart_send_length=uart_send_length_bk ;                    // Enable USCI_A0 TX interrupt
  UART1_SendData8(*uart_send);
  UART1_ITConfig(UART1_IT_TC,ENABLE  );
}
//***********点使能第几路红外发射管
void  Light(u8 port_num)
{
  u8  kk,led_light=0;
  if(port_num&BIT0) led_light|=BIT7;
  if(port_num&BIT1) led_light|=BIT6;
  if(port_num&BIT2) led_light|=BIT5;
  kk =GPIO_ReadOutputData(GPIOC)&0x1f;
  GPIO_Write(GPIOC,  kk|led_light); 
}
//****************读取HC165数据
void  Read165(u8 *rdata)
{
  u8 num=0;
  GPIO_WriteLow(GPIOD,H165_SH);
  asm("nop");asm("nop");
  GPIO_WriteHigh(GPIOD,H165_SH);
  asm("nop");
  for(num=0;num<4;num++)
  {
   for(u8 i=0;i<8;i++)
    {
       *(rdata+num)<<=1;
       if(GPIO_ReadInputPin(GPIOD,H165_DATA)) { *(rdata+num) += 1;}
       GPIO_WriteLow(GPIOD,H165_CLK);             
       asm("nop");
       GPIO_WriteHigh(GPIOD,H165_CLK);       //上升沿
       asm("nop");    
    }
  }  
}
///////////////////////////
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
  ++sys1ms;
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
}
//************************************
#pragma vector=0x13
__interrupt void UART1_TX_IRQHandler(void)
{
  if(UART1->SR&BIT6)
  {
   if(--uart_send_length>0)   UART1_SendData8(*(++uart_send));
   else                     
   {
     UART1_ITConfig(UART1_IT_TC,DISABLE);                       // Disable USCI_A0 TX interrupt  
     uart_busy=0;
   }
  }
}
//***********************************
#pragma vector=0x14
__interrupt void UART1_RX_IRQHandler(void)
{ 
  unsigned char rxdbuft; 
  if(UART1_GetITStatus(UART1_IT_RXNE )!= RESET)  //*接收中断(
  {
    rxdbuft=UART1_ReceiveData8();//*(USART1->DR);读取接收到的数据,当读完数据后自动取消RXNE的中断标志位       
    comrecive1.comtime=0; 
    switch (comrecive1.compra)
       {
                case 0:
                if(rxdbuft==0xfe) comrecive1.compra=1;
                break;       
                case 1:           
                if(rxdbuft>15)   comrecive1.compra=0;
                else     
                  {
                    comrecive1.comlong= rxdbuft-1; 
                    comrecive1.compra=2; 
                    comrecive1.cominlong=0;
                    comrecive1.comcrc=rxdbuft;
                    
                  }
                break;
                case 2:
                comrecive1.comdata[comrecive1.cominlong]=rxdbuft;            
                if(++comrecive1.cominlong>= comrecive1.comlong)
                  {
                    
                    if(comrecive1.comcrc == comrecive1.comdata[comrecive1.cominlong-1]);//(comrecive1.comdata[comrecive1.cominlong-2]<<1)) //   if( rxdbuft==0x43) 
                      {
                        if(comrecive1.comdata[2]==1)//取传感器信息
                        {
                          uart_send_cmd[0] = 0xfe;
                          uart_send_cmd[1] = 0x07;
                          uart_send_cmd[2] = 0x00;
                          uart_send_cmd[3] = 0x02;
                          uart_send_cmd[4] =0x01;
                          uart_send_cmd[5] =23;//配置块个数
                          uart_send_cmd[6] =48; // 传感器检测位置点             
                          uart_send_cmd[7] =sum_verify(&uart_send_cmd[1],6);
                          uart_send_cmd[8] =0x45;
                          uart_send_length_bk=9;
                          uart_event=1;
                          uart_busy=1;
                        }
                        if(comrecive1.comdata[2]==2)//接收状态
                        {
                         uart_event=1;
                         
                        }
                       if(comrecive1.comdata[2]==3)//接收处方
                        {
                          uart_send_cmd[0] = 0xfe;
                          uart_send_cmd[1] = 0x07;
                          uart_send_cmd[2] = 0x00;
                          uart_send_cmd[3] = 0x02;
                          uart_send_cmd[4] =0x03;
                          uart_send_cmd[5] =1;//                                 
                          uart_send_cmd[6] =sum_verify(&uart_send_cmd[1],5);
                          uart_send_cmd[7] =0x45;
                          uart_send_length_bk=8;
                          uart_event=1;
                          uart_busy=1;
                         
                        }       
                      }   
                    comrecive1.compra=0;
                    
                  }
                else    comrecive1.comcrc +=rxdbuft;
                break;
                default: break;   
             }     
              
    
     }  
}




