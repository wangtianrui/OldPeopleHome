/**************************************************************************************************
  Filename:       zmac.c
  Revised:        $Date: 2011-02-28 16:59:59 -0800 (Mon, 28 Feb 2011) $
  Revision:       $Revision: 25230 $


  Description:    This file contains the ZStack MAC Porting Layer


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
  PROVIDED “AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************/

#include "ZComDef.h"
#include "OSAL.h"
#include "ZMAC.h"
#include "mac_main.h"
#include "ssp.h"

#if !defined NONWK
  #include "ZGlobals.h"
#endif

/********************************************************************************************************
 *                                                 MACROS
 ********************************************************************************************************/

/********************************************************************************************************
 *                                               CONSTANTS
 ********************************************************************************************************/

/********************************************************************************************************
 *                                               GLOBALS
 ********************************************************************************************************/
uint32 _ScanChannels;

extern uint8 aExtendedAddress[];

/**************************************************************************************************
 * @fn          MAC_SetRandomSeedCB
 *
 * @brief       MAC function: Set the function pointer for the random seed callback.
 *
 * input parameters
 *
 * @param       pCBFcn - function pointer of the random seed callback
 *
 * output parameters
 *
 * None.
 *
 * @return      none
 **************************************************************************************************
 */
extern void MAC_SetRandomSeedCB(macRNGFcn_t pCBFcn);
/********************************************************************************************************
 *                                               LOCALS
 ********************************************************************************************************/

/* Pointer to scan result buffer */
void *ZMac_ScanBuf = NULL;

/********************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************/

/********************************************************************************************************
 *                                                TYPEDEFS
 ********************************************************************************************************/


/********************************************************************************************************
 *                                                FUNCTIONS
 ********************************************************************************************************/

/********************************************************************************************************
 * @fn      ZMacInit
 *
 * @brief   Initialize MAC.
 *
 * @param   none.
 *
 * @return  status.
 ********************************************************************************************************/
uint8 ZMacInit( void )
{
  uint8 stat;

#if defined( ZCL_KEY_ESTABLISH )
  /* Set the callback function for 16 byte random seed */
  MAC_SetRandomSeedCB( SSP_StoreRandomSeedNV);
#endif

  MAC_Init();
  MAC_InitDevice();

#if !defined NONWK
  if ( ZG_BUILD_RTR_TYPE )
  {
    MAC_InitCoord();
  }
#endif

  // If OK, initialize the MAC
  stat = ZMacReset( TRUE );

  // Turn off interrupts
  osal_int_disable( INTS_ALL );

  return ( stat );
}

/********************************************************************************************************
 * @fn      ZMacReset
 *
 * @brief   Reset the MAC.
 *
 * @param   Default to PIB defaults.
 *
 * @return  status.
 ********************************************************************************************************/
uint8 ZMacReset( bool SetDefaultPIB )
{
  byte stat;
  byte value;

  stat = MAC_MlmeResetReq( SetDefaultPIB );

  // Don't send PAN ID conflict
  value = FALSE;
  MAC_MlmeSetReq( MAC_ASSOCIATED_PAN_COORD, &value );
  MAC_MlmeSetReq( MAC_EXTENDED_ADDRESS, &aExtendedAddress );

  if (ZMac_ScanBuf)
  {
    osal_mem_free(ZMac_ScanBuf);
    ZMac_ScanBuf = NULL;
  }

  return ( stat );
}


/********************************************************************************************************
 * @fn      ZMacGetReq
 *
 * @brief   Read a MAC PIB attribute.
 *
 * @param   attr - PIB attribute to get
 * @param   value - pointer to the buffer to store the attribute
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacGetReq( uint8 attr, uint8 *value )
{
  if ( attr == ZMacExtAddr )
  {
    osal_cpyExtAddr( value, &aExtendedAddress );
    return ZMacSuccess;
  }

  return (ZMacStatus_t) MAC_MlmeGetReq( attr, value );
}


/********************************************************************************************************
 * @fn      ZMacSetReq
 *
 * @brief   Write a MAC PIB attribute.
 *
 * @param   attr - PIB attribute to Set
 * @param   value - pointer to the data
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacSetReq( uint8 attr, byte *value )
{
  if ( attr == ZMacExtAddr )
  {
    osal_cpyExtAddr( aExtendedAddress, value );
  }

  return (ZMacStatus_t) MAC_MlmeSetReq( attr, value );
}

#ifdef MAC_SECURITY
/********************************************************************************************************
 * @fn      ZMacSecurityGetReq
 *
 * @brief   Read a MAC Security PIB attribute.
 *
 * @param   attr - PIB attribute to get
 * @param   value - pointer to the buffer to store the attribute
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacSecurityGetReq( uint8 attr, uint8 *value )
{
  return (ZMacStatus_t) MAC_MlmeGetSecurityReq( attr, value );
}


/********************************************************************************************************
 * @fn      ZMacSecuritySetReq
 *
 * @brief   Write a MAC Security PIB attribute.
 *
 * @param   attr - PIB attribute to Set
 * @param   value - pointer to the data
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacSecuritySetReq( uint8 attr, byte *value )
{
  return (ZMacStatus_t) MAC_MlmeSetSecurityReq( attr, value );
}
#endif /* MAC_SECURITY */

