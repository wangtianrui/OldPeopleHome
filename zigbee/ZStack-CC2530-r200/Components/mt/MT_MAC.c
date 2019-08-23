/**************************************************************************************************
  Filename:       MT_MAC.c
  Revised:        $Date: 2011-03-25 18:16:38 -0700 (Fri, 25 Mar 2011) $
  Revision:       $Revision: 25528 $

  Description:    MonitorTest functions for the MAC layer.


  Copyright 2004-2011 Texas Instruments Incorporated. All rights reserved.

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

#if defined (MT_MAC_FUNC) || defined (MT_MAC_CB_FUNC)  //MAC commands
/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "OSAL.h"
#include "MT.h"
#include "MT_UART.h"
#include "ZMAC.h"
#include "MT_MAC.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* MAC radio */
#include "mac_radio_defs.h"

/* Hal */
#include "hal_uart.h"

#ifdef MAC_SECURITY
  #include "mac_security_pib.h"
#endif

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

/* The length in bytes of the pending address fields in the beacon */
#define MT_MAC_PEND_LEN(pendAddrSpec)   ((((pendAddrSpec) & 0x07) * 2) + \
                                        ((((pendAddrSpec) & 0x70) >> 4) * 8))

/* This matches the value used by nwk */
#define MT_MAC_ED_SCAN_MAXCHANNELS      27

/* Maximum size of pending address spec in beacon notify ind */
#define MT_MAC_PEND_LEN_MAX             32

/* Maximum size of the payload SDU in beacon notify ind */
#define MT_MAC_SDU_LEN_MAX              32

/* Maximum length of scan result in bytes */
#define MT_MAC_SCAN_RESULT_LEN_MAX      32

/* Maximum size of beacon payload */
#define MT_MAC_BEACON_PAYLOAD_MAX       16

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
#define DEFAULT_NSDU_HANDLE             0x00

#define MT_MAC_LEN_ASSOCIATE_IND        0x14          /* Associate Indication */
#define MT_MAC_LEN_ASSOCIATE_CNF        0x0E          /* Associate Confirmation */
#define MT_MAC_LEN_DISASSOCIATE_IND     0x14          /* Disassociate Indication */
#define MT_MAC_LEN_DISASSOCIATE_CNF     0x0c          /* Disassociate Confirmation */
#define MT_MAC_LEN_BEACON_NOTIFY_IND    0x63          /* Beacon Notification */
#define MT_MAC_LEN_ORPHAN_IND           0x13          /* Orphan Indication */
#define MT_MAC_LEN_SCAN_CNF             0x09          /* Scan Confirmation */
#define MT_MAC_LEN_SYNC_LOSS_IND        0x10          /* Sync Loss Indication */
#define MT_MAC_LEN_COMM_STATUS_IND      0x21          /* Comm Status Indication */
#define MT_MAC_LEN_DATA_CNF             0x08          /* Data Confirmation */
#define MT_MAC_LEN_DATA_IND             0x2C          /* Data Indication */
#define MT_MAC_LEN_PURGE_CNF            0x02          /* Purge Confirmation */

/***************************************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************************************/
uint16 _macCallbackSub;

/* storage for MAC beacon payload */
static uint8 mtMacBeaconPayload[MT_MAC_BEACON_PAYLOAD_MAX];

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
static void MT_MacSpi2Sec( ZMacSec_t *pSec, uint8 *pSrc );
static void MT_MacSpi2Addr( zAddrType_t *pDst, uint8 *pSrc );
static void MT_MacAddr2Spi( uint8 *pDst, zAddrType_t *pSrc );
static void MT_MacExtCpy( uint8 *pDst, uint8 *pSrc );
static void MT_MacRevExtCpy( uint8 *pDst, uint8 *pSrc );

void MT_MacResetReq(uint8 *pBuf);
void MT_MacInit(uint8 *pBuf);
void MT_MacStartReq(uint8 *pBuf);
void MT_MacSyncReq(uint8 *pBuf);
void MT_MacDataReq(uint8 *pBuf);
void MT_MacAssociateReq(uint8 *pBuf);
void MT_MacDisassociateReq(uint8 *pBuf);
void MT_MacGetReq(uint8 *pBuf);
void MT_MacSetReq(uint8 *pBuf);
#ifdef MAC_SECURITY
void MT_MacSecurityGetReq(uint8 *pBuf);
void MT_MacSecuritySetReq(uint8 *pBuf);
#endif
void MT_MacScanReq(uint8 * pBuf);
void MT_MacPollReq(uint8 *pBuf);
void MT_MacPurgeReq(uint8 *pBuf);
void MT_MacSetRxGainReq(uint8 *pBuf);
void MT_MacAssociateRsp(uint8 *pBuf);
void MT_MacOrphanRsp(uint8 *pBuf);

