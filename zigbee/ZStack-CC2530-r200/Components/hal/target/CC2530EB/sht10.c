
#include "sht10.h"

/***********************************************
**Function Name:      SHT10_Transstart
**Description:        发送开始时序
**
**                    generates a transmission start
**                          _____         ________
**                    DATA:      |_______|
**                              ___     ___
**                    SCK : ___|   |___|   |______
**Input Parameters:   	无
**Output Parameters: 	无
************************************************/


void SHT10_Transstart(void)
{
    SDIO_DIR_OUT;
    SCLK_DIR_OUT;
    SCLK_LOW;
    SDIO_HIGH;
    DELAY(DURATION1);
    DELAY(DURATION1);
    SCLK_HIGH;
    DELAY(DURATION1);
    SDIO_LOW;
    DELAY(DURATION1);
    SCLK_LOW;
    DELAY(DURATION1);
    SCLK_HIGH;
    DELAY(DURATION1);
    SDIO_HIGH;
    DELAY(DURATION1);
    SCLK_LOW;	
}

/**********************************************************************************************************
**Function Name:      SHT10_WriteByte
**Description:        写时序
**Input Parameters:   无
**Output Parameters: 无
**************************************************************************************/
unsigned char SHT10_WriteByte(unsigned char data)
{
  unsigned char i;
  SDIO_DIR_OUT;
  SCLK_DIR_OUT;
  for(i=0x80;i>0;i=(i>>1))              //shift bit for masking
  {
    if(i&data)
      SDIO_HIGH;                        //masking value with i , write to SENSI-BUS
    else
      SDIO_LOW;
    DELAY(DURATION1);          //pulswith approx. 5 us
    SCLK_HIGH;                          //clk for SENSI-BUS
    DELAY(DURATION1);          //pulswith approx. 5 us
    SCLK_LOW;
    DELAY(DURATION1);          //pulswith approx. 5 us
  }
  SDIO_HIGH;                            //release DATA-line
            //pulswith approx. 5 us
  SDIO_DIR_IN;                     	 //Change SDA to be input
  DELAY(DURATION1);
  SCLK_HIGH;                          //clk for SENSI-BUS
  if(READ_SDIO)
  {
          return 1; 						//error=1 in case of no acknowledge
  }
  DELAY(DURATION1);          //pulswith approx. 5 us
  SCLK_LOW;
  return 0;
}

/**********************************************************************************************************
**Function Name:      SHT10_ReadByte
**Description:        读时序
**Input Parameters:   ack--->reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
**Output Parameters: 无
**********************************************************************************************************/
unsigned char SHT10_ReadByte(unsigned char ack)
{
  unsigned char i,val=0;
  SDIO_DIR_OUT;
  SDIO_HIGH;                            //release DATA-line
  SDIO_DIR_IN;
  for(i=0x80;i>0;i=(i>>1))             //shift bit for masking
  {
      SCLK_HIGH;                          //clk for SENSI-BUS
      DELAY(DURATION1);
      if(READ_SDIO)
        val=(val|i);                  //read bit
      SCLK_LOW;
      DELAY(DURATION1);
  }
  SDIO_DIR_OUT;
  if(ack)                           //in case of "ack==1" pull down DATA-Line
      SDIO_LOW;
  else
      SDIO_HIGH;
  SCLK_HIGH;                            //clk #9 for ack
  DELAY(DURATION1);              //pulswith approx. 5 us
  SCLK_LOW;
  DELAY(DURATION1);
  SDIO_HIGH;                            //release DATA-line
  DELAY(DURATION1);
  SDIO_DIR_IN;
  return val;
}

/**********************************************************************************************************
**Function Name:      SHT10_Connectionreset
**Description:        通讯复位时序
**                   communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
**                          _____________________________________________________         ________
**                    DATA:                                                      |_______|
**                             _    _    _    _    _    _    _    _    _        ___     ___
**                    SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
**Input Parameters:   无
**Output Parameters: 无
**********************************************************************************************************/
void SHT10_Connectionreset(void)
{
  unsigned char ClkCnt;
  SDIO_DIR_OUT;
  SCLK_DIR_OUT;
  SDIO_HIGH;
  SCLK_LOW;                              //Initial state
  DELAY(DURATION1);
  for(ClkCnt=0;ClkCnt<9;ClkCnt++)           //9 SCK cycles
  {
    SCLK_HIGH;
        DELAY(DURATION1); 	
    SCLK_LOW;
    DELAY(DURATION1);
  }
  SHT10_Transstart();                           //transmission start
}

