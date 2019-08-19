/***************************************************************************************************
  Filename:       MT_UTIL.c
  Revised:        $Date: 2011-07-15 15:48:37 -0700 (Fri, 15 Jul 2011) $
  Revision:       $Revision: 26799 $

  Description:    MonitorTest Utility Functions

  Copyright 2007-2011 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

 ***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"

#include "AddrMgr.h"

#include "OnBoard.h"   /* This is here because of the key reading */
#include "hal_key.h"
#include "hal_led.h"
#include "OSAL_Nv.h"
#include "osal.h"
#include "NLMEDE.h"
#include "MT.h"
#include "MT_UTIL.h"
#include "MT_MAC.h"
#include "ssp.h"
#if defined ZCL_KEY_ESTABLISH
#include "zcl_key_establish.h"
#endif
#if defined TC_LINKKEY_JOIN
#include "zcl_se.h"
#endif

#if !defined NONWK
#include "MT_ZDO.h"
#include "MT_SAPI.h"
#include "MT_NWK.h"
#include "MT_AF.h"
#include "AssocList.h"
#include "ZDApp.h"
#include "ZDSecMgr.h"
#endif
/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
#define MT_UTIL_DEVICE_INFO_RESPONSE_LEN 14
#define MT_UTIL_STATUS_LEN    1
#define MT_UTIL_FRM_CTR_LEN   4
// Status + LinkKeyDataLen + Tx+Rx Frame counter.
#define MT_APSME_LINKKEY_GET_RSP_LEN (MT_UTIL_STATUS_LEN + SEC_KEY_LEN + (MT_UTIL_FRM_CTR_LEN * 2))
// Status + NV id
#define MT_APSME_LINKKEY_NV_ID_GET_RSP_LEN (MT_UTIL_STATUS_LEN + 2)

/***************************************************************************************************
 * LOCAL VARIABLES
 ***************************************************************************************************/
#if defined ZCL_KEY_ESTABLISH
uint8 zcl_key_establish_task_id;
#endif

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
#ifdef AUTO_PEND
static void MT_UtilRevExtCpy( uint8 *pDst, uint8 *pSrc );
static void MT_UtilSpi2Addr( zAddrType_t *pDst, uint8 *pSrc );
#endif

#if defined (MT_UTIL_FUNC)
void MT_UtilGetDeviceInfo(void);
void MT_UtilGetNvInfo(void);
void MT_UtilSetPanID(uint8 *pBuf);
void MT_UtilSetChannels(uint8 *pBuf);
void MT_UtilSetSecLevel(uint8 *pBuf);
void MT_UtilSetPreCfgKey(uint8 *pBuf);
void MT_UtilCallbackSub(uint8 *pData);
void MT_UtilKeyEvent(uint8 *pBuf);
void MT_UtilTimeAlive(void);
void MT_UtilLedControl(uint8 *pBuf);
void MT_UtilSrcMatchEnable (uint8 *pBuf);
void MT_UtilSrcMatchAddEntry (uint8 *pBuf);
void MT_UtilSrcMatchDeleteEntry (uint8 *pBuf);
void MT_UtilSrcMatchCheckSrcAddr (uint8 *pBuf);
void MT_UtilSrcMatchAckAllPending (uint8 *pBuf);
void MT_UtilSrcMatchCheckAllPending (uint8 *pBuf);

#if !defined NONWK
void MT_UtilDataReq(uint8 *pBuf);
static void MT_UtilAddrMgrExtAddrLookup(uint8 *pBuf);
static void MT_UtilAddrMgrEntryLookupNwk(uint8 *pBuf);
#if defined MT_SYS_KEY_MANAGEMENT
static void MT_UtilAPSME_LinkKeyDataGet(uint8 *pBuf);
static void MT_UtilAPSME_LinkKeyNvIdGet(uint8 *pBuf);
#endif //MT_SYS_KEY_MANAGEMENT
static void MT_UtilAPSME_RequestKeyCmd(uint8 *pBuf);
static void MT_UtilAssocCount(uint8 *pBuf);
static void MT_UtilAssocFindDevice(uint8 *pBuf);
static void MT_UtilAssocGetWithAddress(uint8 *pBuf);
static void packDev_t(uint8 *pBuf, associated_devices_t *pDev);
#if defined ZCL_KEY_ESTABLISH
static void MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment(uint8 *pBuf);
static void MT_UtilzclGeneral_KeyEstablishment_ECDSASign(uint8 *pBuf);
#endif // ZCL_KEY_ESTABLISH
static void MT_UtilSync(void);
#endif // !defined NONWK
#endif // MT_UTIL_FUNC

