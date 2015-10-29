#include "tm1650.h" 

const unsigned char chipsda[5]={GPIO_PIN_7,GPIO_PIN_6,GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_3};
const unsigned char showaddr[4]={0x68,0x6a,0x6c,0x6e};

void Delay_us(unsigned int i) //nus��ʱ 
{ 
      for(;i>0;i--) 
            nop(); 
} 

//---------��ʼ�ź�------------------
void I2CStart(unsigned char chipn)
{
  GPIO_WriteHigh(GPIOD,T1650_CLK);//��оƬ���Ϳ�ʼ�ź�   
  GPIO_WriteHigh(GPIOC,chipsda[chipn]);
  Delay_us(64);
  GPIO_WriteLow(GPIOC,chipsda[chipn]);
  Delay_us(64);
 // GPIO_WriteLow(GPIOD,T1650_CLK);  
} 
//---------�����ź�-------------------
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
void I2Cask(unsigned char chipn)//1650Ӧ�� 
{  
   GPIO_WriteLow(GPIOD,T1650_CLK);    
   GPIO_Init(GPIOC,chipsda[chipn],GPIO_MODE_IN_FL_NO_IT); 
   Delay_us(64);
    while(GPIO_ReadInputPin(GPIOC,chipsda[chipn]));//�ڵڰ˸�ʱ���½���֮����ʱ 5us����ʼ�ж� ACK �ź�     
    
   GPIO_WriteHigh(GPIOD,T1650_CLK);
   Delay_us(64);
   GPIO_WriteLow(GPIOD,T1650_CLK);
   Delay_us(64);  
  GPIO_Init(GPIOC,chipsda[chipn],GPIO_MODE_OUT_OD_HIZ_SLOW);
}  
///========================================

//----------��ICд����
void I2CWrByte(uchar wr_data,unsigned char chipn)
{    
  unsigned char i;
  
   for(i=0;i<8;i++)//��ʼ����8λ���ݣ�ÿѭ��һ�δ���һλ����
   {	
         GPIO_WriteLow(GPIOD,T1650_CLK);
	if(wr_data&0x80)   GPIO_WriteHigh(GPIOC,chipsda[chipn]);
        else               GPIO_WriteLow(GPIOC,chipsda[chipn]);
        Delay_us(32);
        wr_data <<= 1;//��λ���ݣ���λ��ǰ!		
        GPIO_WriteHigh(GPIOD,T1650_CLK);
	Delay_us(64);
        //GPIO_WriteLow(GPIOD,T1650_CLK);
    }  
  
 } 		 //8λ���ݴ�����1
///////////////////////////////////////////
void led_off(unsigned char chipn)
{
   I2CStart(chipn);   
   I2CWrByte(0x48,chipn); //��������
   I2Cask(chipn);   
   I2CWrByte(0x10,chipn); // ����ʾ
   I2Cask(chipn); 
   I2CStop(chipn);  
}
void led_on(unsigned char chipn)
{
  I2CStart(chipn);  
  I2CWrByte(0x48,chipn); //��������
  I2Cask(chipn);  
  I2CWrByte(0x41,chipn); //6������,����ʾ //0x71,7��;//0x01,8��
  I2Cask(chipn);
  I2CStop(chipn);
}
//----------��ʾ�Ӻ���------------
void xianshi(uchar digit,uchar data,unsigned char chipn)
{
  unsigned char i;  
  I2CStart(chipn); 
  I2CWrByte(showaddr[digit],chipn); // ��ַ 0x68 0x6a  0x6c 0x6e
  I2Cask(chipn); 
  I2CWrByte(data,chipn);  // ��ʾ����
  I2Cask(chipn); 
  I2CStop(chipn); 
}