/**********************************************************************************************************
**Function Name:      SHT10_Softreset
**Description:        软件复位时序resets the sensor by a softreset
**Input Parameters:   无
**Output Parameters: 无
**********************************************************************************************************/
unsigned char SHT10_Softreset(void)
{
  unsigned char error=0;
  SHT10_Connectionreset();              //reset communication
  error+=SHT10_WriteByte(CMD_Soft_Reset);        //send RESET-command to sensor
  return error;                     //error=1 in case of no response form the sensor
}

/**********************************************************************************************************
**Function Name:      SHT10_WriteStatusReg
**Description:        写状态寄存器
**Input Parameters:   *p_value
**Output Parameters: 无
**********************************************************************************************************/
unsigned char SHT10_WriteStatusReg(unsigned char RegVlaue)
{
  unsigned char error=0;
  SHT10_Transstart();                           //transmission start
  error+=SHT10_WriteByte(CMD_Write_STATUS_REG); //send command to sensor
  error+=SHT10_WriteByte(RegVlaue);             //send value of status register
  return error;                             //error>=1 in case of no response form the sensor
}

unsigned char SHT10_ReadStatusReg(void)
{
  unsigned char tmp=0;
  SHT10_Transstart();                           //transmission start
  SHT10_WriteByte(CMD_Read_STATUS_REG);         //send command to sensor
  tmp = SHT10_ReadByte(ACK);             				//send value of status register
  return tmp;                             			//error>=1 in case of no response form the sensor
}
/**********************************************************************************************************
**Function Name:      SHT10_Mearsure
**Description:        读时序      makes a measurement (humidity/temperature) with checksum
**Input Parameters:   *p_value       ,*p_checknum       ,mode
**Output Parameters: 无
**********************************************************************************************************/
unsigned char SHT10_Measure(unsigned int *p_value, unsigned char *p_checksum, unsigned char mode)
{
  unsigned error=0;
  unsigned int i;
  SHT10_Transstart();                   //transmission start
  switch(mode)
  {                                 //send command to sensor
    case TEMPERATURE:
        error+=SHT10_WriteByte(CMD_TEM_MES);
        break;
    case HUMIDITY:
        error+=SHT10_WriteByte(CMD_HUMI_MES);
        break;
  }
  SDIO_DIR_IN;
  for(i=0;i<1500;i++)              //wait until sensor has finished the measurement
  {
      if(READ_SDIO == 0)
        break;
      else
        DELAY(100);
  }
  if(READ_SDIO)
      error+=1;                       			//or timeout (~2 sec.) is reached
  *(p_value)=SHT10_ReadByte(ACK);      		 		//read the first byte (MSB)
  *(p_value)=SHT10_ReadByte(ACK)+(*(p_value)<<8);     //read the second byte (LSB)
  *p_checksum=SHT10_ReadByte(noACK);    				//read checksum
  return(error);
}

/**********************************************************************************************************
**Function Name:      SHT10_Calculate
**Description:        计算
**Input Parameters:   humi [Ticks] (12 bit)
**                    temp [Ticks] (14 bit)
**Output Parameters: humi [%RH]
**                    temp []
**********************************************************************************************************/
float SHT10_Calculate(unsigned int data,unsigned char mode)
{
  const float C1=-4.0;              // for 8 Bit
  const float C2=+0.648;            // for 8 Bit
  const float C3=-0.0000072;        // for 8 Bit
  const float D1=-39.6;             // for 12 Bit @ 3V
  const float D2=+0.04;             // for 12 Bit @ 3V
  const float T1=0.01;              // for 8 bit
  const float T2=0.00128;           // for 8 bit
  float rh_lin;                     // rh_lin: Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   : Temperature []
  if (mode == 1)
  {
    t_C=data*D2+D1;                      //calc. temperature from ticks to []
    return (t_C);
  }
  else if(mode == 2)
  {
    rh_lin=C3*data*data + C2*data + C1;     //calc. humidity from ticks to [%RH]
    rh_true=(t_C-25)*(T1+T2*data)+rh_lin;   //calc. temperature compensated humidity [%RH]
    if(rh_true>100)rh_true=100;       //cut if the value is outside of
    if(rh_true<0.1)rh_true=0.1;       //the physical possible range
    return (rh_true);
  }
  else
    return 0;
}
/**********************************************************************************************************
**Function Name:      SHT10_init
**Description:        初始化SHT10
**********************************************************************************************************/
void SHT10_init(unsigned int Initial_Reg)
{
  SHT10_Connectionreset();
  SHT10_WriteStatusReg(Initial_Reg);
}