#if defined (MT_UTIL_FUNC)
/***************************************************************************************************
 * @fn      MT_UtilProcessing
 *
 * @brief   Process all the DEBUG commands that are issued by test tool
 *
 * @param   pBuf  - pointer to received SPI data message
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_UtilCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
// CC253X MAC Network Processor does not have NV support
#if !defined(CC253X_MACNP)
    case MT_UTIL_GET_DEVICE_INFO:
      MT_UtilGetDeviceInfo();
      break;

    case MT_UTIL_GET_NV_INFO:
      MT_UtilGetNvInfo();
      break;

    case MT_UTIL_SET_PANID:
      MT_UtilSetPanID(pBuf);
      break;

    case MT_UTIL_SET_CHANNELS:
      MT_UtilSetChannels(pBuf);
      break;

    case MT_UTIL_SET_SECLEVEL:
      MT_UtilSetSecLevel(pBuf);
      break;

    case MT_UTIL_SET_PRECFGKEY:
      MT_UtilSetPreCfgKey(pBuf);
      break;
#endif
    case MT_UTIL_CALLBACK_SUB_CMD:
      MT_UtilCallbackSub(pBuf);
      break;

    case MT_UTIL_KEY_EVENT:
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
      MT_UtilKeyEvent(pBuf);
#endif
      break;

    case MT_UTIL_TIME_ALIVE:
      MT_UtilTimeAlive();
      break;

    case MT_UTIL_LED_CONTROL:
#if (defined HAL_LED) && (HAL_LED == TRUE)
      MT_UtilLedControl(pBuf);
#endif
      break;

    case MT_UTIL_SRC_MATCH_ENABLE:
      MT_UtilSrcMatchEnable(pBuf);
      break;

    case MT_UTIL_SRC_MATCH_ADD_ENTRY:
      MT_UtilSrcMatchAddEntry(pBuf);
      break;

    case MT_UTIL_SRC_MATCH_DEL_ENTRY:
      MT_UtilSrcMatchDeleteEntry(pBuf);
      break;

    case MT_UTIL_SRC_MATCH_CHECK_SRC_ADDR:
      MT_UtilSrcMatchCheckSrcAddr(pBuf);
      break;

    case MT_UTIL_SRC_MATCH_ACK_ALL_PENDING:
      MT_UtilSrcMatchAckAllPending(pBuf);
      break;

    case MT_UTIL_SRC_MATCH_CHECK_ALL_PENDING:
      MT_UtilSrcMatchCheckAllPending(pBuf);
      break;

    case MT_UTIL_TEST_LOOPBACK:
      MT_BuildAndSendZToolResponse((MT_RPC_CMD_SRSP|(uint8)MT_RPC_SYS_UTIL), MT_UTIL_TEST_LOOPBACK,
                                    pBuf[MT_RPC_POS_LEN], (pBuf+MT_RPC_FRAME_HDR_SZ));
      break;

#if !defined NONWK
    case MT_UTIL_DATA_REQ:
      MT_UtilDataReq(pBuf);
      break;

    case MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP:
      MT_UtilAddrMgrExtAddrLookup(pBuf);
      break;

    case MT_UTIL_ADDRMGR_NWK_ADDR_LOOKUP:
      MT_UtilAddrMgrEntryLookupNwk(pBuf);
      break;

#if defined MT_SYS_KEY_MANAGEMENT
    case MT_UTIL_APSME_LINK_KEY_DATA_GET:
      MT_UtilAPSME_LinkKeyDataGet(pBuf);
      break;

    case MT_UTIL_APSME_LINK_KEY_NV_ID_GET:
      MT_UtilAPSME_LinkKeyNvIdGet(pBuf);
      break;
#endif // MT_SYS_KEY_MANAGEMENT

    case MT_UTIL_APSME_REQUEST_KEY_CMD:
      MT_UtilAPSME_RequestKeyCmd(pBuf);
      break;

    case MT_UTIL_ASSOC_COUNT:
      MT_UtilAssocCount(pBuf);
      break;

    case MT_UTIL_ASSOC_FIND_DEVICE:
      MT_UtilAssocFindDevice(pBuf);
      break;

    case MT_UTIL_ASSOC_GET_WITH_ADDRESS:
      MT_UtilAssocGetWithAddress(pBuf);
      break;

#if defined ZCL_KEY_ESTABLISH
    case MT_UTIL_ZCL_KEY_EST_INIT_EST:
      MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment(pBuf);
      break;

    case MT_UTIL_ZCL_KEY_EST_SIGN:
      MT_UtilzclGeneral_KeyEstablishment_ECDSASign(pBuf);
      break;
#endif

    case MT_UTIL_SYNC_REQ:
      MT_UtilSync();
      break;
#endif /* !defined NONWK */

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_UtilGetDeviceInfo
 *
 * @brief   The Get Device Info serial message.
 *
 * @param   None.
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilGetDeviceInfo(void)
{
  uint8  *buf;
  uint8  *pBuf;
  uint8  bufLen = MT_UTIL_DEVICE_INFO_RESPONSE_LEN;
  uint16 *assocList = NULL;

#if !defined NONWK
  uint8  assocCnt = 0;

  if (ZG_DEVICE_RTR_TYPE)
  {
    assocList = AssocMakeList( &assocCnt );
    bufLen += (assocCnt * sizeof(uint16));
  }
#endif

  buf = osal_mem_alloc( bufLen );
  if ( buf )
  {
    pBuf = buf;

    *pBuf++ = ZSUCCESS; // Status

    osal_nv_read( ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, pBuf );
    pBuf += Z_EXTADDR_LEN;

#if defined NONWK
    // Skip past ZStack only parameters for NONWK
    *pBuf++ = 0;
    *pBuf++ = 0;
    *pBuf++ = 0;
    *pBuf++ = 0;
    *pBuf = 0;
#else
    {
      uint16 shortAddr = NLME_GetShortAddr();
      *pBuf++ = LO_UINT16( shortAddr );
      *pBuf++ = HI_UINT16( shortAddr );
    }

    /* Return device type */
    *pBuf++ = ZSTACK_DEVICE_BUILD;

    /*Return device state */
    *pBuf++ = (uint8)devState;

    if (ZG_DEVICE_RTR_TYPE)
    {
      *pBuf++ = assocCnt;

      if ( assocCnt )
      {
        uint8 x;
        uint16 *puint16 = assocList;

        for ( x = 0; x < assocCnt; x++, puint16++ )
        {
          *pBuf++ = LO_UINT16( *puint16 );
          *pBuf++ = HI_UINT16( *puint16 );
        }
      }
    }
    else
    {
      *pBuf++ = 0;
    }
#endif

    MT_BuildAndSendZToolResponse( ((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL),
                                 MT_UTIL_GET_DEVICE_INFO,
                                 bufLen, buf );

    osal_mem_free( buf );
  }

  if ( assocList )
  {
    osal_mem_free( assocList );
  }
}

/***************************************************************************************************
 * @fn      MT_UtilGetNvInfo
 *
 * @brief   The Get NV Info serial message.
 *
 * @param   None.
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilGetNvInfo(void)
{
  uint8 len;
  uint8 stat;
  uint8 *buf;
  uint8 *pBuf;
  uint16 tmp16;
  uint32 tmp32;

  /*
    Get required length of buffer
    Status + ExtAddr + ChanList + PanID  + SecLevel + PreCfgKey
  */
  len = 1 + Z_EXTADDR_LEN + 4 + 2 + 1 + SEC_KEY_LEN;

  buf = osal_mem_alloc( len );
  if ( buf )
  {
    /* Assume NV not available */
    osal_memset( buf, 0xFF, len );

    /* Skip over status */
    pBuf = buf + 1;

    /* Start with 64-bit extended address */
    stat = osal_nv_read( ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, pBuf );
    if ( stat ) stat = 0x01;
    pBuf += Z_EXTADDR_LEN;

    /* Scan channel list (bit mask) */
    if (  osal_nv_read( ZCD_NV_CHANLIST, 0, sizeof( tmp32 ), &tmp32 ) )
      stat |= 0x02;
    else
    {
      pBuf[0] = BREAK_UINT32( tmp32, 3 );
      pBuf[1] = BREAK_UINT32( tmp32, 2 );
      pBuf[2] = BREAK_UINT32( tmp32, 1 );
      pBuf[3] = BREAK_UINT32( tmp32, 0 );
    }
    pBuf += sizeof( tmp32 );

    /* ZigBee PanID */
    if ( osal_nv_read( ZCD_NV_PANID, 0, sizeof( tmp16 ), &tmp16 ) )
      stat |= 0x04;
    else
    {
      pBuf[0] = LO_UINT16( tmp16 );
      pBuf[1] = HI_UINT16( tmp16 );
    }
    pBuf += sizeof( tmp16 );

    /* Security level */
    if ( osal_nv_read( ZCD_NV_SECURITY_LEVEL, 0, sizeof( uint8 ), pBuf++ ) )
      stat |= 0x08;

    /* Pre-configured security key */
    if ( osal_nv_read( ZCD_NV_PRECFGKEY, 0, SEC_KEY_LEN, pBuf ) )
      stat |= 0x10;

    /* Status bit mask - bit=1 indicates failure */
    *buf = stat;

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), MT_UTIL_GET_NV_INFO,
                                  len, buf );

    osal_mem_free( buf );
  }
}

