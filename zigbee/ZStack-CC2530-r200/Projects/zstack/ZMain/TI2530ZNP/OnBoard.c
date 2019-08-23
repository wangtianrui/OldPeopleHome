/**************************************************************************************************
  Filename:       OnBoard.c
  Revised:        $Date: 2010-08-02 13:44:14 -0700 (Mon, 02 Aug 2010) $
  Revision:       $Revision: 23273 $

  Description:    This file contains the UI and control for the
                  peripherals on the EVAL development board
  Notes:          This file targets the Chipcon CC2530/31ZNP


  Copyright 2005-2010 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "ZGlobals.h"
#include "OnBoard.h"
#include "OSAL.h"
#include "MT.h"
#include "MT_SYS.h"
#include "DebugTrace.h"

/* Hal */
#include "hal_lcd.h"
#include "hal_mcu.h"
#include "hal_timer.h"
#include "hal_key.h"
#include "hal_led.h"

/* Allow access macRandomByte() */
#include "mac_radio_defs.h"

/*********************************************************************
 * CONSTANTS
 */

// Task ID not initialized
#define NO_TASK_ID 0xFF

// Minimum length RAM "pattern" for Stack check
#define MIN_RAM_INIT 12

/*********************************************************************
 * GLOBAL VARIABLES
 */

#if defined MAKE_CRC_SHDW
#pragma location="CRC_SHDW"
const CODE uint16 _crcShdw = 0xFFFF;
#pragma required=_crcShdw

#else  // if !defined MAKE_CRC_SHDW

#if defined FAKE_CRC_SHDW
#pragma location="CRC_SHDW"
const CODE uint16 _crcShdw = 0x2010;
#pragma required=_crcShdw
#pragma location="CHECKSUM"
const CODE uint16 _crcFake = 0x2010;
#pragma required=_crcFake
#endif

#pragma location="LOCK_BITS_ADDRESS_SPACE"
__no_init uint8 _lockBits[16];
#pragma required=_lockBits

#if defined ZCL_KEY_ESTABLISH
#include "zcl_cert_data.c"
#else
#pragma location="IEEE_ADDRESS_SPACE"
__no_init uint8 _nvIEEE[Z_EXTADDR_LEN];
#pragma required=_nvIEEE
#endif

#pragma location="RESERVED_ADDRESS_SPACE"
__no_init uint8 _reserved[1932];
#pragma required=_reserved
#endif

// 64-bit Extended Address of this device
uint8 aExtendedAddress[8];

/*********************************************************************
 * LOCAL VARIABLES
 */

// Registered keys task ID, initialized to NOT USED.
static uint8 registeredKeysTaskID = NO_TASK_ID;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void ChkReset( void );

/*********************************************************************
 * @fn      InitBoard()
 * @brief   Initialize the CC2420DB Board Peripherals
 * @param   level: COLD,WARM,READY
 * @return  None
 */
void InitBoard( uint8 level )
{
  if ( level == OB_COLD )
  {
    // IAR does not zero-out this byte below the XSTACK.
    *(uint8 *)0x0 = 0;
    // Interrupts off
    osal_int_disable( INTS_ALL );
    // Check for Brown-Out reset
    ChkReset();

    // Special ZNP CFG1 (CFG0 handled in hal_board_cfg.h.
#if defined CC2531ZNP
    znpCfg1 = ZNP_CFG1_UART;
#elif defined CC2530_MK
    znpCfg1 = ZNP_CFG1_SPI;
    znpCfg0 = ZNP_CFG0_32K_OSC;
#else
    znpCfg1 = P2_0;
    znpCfg0 = P1_2;
    // Tri-state the 2 CFG inputs after being read (see hal_board_cfg_xxx.h for CFG0.)
    P1INP |= BV(2);
    P2INP |= BV(0);
#endif
  }
  else  // !OB_COLD
  {
    /* Initialize Key stuff */
    HalKeyConfig(HAL_KEY_INTERRUPT_DISABLE, OnBoard_KeyCallback);
  }
}

/*********************************************************************
 * @fn      ChkReset()
 * @brief   Check reset bits - if reset cause is unknown, assume a
 *          brown-out (low power), assume batteries are not reliable,
 *          hang in a loop and sequence through the LEDs.
 * @param   None
 * @return  None
 *********************************************************************/
void ChkReset( void )
{
  uint8 rib;

  // Isolate reset indicator bits
  rib = SLEEPSTA & LRESET;

  if ( rib == RESETPO )
  {
    // Put code here to handle Power-On reset
  }
  else if ( rib == RESETEX )
  {
    // Put code here to handle External reset
  }
  else if ( rib == RESETWD )
  {
    // Put code here to handle WatchDog reset
  }
  else  // Unknown reason - not expected.
  {
    HAL_ASSERT(0);
  }
}

/*********************************************************************
 *                        "Keyboard" Support
 *********************************************************************/

/*********************************************************************
 * Keyboard Register function
 *
 * The keyboard handler is setup to send all keyboard changes to
 * one task (if a task is registered).
 *
 * If a task registers, it will get all the keys. You can change this
 * to register for individual keys.
 *********************************************************************/
uint8 RegisterForKeys( uint8 task_id )
{
  // Allow only the first task
  if ( registeredKeysTaskID == NO_TASK_ID )
  {
    registeredKeysTaskID = task_id;
    return ( true );
  }
  else
    return ( false );
}