/********************************************************************************************************
 * @fn      ZMacAssociateReq
 *
 * @brief   Request an association with a coordinator.
 *
 * @param   structure with info need to associate.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacAssociateReq( ZMacAssociateReq_t *pData )
{
  /* Right now, set security to zero */
  pData->Sec.SecurityLevel = false;

  MAC_MlmeAssociateReq ( (macMlmeAssociateReq_t *)pData);
  return ( ZMacSuccess );
}

/********************************************************************************************************
 * @fn      ZMacAssociateRsp
 *
 * @brief   Request to send an association response message.
 *
 * @param   structure with associate response and info needed to send it.
 *
 * @return  MAC_SUCCESS or MAC error code
 ********************************************************************************************************/
uint8 ZMacAssociateRsp( ZMacAssociateRsp_t *pData )
{
  /* TBD: set security to zero for now. Require Ztool change */
  pData->Sec.SecurityLevel = false;

  return ( MAC_MlmeAssociateRsp( (macMlmeAssociateRsp_t *) pData ) );
}

/********************************************************************************************************
 * @fn      ZMacDisassociateReq
 *
 * @brief   Request to send a disassociate request message.
 *
 * @param   structure with info need send it.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacDisassociateReq( ZMacDisassociateReq_t *pData )
{
  /* Right now, set security to zero */
  pData->Sec.SecurityLevel = false;

  MAC_MlmeDisassociateReq( (macMlmeDisassociateReq_t *)pData);
  return ( ZMacSuccess );
}

/********************************************************************************************************
 * @fn      ZMacOrphanRsp
 *
 * @brief   Allows next higher layer to respond to an orphan indication message.
 *
 * @param   structure with info need send it.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacOrphanRsp( ZMacOrphanRsp_t *pData )
{
  /* Right now, set security to zero */
  pData->Sec.SecurityLevel = false;

  MAC_MlmeOrphanRsp( (macMlmeOrphanRsp_t *)pData);
  return ( ZMacSuccess );
}

/********************************************************************************************************
 * @fn      ZMacScanReq
 *
 * @brief   This function is called to perform a network scan.
 *
 * @param   param - structure with info need send it.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacScanReq( ZMacScanReq_t *pData )
{
  _ScanChannels = pData->ScanChannels;

  /* scan in progress */
  if (ZMac_ScanBuf != NULL)
  {
    return MAC_SCAN_IN_PROGRESS;
  }

  if (pData->ScanType != ZMAC_ORPHAN_SCAN)
  {
    /* Allocate memory depends on the scan type */
    if (pData->ScanType == ZMAC_ED_SCAN)
    {
      if ((ZMac_ScanBuf = osal_mem_alloc(ZMAC_ED_SCAN_MAXCHANNELS)) == NULL)
      {
        return MAC_NO_RESOURCES;
      }
      osal_memset(ZMac_ScanBuf, 0, ZMAC_ED_SCAN_MAXCHANNELS);
      pData->Result.pEnergyDetect = ((uint8*)ZMac_ScanBuf) + MAC_CHAN_11;
    }
    else if (pData->MaxResults > 0)
    {
      if ((ZMac_ScanBuf = pData->Result.pPanDescriptor =
           osal_mem_alloc( sizeof( ZMacPanDesc_t ) * pData->MaxResults )) == NULL)
      {
        return MAC_NO_RESOURCES;
      }
    }
  }

  /* Channel Page */
  pData->ChannelPage = 0x00;

  MAC_MlmeScanReq ((macMlmeScanReq_t *)pData);

  return ZMacSuccess;
}


/********************************************************************************************************
 * @fn      ZMacStartReq
 *
 * @brief   This function is called to tell the MAC to transmit beacons
 *          and become a coordinator.
 *
 * @param   structure with info need send it.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacStartReq( ZMacStartReq_t *pData )
{
  uint8 stat;

  // Probably want to keep the receiver on
  stat = true;
  MAC_MlmeSetReq( MAC_RX_ON_WHEN_IDLE, &stat );

  /* Right now, set security to zero */
  pData->RealignSec.SecurityLevel = false;
  pData->BeaconSec.SecurityLevel = false;


  MAC_MlmeStartReq((macMlmeStartReq_t *) pData);

  // MAC does not issue mlmeStartConfirm(), so we have to
  // mlmeStartConfirm( stat );  This needs to be addressed some how

  return ZMacSuccess;
}

