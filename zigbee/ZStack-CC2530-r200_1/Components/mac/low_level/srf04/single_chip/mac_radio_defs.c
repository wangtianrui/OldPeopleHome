/**************************************************************************************************
  Filename:       mac_radio_defs.c
  Revised:        $Date: 2010-12-23 14:50:47 -0800 (Thu, 23 Dec 2010) $
  Revision:       $Revision: 24688 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2010 Texas Instruments Incorporated. All rights reserved.

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

/* ------------------------------------------------------------------------------------------------
 *                                             Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "mac_radio_defs.h"
#include "hal_types.h"
#include "hal_assert.h"
#include "hal_mcu.h"
#include "mac_pib.h"


/* ------------------------------------------------------------------------------------------------
 *                                        Global Constants
 * ------------------------------------------------------------------------------------------------
 */
#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590)

#ifdef HAL_MCU_CC2533
const uint8 CODE macRadioDefsTxPwrBare[] =
{
  4,  /* tramsmit power level of the first entry */
  (uint8)(int8)-21, /* transmit power level of the last entry */
  /*   4 dBm */   0xEC,   /* characterized as  4.5 dBm in datasheet */
  /*   3 dBm */   0xDC,   /* characterized as  3   dBm in datasheet */
  /*   2 dBm */   0xDC,   
  /*   1 dBm */   0xCC,   /* characterized as  1.7 dBm in datasheet */
  /*   0 dBm */   0xBC,   /* characterized as  0.3 dBm in datasheet */
  /*  -1 dBm */   0xAC,   /* characterized as -1   dBm in datasheet */
  /*  -2 dBm */   0xAC,
  /*  -3 dBm */   0x9C,   /* characterized as -2.8 dBm in datasheet */
  /*  -4 dBm */   0x9C,   
  /*  -5 dBm */   0x8C,   /* characterized as -4.1 dBm in datasheet */
  /*  -6 dBm */   0x7C,   /* characterized as -5.9 dBm in datasheet */
  /*  -7 dBm */   0x7C,
  /*  -8 dBm */   0x6C,   /* characterized as -7.7 dBm in datasheet */
  /*  -9 dBm */   0x6C,
  /* -10 dBm */   0x5C,   /* characterized as -9.9 dBm in datasheet */
  /* -11 dBm */   0x5C,
  /* -12 dBm */   0x5C,   /* characterized as -9.9 dBm in datasheet */
  /* -13 dBm */   0x4C,   /* characterized as -12.8 dBm in datasheet */
  /* -14 dBm */   0x4C,
  /* -15 dBm */   0x3C,   /* characterized as -14.9 dBm in datasheet */
  /* -16 dBm */   0x3C, 
  /* -17 dBm */   0x2C,   /* characterized as -16.6 dBm in datasheet */
  /* -18 dBm */   0x2C,
  /* -19 dBm */   0x1C,   /* characterized as -18.7 dBm in datasheet */
  /* -20 dBm */   0x1C,   /* characterized as -18.7 dBm in datasheet */
  /* -21 dBm */   0x0C
};
#else /* HAL_MCU_CC2533 */
const uint8 CODE macRadioDefsTxPwrBare[] =
{
  3,  /* tramsmit power level of the first entry */
  (uint8)(int8)-22, /* transmit power level of the last entry */
  /*   3 dBm */   0xF5,   /* characterized as  4.5 dBm in datasheet */
  /*   2 dBm */   0xE5,   /* characterized as  2.5 dBm in datasheet */
  /*   1 dBm */   0xD5,   /* characterized as  1   dBm in datasheet */
  /*   0 dBm */   0xD5,   /* characterized as  1   dBm in datasheet */
  /*  -1 dBm */   0xC5,   /* characterized as -0.5 dBm in datasheet */
  /*  -2 dBm */   0xB5,   /* characterized as -1.5 dBm in datasheet */
  /*  -3 dBm */   0xA5,   /* characterized as -3   dBm in datasheet */
  /*  -4 dBm */   0x95,   /* characterized as -4   dBm in datasheet */
  /*  -5 dBm */   0x95,
  /*  -6 dBm */   0x85,   /* characterized as -6   dBm in datasheet */
  /*  -7 dBm */   0x85,
  /*  -8 dBm */   0x75,   /* characterized as -8   dBm in datasheet */
  /*  -9 dBm */   0x75,
  /* -10 dBm */   0x65,   /* characterized as -10  dBm in datasheet */
  /* -11 dBm */   0x65,
  /* -12 dBm */   0x55,   /* characterized as -12  dBm in datasheet */
  /* -13 dBm */   0x55,
  /* -14 dBm */   0x45,   /* characterized as -14  dBm in datasheet */
  /* -15 dBm */   0x45,
  /* -16 dBm */   0x35,   /* characterized as -16  dBm in datasheet */
  /* -17 dBm */   0x35,
  /* -18 dBm */   0x25,   /* characterized as -18  dBm in datasheet */
  /* -19 dBm */   0x25,
  /* -20 dBm */   0x15,   /* characterized as -20  dBm in datasheet */
  /* -21 dBm */   0x15,
  /* -22 dBm */   0x05    /* characterized as -22  dBm in datasheet */
};
#endif /* HAL_MCU_CC2533 */
#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || 
          (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590) */