/***************************************************************************************************
 * @fn      MT_UtilSetPanID
 *
 * @brief   Set PanID message
 *
 * @param   pBuf - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetPanID(uint8 *pBuf)
{
  uint16 temp16;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  temp16 = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += sizeof(uint16);

  retValue = osal_nv_write(ZCD_NV_PANID, 0, osal_nv_item_len( ZCD_NV_PANID ), &temp16);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_UtilSetChannels
 *
 * @brief   Set Channels
 *
 * @param   pBuf - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetChannels(uint8 *pBuf)
{
  uint32 tmp32;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  tmp32 = BUILD_UINT32(pBuf[0], pBuf[1], pBuf[2], pBuf[3]);

  retValue = osal_nv_write(ZCD_NV_CHANLIST, 0, osal_nv_item_len( ZCD_NV_CHANLIST ), &tmp32);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_UtilSetSecLevel
 *
 * @brief   Set Sec Level
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetSecLevel(uint8 *pBuf)
{
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = osal_nv_write( ZCD_NV_SECURITY_LEVEL, 0, osal_nv_item_len( ZCD_NV_SECURITY_LEVEL ), pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );

}

/***************************************************************************************************
 * @fn      MT_UtilSetPreCfgKey
 *
 * @brief   Set Pre Cfg Key
 *
 * @param   pBuf - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetPreCfgKey(uint8 *pBuf)
{
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = osal_nv_write( ZCD_NV_PRECFGKEY, 0, osal_nv_item_len( ZCD_NV_PRECFGKEY ), pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );

}

/***************************************************************************************************
 * @fn      MT_UtilCallbackSub
 *
 * @brief   The Callback subscribe.
 *
 * @param   pBuf - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilCallbackSub(uint8 *pBuf)
{
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  uint8 retValue = ZFailure;

#if defined(MT_MAC_CB_FUNC) || defined(MT_NWK_CB_FUNC) || defined(MT_ZDO_CB_FUNC) || defined(MT_AF_CB_FUNC) || defined(MT_SAPI_CB_FUNC)
  uint8 subSystem;
  uint16 subscribed_command;

  // Move past header
  retValue = ZSuccess;
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Command */
  subscribed_command = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Subsystem - 5 bits on the MSB of the command */
  subSystem = HI_UINT16(subscribed_command) & 0x1F ;

  /* What is the action - SUBSCRIBE or !SUBSCRIBE */
  if (*pBuf)
  {
    /* Turn ON */
  #if defined( MT_MAC_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_MAC) || (subscribed_command == 0xFFFF))
      _macCallbackSub = 0xFFFF;
  #endif

  #if defined( MT_NWK_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_NWK) || (subscribed_command == 0xFFFF))
      _nwkCallbackSub = 0xFFFF;
  #endif

  #if defined( MT_ZDO_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_ZDO) || (subscribed_command == 0xFFFF))
      _zdoCallbackSub = 0xFFFFFFFF;
  #endif

  #if defined( MT_AF_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_AF) || (subscribed_command == 0xFFFF))
      _afCallbackSub = 0xFFFF;
  #endif

  #if defined( MT_SAPI_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_SAPI) || (subscribed_command == 0xFFFF))
      _sapiCallbackSub = 0xFFFF;
  #endif
  }
  else
  {
    /* Turn OFF */
  #if defined( MT_MAC_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_MAC) || (subscribed_command == 0xFFFF))
      _macCallbackSub = 0x0000;
  #endif

  #if defined( MT_NWK_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_NWK) || (subscribed_command == 0xFFFF))
      _nwkCallbackSub = 0x0000;
  #endif

  #if defined( MT_ZDO_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_ZDO) || (subscribed_command == 0xFFFF))
      _zdoCallbackSub = 0x00000000;
  #endif

  #if defined( MT_AF_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_AF) || (subscribed_command == 0xFFFF))
      _afCallbackSub = 0x0000;
  #endif

  #if defined( MT_SAPI_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_SAPI) || (subscribed_command == 0xFFFF))
        _sapiCallbackSub = 0x0000;
  #endif
  }