/***************************************************************************************************
 * @fn      MT_MacCommandProcessing
 *
 * @brief   Process all the MAC commands that are issued by test tool
 *
 * @param   pBuf - pointer to the msg buffer
 *
 *          | LEN  | CMD0  | CMD1  |  DATA  |
 *          |  1   |   1   |   1   |  0-255 |
 *
 * @return  void
 ***************************************************************************************************/
uint8 MT_MacCommandProcessing (uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_MAC_RESET_REQ:
      MT_MacResetReq(pBuf);
      break;

    case MT_MAC_INIT:
      MT_MacInit(pBuf);
      break;

    case MT_MAC_START_REQ:
      MT_MacStartReq(pBuf);
      break;

    case MT_MAC_SYNC_REQ:
      MT_MacSyncReq(pBuf);
      break;

    case MT_MAC_DATA_REQ:
      MT_MacDataReq(pBuf);
      break;

    case MT_MAC_ASSOCIATE_REQ:
      MT_MacAssociateReq(pBuf);
      break;

    case MT_MAC_DISASSOCIATE_REQ:
      MT_MacDisassociateReq(pBuf);
      break;

    case MT_MAC_GET_REQ:
      MT_MacGetReq(pBuf);
      break;

    case MT_MAC_SET_REQ:
      MT_MacSetReq(pBuf);
      break;

#ifdef MAC_SECURITY      
    case MT_MAC_SECURITY_GET_REQ:
      MT_MacSecurityGetReq(pBuf);
      break;

    case MT_MAC_SECURITY_SET_REQ:
      MT_MacSecuritySetReq(pBuf);
      break;
#endif /* MAC_SECURITY */      

    case MT_MAC_GTS_REQ:
      /* Not supported */
      break;

    case MT_MAC_SCAN_REQ:
      MT_MacScanReq(pBuf);
      break;

    case MT_MAC_POLL_REQ:
      MT_MacPollReq(pBuf);
      break;

    case MT_MAC_PURGE_REQ:
      MT_MacPurgeReq(pBuf);
      break;

    case MT_MAC_SET_RX_GAIN_REQ:
      MT_MacSetRxGainReq(pBuf);
      break;

    case MT_MAC_ASSOCIATE_RSP:
      MT_MacAssociateRsp(pBuf);
      break;

    case MT_MAC_ORPHAN_RSP:
      MT_MacOrphanRsp(pBuf);
      break;

    default:
    status = MT_RPC_ERR_COMMAND_ID;
    break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_MacReset
 *
 * @brief   Process MAC Reset command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacResetReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = ZMacReset(*pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacInit
 *
 * @brief   Process Mac Init command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacInit(uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = ZMacInit();

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacStart
 *
 * @brief   Process MAC Start command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacStartReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;
#ifdef RTR_NWK
  ZMacStartReq_t startReq;
#endif

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef RTR_NWK

  /* StartTime */
  startReq.StartTime = BUILD_UINT32 (pBuf[0], pBuf[1], pBuf[2], pBuf[3]);
  pBuf += 4;

  /* PanID */
  startReq.PANID = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf += 2;

  /* Fill in other fields sequentially incrementing the pointer*/

  startReq.LogicalChannel    =  *pBuf++;
  startReq.ChannelPage       =  *pBuf++;
  startReq.BeaconOrder       =  *pBuf++;
  startReq.SuperframeOrder   =  *pBuf++;
  startReq.PANCoordinator    =  *pBuf++;
  startReq.BatteryLifeExt    =  *pBuf++;
  startReq.CoordRealignment  =  *pBuf++;

  /* Realign Security Information */
  MT_MacSpi2Sec( &startReq.RealignSec, pBuf );
  pBuf += ZTEST_DEFAULT_SEC_LEN;

  /* Beacon Security Information */
  MT_MacSpi2Sec( &startReq.BeaconSec, pBuf );

  /* Call corresponding ZMAC function */
  retValue = ZMacStartReq( &startReq );

#else

  retValue = ZMacDenied;

#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacSync
 *
 * @brief   Process MAC Sync command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacSyncReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacSyncReq_t syncReq;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* LogicalChannel */
  syncReq.LogicalChannel = *pBuf++;

  /* ChannelPage */
  syncReq.ChannelPage = *pBuf++;

  /* TrackBeacon */
  syncReq.TrackBeacon    = *pBuf;

  /* Call corresponding ZMAC function */
  retValue = ZMacSyncReq( &syncReq );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacDataReq
 *
 * @brief   Process MAC Data Request command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacDataReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacDataReq_t dataReq;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

   /* Destination address mode */
  dataReq.DstAddr.addrMode = *pBuf++;

  /* Destination address */
  MT_MacSpi2Addr( &dataReq.DstAddr, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* Destination Pan ID */
  dataReq.DstPANId = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf += 2;

  /* Source address mode */
  dataReq.SrcAddrMode = *pBuf++;

  /* Handle */
  dataReq.Handle = *pBuf++;

  /* TxOptions */
  dataReq.TxOptions = *pBuf++;

  /* Channel */
  dataReq.Channel = *pBuf++;

  /* Power */
  dataReq.Power = *pBuf++;

  /* Data Security */
  MT_MacSpi2Sec( &dataReq.Sec, pBuf );
  pBuf += ZTEST_DEFAULT_SEC_LEN;

  /* Data length */
  dataReq.msduLength = *pBuf++;

  /* Data - Just pass the pointer to the structure */
  dataReq.msdu = pBuf;

  /* Call corresponding ZMAC function */
  retValue = ZMacDataReq( &dataReq );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacAssociateReq
 *
 * @brief   Process MAC Get Request command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacAssociateReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacAssociateReq_t    assocReq;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Logical Channel */
  assocReq.LogicalChannel = *pBuf++;

  /* Channel Page */
  assocReq.ChannelPage = *pBuf++;

  /* Address Mode */
  assocReq.CoordAddress.addrMode = *pBuf++;

  /* Coordinator Address, address mode must be set at this point */
  MT_MacSpi2Addr( &assocReq.CoordAddress, pBuf );
  pBuf += Z_EXTADDR_LEN;

  /* Coordinator PanID */
  assocReq.CoordPANId = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf += 2;

  /* Capability information */
  assocReq.CapabilityFlags = *pBuf++;

  /* Security Information */
  MT_MacSpi2Sec( &assocReq.Sec, pBuf );

  /* Call corresponding ZMAC function */
  retValue = ZMacAssociateReq( &assocReq );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacDisassociateReq
 *
 * @brief   Process MAC Get Request command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacDisassociateReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacDisassociateReq_t disassocReq;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Device address mode */
  disassocReq.DeviceAddress.addrMode = *pBuf++;

  /* Device address - Device address mode have to be set to use this function*/
  MT_MacSpi2Addr( &disassocReq.DeviceAddress, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* Pan ID */
  disassocReq.DevicePanId = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf += 2;

  /* Disassociate reason */
  disassocReq.DisassociateReason = *pBuf++;

  /* TxIndirect */
  disassocReq.TxIndirect = *pBuf++;

  /* Security Information */
  MT_MacSpi2Sec( &disassocReq.Sec, pBuf );

  /* Call corresponding ZMAC function */
  retValue = ZMacDisassociateReq( &disassocReq );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacGetReq
 *
 * @brief   Process MAC Get Request command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacGetReq(uint8 *pBuf)
{
  uint8 respLen, cmdId, attr;
  uint8 *pRetBuf;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Response length is 16 bytes + 1 byte status */
  respLen = ZTEST_DEFAULT_PARAM_LEN;

  /* Allocate */
  pRetBuf = osal_mem_alloc(respLen);

  /* Attribute to be read */
  attr = *pBuf;

  if (pRetBuf)
  {
    /* Zero everything */
    osal_memset(pRetBuf, 0, respLen);
    /* Default to ZFailure */
    pRetBuf[0] = ZFailure;
    /* Read the pib value */
    pRetBuf[0] = ZMacGetReq(attr, &pRetBuf[1]);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, respLen, pRetBuf );

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn      MT_MacSetReq
 *
 * @brief   Process MAC Set Req command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacSetReq(uint8 *pBuf)
{
  uint8 retValue, cmdId, attr;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /*
    In the data field of 'msg', the first byte is the attribute and remainder
    is the attribute value. So the pointer 'pBuf' points directly to the attribute.
    The value of the attribute is from the next byte position
  */
  attr = *pBuf;

  /* special case for beacon payload */
  if ( attr == ZMacBeaconMSDU )
  {
    osal_memcpy( mtMacBeaconPayload, pBuf + 1, MT_MAC_BEACON_PAYLOAD_MAX );
    retValue = ZMacSetReq( (ZMacAttributes_t)attr ,  (byte *) &mtMacBeaconPayload );
  }
  else
  {
    retValue = ZMacSetReq( (ZMacAttributes_t)attr , pBuf + 1 );
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

#ifdef MAC_SECURITY
/***************************************************************************************************
 * @fn      MT_MacSecurityGetReq
 *
 * @brief   Process MAC Security Get Request command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacSecurityGetReq(uint8 *pBuf)
{
  uint8 respLen, cmdId, attr;
  uint8 *pRetBuf;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Response length is 25 bytes + 2 bytes index + 1 byte status */
  respLen = ZTEST_DEFAULT_SEC_PARAM_LEN;

  /* Allocate */
  pRetBuf = osal_mem_alloc(respLen);

  /* Attribute to be read */
  attr = *pBuf++;

  if (pRetBuf)
  {
    /* Zero everything */
    osal_memset(pRetBuf, 0, respLen);
    
    switch (attr)
    {
      case MAC_KEY_ID_LOOKUP_ENTRY:
      case MAC_KEY_DEVICE_ENTRY:
      case MAC_KEY_USAGE_ENTRY:
        /* These security PIBs have two parameters */
        pRetBuf[1] = *pBuf++;
        pRetBuf[2] = *pBuf;
        break;
      case MAC_KEY_ENTRY:
      case MAC_DEVICE_ENTRY:
      case MAC_SECURITY_LEVEL_ENTRY:
        /* These security PIBs have one parameter */
        pRetBuf[1] = *pBuf;
        break;      
    }
    
    /* Other MAC Security PIB items. Read the pib value */
    pRetBuf[0] = ZMacSecurityGetReq(attr, &pRetBuf[1]);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, respLen, pRetBuf );

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn      MT_MacSecuritySetReq
 *
 * @brief   Process MAC Set Req command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacSecuritySetReq(uint8 *pBuf)
{
  uint8 cmdId, attr;
  uint8 retValue = ZMAC_SUCCESS;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /*
    In the data field of 'msg', the first byte is the attribute and remainder
    is the attribute value. So the pointer 'pBuf' points directly to the attribute.
    The value of the attribute is from the next byte position
  */
  attr = *pBuf++;
  if (attr == MAC_KEY_TABLE || attr == MAC_DEVICE_TABLE || attr == MAC_SECURITY_LEVEL_TABLE)
  {
    pBuf = NULL;
  }
  retValue = ZMacSecuritySetReq( (ZMacAttributes_t)attr , pBuf );      

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}
#endif /* MAC_SECURITY */

/***************************************************************************************************
 * @fn      MT_MacScanReq
 *
 * @brief   Process MAC Scan Req command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacScanReq(uint8 * pBuf)
{
  uint8 retValue, cmdId;
  ZMacScanReq_t scanReq;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* ScanChannels is the 32-bit channel list */
  scanReq.ScanChannels = BUILD_UINT32 (pBuf[0], pBuf[1], pBuf[2], pBuf[3]);
  pBuf += 4;

  /* Fill in fields sequentially incrementing the pointer */
  scanReq.ScanType = *pBuf++;

  /* ScanDuration */
  scanReq.ScanDuration = *pBuf++;

  /* Channel Page */
  scanReq.ChannelPage = *pBuf++;

  /* MaxResults */
  scanReq.MaxResults = *pBuf++;

  /* Security Information */
  MT_MacSpi2Sec( &scanReq.Sec, pBuf );

  /* Call corresponding ZMAC function */
  retValue =  ZMacScanReq( &scanReq );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacPollReq
 *
 * @brief   Process MAC Poll Req command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacPollReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacPollReq_t pollReq;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Coordinator address mode */
  pollReq.CoordAddress.addrMode = *pBuf++;

  /* Coordinator address - Device address mode have to be set to use this function */
  MT_MacSpi2Addr( &pollReq.CoordAddress, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* Coordinator Pan ID */
  pollReq.CoordPanId = BUILD_UINT16(pBuf[0] , pBuf[1]);
  pBuf += 2;

  /* Security Information */
  MT_MacSpi2Sec(&pollReq.Sec, pBuf);

  /* Call corresponding ZMAC function */
  retValue = ZMacPollReq( &pollReq );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacPurgeReq
 *
 * @brief   Process MAC Purge Req command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacPurgeReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* First and only byte - MsduHandle */
  retValue = ZMacPurgeReq (*pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_MacSetRxGainReq
 *
 * @brief   Process MAC Rx Gain Req command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacSetRxGainReq(uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef HAL_PA_LNA
  /* Toggle PA/LNA mode */
  if (*pBuf)
  {
    HAL_PA_LNA_RX_HGM();
  }
  else
  {
    HAL_PA_LNA_RX_LGM();
  }
  retValue = ZSuccess;
#else
  retValue = ZFailure;
#endif


  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn          MT_MacAssociateRsp
 *
 * @brief       Process MAC Associate Rsp command that are issued by test tool
 *
 * @param       pBuf - Buffer contains the data
 *
 * @return      void
 ***************************************************************************************************/
void MT_MacAssociateRsp(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacAssociateRsp_t assocRsp;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef RTR_NWK

  /* The address of the device requesting association */
  MT_MacExtCpy(assocRsp.DeviceAddress, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* The short address allocated to the (associated) device */
  assocRsp.AssocShortAddress = BUILD_UINT16(pBuf[0],pBuf[1]);
  pBuf += 2;

  /* Status of the association */
  assocRsp.Status = *pBuf++;

  /* Security Information */
  MT_MacSpi2Sec( &assocRsp.Sec, pBuf );

  /* Call corresponding ZMAC function */
  retValue = ZMacAssociateRsp( &assocRsp );

#else

  retValue = ZMacDenied;

#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_MacOrphanRsp
 *
 * @brief   Process MAC Orphan Response command that are issued by test tool
 *
 * @param   pBuf - Buffer contains the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_MacOrphanRsp(uint8 *pBuf)
{
  uint8 retValue, cmdId;
  ZMacOrphanRsp_t orphanRsp;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Extended address of the device sending the notification */
  MT_MacRevExtCpy( orphanRsp.OrphanAddress, pBuf );
  pBuf += Z_EXTADDR_LEN;

  /* Short address of the orphan device */
  orphanRsp.ShortAddress = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf += 2;

  /* Associated member */
  orphanRsp.AssociatedMember = *pBuf++;

  /* Security Information */
  MT_MacSpi2Sec( &orphanRsp.Sec, pBuf );

  /* Call corresponding ZMAC function */
  retValue = ZMacOrphanRsp( &orphanRsp );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_MAC), cmdId, 1, &retValue);
}

#if defined ( MT_MAC_CB_FUNC )

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkSyncLossInd
 *
 * @brief       Process the callback subscription for nwk_sync_loss_ind
 *
 * @param       byte LossReason
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkSyncLossInd( ZMacSyncLossInd_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_SYNC_LOSS_IND;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /*  Status - loss reason */
    *tp++ = param->hdr.Status;

    /* Pan Id */
    *tp++ = LO_UINT16( param->PANId );
    *tp++ = HI_UINT16( param->PANId );

    /* Logical Channel */
    *tp++ = param->LogicalChannel;

    /* Channel Page */
    *tp++ = param->ChannelPage;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_SYNC_LOSS_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkAssociateInd
 *
 * @brief       Process the callback subscription for nwk_associate_ind
 *
 * @param       pointer of type macnwk_associate_ind_t
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkAssociateInd( ZMacAssociateInd_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_ASSOCIATE_IND;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Extended address */
    MT_MacAddr2Spi(pRetBuf, (zAddrType_t*)param->DeviceAddress);
    tp += Z_EXTADDR_LEN;

    /* Capability Information */
    *tp++ = param->CapabilityFlags;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_ASSOCIATE_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkAssociateCnf
 *
 * @brief       Process the callback subscription for nwk_associate_cnf
 *
 * @param       pointer of type macnwk_associate_cnf_t
 *
 * @return      SUCCESS if message sent succesfully , else N_FAIL
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkAssociateCnf( ZMacAssociateCnf_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_ASSOCIATE_CNF;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Status */
    *tp++ = param->hdr.Status;

    /* Short address */
    *tp++ = LO_UINT16( param->AssocShortAddress );
    *tp++ = HI_UINT16( param->AssocShortAddress );

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_ASSOCIATE_CNF, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkBeaconNotifyInd
 *
 * @brief       Process the callback subscription for
 *              beacon_notify_ind.
 *
 * @param       pointer to ZMacBeaconNotifyInd_t
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkBeaconNotifyInd ( ZMacBeaconNotifyInd_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_BEACON_NOTIFY_IND;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* BSN */
    *tp++ = param->BSN;

    /* Timestamp */
    *tp++ = BREAK_UINT32( param->pPanDesc->TimeStamp, 0 );
    *tp++ = BREAK_UINT32( param->pPanDesc->TimeStamp, 1 );
    *tp++ = BREAK_UINT32( param->pPanDesc->TimeStamp, 2 );
    *tp++ = BREAK_UINT32( param->pPanDesc->TimeStamp, 3 );

    /* Coordinator address mode */
    *tp++ = param->pPanDesc->CoordAddress.addrMode;

    /* Coordinator address */
    MT_MacAddr2Spi( tp, &param->pPanDesc->CoordAddress );
    tp += Z_EXTADDR_LEN;

    /* PAN ID */
    *tp++ = LO_UINT16( param->pPanDesc->CoordPANId );
    *tp++ = HI_UINT16( param->pPanDesc->CoordPANId );

    /* Superframe spec */
    *tp++ = LO_UINT16( param->pPanDesc->SuperframeSpec );
    *tp++ = HI_UINT16( param->pPanDesc->SuperframeSpec );

    /* LogicalChannel */
    *tp++ = param->pPanDesc->LogicalChannel;

    /* GTSPermit */
    *tp++ = param->pPanDesc->GTSPermit;

    /* LinkQuality */
    *tp++ = param->pPanDesc->LinkQuality;

    /* SecurityFailure */
    *tp++ = param->pPanDesc->SecurityFailure;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->pPanDesc->Sec);
    tp += ZTEST_DEFAULT_SEC_LEN;

    /* PendingAddrSpec */
    *tp++ = param->PendAddrSpec;

    /* AddrList */
    osal_memset( tp, 0, MT_MAC_PEND_LEN_MAX );
    osal_memcpy( tp, param->AddrList, MIN(MT_MAC_PEND_LEN_MAX, MT_MAC_PEND_LEN(param->PendAddrSpec)) );
    tp += MT_MAC_PEND_LEN_MAX;

    /* SDULength */
    *tp++ = param->sduLength;

    /* SDU */
    osal_memset( tp, 0, MT_MAC_SDU_LEN_MAX );
    osal_memcpy( tp, param->sdu, MIN(MT_MAC_SDU_LEN_MAX, param->sduLength) );

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_BEACON_NOTIFY_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);

  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkDataCnf
 *
 * @brief       Process the callback subscription for nwk_data_cnf
 *
 * @param       pointer of type macnwk_data_cnf_t
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkDataCnf( ZMacDataCnf_t *param )
{
  uint8 *pRetBuf, *tp;

  /* Allocate */
  pRetBuf = osal_mem_alloc(MT_MAC_LEN_DATA_CNF);

  if (pRetBuf)
  {
    tp = pRetBuf;

    /* Status */
    *tp++ = param->hdr.Status;

    /* Handle */
    *tp++ = param->msduHandle;

    /* Timestamp */
    *tp++ = BREAK_UINT32( param->Timestamp, 0 );
    *tp++ = BREAK_UINT32( param->Timestamp, 1 );
    *tp++ = BREAK_UINT32( param->Timestamp, 2 );
    *tp++ = BREAK_UINT32( param->Timestamp, 3 );

    /* Timestamp2 */
    *tp++ = LO_UINT16( param->Timestamp2);
    *tp = HI_UINT16( param->Timestamp2);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_DATA_CNF, MT_MAC_LEN_DATA_CNF, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkDataInd
 *
 * @brief       Process the callback subscription for nwk_data_ind
 *
 * @param       pointer of type macnwk_data_ind_t
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkDataInd( ZMacDataInd_t *param )
{
  uint8 respLen, tempLen;
  uint8 *pRetBuf, *tp;

  /* Packet length is 44 + 102 data */
  respLen = MT_MAC_LEN_DATA_IND + ZTEST_DEFAULT_DATA_LEN;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Src address mode */
    *tp++ = param->SrcAddr.addrMode;

    if (param->SrcAddr.addrMode != SADDR_MODE_NONE)
    {
      /* Src Address */
      MT_MacAddr2Spi( tp, &param->SrcAddr );
    }
    else
    {
      /* No address */
      for ( uint8 i = 0; i< Z_EXTADDR_LEN; i++ )
      {
        tp[i] = 0x00;
      }
    }
    tp += Z_EXTADDR_LEN;

    /* Dst address mode */
    *tp++ = param->DstAddr.addrMode;

    /* Dst address */
    MT_MacAddr2Spi( tp, &param->DstAddr );
    tp += Z_EXTADDR_LEN;

       /* Timestamp */
    *tp++ = BREAK_UINT32( param->Timestamp, 0 );
    *tp++ = BREAK_UINT32( param->Timestamp, 1 );
    *tp++ = BREAK_UINT32( param->Timestamp, 2 );
    *tp++ = BREAK_UINT32( param->Timestamp, 3 );

    /* Timestamp2 */
    *tp++ = LO_UINT16( param->Timestamp2);
    *tp++ = HI_UINT16( param->Timestamp2);

    /* Src Pan Id */
    *tp++ = LO_UINT16( param->SrcPANId );
    *tp++ = HI_UINT16( param->SrcPANId );

    /* Dst Pan Id */
    *tp++ = LO_UINT16( param->DstPANId );
    *tp++ = HI_UINT16( param->DstPANId );

    /* mpdu Link Quality */
    *tp++ = param->mpduLinkQuality;

    /* LQI */
    *tp++ = param->Correlation;

    /* RSSI */
    *tp++ = param->Rssi;

    /* DSN */
    *tp++ = param->Dsn;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);
    tp += ZTEST_DEFAULT_SEC_LEN;

    /* Determine the length of the actual data */
    if ( param->msduLength < ZTEST_DEFAULT_DATA_LEN )
      tempLen = param->msduLength;
    else
      tempLen = ZTEST_DEFAULT_DATA_LEN;

    /* Length */
    *tp++ = tempLen;

    /* Copy the data according to Len, the rest are zeroed out */
    osal_memset( tp, 0, ZTEST_DEFAULT_DATA_LEN );
    osal_memcpy( tp, param->msdu, tempLen );

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_DATA_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkDisassociateInd
 *
 * @brief       Process the callback subscription for nwk_disassociate_ind
 *
 * @param       pointer of type macnwk_disassociate_ind_t
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkDisassociateInd( ZMacDisassociateInd_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_DISASSOCIATE_IND;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Extended address */
    MT_MacRevExtCpy( pRetBuf, param->DeviceAddress );
    tp += Z_EXTADDR_LEN;

    /* Disassociate Reason */
    *tp++ = param->DisassociateReason;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_DISASSOCIATE_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkDisassociateCnf
 *
 * @brief       Process the callback subscription for nwk_disassociate_cnf
 *
 * @param       param
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkDisassociateCnf( ZMacDisassociateCnf_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_DISASSOCIATE_CNF;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Status */
    *tp++ = param->hdr.Status;

    /* DeviceAddress */
    *tp++ = param->DeviceAddress.addrMode;

    /* Copy Address */
    MT_MacAddr2Spi( tp, &param->DeviceAddress );
    tp += Z_EXTADDR_LEN;

    /* Pan ID */
    *tp++ = LO_UINT16( param->panID );
    *tp = HI_UINT16( param->panID );

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_DISASSOCIATE_CNF, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkOrphanInd
 *
 * @brief       Process the callback subscription for nwk_orphan_ind
 *
 * @param       pointer of type macnwk_orphan_ind_t
 *
 * @return      SUCCESS if message sent succesfully , else N_FAIL
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkOrphanInd( ZMacOrphanInd_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_ORPHAN_IND;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Extended address */
    MT_MacRevExtCpy( tp, param->OrphanAddress );
    tp += Z_EXTADDR_LEN;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_ORPHAN_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkPollCnf
 *
 * @brief       Process the callback subscription for nwk_poll_cnf
 *
 * @param       byte Status
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkPollCnf( byte Status )
{
  uint8 retValue;

  /*The only data byte is Status */
  retValue = Status;

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_POLL_CNF, 1, &retValue);
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkScanCnf
 *
 * @brief       Process the callback subscription for nwk_scan_cnf
 *
 * @param       pointer of type macnwk_scan_cnf_t
 *
 * @return      SUCCESS if message sent succesfully , else N_FAIL
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkScanCnf( ZMacScanCnf_t *param )
{
  uint8 respLen, resultLen;
  uint8 *pRetBuf, *tp;

  /* Depends on the type of scan, calculate the required length */
  if ( param->ScanType == ZMAC_ED_SCAN )
    resultLen = MT_MAC_ED_SCAN_MAXCHANNELS;
  else if ( param->ScanType == ZMAC_ACTIVE_SCAN )
    resultLen = (param->ResultListSize * sizeof( ZMacPanDesc_t ));
  else if ( param->ScanType == ZMAC_PASSIVE_SCAN )
    resultLen = (param->ResultListSize * sizeof( ZMacPanDesc_t ));
  else if ( param->ScanType == ZMAC_ORPHAN_SCAN )
    resultLen = 0;
  else
    return;

  /* Make sure the result wont be more than the size */
  resultLen = MIN(resultLen, MT_MAC_SCAN_RESULT_LEN_MAX);

  respLen = MT_MAC_LEN_SCAN_CNF + MT_MAC_SCAN_RESULT_LEN_MAX + 1;  /* Extra byte for the length of the list */

  /* Allocate */
  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Status */
    *tp++ = param->hdr.Status;

    /* ED max energy parameter no longer used */
    *tp++ = 0;

    /* Scan type */
    *tp++ = param->ScanType;

    /* Channel page */
    *tp++ = param->ChannelPage;

    /* Unscanned channel list */
    *tp++ = BREAK_UINT32( param->UnscannedChannels, 0 );
    *tp++ = BREAK_UINT32( param->UnscannedChannels, 1 );
    *tp++ = BREAK_UINT32( param->UnscannedChannels, 2 );
    *tp++ = BREAK_UINT32( param->UnscannedChannels, 3 );

    /* Result count */
    *tp++ = param->ResultListSize;

    /* MAX length of the result List */
    *tp++ = MT_MAC_SCAN_RESULT_LEN_MAX;

    /* PAN descriptor information */
    osal_memcpy( tp, param->Result.pPanDescriptor, resultLen );

    /* clear extra buffer space at end, if any */
    osal_memset( tp, 0, (MT_MAC_SCAN_RESULT_LEN_MAX - resultLen));

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_SCAN_CNF, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubCommStatusInd
 *
 * @brief       Process the callback subscription for comm_status_ind.
 *
 * @param       None
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubCommStatusInd ( ZMacCommStatusInd_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_COMM_STATUS_IND;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Status */
    *tp++ = param->hdr.Status;

    /* Source address */
    *tp++ = param->SrcAddress.addrMode;
    MT_MacAddr2Spi( tp, &param->SrcAddress );
    tp += Z_EXTADDR_LEN;

    /* Destination address */
    *tp++ = param->DstAddress.addrMode;
    MT_MacAddr2Spi( tp, &param->DstAddress );
    tp += Z_EXTADDR_LEN;

    /* PAN ID */
    *tp++ = LO_UINT16( param->PANId );
    *tp++ = HI_UINT16( param->PANId );

    /* Reason */
    *tp++ = param->Reason;

    /* Security */
    MT_MacSpi2Sec ((ZMacSec_t *)tp, (uint8 *)&param->Sec);

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_COMM_STATUS_IND, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkStartCnf
 *
 * @brief       Process the callback subscription for nwk_start_cnf
 *
 * @param       byte Status
 *
 * @return      None
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkStartCnf( uint8 Status )
{
  uint8 retValue;

  retValue = Status;

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_START_CNF, 1, &retValue);
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkRxEnableCnf
 *
 * @brief       Process the callback subscription for nwk_Rx_Enable_cnf
 *
 * @param
 *
 * @return      SUCCESS if message sent succesfully , else N_FAIL
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkRxEnableCnf ( byte Status )
{
  uint8 retValue;

  /* The only data byte is Status */
  retValue = Status;

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_RX_ENABLE_CNF, 1, &retValue);
}

/***************************************************************************************************
 * @fn          nwk_MTCallbackSubNwkPurgeCnf
 *
 * @brief       Process the callback subscription for nwk_purge_cnf
 *
 * @param       pointer of type ZMacPurgeCnf_t
 *
 * @return      SUCCESS if message sent succesfully , else N_FAIL
 ***************************************************************************************************/
void nwk_MTCallbackSubNwkPurgeCnf( ZMacPurgeCnf_t *param )
{
  uint8 respLen;
  uint8 *pRetBuf, *tp;

  respLen = MT_MAC_LEN_PURGE_CNF;

  pRetBuf = osal_mem_alloc (respLen);

  if ( pRetBuf )
  {
    tp = pRetBuf;

    /* Status */
    *tp++ = param->hdr.Status;

    /* Handle */
    *tp = param->msduHandle;

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_MAC), MT_MAC_PURGE_CNF, respLen, pRetBuf);

    /* Deallocate */
    osal_mem_free(pRetBuf);
  }
}

#endif // MT_MAC_CB_FUNC

/***************************************************************************************************
 * SUPPORT
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      MT_MacExtCpy
 *
 * @brief
 *
 *   Copy an extended address.
 *
 * @param   pDst - Pointer to data destination
 * @param   pSrc - Pointer to data source
 *
 * @return  void
 ***************************************************************************************************/
static void MT_MacExtCpy( uint8 *pDst, uint8 *pSrc )
{
  int8 i;

  for ( i = 0; i < Z_EXTADDR_LEN; i++ )
  {
    *pDst++ = pSrc[i];
  }
}

/***************************************************************************************************
 * @fn      MT_MacRevExtCpy
 *
 * @brief
 *
 *   Reverse-copy an extended address.
 *
 * @param   pDst - Pointer to data destination
 * @param   pSrc - Pointer to data source
 *
 * @return  void
 ***************************************************************************************************/
static void MT_MacRevExtCpy( uint8 *pDst, uint8 *pSrc )
{
  int8 i;

  for ( i = Z_EXTADDR_LEN - 1; i >= 0; i-- )
  {
    *pDst++ = pSrc[i];
  }
}

/***************************************************************************************************
 * @fn      MT_MacSpi2Addr
 *
 * @brief   Copy an address from an SPI message to an address struct.  The
 *          addrMode in pAddr must already be set.
 *
 * @param   pDst - Pointer to address struct
 * @param   pSrc - Pointer SPI message byte array
 *
 * @return  void
 ***************************************************************************************************/
static void MT_MacSpi2Addr( zAddrType_t *pDst, uint8 *pSrc )
{
  if ( pDst->addrMode == Addr16Bit )
  {
    pDst->addr.shortAddr = BUILD_UINT16( pSrc[0] , pSrc[1] );
  }
  else if ( pDst->addrMode == Addr64Bit )
  {
    MT_MacRevExtCpy( pDst->addr.extAddr, pSrc );
  }
}

/***************************************************************************************************
 * @fn      MT_MacSpi2Sec
 *
 * @brief   Copy Security information from SPI message to a Sec structure
 *
 * @param   pSec - Pointer to security struct
 * @param   pSrc - Pointer SPI message byte array
 *
 * @return  void
 ***************************************************************************************************/
static void MT_MacSpi2Sec( ZMacSec_t *pSec, uint8 *pSrc )
{
  /* Copy the security structure directly from the byte array */
  osal_memcpy (pSec, pSrc, sizeof (ZMacSec_t));
}

/***************************************************************************************************
 * @fn      MT_MacAddr2Spi
 *
 * @brief   Copy an address from an address struct to an SPI message.
 *
 * @param   pDst - Pointer SPI message byte array
 * @param   pSrc - Pointer to address struct
 *
 * @return  void
 ***************************************************************************************************/
static void MT_MacAddr2Spi( uint8 *pDst, zAddrType_t *pSrc )
{
  if ( pSrc->addrMode == Addr16Bit )
  {
    *pDst++ = LO_UINT16( pSrc->addr.shortAddr );
    *pDst++ = HI_UINT16( pSrc->addr.shortAddr );
    *pDst++ = 0; *pDst++ = 0; *pDst++ = 0;
    *pDst++ = 0; *pDst++ = 0; *pDst = 0;
  }
  else if ( pSrc->addrMode == Addr64Bit )
  {
    for ( uint8 i = 0; i< Z_EXTADDR_LEN; i++ )
    {
     *pDst++ = pSrc->addr.extAddr[i];
    }
  }
  else
  {
    for ( uint8 i = 0; i< Z_EXTADDR_LEN; i++ )
    {
     *pDst++ = pSrc->addr.extAddr[i];
    }
  }
}

/***************************************************************************************************
 ***************************************************************************************************/
#endif // MT_MAC_FUNC
