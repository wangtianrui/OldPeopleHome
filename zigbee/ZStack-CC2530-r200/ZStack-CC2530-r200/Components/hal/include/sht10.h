#ifndef	__SHT10_h__
#define	__SHT10_h__
//	write your header here
#ifndef _IOCC2530_
#define _IOCC2530_
#include "ioCC2530.h"
#endif

#define IO_IN   0
#define IO_OUT  1

#define IO_DIR_PORT_PIN(port, pin, dir)  \
   do {                                  \
      if (dir == IO_OUT)                 \
         P##port##DIR |= (0x01<<(pin));  \
      else                               \
         P##port##DIR &= ~(0x01<<(pin)); \
   }while(0)

#define IO_Port	0


#define I2C_D P0_6
#define I2C_C P0_0

#define I2C_D_NU	6
#define I2C_C_NU	0

//#define IO_Port2 (IO_Port)
#define CFG_WRITE(x)				do{IO_DIR_PORT_PIN(0,x,IO_OUT);}while(0)
#define CFG_READ(x)				do{IO_DIR_PORT_PIN(0,x,IO_IN);}while(0)

#define MAKE_HIGH(x)				do{x = 1;}while(0)												
#define MAKE_LOW(x)				do{x = 0;}while(0)	

#define BIT_READ(x)				x

#define DURATION1 	1000

/*****************************************
******************************************/

#define SCLK_DIR_OUT 	CFG_WRITE(I2C_C_NU)
#define SDIO_DIR_IN 	CFG_READ(I2C_D_NU)
#define SDIO_DIR_OUT 	CFG_WRITE(I2C_D_NU)

#define SCLK_HIGH		MAKE_HIGH(I2C_C)
#define SCLK_LOW		MAKE_LOW(I2C_C)

#define SDIO_HIGH		MAKE_HIGH(I2C_D)
#define SDIO_LOW		MAKE_LOW(I2C_D)

#define READ_SDIO		BIT_READ(I2C_D)

/**************************************
***************************************/

#define DELAY(DURATION)		{ unsigned int i = DURATION; while(i--);}


//SH10 Command

#define CMD_RESERVED 		0x00	//±£Áô
#define CMD_TEM_MES		0x03	//ÎÂ¶È²âÁ¿ÃüÁî
#define CMD_HUMI_MES		0x05	//Êª¶È²âÁ¿ÃüÁî
#define CMD_Read_STATUS_REG	0x07	//¶Á×´Ì¬¼Ä´æÆ÷
#define CMD_Write_STATUS_REG	0x06	//Ð´×´Ì¬¼Ä´æÆ÷
#define CMD_Soft_Reset		0x1E	//Èí¸´Î»


#define bitselect     0x01    //Ñ¡ÔñÎÂ¶ÈÓëÊª¶ÈµÄµÍÎ»¶Á
#define noACK         0
#define ACK           1
#define HUMIDITY      2
#define TEMPERATURE   1

void SHT10_Transstart(void);
unsigned char SHT10_WriteByte(unsigned char data);
unsigned char SHT10_ReadByte(unsigned char ack);
void SHT10_Connectionreset(void);
unsigned char SHT10_Softreset(void);
unsigned char SHT10_WriteStatusReg(unsigned char RegVlaue);
unsigned char SHT10_ReadStatusReg(void);
unsigned char SHT10_Measure(unsigned int *p_value, unsigned char *p_checksum, unsigned char mode);
float SHT10_Calculate(unsigned int data,unsigned char mode);
void SHT10_init(unsigned int Initial_Reg);


#endif


