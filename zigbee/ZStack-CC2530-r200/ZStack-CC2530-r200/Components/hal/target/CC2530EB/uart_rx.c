
#include "uart_rx.h"

static uchar temp;

/****************************************************************
*函数功能 ：初始化串口1										
*入口参数 ：无												
*返 回 值 ：无							
*说    明 ：57600-8-n-1						
****************************************************************/
void initUARTrx(void)
{

    CLKCONCMD &= ~0x40;              //晶振
    while(!(SLEEPSTA & 0x40));      //等待晶振稳定
    CLKCONCMD &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEPCMD |= 0x04; 		 //关闭不用的RC振荡器

    PERCFG = 0x00;				//位置1 P0口
    P0SEL = 0x3c;				//P0用作串口

    U0CSR |= 0x80;				//UART方式
    U0GCR |= 10;				//baud_e
    U0BAUD |= 59;				//波特率设为57600
    UTX0IF = 1;

    U0CSR |= 0X40;				//允许接收
    IEN0 |= 0x84;				//开总中断，接收中断
}
uchar ReceiveData(void){
  return(temp);
}


/****************************************************************
*函数功能 ：串口接收一个字符					
*入口参数 : 无						
*返 回 值 ：无				
*说    明 ：接收完成后打开接收				
****************************************************************/
#pragma vector = URX0_VECTOR
 __interrupt void UART0_ISR(void)
 {
 	URX0IF = 0;				//清中断标志
	temp = U0DBUF;
 }
