#ifndef __STM8S_MAIN
#define __STM8S_MAIN
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80


#include "tm1650.h" 
unsigned int  systime=0;
const unsigned char  lednest[12]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6f,0x00,0xff};

void  uart_send_str (unsigned char *SendData);
void  sysclk_ini(void);
void  sysio_ini(void);
void  tmi1_ini(void);
void  Uart_Init(void);
#endif