#if defined HAL_PA_LNA || defined MAC_RUNTIME_CC2591
const uint8 CODE macRadioDefsTxPwrCC2591[] =
{
  20,  /* tramsmit power level of the first entry */
  (uint8)(int8)10, /* transmit power level of the last entry */
  /*  20 dBm */   0xE5,   /* characterized as 20 dBm in datasheet */
  /*  19 dBm */   0xD5,   /* characterized as 19 dBm in datasheet */
  /*  18 dBm */   0xC5,   /* characterized as 18 dBm in datasheet */
  /*  17 dBm */   0xB5,   /* characterized as 17 dBm in datasheet */
  /*  16 dBm */   0xA5,   /* characterized as 16 dBm in datasheet */
  /*  15 dBm */   0xA5,
  /*  14 dBm */   0x95,   /* characterized as 14.5 dBm in datasheet */
  /*  13 dBm */   0x85,   /* characterized as 13 dBm in datasheet */
  /*  12 dBm */   0x85,
  /*  11 dBm */   0x75,   /* characterized as 11.5 dBm in datasheet */
  /*  10 dBm */   0x65    /* characterized as 10 dBm in datasheet */
};
#endif


/* Placeholder for CC2590 */
#if defined HAL_PA_LNA_CC2590 || defined MAC_RUNTIME_CC2590
const uint8 CODE macRadioDefsTxPwrCC2590[] =
{
  11,  /* tramsmit power level of the first entry */
  (uint8)(int8)-12, /* transmit power level of the last entry */
  /* Note that the characterization is preliminary */
  /* 11 dBm */   0xF5,   /* characterized as 11.8 dBm */
  /* 10 dBm */   0xE5,   /* characterized as 10.6 dBm */
  /*  9 dBm */   0xD5,   /* characterized as 9.5 dBm */
  /*  8 dBm */   0xC5,   /* characterized as 8.5 dBm */
  /*  7 dBm */   0xB5,   /* characterized as 7.6 dBm */
  /*  6 dBm */   0xA5,   /* characterized as 6.4 dBm */
  /*  5 dBm */   0xA5,   /* characterized as 6.4 dBm */
  /*  4 dBm */   0x95,   /* characterized as 4.7 dBm */
  /*  3 dBm */   0x85,   /* characterized as 3.5 dBm */
  /*  2 dBm */   0x75,   /* characterized as 2.0 dBm */
  /*  1 dBm */   0x75,   /* characterized as 2.0 dBm */
  /*  0 dBm */   0x65,   /* characterized as 0.3 dBm */
  /* -1 dBm */   0x65,   /* characterized as 0.3 dBm */
  /* -2 dBm */   0x55,   /* characterized as -1.7 dBm */
  /* -3 dBm */   0x55,   /* characterized as -1.7 dBm */
  /* -4 dBm */   0x45,   /* characterized as -3.9 dBm */
  /* -5 dBm */   0x45,   /* characterized as -3.9 dBm */
  /* -6 dBm */   0x45,   /* characterized as -3.9 dBm */
  /* -7 dBm */   0x35,   /* characterized as -6.3 dBm*/
  /* -8 dBm */   0x25,   /* characterized as -7.8 dBm */
  /* -9 dBm */   0x25,   /* characterized as -7.8 dBm */
  /* -10 dBm */  0x15,   /* characterized as -9.8 dBm */
  /* -11 dBm */  0x15,   /* characterized as -9.8 dBm */
  /* -12 dBm */  0x05,   /* characterized as -11.8 dBm */
};
#endif

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590

/* TX power table array */
const uint8 CODE *const CODE macRadioDefsTxPwrTables[] =
{
#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590)
  macRadioDefsTxPwrBare,
#endif

#if defined HAL_PA_LNA || defined MAC_RUNTIME_CC2591
  macRadioDefsTxPwrCC2591,
#endif

#if defined HAL_PA_LNA_CC2590 || defined MAC_RUNTIME_CC2590
  macRadioDefsTxPwrCC2590,
#endif
};

/* RSSI offset adjustment value array */
const int8 CODE macRadioDefsRssiAdj[] =
{
#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590)
  0,
#endif

#if defined HAL_PA_LNA || defined MAC_RUNTIME_CC2591
  -6, /* high gain mode */
  6,  /* low gain mode */
#endif

#if defined HAL_PA_LNA_CC2590 || defined MAC_RUNTIME_CC2590
-10,  /* TBD: place holder for high gain mode */
  0,  /* TBD: place holder for low gain mode */
#endif
};

#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || ... */

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
uint8 macRadioDefsRefTableId = 0;
#endif

/* RF observable control register value to output PA signal */
#define RFC_OBS_CTRL_PA_PD_INV        0x68

/* RF observable control register value to output LNA signal */
#define RFC_OBS_CTRL_LNAMIX_PD_INV    0x6A

