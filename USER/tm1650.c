#include "tm1650.h" 

const unsigned char chipsda[5]={GPIO_PIN_7,GPIO_PIN_6,GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_3};
const unsigned char showaddr[4]={0x68,0x6a,0x6c,0x6e};

void Delay_us(unsigned int i) //nus延时 
{ 
      for(;i>0;i--) 
            nop(); 
} 

//---------开始信号------------------
void I2CStart(unsigned char chipn)
{
  GPIO_WriteHigh(GPIOD,T1650_CLK);//给芯片发送开始信号   
  GPIO_WriteHigh(GPIOC,chipsda[chipn]);
  Delay_us(64);
  GPIO_WriteLow(GPIOC,chipsda[chipn]);
  Delay_us(64);
 // GPIO_WriteLow(GPIOD,T1650_CLK);  
} 
//---------结束信号-------------------
void I2CStop(unsigned char chipn)
{ 
  GPIO_WriteLow(GPIOD,T1650_CLK); 
  Delay_us(64); 
  GPIO_WriteLow(GPIOC,chipsda[chipn]);
  Delay_us(64);
  GPIO_WriteHigh(GPIOD,T1650_CLK);
  Delay_us(64);
  GPIO_WriteHigh(GPIOC,chipsda[chipn]);
  Delay_us(4);
}
///============================================= 
void I2Cask(unsigned char chipn)//1650应答 
{  
   GPIO_WriteLow(GPIOD,T1650_CLK);    
   GPIO_Init(GPIOC,chipsda[chipn],GPIO_MODE_IN_FL_NO_IT); 
   Delay_us(64);
    while(GPIO_ReadInputPin(GPIOC,chipsda[chipn]));//在第八个时钟下降沿之后延时 5us，开始判断 ACK 信号     
    
   GPIO_WriteHigh(GPIOD,T1650_CLK);
   Delay_us(64);
   GPIO_WriteLow(GPIOD,T1650_CLK);
   Delay_us(64);  
  GPIO_Init(GPIOC,chipsda[chipn],GPIO_MODE_OUT_OD_HIZ_SLOW);
}  
///========================================

//----------给IC写数据
void I2CWrByte(uchar wr_data,unsigned char chipn)
{    
  unsigned char i;
  
   for(i=0;i<8;i++)//开始传送8位数据，每循环一次传送一位数据
   {	
         GPIO_WriteLow(GPIOD,T1650_CLK);
	if(wr_data&0x80)   GPIO_WriteHigh(GPIOC,chipsda[chipn]);
        else               GPIO_WriteLow(GPIOC,chipsda[chipn]);
        Delay_us(32);
        wr_data <<= 1;//移位数据，高位在前!		
        GPIO_WriteHigh(GPIOD,T1650_CLK);
	Delay_us(64);
        //GPIO_WriteLow(GPIOD,T1650_CLK);
    }  
  
 } 		 //8位数据传送完1
///////////////////////////////////////////
void led_off(unsigned char chipn)
{
   I2CStart(chipn);   
   I2CWrByte(0x48,chipn); //数据设置
   I2Cask(chipn);   
   I2CWrByte(0x10,chipn); // 关显示
   I2Cask(chipn); 
   I2CStop(chipn);  
}
void led_on(unsigned char chipn)
{
  I2CStart(chipn);  
  I2CWrByte(0x48,chipn); //数据设置
  I2Cask(chipn);  
  I2CWrByte(0x41,chipn); //6级亮度,开显示 //0x71,7级;//0x01,8级
  I2Cask(chipn);
  I2CStop(chipn);
}
//----------显示子函数------------
void xianshi(uchar digit,uchar data,unsigned char chipn)
{
  unsigned char i;  
  I2CStart(chipn); 
  I2CWrByte(showaddr[digit],chipn); // 地址 0x68 0x6a  0x6c 0x6e
  I2Cask(chipn); 
  I2CWrByte(data,chipn);  // 显示数据
  I2Cask(chipn); 
  I2CStop(chipn); 
}


