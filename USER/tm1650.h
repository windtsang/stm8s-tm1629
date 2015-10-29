#ifndef __STM8S_T
#define __STM8S_T
#include "stm8s.h" 

 
#define uchar unsigned char
#define uint  unsigned int

#define   T1650_CLK     GPIO_PIN_2      //pd2
#define   T1650_1DATA   GPIO_PIN_7      //pc7
#define   T1650_2DATA   GPIO_PIN_6      //pc6
#define   T1650_3DATA   GPIO_PIN_5      //pc5
#define   T1650_4DATA   GPIO_PIN_4      //pc4
#define   T1650_5DATA   GPIO_PIN_3      //pc3

#define   KEY_CLR_D   0xef
#define   KEY_SET     0xee

void xianshi(uchar digit,uchar data,unsigned char chipn);
void Delay_us(unsigned int i);
void led_off(unsigned char chipn);
void led_on(unsigned char chipn);
#endif