#endif  // MT_MAC_CB_FUNC || MT_NWK_CB_FUNC || MT_ZDO_CB_FUNC || MT_AF_CB_FUNC || MT_SAPI_CB_FUNC || MT_SAPI_CB_FUNC

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}

#if (defined HAL_KEY) && (HAL_KEY == TRUE)
/***************************************************************************************************
 * @fn      MT_UtilKeyEvent
 *
 * @brief   Process Key Event
 *
 * @param   pBuf - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilKeyEvent(uint8 *pBuf)
{
  uint8 x = 0;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Translate between SPI values to device values */
  if ( *pBuf & 0x01 )
    x |= HAL_KEY_SW_1;
  if ( *pBuf & 0x02 )
    x |= HAL_KEY_SW_2;
  if ( *pBuf & 0x04 )
    x |= HAL_KEY_SW_3;
  if ( *pBuf & 0x08 )
  x |= HAL_KEY_SW_4;
#if defined ( HAL_KEY_SW_5 )
  if ( *pBuf & 0x10 )
    x |= HAL_KEY_SW_5;
#endif
#if defined ( HAL_KEY_SW_6 )
  if ( *pBuf & 0x20 )
    x |= HAL_KEY_SW_6;
#endif
#if defined ( HAL_KEY_SW_7 )
  if ( *pBuf & 0x40 )
    x |= HAL_KEY_SW_7;
#endif
#if defined ( HAL_KEY_SW_8 )
  if ( *pBuf & 0x80 )
    x |= HAL_KEY_SW_8;
#endif
  pBuf++;

  retValue = OnBoard_SendKeys(x, *pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}
#endif

/***************************************************************************************************
 * @fn      MT_UtilTimeAlive
 *
 * @brief   Process Time Alive
 *
 * @param   None.
 *
 * @return  None
 ***************************************************************************************************/
void MT_UtilTimeAlive(void)
{
  uint8 timeAlive[4];
  uint32 tmp32;

  /* Time since last reset (seconds) */
  tmp32 = osal_GetSystemClock() / 1000;

  /* Convert to high byte first into temp buffer */
  timeAlive[0] = BREAK_UINT32(tmp32, 0);
  timeAlive[1] = BREAK_UINT32(tmp32, 1);
  timeAlive[2] = BREAK_UINT32(tmp32, 2);
  timeAlive[3] = BREAK_UINT32(tmp32, 3);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL),
                                       MT_UTIL_TIME_ALIVE, sizeof(timeAlive), timeAlive);
}

