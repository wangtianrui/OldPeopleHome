#ifndef _UART_RXH
#define _UART_RXH

#include <ioCC2530.h>
#include <string.h>

#define uint unsigned int
#define uchar unsigned char
#define RXMAXLENGTH 20
#define FALSE 0
#define TURE 1
//定义控制灯的端口
#define led2 P1_1

//函数声明
void Delay(uint);
void initUARTrx(void);
uchar ReceiveData(void);
void ReceiverAction(void);
void Init_LED_IO(void);

#endif