/********************************************************************************************************
 * @fn      ZMacSyncReq
 *
 * @brief   This function is called to request a sync to the current
 *          networks beacons.
 *
 * @param   LogicalChannel -
 * @param   TrackBeacon - true/false
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacSyncReq( ZMacSyncReq_t *pData )
{
  MAC_MlmeSyncReq( (macMlmeSyncReq_t *)pData);
  return ZMacSuccess;
}

/********************************************************************************************************
 * @fn      ZMacPollReq
 *
 * @brief   This function is called to request MAC data request poll.
 *
 * @param   coordAddr -
 * @param   coordPanId -
 * @param   SecurityEnable - true or false.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacPollReq( ZMacPollReq_t *pData )
{
  /* Right now, set security to zero */
  pData->Sec.SecurityLevel = false;

  MAC_MlmePollReq ((macMlmePollReq_t *)pData);
  return ( ZMacSuccess );
}

/********************************************************************************************************
 * @fn      ZMacDataReqSec
 *
 * @brief   Send a MAC Data Frame packet, calls the passed in function to apply non-MAC security
 *          on the MAC data field after the MAC buffer allocation.
 *
 * @param   pData - structure containing data and where to send it.
 * @param   secCB - callback function to apply security, NULL indicates no security
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacDataReqSec( ZMacDataReq_t *pData, applySecCB_t secCB )
{
  macMcpsDataReq_t *pBuf;

  /* Allocate memory */
  pBuf = MAC_McpsDataAlloc( pData->msduLength, pData->Sec.SecurityLevel, pData->Sec.KeyIdMode );

  if ( pBuf )
  {
    /* Copy the addresses */
    osal_memcpy( &pBuf->mac, pData, sizeof (macDataReq_t) );

    /* Copy data */
    osal_memcpy( pBuf->msdu.p, pData->msdu, pData->msduLength );

    /* Copy Security parameters */
    osal_memcpy( &pBuf->sec, &pData->Sec, sizeof (macSec_t));

    /* Encrypt in place */
    if ( secCB && pBuf->msdu.len && pBuf->msdu.p )
    {
      if ( secCB( pBuf->msdu.len, pBuf->msdu.p ) != ZSuccess )
      {
        // Deallocate the buffer.  MAC_McpsDataAlloc() calls osal_msg_allocate() and
        // returns the same pointer.
        osal_msg_deallocate( (uint8 *)pBuf );

        return ( MAC_NO_RESOURCES );
      }
    }

    /* Call Mac Data Request */
    MAC_McpsDataReq( pBuf );

    return ( ZMacSuccess );
  }

  return ( MAC_NO_RESOURCES );
}

/********************************************************************************************************
 * @fn      ZMacDataReq
 *
 * @brief   Send a MAC Data Frame packet.
 *
 * @param   structure containing data and where to send it.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacDataReq( ZMacDataReq_t *pData )
{
  return ZMacDataReqSec( pData, NULL );
}

/********************************************************************************************************
 * @fn      ZMacPurgeReq
 *
 * @brief   Purge a MAC Data Frame packet.
 *
 * @param   MSDU data handle.
 *
 * @return  status
 ********************************************************************************************************/
uint8 ZMacPurgeReq( byte Handle )
{
  MAC_McpsPurgeReq( Handle );
  return ZMacSuccess;
}

/********************************************************************************************************
 * @fn      ZMacSrcMatchEnable
 *
 * @brief   This function is call to enable AUTOPEND and source address matching.
 *
 * @param   addressType - address type that the application uses
 *                        SADDR_MODE_SHORT or SADDR_MODE_EXT.
 *          numEntries  - number of source address table entries to be used
 *
 * @return  status
 ********************************************************************************************************/
ZMacStatus_t ZMacSrcMatchEnable (uint8 addrType, uint8 numEntries)
{
  return (MAC_SrcMatchEnable(addrType, numEntries));
}

/********************************************************************************************************
 * @fn      ZMacSrcMatchAddEntry
 *
 * @brief   This function is called to add a short or extended address to source address table.
 *
 * @param   addr - a pointer to sAddr_t which contains addrMode
 *                     and a union of a short 16-bit MAC address or an extended
 *                     64-bit MAC address to be added to the source address table.
 *          panID - the device PAN ID. It is only used when the addr is
 *                      using short address
 *
 * @return  status
 ********************************************************************************************************/
ZMacStatus_t ZMacSrcMatchAddEntry (zAddrType_t *addr, uint16 panID)
{
  return (MAC_SrcMatchAddEntry ((sAddr_t*)addr, panID));
}