#if (defined HAL_LED) && (HAL_LED == TRUE)
/***************************************************************************************************
 * @fn      MT_UtilLedControl
 *
 * @brief   Process the LED Control Message
 *
 * @param   pBuf - pointer to the received data
 *
 * @return  None
 ***************************************************************************************************/
void MT_UtilLedControl(uint8 *pBuf)
{
  uint8 iLed, Led, iMode, Mode, cmdId;
  uint8 retValue = ZFailure;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* LED and Mode */
  iLed = *pBuf++;
  iMode = *pBuf;

  if ( iLed == 1 )
    Led = HAL_LED_1;
  else if ( iLed == 2 )
    Led = HAL_LED_2;
#if 0
  else if ( iLed == 3 )
    Led = HAL_LED_3;
  else if ( iLed == 4 )
    Led = HAL_LED_4;
#endif
  else if ( iLed == 0xFF )
    Led = HAL_LED_ALL;
  else
    Led = 0;

  if ( iMode == 0 )
    Mode = HAL_LED_MODE_OFF;
  else if ( iMode == 1 )
    Mode = HAL_LED_MODE_ON;
  else if ( iMode == 2 )
    Mode = HAL_LED_MODE_BLINK;
  else if ( iMode == 3 )
    Mode = HAL_LED_MODE_FLASH;
  else if ( iMode == 4 )
    Mode = HAL_LED_MODE_TOGGLE;
  else
    Led = 0;

  if ( Led != 0 )
  {
    HalLedSet (Led, Mode);
    retValue = ZSuccess;
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}
#endif /* HAL_LED */


/***************************************************************************************************
 * @fn          MT_UtilSrcMatchEnable
 *
 * @brief      Enabled AUTOPEND and source address matching.
 *
 * @param      pBuf - Buffer contains the data
 *
 * @return     void
 ***************************************************************************************************/
void MT_UtilSrcMatchEnable (uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef AUTO_PEND
  /* Call the routine */
  retValue = ZMacSrcMatchEnable (pBuf[0], pBuf[1]);
#else
  retValue = ZMacUnsupported;
#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );

}

/***************************************************************************************************
 * @fn          MT_UtilSrcMatchAddEntry
 *
 * @brief       Add a short or extended address to source address table.
 *
 * @param       pBuf - Buffer contains the data
 *
 * @return      void
 ***************************************************************************************************/
void MT_UtilSrcMatchAddEntry (uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef AUTO_PEND
  uint16 panID;
  zAddrType_t devAddr;

  /* Address mode */
  devAddr.addrMode = *pBuf++;

  /* Address based on the address mode */
  MT_UtilSpi2Addr( &devAddr, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* PanID */
  panID = BUILD_UINT16( pBuf[0] , pBuf[1] );

  /* Call the routine */
  retValue =  ZMacSrcMatchAddEntry (&devAddr, panID);
#else
  retValue = ZMacUnsupported;
#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn          MT_UtilSrcMatchDeleteEntry
 *
 * @brief      Delete a short or extended address from source address table.
 *
 * @param      pBuf - Buffer contains the data
 *
 * @return     void
 ***************************************************************************************************/
void MT_UtilSrcMatchDeleteEntry (uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef AUTO_PEND
  uint16 panID;
  zAddrType_t devAddr;

  /* Address mode */
  devAddr.addrMode = *pBuf++;

  /* Address based on the address mode */
  MT_UtilSpi2Addr( &devAddr, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* PanID */
  panID = BUILD_UINT16( pBuf[0] , pBuf[1] );

  /* Call the routine */
  retValue =  ZMacSrcMatchDeleteEntry (&devAddr, panID);
#else
  retValue = ZMacUnsupported;
#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn          MT_UtilSrcMatchCheckSrcAddr
 *
 * @brief      Check if a short or extended address is in the source address table.
 *
 * @param      pBuf - Buffer contains the data
 *
 * @return     void
 ***************************************************************************************************/
void MT_UtilSrcMatchCheckSrcAddr (uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retArray[2];

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#if 0  /* Unsupported  */
  uint16 panID;
  zAddrType_t devAddr;

  /* Address mode */
  devAddr.addrMode = *pBuf++;

  /* Address based on the address mode */
  MT_UtilSpi2Addr( &devAddr, pBuf);
  pBuf += Z_EXTADDR_LEN;

  /* PanID */
  panID = BUILD_UINT16( pBuf[0] , pBuf[1] );

  /* Call the routine */
  retArray[1] =  ZMacSrcMatchCheckSrcAddr (&devAddr, panID);

    /* Return failure if the index is invalid */
  if (retArray[1] == ZMacSrcMatchInvalidIndex )
  {
    retArray[0] = ZFailure;
  }
  else
  {
    retArray[0] = ZSuccess;
  }
#else
  retArray[0] = ZMacUnsupported;
  retArray[1] = ZMacSrcMatchInvalidIndex;
#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 2, retArray );
}

/***************************************************************************************************
 * @fn          MT_UtilSrcMatchAckAllPending
 *
 * @brief       Enabled/disable acknowledging all packets with pending bit set
 *              It is normally enabled when adding new entries to
 *              the source address table fails due to the table is full, or
 *              disabled when more entries are deleted and the table has
 *              empty slots.
 *
 * @param       pBuf - Buffer contains the data
 *
 * @return      void
 ***************************************************************************************************/
void MT_UtilSrcMatchAckAllPending (uint8 *pBuf)
{
  uint8 retValue, cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef AUTO_PEND
  /* Call the routine */
  retValue = ZMacSrcMatchAckAllPending(*pBuf);
#else
  retValue = ZMacUnsupported;
#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn          MT_UtilSrcMatchCheckAllPending
 *
 * @brief       Check if acknowledging all packets with pending bit set
 *              is enabled.
 *
 * @param       pBuf - Buffer contains the data
 *
 * @return      void
 ***************************************************************************************************/
void MT_UtilSrcMatchCheckAllPending (uint8 *pBuf)
{
  uint8 retArray[2], cmdId;

  /* Parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

#ifdef AUTO_PEND
  /* Call the routine */
  retArray[0] = ZMacSuccess;
  retArray[1] = ZMacSrcMatchCheckAllPending();
#else
  retArray[0] = ZMacUnsupported;
  retArray[1] = FALSE;
#endif

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 2, retArray );
}

/***************************************************************************************************
 * SUPPORT
 ***************************************************************************************************/

#ifdef AUTO_PEND
/***************************************************************************************************
 * @fn      MT_UtilRevExtCpy
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
static void MT_UtilRevExtCpy( uint8 *pDst, uint8 *pSrc )
{
  int8 i;

  for ( i = Z_EXTADDR_LEN - 1; i >= 0; i-- )
  {
    *pDst++ = pSrc[i];
  }
}

/***************************************************************************************************
 * @fn      MT_UtilSpi2Addr
 *
 * @brief   Copy an address from an SPI message to an address struct.  The
 *          addrMode in pAddr must already be set.
 *
 * @param   pDst - Pointer to address struct
 * @param   pSrc - Pointer SPI message byte array
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilSpi2Addr( zAddrType_t *pDst, uint8 *pSrc )
{
  if ( pDst->addrMode == Addr16Bit )
  {
    pDst->addr.shortAddr = BUILD_UINT16( pSrc[0] , pSrc[1] );
  }
  else if ( pDst->addrMode == Addr64Bit )
  {
    MT_UtilRevExtCpy( pDst->addr.extAddr, pSrc );
  }
}
#endif // AUTO_PEND

#if !defined NONWK
/**************************************************************************************************
 * @fn      MT_UtilDataReq
 *
 * @brief   Process the MAC Data Request command.
 *
 * @param   pBuf - pointer to the received data
 *
 * @return  None
**************************************************************************************************/
void MT_UtilDataReq(uint8 *pBuf)
{
  uint8 rtrn = NwkPollReq(pBuf[MT_RPC_POS_DAT0]);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), MT_UTIL_DATA_REQ,
                                                                               1, &rtrn);
}

/***************************************************************************************************
 * @fn      MT_UtilAddrMgrExtAddrLookup
 *
 * @brief   Proxy the AddrMgrExtAddrLookup() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAddrMgrExtAddrLookup(uint8 *pBuf)
{
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  (void)AddrMgrExtAddrLookup(BUILD_UINT16(pBuf[Z_EXTADDR_LEN], pBuf[Z_EXTADDR_LEN+1]), pBuf);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                                               Z_EXTADDR_LEN, (uint8 *)pBuf);
}

/***************************************************************************************************
 * @fn      MT_UtilAddrMgrEntryLookupNwk
 *
 * @brief   Proxy the AddrMgrEntryLookupNwk() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAddrMgrEntryLookupNwk(uint8 *pBuf)
{
  AddrMgrEntry_t entry;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  entry.nwkAddr = BUILD_UINT16(pBuf[0], pBuf[1]);
  (void)AddrMgrEntryLookupNwk(&entry);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                                                  Z_EXTADDR_LEN, entry.extAddr);
}

#if defined MT_SYS_KEY_MANAGEMENT
/***************************************************************************************************
 * @fn      MT_UtilAPSME_LinkKeyDataGet
 *
 * @brief   Retrieves APS Link Key data from NV.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAPSME_LinkKeyDataGet(uint8 *pBuf)
{
  uint8 rsp[MT_APSME_LINKKEY_GET_RSP_LEN];
  APSME_LinkKeyData_t *pData = NULL;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  uint16 apsLinkKeyNvId;
  uint32 *apsRxFrmCntr;
  uint32 *apsTxFrmCntr;

  pBuf += MT_RPC_FRAME_HDR_SZ;

  *rsp = APSME_LinkKeyNVIdGet(pBuf, &apsLinkKeyNvId);

  if (SUCCESS == *rsp)
  {
    pData = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

    if (pData != NULL)
    {
      // retrieve key from NV
      if ( osal_nv_read( apsLinkKeyNvId, 0,
                        sizeof(APSME_LinkKeyData_t), pData) == SUCCESS)

      {
        apsRxFrmCntr = &ApsLinkKeyFrmCntr[apsLinkKeyNvId - ZCD_NV_APS_LINK_KEY_DATA_START].rxFrmCntr;
        apsTxFrmCntr = &ApsLinkKeyFrmCntr[apsLinkKeyNvId - ZCD_NV_APS_LINK_KEY_DATA_START].txFrmCntr;

        uint8 *ptr = rsp+1;
        (void)osal_memcpy(ptr, pData->key, SEC_KEY_LEN);
        ptr += SEC_KEY_LEN;
        *ptr++ = BREAK_UINT32(*apsTxFrmCntr, 0);
        *ptr++ = BREAK_UINT32(*apsTxFrmCntr, 1);
        *ptr++ = BREAK_UINT32(*apsTxFrmCntr, 2);
        *ptr++ = BREAK_UINT32(*apsTxFrmCntr, 3);
        *ptr++ = BREAK_UINT32(*apsRxFrmCntr, 0);
        *ptr++ = BREAK_UINT32(*apsRxFrmCntr, 1);
        *ptr++ = BREAK_UINT32(*apsRxFrmCntr, 2);
        *ptr++ = BREAK_UINT32(*apsRxFrmCntr, 3);
      }

      // clear copy of key in RAM
      osal_memset( pData, 0x00, sizeof(APSME_LinkKeyData_t) );

      osal_mem_free(pData);
    }
  }
  else
  {
    // set data key and counters 0xFF
    osal_memset(&rsp[1], 0xFF, SEC_KEY_LEN + (MT_UTIL_FRM_CTR_LEN * 2));
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                       MT_APSME_LINKKEY_GET_RSP_LEN, rsp);

  // clear key data
  osal_memset(rsp, 0x00, MT_APSME_LINKKEY_GET_RSP_LEN);

}

/***************************************************************************************************
 * @fn      MT_UtilAPSME_LinkKeyNvIdGet
 *
 * @brief   Retrieves APS Link Key NV ID from the entry table.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAPSME_LinkKeyNvIdGet(uint8 *pBuf)
{
  uint8 rsp[MT_APSME_LINKKEY_NV_ID_GET_RSP_LEN];
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  uint16 apsLinkKeyNvId;

  pBuf += MT_RPC_FRAME_HDR_SZ;

  *rsp = APSME_LinkKeyNVIdGet(pBuf, &apsLinkKeyNvId);

  if (SUCCESS == *rsp)
  {
    rsp[1] = LO_UINT16(apsLinkKeyNvId);
    rsp[2] = HI_UINT16(apsLinkKeyNvId);
  }
  else
  {
    // send failure response with invalid NV ID
    osal_memset(&rsp[1], 0xFF, 2);
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                       MT_APSME_LINKKEY_NV_ID_GET_RSP_LEN, rsp);
}
#endif // MT_SYS_KEY_MANAGEMENT

/***************************************************************************************************
 * @fn      MT_UtilAPSME_RequestKeyCmd
 *
 * @brief   Send RequestKey command message to TC for a specific partner Address.
 *
 * @param   pBuf  - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilAPSME_RequestKeyCmd(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 partnerAddr[Z_EXTADDR_LEN];
  uint8 retValue;

  // parse header
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* PartnerAddress */
  osal_memcpy(partnerAddr, pBuf, Z_EXTADDR_LEN);

  retValue = (uint8)ZDSecMgrRequestAppKey(partnerAddr);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_UtilAssocCount
 *
 * @brief   Proxy the AssocCount() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAssocCount(uint8 *pBuf)
{
  uint16 cnt;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  cnt = AssocCount(pBuf[0], pBuf[1]);
  pBuf[0] = LO_UINT16(cnt);
  pBuf[1] = HI_UINT16(cnt);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 2, pBuf);
}

/***************************************************************************************************
 * @fn      MT_UtilAssocFindDevice
 *
 * @brief   Get an associated device by index.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAssocFindDevice(uint8 *pBuf)
{
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  uint8 buf[sizeof(associated_devices_t)];

  packDev_t(buf, AssocFindDevice(pBuf[MT_RPC_FRAME_HDR_SZ]));
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                       sizeof(associated_devices_t), buf);
}

/***************************************************************************************************
 * @fn      MT_UtilAssocGetWithAddress
 *
 * @brief   Get an associated device by address.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAssocGetWithAddress(uint8 *pBuf)
{
  extern associated_devices_t *AssocGetWithAddress(uint8 *extAddr, uint16 shortAddr);
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  uint8 buf[sizeof(associated_devices_t)];

  pBuf += MT_RPC_FRAME_HDR_SZ;
  packDev_t(buf, AssocGetWithAddress(((AddrMgrExtAddrValid(pBuf)) ? pBuf : NULL),
                                  BUILD_UINT16(pBuf[Z_EXTADDR_LEN], pBuf[Z_EXTADDR_LEN+1])));

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                       sizeof(associated_devices_t), buf);
}

/***************************************************************************************************
 * @fn      packDev_t
 *
 * @brief   Pack an associated_devices_t structure into a byte buffer (pack INVALID_NODE_ADDR if
 *          the pDev parameter is NULL).
 *
 * @param   pBuf - pointer to the buffer into which to pack the structure.
 * @param   pDev - pointer to the structure.
 *
 * @return  void
 ***************************************************************************************************/
static void packDev_t(uint8 *pBuf, associated_devices_t *pDev)
{
  if (NULL == pDev)
  {
    uint16 rtrn = INVALID_NODE_ADDR;
    *pBuf++ = LO_UINT16(rtrn);
    *pBuf++ = HI_UINT16(rtrn);
  }
  else
  {
    *pBuf++ = LO_UINT16(pDev->shortAddr);
    *pBuf++ = HI_UINT16(pDev->shortAddr);
    *pBuf++ = LO_UINT16(pDev->addrIdx);
    *pBuf++ = HI_UINT16(pDev->addrIdx);
    *pBuf++ = pDev->nodeRelation;
    *pBuf++ = pDev->devStatus;
    *pBuf++ = pDev->assocCnt;
    *pBuf++ = pDev->age;
    *pBuf++ = pDev->linkInfo.txCounter;
    *pBuf++ = pDev->linkInfo.txCost;
    *pBuf++ = pDev->linkInfo.rxLqi;
    *pBuf++ = pDev->linkInfo.inKeySeqNum;
    *pBuf++ = BREAK_UINT32(pDev->linkInfo.inFrmCntr, 0);
    *pBuf++ = BREAK_UINT32(pDev->linkInfo.inFrmCntr, 1);
    *pBuf++ = BREAK_UINT32(pDev->linkInfo.inFrmCntr, 2);
    *pBuf++ = BREAK_UINT32(pDev->linkInfo.inFrmCntr, 3);
    *pBuf++ = LO_UINT16(pDev->linkInfo.txFailure);
    *pBuf++ = HI_UINT16(pDev->linkInfo.txFailure);
  }
}

#if defined ZCL_KEY_ESTABLISH
/***************************************************************************************************
 * @fn      MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment
 *
 * @brief   Proxy the zclGeneral_KeyEstablish_InitiateKeyEstablishment() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment(uint8 *pBuf)
{
  afAddrType_t partnerAddr;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  partnerAddr.panId = 0;  // Not an inter-pan message.
  partnerAddr.endPoint = pBuf[2];
  partnerAddr.addrMode = (afAddrMode_t)pBuf[3];
  if (afAddr64Bit == partnerAddr.addrMode)
  {
    (void)osal_memcpy(partnerAddr.addr.extAddr, pBuf+4, Z_EXTADDR_LEN);
  }
  else
  {
    partnerAddr.addr.shortAddr = BUILD_UINT16(pBuf[4], pBuf[5]);
  }

  zcl_key_establish_task_id = pBuf[0];
  *pBuf = zclGeneral_KeyEstablish_InitiateKeyEstablishment(MT_TaskID, &partnerAddr, pBuf[1]);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, pBuf);
}

/***************************************************************************************************
 * @fn      MT_UtilzclGeneral_KeyEstablishment_ECDSASign
 *
 * @brief   Proxy the zclGeneral_KeyEstablishment_ECDSASign() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilzclGeneral_KeyEstablishment_ECDSASign(uint8 *pBuf)
{
#if defined TC_LINKKEY_JOIN
  uint8 *output = osal_mem_alloc(SE_PROFILE_SIGNATURE_LENGTH+1);
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  if (NULL == output)
  {
    *pBuf = FAILURE;
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, pBuf);
  }
  else
  {
    *output = zclGeneral_KeyEstablishment_ECDSASign(pBuf+1, *pBuf, output+1);
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                         SE_PROFILE_SIGNATURE_LENGTH+1, output);
    osal_mem_free(output);
  }
#endif
}

/***************************************************************************************************
 * @fn      MT_UtilKeyEstablishInd
 *
 * @brief   Proxy the ZCL_KEY_ESTABLISH_IND command.
 *
 * @param   pInd - Pointer to a keyEstablishmentInd_t structure.
 *
 * @return  None
 ***************************************************************************************************/
void MT_UtilKeyEstablishInd(keyEstablishmentInd_t *pInd)
{
  uint8 msg[6];

  msg[0] = zcl_key_establish_task_id;
  msg[1] = pInd->hdr.event;
  msg[2] = pInd->hdr.status;
  msg[3] = pInd->waitTime;
  msg[4] = LO_UINT16(pInd->keyEstablishmentSuite);
  msg[5] = HI_UINT16(pInd->keyEstablishmentSuite);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_UTIL),
                                       MT_UTIL_ZCL_KEY_ESTABLISH_IND, 6, msg);
}
#endif

/***************************************************************************************************
 * @fn      MT_UtilSync
 *
 * @brief   Process the MT_UTIL_SYNC command
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
static void MT_UtilSync(void)
{
 MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ|(uint8)MT_RPC_SYS_UTIL),MT_UTIL_SYNC_REQ,0,0);
}
#endif /* !defined NONWK */
#endif /* MT_UTIL_FUNC */
/**************************************************************************************************
 **************************************************************************************************/