/* OBSSELn register value to select RF observable 0 */
#define OBSSEL_OBS_CTRL0             0xFB

/* OBSSELn register value to select RF observable 1 */
#define OBSSEL_OBS_CTRL1             0xFC


/**************************************************************************************************
 * @fn          MAC_SelectRadioRegTable
 *
 * @brief       Select radio register table in case multiple register tables are included
 *              in the build
 *
 * @param       txPwrTblIdx - TX power register value table index
 * @param       rssiAdjIdx - RSSI adjustment value index
 *
 * @return      none
 **************************************************************************************************
 */
extern void MAC_SetRadioRegTable ( uint8 txPwrTblIdx, uint8 rssiAdjIdx )
{
  /* Depending on compile flags, the parameters may not be used */
  (void) txPwrTblIdx;
  (void) rssiAdjIdx;

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590
  if (txPwrTblIdx >= sizeof(macRadioDefsTxPwrTables)/sizeof(macRadioDefsTxPwrTables[0]))
  {
    txPwrTblIdx = 0;
  }
#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 */

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590

  if (rssiAdjIdx >= sizeof(macRadioDefsRssiAdj)/sizeof(macRadioDefsRssiAdj[0]))
  {
    rssiAdjIdx = 0;
  }

  macRadioDefsRefTableId = (txPwrTblIdx << 4) | rssiAdjIdx;

#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || .. */
}


/**************************************************************************************************
 * @fn          macRadioTurnOnPower
 *
 * @brief       Logic and sequence for powering up the radio.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRadioTurnOnPower(void)
{
  /* Enable RF error trap */
  MAC_MCU_RFERR_ENABLE_INTERRUPT();

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590
  /* table ID is referenced only when runtime configuratino is enabled */
  if (macRadioDefsRefTableId & 0xf0)
#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 */

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
  { /* either if compound statement or non-conditional compound statement */

    /* (Re-)Configure PA and LNA control signals to RF frontend chips.
    * Note that The register values are not retained during sleep.
    */

    /* P1_1 -> PAEN */
    RFC_OBS_CTRL0 = RFC_OBS_CTRL_PA_PD_INV;
    OBSSEL1       = OBSSEL_OBS_CTRL0;

    /* P1_4 -> EN (LNA control) */
    RFC_OBS_CTRL1 = RFC_OBS_CTRL_LNAMIX_PD_INV;
    OBSSEL4       = OBSSEL_OBS_CTRL1;
    
    /* For any RX, change CCA settings for CC2591 compression workaround.
     * This will override LNA control if CC2591_COMPRESSION_WORKAROUND 
     * flag is defined. 
     */
    COMPRESSION_WORKAROUND_ON();
  }
#endif /* defined MAC_RUNTIME_CC2591 || ... || defined HAL_PA_LNA_CC2590 */

  if (macChipVersion <= REV_B)
  {
    /* radio initializations for disappearing RAM; PG1.0 and before only */
    MAC_RADIO_SET_PAN_ID(macPib.panId);
    MAC_RADIO_SET_SHORT_ADDR(macPib.shortAddress);
    MAC_RADIO_SET_IEEE_ADDR(macPib.extendedAddress.addr.extAddr);
  }

  /* Turn on frame filtering */
  MAC_RADIO_TURN_ON_RX_FRAME_FILTERING();
}


/**************************************************************************************************
 * @fn          macRadioTurnOffPower
 *
 * @brief       Logic and sequence for powering down the radio.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRadioTurnOffPower(void)
{
  /* Disable RF error trap */
  MAC_MCU_RFERR_DISABLE_INTERRUPT();
}


/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */
#if (HAL_CPU_CLOCK_MHZ != 32)
#error "ERROR: The only tested/supported clock speed is 32 MHz."
#endif

#if (MAC_RADIO_RECEIVER_SENSITIVITY_DBM > MAC_SPEC_MIN_RECEIVER_SENSITIVITY)
#error "ERROR: Radio sensitivity does not meet specification."
#endif

#if defined (HAL_PA_LNA) && defined (HAL_PA_LNA_CC2590)
#error "ERROR: HAL_PA_LNA and HAL_PA_LNA_CC2590 cannot be both defined."
#endif

#if defined (HAL_PA_LNA) && defined (MAC_RUNTIME_CC2591)
#error "ERROR: HAL_PA_LNA and MAC_RUNTIME_CC2591 cannot be both defined."
#endif

#if defined (HAL_PA_LNA_CC2590) && defined (MAC_RUNTIME_CC2590)
#error "ERROR: HAL_PA_LNA_CC2590 and MAC_RUNTIME_CC2590 cannot be both defined."
#endif

#if defined (CC2591_COMPRESSION_WORKAROUND) && !defined (HAL_PA_LNA)
#error "ERROR: CC2591_COMPRESSION_WORKAROUND can only be defined when HAL_PA_LNA is defined."
#endif

#if defined (CC2591_COMPRESSION_WORKAROUND)
#warning "WARNING: Contact TI customer support if your reference design is based on CC2530-CC2591EM rev 2.0 or prior."
#endif

/**************************************************************************************************
 */