/*********************************************************************
 * @fn      OnBoard_SendKeys
 *
 * @brief   Send "Key Pressed" message to application.
 *
 * @param   keys  - keys that were pressed
 *          state - shifted
 *
 * @return  status
 *********************************************************************/
uint8 OnBoard_SendKeys( uint8 keys, uint8 state )
{
  keyChange_t *msgPtr;

  if ( registeredKeysTaskID != NO_TASK_ID )
  {
    // Send the address to the task
    msgPtr = (keyChange_t *)osal_msg_allocate( sizeof(keyChange_t) );
    if ( msgPtr )
    {
      msgPtr->hdr.event = KEY_CHANGE;
      msgPtr->state = state;
      msgPtr->keys = keys;

      osal_msg_send( registeredKeysTaskID, (uint8 *)msgPtr );
    }
    return ( ZSuccess );
  }
  else
    return ( ZFailure );
}

/*********************************************************************
 * @fn      OnBoard_KeyCallback
 *
 * @brief   Callback service for keys
 *
 * @param   keys  - keys that were pressed
 *          state - shifted
 *
 * @return  void
 *********************************************************************/
void OnBoard_KeyCallback ( uint8 keys, uint8 state )
{
  uint8 shift;
  (void)state;

  shift = (keys & HAL_KEY_SW_6) ? true : false;

  if ( OnBoard_SendKeys( keys, shift ) != ZSuccess )
  {
    // Process SW1 here
    if ( keys & HAL_KEY_SW_1 )  // Switch 1
    {
    }
    // Process SW2 here
    if ( keys & HAL_KEY_SW_2 )  // Switch 2
    {
    }
    // Process SW3 here
    if ( keys & HAL_KEY_SW_3 )  // Switch 3
    {
    }
    // Process SW4 here
    if ( keys & HAL_KEY_SW_4 )  // Switch 4
    {
    }
    // Process SW5 here
    if ( keys & HAL_KEY_SW_5 )  // Switch 5
    {
    }
    // Process SW6 here
    if ( keys & HAL_KEY_SW_6 )  // Switch 6
    {
    }
  }
}

/*********************************************************************
 * @fn      OnBoard_stack_used
 *
 * @brief   Runs through the stack looking for touched memory.
 *
 * @param   none
 *
 * @return  Maximum number of bytes used by the stack.
 *********************************************************************/
uint16 OnBoard_stack_used(void)
{
  uint8 const *ptr;
  uint8 cnt = 0;

  for (ptr = CSTACK_END; ptr > CSTACK_BEG; ptr--)
  {
    if (STACK_INIT_VALUE == *ptr)
    {
      if (++cnt >= MIN_RAM_INIT)
      {
        ptr += MIN_RAM_INIT;
        break;
      }
    }
    else
    {
      cnt = 0;
    }
  }

  return (uint16)(CSTACK_END - ptr + 1);
}

/*********************************************************************
 * @fn      _itoa
 *
 * @brief   convert a 16bit number to ASCII
 *
 * @param   num -
 *          buf -
 *          radix -
 *
 * @return  void
 *
 *********************************************************************/
void _itoa(uint16 num, uint8 *buf, uint8 radix)
{
  char c,i;
  uint8 *p, rst[5];

  p = rst;
  for ( i=0; i<5; i++,p++ )
  {
    c = num % radix;  // Isolate a digit
    *p = c + (( c < 10 ) ? '0' : '7');  // Convert to Ascii
    num /= radix;
    if ( !num )
      break;
  }

  for ( c=0 ; c<=i; c++ )
    *buf++ = *p--;  // Reverse character order

  *buf = '\0';
}

/*********************************************************************
 * @fn        Onboard_rand
 *
 * @brief    Random number generator
 *
 * @param   none
 *
 * @return  uint16 - new random number
 *
 *********************************************************************/
uint16 Onboard_rand( void )
{
  return ( MAC_RADIO_RANDOM_WORD() );
}

/*********************************************************************
 * @fn        Onboard_wait
 *
 * @brief    Delay wait
 *
 * @param   uint16 - time to wait
 *
 * @return  none
 *
 *********************************************************************/
void Onboard_wait( uint16 timeout )
{
  while (timeout--)
  {
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }
}

/*********************************************************************
 * @fn      Onboard_soft_reset
 *
 * @brief   Effect a soft reset.
 *
 * @param   none
 *
 * @return  none
 *
 *********************************************************************/
__near_func void Onboard_soft_reset( void )
{
  HAL_DISABLE_INTERRUPTS();
  // Abort all DMA channels to insure that ongoing operations do not
  // interfere with re-configuration.
  DMAARM = 0x80 | 0x1F;
  asm("LJMP 0x0");
}

/*********************************************************************
 *                    EXTERNAL I/O FUNCTIONS
 *
 * User defined functions to control external devices. Add your code
 * to the following functions to control devices wired to DB outputs.
 *
 *********************************************************************/

void BigLight_On( void )
{
  // Put code here to turn on an external light
}

void BigLight_Off( void )
{
  // Put code here to turn off an external light
}

void BuzzerControl( uint8 on )
{
  // Put code here to turn a buzzer on/off
  (void)on;
}

void Dimmer( uint8 lvl )
{
  // Put code here to control a dimmer
  (void)lvl;
}

// No dip switches on this board
uint8 GetUserDipSw( void )
{
  return 0;
}

/*********************************************************************
*********************************************************************/