/********************************************************************************************************
 * @fn      ZMacSrcMatchDeleteEntry
 *
 * @brief   This function is called to delete a short or extended address from source address table.
 *
 * @param   addr - a pointer to sAddr_t which contains addrMode
 *                     and a union of a short 16-bit MAC address or an extended
 *                     64-bit MAC address to be added to the source address table.
 *          panID - the device PAN ID. It is only used when the addr is
 *                      using short address
 *
 * @return  status
 ********************************************************************************************************/
ZMacStatus_t ZMacSrcMatchDeleteEntry (zAddrType_t *addr, uint16 panID)
{
  return (MAC_SrcMatchDeleteEntry ((sAddr_t*)addr, panID));
}

/********************************************************************************************************
 * @fn       ZMacSrcMatchAckAllPending
 *
 * @brief    Enabled/disable acknowledging all packets with pending bit set
 *           It is normally enabled when adding new entries to
 *           the source address table fails due to the table is full, or
 *           disabled when more entries are deleted and the table has
 *           empty slots.
 *
 * @param    option - true (acknowledging all packets with pending field set)
 *                    false (acknowledging all packets with pending field cleared)
 *
 * @return   status
 ********************************************************************************************************/
ZMacStatus_t ZMacSrcMatchAckAllPending (uint8 option)
{
  MAC_SrcMatchAckAllPending (option);

  return ZMacSuccess;
}

/********************************************************************************************************
 * @fn       ZMacSrcMatchCheckAllPending
 *
 * @brief    This function is called to check if acknowledging all packets with pending bit set is enabled.
 *
 * @param    none
 *
 * @return   status
 ********************************************************************************************************/
ZMacStatus_t ZMacSrcMatchCheckAllPending (void)
{
  return (MAC_SrcMatchCheckAllPending ());
}

/********************************************************************************************************
 * @fn      - ZMACPwrOnReq
 *
 * @brief   - This function requests the MAC to power on the radio hardware
 *            and wake up.  When the power on procedure is complete the MAC
 *            will send a MAC_PWR_ON_CNF to the application.
 *
 * @input   - None.
 *
 * @output  - None.
 *
 * @return  - None.
 ********************************************************************************************************/
void ZMacPwrOnReq ( void )
{
  MAC_PwrOnReq();
}

/********************************************************************************************************
 * @fn          MAC_PwrMode
 *
 * @brief       This function returns the current power mode of the MAC.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      The current power mode of the MAC.
 ********************************************************************************************************/
uint8 ZMac_PwrMode(void)
{
  return (MAC_PwrMode());
}

/********************************************************************************************************
 * @fn      ZMacSetTransmitPower
 *
 * @brief   Set the transmitter power according to the level setting param.
 *
 * @param   Valid power level setting as defined in ZMAC.h.
 *
 * @return  ZMacSuccess if PHY_TRANSMIT_POWER found or ZMacUnsupportedAttribute.
 ********************************************************************************************************/
uint8 ZMacSetTransmitPower( ZMacTransmitPower_t level )
{
  return MAC_MlmeSetReq( ZMacPhyTransmitPowerSigned, &level );
}

/********************************************************************************************************
 * @fn      ZMacSendNoData
 *
 * @brief   This function sends an empty msg
 *
 * @param   DstAddr   - destination short address
 *          DstPANId  - destination pan id
 *
 * @return  None
 ********************************************************************************************************/
void ZMacSendNoData ( uint16 DstAddr, uint16 DstPANId )
{
  macMcpsDataReq_t *pBuf;

  /* Allocate memory */
  pBuf = MAC_McpsDataAlloc(0, MAC_SEC_LEVEL_NONE, MAC_KEY_ID_MODE_NONE);

  if (pBuf)
  {
    /* Fill in src information */
    pBuf->mac.srcAddrMode              = SADDR_MODE_SHORT;

    /* Fill in dst information */
    pBuf->mac.dstAddr.addr.shortAddr   = DstAddr;
    pBuf->mac.dstAddr.addrMode         = SADDR_MODE_SHORT;
    pBuf->mac.dstPanId                 = DstPANId;

    /* Misc information */
    pBuf->mac.msduHandle               = 0;
    pBuf->mac.txOptions                = ZMAC_TXOPTION_ACK | ZMAC_TXOPTION_NO_RETRANS | ZMAC_TXOPTION_NO_CNF;

    /* Right now, set security to zero */
    pBuf->sec.securityLevel = false;

    /* Call Mac Data Request */
    MAC_McpsDataReq(pBuf);
  }

}

/********************************************************************************************************
 * @fn      ZMacStateIdle
 *
 * @brief   This function returns true if the MAC state is idle.
 *
 * @param   none
 *
 * @return  TRUE if the MAC state is idle, FALSE otherwise.
 ********************************************************************************************************/
uint8 ZMacStateIdle( void )
{
  return macStateIdle();
}
