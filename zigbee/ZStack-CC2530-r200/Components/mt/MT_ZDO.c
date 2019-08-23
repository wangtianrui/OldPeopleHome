/**************************************************************************************************
  Filename:       MT_ZDO.c
  Revised:        $Date: 2011-06-07 14:34:55 -0700 (Tue, 07 Jun 2011) $
  Revision:       $Revision: 26241 $

  Description:    MonitorTest functions for the ZDO layer.


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

#ifdef MT_ZDO_FUNC

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "ZComDef.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "MT.h"
#include "MT_ZDO.h"
#include "APSMEDE.h"
#include "ZDConfig.h"
#include "ZDProfile.h"
#include "ZDObject.h"
#include "ZDApp.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

#if defined ( MT_SYS_KEY_MANAGEMENT )
  #include "ZDSecMgr.h"
#endif

#include "nwk_util.h"

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
#define MT_ZDO_END_DEVICE_ANNCE_IND_LEN   0x0D
#define MT_ZDO_ADDR_RSP_LEN               0x0D
#define MT_ZDO_BIND_UNBIND_RSP_LEN        0x03
#define MT_ZDO_BEACON_IND_LEN             21
#define MT_ZDO_BEACON_IND_PACK_LEN        (MT_UART_TX_BUFF_MAX - SPI_0DATA_MSG_LEN)
#define MT_ZDO_JOIN_CNF_LEN               5

// Message must pack nwk addr, entire (not just pointer to) ieee addr, and packet cost, so the
// sizeof(zdoConcentratorInd_t) is not usable.
#define MT_ZDO_CONCENTRATOR_IND_LEN      (2 + Z_EXTADDR_LEN + 1)

#define MTZDO_RESPONSE_BUFFER_LEN   100

#define MTZDO_MAX_MATCH_CLUSTERS    16
#define MTZDO_MAX_ED_BIND_CLUSTERS  15

// Conversion from ZDO Cluster Id to the RPC AREQ Id is direct as follows:
#define MT_ZDO_CID_TO_AREQ_ID(CId)  ((uint8)(CId) | 0x80)

#define MT_ZDO_STATUS_LEN   1

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
uint32 _zdoCallbackSub;
uint8 *pBeaconIndBuf = NULL;

/**************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/
#if defined (MT_ZDO_FUNC)
void MT_ZdoNWKAddressRequest(uint8 *pBuf);
void MT_ZdoIEEEAddrRequest(uint8 *pBuf);
void MT_ZdoNodeDescRequest(uint8 *pBuf);
void MT_ZdoPowerDescRequest(uint8 *pBuf);
void MT_ZdoSimpleDescRequest(uint8 *pBuf);
void MT_ZdoActiveEpRequest(uint8 *pBuf);
void MT_ZdoMatchDescRequest(uint8 *pBuf);
void MT_ZdoComplexDescRequest(uint8 *pBuf);
void MT_ZdoUserDescRequest(uint8 *pBuf);
void MT_ZdoEndDevAnnce(uint8 *pBuf);
void MT_ZdoUserDescSet(uint8 *pBuf);
void MT_ZdoServiceDiscRequest(uint8 *pBuf);
void MT_ZdoEndDevBindRequest(uint8 *pBuf);
void MT_ZdoBindRequest(uint8 *pBuf);
void MT_ZdoUnbindRequest(uint8 *pBuf);
void MT_ZdoMgmtNwkDiscRequest(uint8 *pBuf);
#if defined ( MT_SYS_KEY_MANAGEMENT )
void MT_ZdoSetLinkKey(uint8 *pBuf);
void MT_ZdoRemoveLinkKey(uint8 *pBuf);
void MT_ZdoGetLinkKey(uint8 *pBuf);
#endif /* MT_SYS_KEY_MANAGEMENT */
void MT_ZdoNetworkDiscoveryReq(uint8 *pBuf);
void MT_ZdoJoinReq(uint8 *pBuf);
/* Call back function */
void *MT_ZdoNwkDiscoveryCnfCB ( void *pStr );
void *MT_ZdoBeaconIndCB ( void *pStr );
void *MT_ZdoJoinCnfCB ( void *pStr );
#if defined (MT_ZDO_MGMT)
void MT_ZdoMgmtLqiRequest(uint8 *pBuf);
void MT_ZdoMgmtRtgRequest(uint8 *pBuf);
void MT_ZdoMgmtBindRequest(uint8 *pBuf);
void MT_ZdoMgmtLeaveRequest(uint8 *pBuf);
void MT_ZdoMgmtDirectJoinRequest(uint8 *pBuf);
void MT_ZdoMgmtPermitJoinRequest(uint8 *pBuf);
void MT_ZdoMgmtNwkUpdateRequest(uint8 *pBuf);
#endif /* MT_ZDO_MGMT */
void MT_ZdoStartupFromApp(uint8 *pBuf);
void MT_ZdoRegisterForZDOMsg(uint8 *pBuf);
void MT_ZdoRemoveRegisteredCB(uint8 *pBuf);
#endif /* MT_ZDO_FUNC */

#if defined (MT_ZDO_CB_FUNC)
uint8 MT_ZdoHandleExceptions( afIncomingMSGPacket_t *pData, zdoIncomingMsg_t *inMsg );
void MT_ZdoAddrRspCB( ZDO_NwkIEEEAddrResp_t *pMsg, uint16 clusterID );
void MT_ZdoEndDevAnnceCB( ZDO_DeviceAnnce_t *pMsg, uint16 srcAddr );
void MT_ZdoBindUnbindRspCB( uint16 clusterID, uint16 srcAddr, uint8 status );
void* MT_ZdoSrcRtgCB( void *pStr );
static void *MT_ZdoConcentratorIndCB(void *pStr);
#endif /* MT_ZDO_CB_FUNC */

#if defined (MT_ZDO_FUNC)
/***************************************************************************************************
 * @fn      MT_ZdoInit
 *
 * @brief   MT ZDO initialization
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void MT_ZdoInit(void)
{
#ifdef MT_ZDO_CB_FUNC
  /* Register with ZDO for indication callbacks */
  ZDO_RegisterForZdoCB(ZDO_SRC_RTG_IND_CBID, &MT_ZdoSrcRtgCB);
  ZDO_RegisterForZdoCB(ZDO_CONCENTRATOR_IND_CBID, &MT_ZdoConcentratorIndCB);
#endif
}

/***************************************************************************************************
 * @fn      MT_ZdoCommandProcessing
 *
 * @brief
 *
 *   Process all the ZDO commands that are issued by test tool
 *
 * @param   pBuf - pointer to the msg buffer
 *
 *          | LEN  | CMD0  | CMD1  |  DATA  |
 *          |  1   |   1   |   1   |  0-255 |
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_ZdoCommandProcessing(uint8* pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
#if defined ( ZDO_NWKADDR_REQUEST )
    case MT_ZDO_NWK_ADDR_REQ:
      MT_ZdoNWKAddressRequest(pBuf);
      break;
#endif

#if defined ( ZDO_IEEEADDR_REQUEST )
    case MT_ZDO_IEEE_ADDR_REQ:
      MT_ZdoIEEEAddrRequest(pBuf);
      break;
#endif

#if defined ( ZDO_NODEDESC_REQUEST )
    case MT_ZDO_NODE_DESC_REQ:
      MT_ZdoNodeDescRequest(pBuf);
      break;
#endif

#if defined ( ZDO_POWERDESC_REQUEST )
    case MT_ZDO_POWER_DESC_REQ:
      MT_ZdoPowerDescRequest(pBuf);
      break;
#endif

#if defined ( ZDO_SIMPLEDESC_REQUEST )
    case MT_ZDO_SIMPLE_DESC_REQ:
      MT_ZdoSimpleDescRequest(pBuf);
      break;
#endif

#if defined ( ZDO_ACTIVEEP_REQUEST )
    case MT_ZDO_ACTIVE_EP_REQ:
      MT_ZdoActiveEpRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MATCH_REQUEST )
    case MT_ZDO_MATCH_DESC_REQ:
      MT_ZdoMatchDescRequest(pBuf);
      break;
#endif

#if defined ( ZDO_COMPLEXDESC_REQUEST )
    case MT_ZDO_COMPLEX_DESC_REQ:
      MT_ZdoComplexDescRequest(pBuf);
      break;
#endif

#if defined ( ZDO_USERDESC_REQUEST )
    case MT_ZDO_USER_DESC_REQ:
      MT_ZdoUserDescRequest(pBuf);
      break;
#endif

#if defined ( ZDO_ENDDEVICE_ANNCE )
    case MT_ZDO_END_DEV_ANNCE:
      MT_ZdoEndDevAnnce(pBuf);
      break;
#endif

#if defined ( ZDO_USERDESCSET_REQUEST )
    case MT_ZDO_USER_DESC_SET:
      MT_ZdoUserDescSet(pBuf);
      break;
#endif

#if defined ( ZDO_SERVERDISC_REQUEST )
    case MT_ZDO_SERVICE_DISC_REQ:
      MT_ZdoServiceDiscRequest(pBuf);
      break;
#endif

#if defined ( ZDO_ENDDEVICEBIND_REQUEST )
    case MT_ZDO_END_DEV_BIND_REQ:
      MT_ZdoEndDevBindRequest(pBuf);
      break;
#endif

#if defined ( ZDO_BIND_UNBIND_REQUEST )
    case MT_ZDO_BIND_REQ:
      MT_ZdoBindRequest(pBuf);
      break;
#endif

#if defined ( ZDO_BIND_UNBIND_REQUEST )
    case MT_ZDO_UNBIND_REQ:
      MT_ZdoUnbindRequest(pBuf);
      break;
#endif

#if defined ( MT_SYS_KEY_MANAGEMENT )
    case MT_ZDO_SET_LINK_KEY:
      MT_ZdoSetLinkKey(pBuf);
      break;

    case MT_ZDO_REMOVE_LINK_KEY:
      MT_ZdoRemoveLinkKey(pBuf);
      break;

    case MT_ZDO_GET_LINK_KEY:
      MT_ZdoGetLinkKey(pBuf);
      break;
#endif // MT_SYS_KEY_MANAGEMENT

#if defined ( ZDO_MANUAL_JOIN )
    case MT_ZDO_NWK_DISCOVERY_REQ:
      MT_ZdoNetworkDiscoveryReq(pBuf);
      break;

    case MT_ZDO_JOIN_REQ:
      MT_ZdoJoinReq(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_NWKDISC_REQUEST )
    case MT_ZDO_MGMT_NWKDISC_REQ:
      MT_ZdoMgmtNwkDiscRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_LQI_REQUEST )
    case MT_ZDO_MGMT_LQI_REQ:
      MT_ZdoMgmtLqiRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_RTG_REQUEST )
    case MT_ZDO_MGMT_RTG_REQ:
      MT_ZdoMgmtRtgRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_BIND_REQUEST )
    case MT_ZDO_MGMT_BIND_REQ:
      MT_ZdoMgmtBindRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_LEAVE_REQUEST )
    case MT_ZDO_MGMT_LEAVE_REQ:
      MT_ZdoMgmtLeaveRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_JOINDIRECT_REQUEST )
    case MT_ZDO_MGMT_DIRECT_JOIN_REQ:
      MT_ZdoMgmtDirectJoinRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_PERMIT_JOIN_REQUEST )
    case MT_ZDO_MGMT_PERMIT_JOIN_REQ:
      MT_ZdoMgmtPermitJoinRequest(pBuf);
      break;
#endif

#if defined ( ZDO_MGMT_NWKUPDATE_REQUEST )
    case MT_ZDO_MGMT_NWK_UPDATE_REQ:
      MT_ZdoMgmtNwkUpdateRequest(pBuf);
      break;
#endif

#if defined ( ZDO_NETWORKSTART_REQUEST )
    case MT_ZDO_STARTUP_FROM_APP:
      MT_ZdoStartupFromApp(pBuf);
      break;
#endif

    case MT_ZDO_MSG_CB_REGISTER:
      MT_ZdoRegisterForZDOMsg(pBuf);
      break;

    case MT_ZDO_MSG_CB_REMOVE:
      MT_ZdoRemoveRegisteredCB(pBuf);
      break;

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_ZdoNwkAddrReq
 *
 * @brief   Handle a nwk address request.
 *
 * @param   pData  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoNWKAddressRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint8 reqType;
  uint8 startIndex;
  uint8 *pExtAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* parse parameters */
  pExtAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  /* Request type */
  reqType = *pBuf++;

  /* Start index */
  startIndex = *pBuf;

  retValue = (uint8)ZDP_NwkAddrReq(pExtAddr, reqType, startIndex, 0);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoIEEEAddrRequest
 *
 * @brief   Handle a IEEE address request.
 *
 * @param   pData  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoIEEEAddrRequest (uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint16 shortAddr;
  uint8 reqType;
  uint8 startIndex;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  shortAddr = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += 2;

  /* request type */
  reqType = *pBuf++;

  /* start index */
  startIndex = *pBuf;

  retValue = (uint8)ZDP_IEEEAddrReq(shortAddr, reqType, startIndex, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoNodeDescRequest
 *
 * @brief   Handle a Node Descriptor request.
 *
 * @param   pData  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoNodeDescRequest (uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Destination address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  retValue = (uint8)ZDP_NodeDescReq( &destAddr, shortAddr, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoPowerDescRequest
 *
 * @brief   Handle a Power Descriptor request.
 *
 * @param   pData  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoPowerDescRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  retValue = (uint8)ZDP_PowerDescReq( &destAddr, shortAddr, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoSimpleDescRequest
 *
 * @brief   Handle a Simple Descriptor request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoSimpleDescRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint8 epInt;
  zAddrType_t destAddr;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* endpoint/interface */
  epInt = *pBuf++;

  retValue = (uint8)ZDP_SimpleDescReq( &destAddr, shortAddr, epInt, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoActiveEpRequest
 *
 * @brief   Handle a Active EP request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoActiveEpRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  retValue = (uint8)ZDP_ActiveEPReq( &destAddr, shortAddr, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMatchDescRequest
 *
 * @brief   Handle a Match Descriptor request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMatchDescRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue = 0;
  uint8 i, numInClusters, numOutClusters;
  uint16 profileId;
  zAddrType_t destAddr;
  uint16 shortAddr;
  uint16 inClusters[MTZDO_MAX_MATCH_CLUSTERS], outClusters[MTZDO_MAX_MATCH_CLUSTERS];

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Profile ID */
  profileId = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* NumInClusters */
  numInClusters = *pBuf++;
  if ( numInClusters <= MTZDO_MAX_MATCH_CLUSTERS )
  {
    /* IN clusters */
    for ( i = 0; i < numInClusters; i++ )
    {
      inClusters[i] = BUILD_UINT16( pBuf[0], pBuf[1]);
      pBuf += 2;
    }
  }
  else
  {
    retValue = ZDP_INVALID_REQTYPE;
  }

  /* NumOutClusters */
  numOutClusters = *pBuf++;
  if ( numOutClusters <= MTZDO_MAX_MATCH_CLUSTERS )
  {
    /* OUT Clusters */
    for ( i = 0; i < numOutClusters; i++ )
    {
      outClusters[i] = BUILD_UINT16( pBuf[0], pBuf[1]);
      pBuf += 2;
    }
  }
  else
  {
    retValue = ZDP_INVALID_REQTYPE;
  }

  if ( retValue == 0 )
  {
    retValue = (uint8)ZDP_MatchDescReq( &destAddr, shortAddr, profileId, numInClusters,
                                       inClusters, numOutClusters, outClusters, 0);
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoComplexDescRequest
 *
 * @brief   Handle a Complex Descriptor request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoComplexDescRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  retValue = (uint8)ZDP_ComplexDescReq( &destAddr, shortAddr, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoUserDescRequest
 *
 * @brief   Handle a User Descriptor request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoUserDescRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1]);
  pBuf += 2;

  retValue = (uint8)ZDP_UserDescReq( &destAddr, shortAddr, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoEndDevAnnce
 *
 * @brief   Handle a End Device Announce Descriptor request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoEndDevAnnce(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint16 shortAddr;
  uint8 *pIEEEAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* network address */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* extended address */
  pIEEEAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  retValue = (uint8)ZDP_DeviceAnnce( shortAddr, pIEEEAddr, *pBuf, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoUserDescSet
 *
 * @brief   Handle a User Descriptor Set.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoUserDescSet(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint16 shortAddr;
  UserDescriptorFormat_t userDesc;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Network address of interest */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* User descriptor */
  userDesc.len = *pBuf++;
  osal_memcpy( userDesc.desc, pBuf, userDesc.len );
  pBuf += 16;

  retValue = (uint8)ZDP_UserDescSet( &destAddr, shortAddr, &userDesc, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoServiceDiscRequest
 *
 * @brief   Handle a Server Discovery request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoServiceDiscRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint16 serviceMask;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Service Mask */
  serviceMask = BUILD_UINT16( pBuf[0], pBuf[1]);
  pBuf += 2;

  retValue = (uint8)ZDP_ServerDiscReq( serviceMask, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoEndDevBindRequest
 *
 * @brief   Handle a End Device Bind request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoEndDevBindRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue = 0;
  uint8 i, epInt, numInClusters, numOutClusters;
  zAddrType_t destAddr;
  uint16 shortAddr;
  uint16 profileID, inClusters[MTZDO_MAX_ED_BIND_CLUSTERS], outClusters[MTZDO_MAX_ED_BIND_CLUSTERS];

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Local coordinator of the binding */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* For now, skip past the extended address */
  pBuf += Z_EXTADDR_LEN;

  /* Endpoint */
  epInt = *pBuf++;

  /* Profile ID */
  profileID = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* NumInClusters */
  numInClusters = *pBuf++;
  if ( numInClusters <= MTZDO_MAX_ED_BIND_CLUSTERS )
  {
    for ( i = 0; i < numInClusters; i++ )
    {
      inClusters[i] = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
    }
  }
  else
  {
    retValue = ZDP_INVALID_REQTYPE;
  }

  /* NumOutClusters */
  numOutClusters = *pBuf++;
  if ( numOutClusters <= MTZDO_MAX_ED_BIND_CLUSTERS )
  {
    for ( i = 0; i < numOutClusters; i++ )
    {
      outClusters[i] = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
    }
  }
  else
  {
    retValue = ZDP_INVALID_REQTYPE;
  }

  if ( retValue == 0 )
  {
    retValue = (uint8)ZDP_EndDeviceBindReq( &destAddr, shortAddr, epInt, profileID,
                                          numInClusters, inClusters, numOutClusters, outClusters, 0);
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoBindRequest
 *
 * @brief   Handle a Bind request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoBindRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr, devAddr;
  uint8 *pSrcAddr, *ptr;
  uint8 srcEPInt, dstEPInt;
  uint16 clusterID;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* SrcAddress */
  pSrcAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  /* SrcEPInt */
  srcEPInt = *pBuf++;

  /* ClusterID */
  clusterID = BUILD_UINT16( pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Destination Address mode */
  devAddr.addrMode = *pBuf++;

  /* Destination Address */
  if ( devAddr.addrMode == Addr64Bit )
  {
    ptr = pBuf;
    osal_cpyExtAddr( devAddr.addr.extAddr, ptr );
  }
  else
  {
    devAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  }
  /* The short address occupies LSB two bytes */
  pBuf += Z_EXTADDR_LEN;

  /* DstEPInt */
  dstEPInt = *pBuf;

  retValue = (uint8)ZDP_BindReq( &destAddr, pSrcAddr, srcEPInt, clusterID, &devAddr, dstEPInt, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoUnbindRequest
 *
 * @brief   Handle a Unbind request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoUnbindRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr, devAddr;
  uint8 *pSrcAddr, *ptr;
  uint8 srcEPInt, dstEPInt;
  uint16 clusterID;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* SrcAddress */
  pSrcAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  /* SrcEPInt */
  srcEPInt = *pBuf++;

  /* ClusterID */
  clusterID = BUILD_UINT16( pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Destination Address mode */
  devAddr.addrMode = *pBuf++;

  /* Destination Address */
  if ( devAddr.addrMode == Addr64Bit )
  {
    ptr = pBuf;
    osal_cpyExtAddr( devAddr.addr.extAddr, ptr );
  }
  else
  {
    devAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  }
  /* The short address occupies LSB two bytes */
  pBuf += Z_EXTADDR_LEN;

  /* dstEPInt */
  dstEPInt = *pBuf;

  retValue = (uint8)ZDP_UnbindReq( &destAddr, pSrcAddr, srcEPInt, clusterID, &devAddr, dstEPInt, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

#if defined (MT_SYS_KEY_MANAGEMENT)
/***************************************************************************************************
 * @fn      MT_ZdoSetLinkKey
 *
 * @brief   Set an application or trust center link key.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoSetLinkKey(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint8 *pExtAddr;
  uint8 *pKey;
  uint16 shortAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* ShortAddr */
  shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Extended Addr */
  pExtAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  /* Key data */
  pKey = pBuf;

  retValue = (uint8)ZDSecMgrAddLinkKey( shortAddr, pExtAddr, pKey);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoRemoveLinkKey
 *
 * @brief   Remove an application or trust center link key.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoRemoveLinkKey(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint8 *pExtAddr;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* ShortAddr */
  pExtAddr = pBuf;

  retValue = ZDSecMgrDeviceRemoveByExtAddr( pExtAddr );

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoGetLinkKey
 *
 * @brief   Get the application link key.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoGetLinkKey(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  uint8 *pExtAddr;
  uint8 *retBuf = NULL;
  uint8 len;
  APSME_LinkKeyData_t *pApsLinkKey = NULL;
  uint16 apsLinkKeyNvId;

  // parse header
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  // Extended Address
  pExtAddr = pBuf;

  // Fetch the key NV ID
  retValue = APSME_LinkKeyNVIdGet( pExtAddr, &apsLinkKeyNvId );

  if (retValue == ZSuccess)
  {
    if ((pApsLinkKey = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t))) != NULL)
    {
      // retrieve key from NV
      if (osal_nv_read( apsLinkKeyNvId, 0,
                       sizeof(APSME_LinkKeyData_t), pApsLinkKey) != SUCCESS)
      {
        retValue = ZNwkUnknownDevice;
      }
    }
    else
    {
      retValue = ZNwkUnknownDevice;
    }
  }

  // Construct the response message
  len = MT_ZDO_STATUS_LEN + Z_EXTADDR_LEN + SEC_KEY_LEN; // status + extAddr + key
  if ((retBuf = (uint8 *)osal_mem_alloc(len)) != NULL)
  {
    if (retValue == ZSuccess)
    {
      // Extended Address
      osal_memcpy( &(retBuf[1]), pExtAddr, Z_EXTADDR_LEN );

      // Key data
      osal_memcpy( &(retBuf[1 + Z_EXTADDR_LEN]), pApsLinkKey->key, SEC_KEY_LEN );
    }
    else
    {
      // Failed case - set the rest fields to all FF
      osal_memset( &(retBuf[1]), 0xFF, Z_EXTADDR_LEN + SEC_KEY_LEN );
    }

    retBuf[0] = retValue;  // Status

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, len, retBuf);

    // clear retBuf because it contains key data and free allocated memory
    osal_memset(retBuf, 0x00, len);

    osal_mem_free(retBuf);
  }

  // clear copy of key in RAM
  if (pApsLinkKey != NULL)
  {
    osal_memset(pApsLinkKey, 0x00, sizeof(APSME_LinkKeyData_t));

    osal_mem_free(pApsLinkKey);
  }

  return;
}
#endif // MT_SYS_KEY_MANAGEMENT

#if defined (MT_ZDO_MGMT)
/***************************************************************************************************
 * @fn      MT_ZdoMgmtNwkDiscRequest
 *
 * @brief   Handle a Mgmt Nwk Discovery request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtNwkDiscRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint32 scanChannels;
  uint8 scanDuration, startIndex;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Scan Channels */
  scanChannels = BUILD_UINT32( pBuf[0], pBuf[1], pBuf[2], pBuf[3] );
  pBuf += 4;

  /* Scan Duration */
  scanDuration = *pBuf++;

  /* Start Index */
  startIndex = *pBuf;

  retValue = (uint8)ZDP_MgmtNwkDiscReq( &destAddr, scanChannels, scanDuration, startIndex, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMgmtLqiRequest
 *
 * @brief   Handle a Mgmt Lqi request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtLqiRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint8 startIndex;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Start Index */
  startIndex = *pBuf;

  retValue = (uint8)ZDP_MgmtLqiReq( &destAddr, startIndex, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMgmtRtgRequest
 *
 * @brief   Handle a Mgmt Rtg request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtRtgRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint8 startIndex;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev Address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Start Index */
  startIndex = *pBuf;

  retValue = (byte)ZDP_MgmtRtgReq( &destAddr, startIndex, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMgmtBindRequest
 *
 * @brief   Handle a Mgmt Bind request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtBindRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint8 startIndex;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Dev Address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Start Index */
  startIndex = *pBuf;

  retValue = (uint8)ZDP_MgmtBindReq( &destAddr, startIndex, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMgmtLeaveRequest
 *
 * @brief   Handle a Mgmt Leave request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtLeaveRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint8 *pIEEEAddr;
  uint8 removeChildren, rejoin;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Destination Address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* IEEE address */
  pIEEEAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  /* Remove Children */
  removeChildren = *pBuf++;

  /* Rejoin */
  rejoin = *pBuf;

  retValue = (byte)ZDP_MgmtLeaveReq( &destAddr, pIEEEAddr, removeChildren, rejoin, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}


/***************************************************************************************************
 * @fn      MT_ZdoMgmtDirectJoinRequest
 *
 * @brief   Handle a Mgmt Direct Join request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtDirectJoinRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint8 *deviceAddr;
  uint8 capInfo;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Destination Address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Device Address */
  deviceAddr = pBuf;
  pBuf += Z_EXTADDR_LEN;

  /* Capability information */
  capInfo = *pBuf;

  retValue = (uint8)ZDP_MgmtDirectJoinReq( &destAddr, deviceAddr, capInfo, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMgmtPermitJoinRequest
 *
 * @brief   Handle a Mgmt Permit Join request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtPermitJoinRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint8 duration, tcSignificance;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Destination Address */
  destAddr.addrMode = Addr16Bit;
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Duration */
  duration = *pBuf++;

  /* Trust center significance */
  tcSignificance = *pBuf;

  retValue = (byte)ZDP_MgmtPermitJoinReq( &destAddr, duration, tcSignificance, 0);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_ZdoMgmtNwkUpdateRequest
 *
 * @brief   Handle a Mgmt Nwk Update request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoMgmtNwkUpdateRequest(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue;
  zAddrType_t destAddr;
  uint32 channelMask;
  uint8 scanDuration, scanCount;
  uint16 nwkManagerAddr;

    /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Destination address */
  destAddr.addr.shortAddr = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  /* Destination address mode */
  destAddr.addrMode = *pBuf++;

  channelMask = BUILD_UINT32( pBuf[0], pBuf[1], pBuf[2], pBuf[3]);
  pBuf += 4;

  /* Scan duration */
  scanDuration = *pBuf++;

  /* Scan count */
  scanCount = *pBuf++;

  /* NWK manager address */
  nwkManagerAddr = BUILD_UINT16( pBuf[0], pBuf[1] );

  /* Send the Management Network Update request */
  retValue = (uint8)ZDP_MgmtNwkUpdateReq( &destAddr, channelMask, scanDuration,
                                          scanCount, _NIB.nwkUpdateId+1, nwkManagerAddr );

  /*
    Since we don't recevied our own broadcast messages, we should
    send a unicast copy of the message to ourself.
  */
  if ( destAddr.addrMode == AddrBroadcast )
  {
    destAddr.addrMode = Addr16Bit;
    destAddr.addr.shortAddr = _NIB.nwkDevAddress;
    retValue = (uint8) ZDP_MgmtNwkUpdateReq( &destAddr, channelMask, scanDuration,
                                             scanCount, _NIB.nwkUpdateId+1, nwkManagerAddr );
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue);
}
#endif /* MT_ZDO_MGMT */

/***************************************************************************************************
 * @fn      MT_ZdoStartupFromApp
 *
 * @brief   Handle a Startup from App request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoStartupFromApp(uint8 *pBuf)
{
  uint8 cmd0, cmd1, retValue;

  /* parse header */
  cmd0 = pBuf[MT_RPC_POS_CMD0];
  cmd1 = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = ZDOInitDevice(100);

  if (MT_RPC_CMD_SREQ == (cmd0 & MT_RPC_CMD_TYPE_MASK))
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP|(uint8)MT_RPC_SYS_ZDO), cmd1,1, &retValue);
  }
}


/***************************************************************************************************
 * @fn      MT_ZdoNetworkDiscoveryReq
 *
 * @brief   Handle a ZDO Network Discovery request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoNetworkDiscoveryReq(uint8 *pBuf)
{
  uint8  retValue = ZFailure;
  uint8  cmdId;
  uint32 scanChannels;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Packet format */
  /* scan channels (4) | scan duration (1) */

  /* Scan channels */
  scanChannels = osal_build_uint32(pBuf, 4);
  pBuf += 4;

  retValue = ZDApp_NetworkDiscoveryReq(scanChannels, *pBuf);

  // Register ZDO callback for MT to handle the network discovery confirm
  // and beacon notification confirm
  ZDO_RegisterForZdoCB( ZDO_NWK_DISCOVERY_CNF_CBID, &MT_ZdoNwkDiscoveryCnfCB );
  ZDO_RegisterForZdoCB( ZDO_BEACON_NOTIFY_IND_CBID, &MT_ZdoBeaconIndCB );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue );
}


/***************************************************************************************************
 * @fn      MT_ZdoJoinReq
 *
 * @brief   Handle a ZDO Join request.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 ***************************************************************************************************/
void MT_ZdoJoinReq(uint8 *pBuf)
{
  uint8  retValue = ZFailure;
  uint8  cmdId;
  uint16 panId;
  uint16 chosenParent;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Packet format */
  /* channel     (1) | panID (2) | extendedPanID (8) | chosenParent (2) |
   * parentDepth (1) | stackProfile  (1)
   */

  panId        = BUILD_UINT16(pBuf[1], pBuf[2]);
  chosenParent = BUILD_UINT16(pBuf[11], pBuf[12]);

  retValue = ZDApp_JoinReq(pBuf[0], panId, &(pBuf[3]), chosenParent, pBuf[13], pBuf[14]);

  /* Register for MT to receive Join Confirm */
  ZDO_RegisterForZdoCB( ZDO_JOIN_CNF_CBID, &MT_ZdoJoinCnfCB );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_ZDO), cmdId, 1, &retValue );

}

/***************************************************************************************************
 * @fn          MT_ZdoNwkDiscoveryCnfCB
 *
 * @brief       Send an indication to inform host device the completion of
 *              network discovery scan
 *
 * @param       pStr - pointer to a parameter and a structure of parameters
 *
 * @return      void
 ***************************************************************************************************/
void *MT_ZdoNwkDiscoveryCnfCB ( void *pStr )
{
  /* pStr: status (uint8) */
  /* Packet Format */
  /* Status (1) */

  // Scan completed. De-register the callback with ZDO
  ZDO_DeregisterForZdoCB( ZDO_NWK_DISCOVERY_CNF_CBID );
  ZDO_DeregisterForZdoCB( ZDO_BEACON_NOTIFY_IND_CBID );

  // Send the buffered beacon indication
  MT_ZdoBeaconIndCB ( NULL );

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                         MT_ZDO_NWK_DISCOVERY_CNF, 1, pStr);
  return NULL;
}



/***************************************************************************************************
 * @fn          MT_ZdoBeaconIndCB
 *
 * @brief       Send an indication to host device of a beacon notification
 *
 * @param       pStr -  pointer to a parameter and a structure of parameters
 *
 * @return      void
 ***************************************************************************************************/
void *MT_ZdoBeaconIndCB ( void *pStr )
{
  zdoBeaconInd_t *pBeacon = pStr;
  uint8 *pTmp;

  /* Packet Format */
  /* devCnt (1) | device #1 (21) | device #2 (21) |... | device #n (21) */

  if( pStr != NULL)
  {
    if( pBeaconIndBuf == NULL )
    {
      // If pBeaconIndBuf has not been allocated yet
      // allocate memory now with MAX_UART_TX_BUFF
      if( NULL == (pBeaconIndBuf = (uint8 *)osal_mem_alloc(MT_ZDO_BEACON_IND_PACK_LEN)))
      {
        // Memory failure
        return NULL;
      }
      pBeaconIndBuf[0] = 0; // First byte is devCnt. Initialize to 0.
    }

    // Fill in the buffer with the beacon indication
    pTmp = pBeaconIndBuf + (1 + pBeaconIndBuf[0] * MT_ZDO_BEACON_IND_LEN);
    *pTmp++ = LO_UINT16(pBeacon->sourceAddr);
    *pTmp++ = HI_UINT16(pBeacon->sourceAddr);
    *pTmp++ = LO_UINT16(pBeacon->panID);
    *pTmp++ = HI_UINT16(pBeacon->panID);
    *pTmp++ = pBeacon->logicalChannel;
    *pTmp++ = pBeacon->permitJoining;
    *pTmp++ = pBeacon->routerCapacity;
    *pTmp++ = pBeacon->deviceCapacity;
    *pTmp++ = pBeacon->protocolVersion;
    *pTmp++ = pBeacon->stackProfile;
    *pTmp++ = pBeacon->LQI;
    *pTmp++ = pBeacon->depth;
    *pTmp++ = pBeacon->updateID;
    osal_memcpy( pTmp, pBeacon->extendedPanID, Z_EXTADDR_LEN);

    pBeaconIndBuf[0] += 1; // Increment the devCnt

    // Check if the buffer can fit in another beacon
    if( ((pBeaconIndBuf[0] + 1) * MT_ZDO_BEACON_IND_LEN + 1) > MT_ZDO_BEACON_IND_PACK_LEN )
    {
      // Packet full, send the packet over MT
      MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                   MT_ZDO_BEACON_NOTIFY_IND,
                                   (pBeaconIndBuf[0] * MT_ZDO_BEACON_IND_LEN + 1), pBeaconIndBuf);
      pBeaconIndBuf[0] = 0; // Reset the devCnt back to zero
    }
  }
  else
  {
    if( (pBeaconIndBuf != NULL) && (pBeaconIndBuf[0] != 0) )
    {
      // End of beacon indication, send the packet over MT
      MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                   MT_ZDO_BEACON_NOTIFY_IND,
                                   (pBeaconIndBuf[0] * MT_ZDO_BEACON_IND_LEN + 1), pBeaconIndBuf);
    }
    // Free the allocated memory
    if(pBeaconIndBuf != NULL)
    {
      osal_mem_free(pBeaconIndBuf);
      pBeaconIndBuf = NULL;
    }
  }

  return NULL;
}



/***************************************************************************************************
 * @fn          MT_ZdoJoinCnfCB
 *
 * @brief       Handle the ZDO Join Confirm from ZDO
 *
 * @param       pStr - pointer to a parameter and a structure of parameters
 *
 * @return      void
 ***************************************************************************************************/
void *MT_ZdoJoinCnfCB ( void *pStr )
{
  /* pStr: zdoJoinCnf_t* */
  /* Packet Format */
  /* Status (1) | device addr (2) | parent addr (2) */

  uint8 buf[MT_ZDO_JOIN_CNF_LEN];
  zdoJoinCnf_t *joinCnf = pStr;

  /* Join Complete. De-register the callback with ZDO */
  ZDO_DeregisterForZdoCB( ZDO_JOIN_CNF_CBID );

  buf[0] = joinCnf->status;
  buf[1] = LO_UINT16( joinCnf->deviceAddr );
  buf[2] = HI_UINT16( joinCnf->deviceAddr );
  buf[3] = LO_UINT16( joinCnf->parentAddr );
  buf[4] = HI_UINT16( joinCnf->parentAddr );

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                               MT_ZDO_JOIN_CNF, MT_ZDO_JOIN_CNF_LEN, buf);

  return NULL;
}

/*************************************************************************************************
 * @fn      MT_ZdoRegisterForZDOMsg(pBuf);
 *
 * @brief   MT proxy for ZDO_RegisterForZDOMsg.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 *************************************************************************************************/
void MT_ZdoRegisterForZDOMsg(uint8 *pBuf)
{
  uint8 cmd0, cmd1, tmp;
  uint16 cId;

  /* parse header */
  cmd0 = pBuf[MT_RPC_POS_CMD0];
  cmd1 = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  cId = BUILD_UINT16(pBuf[0], pBuf[1]);
  tmp = ZDO_RegisterForZDOMsg(MT_TaskID, cId);

  if (MT_RPC_CMD_SREQ == (cmd0 & MT_RPC_CMD_TYPE_MASK))
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP|(uint8)MT_RPC_SYS_ZDO), cmd1, 1, &tmp);
  }
}

/*************************************************************************************************
 * @fn      MT_ZdoRemoveRegisteredCB(pBuf);
 *
 * @brief   MT proxy for ZDO_RemoveRegisteredCB.
 *
 * @param   pBuf  - MT message data
 *
 * @return  void
 *************************************************************************************************/
void MT_ZdoRemoveRegisteredCB(uint8 *pBuf)
{
  uint8 cmd0, cmd1, tmp;
  uint16 cId;

  /* parse header */
  cmd0 = pBuf[MT_RPC_POS_CMD0];
  cmd1 = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  cId = BUILD_UINT16(pBuf[0], pBuf[1]);
  tmp = ZDO_RemoveRegisteredCB(MT_TaskID, cId);

  if (MT_RPC_CMD_SREQ == (cmd0 & MT_RPC_CMD_TYPE_MASK))
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP|(uint8)MT_RPC_SYS_ZDO), cmd1, 1, &tmp);
  }
}

#endif /* MT_ZDO_FUNC */


/***************************************************************************************************
 * Callback handling function
 ***************************************************************************************************/

#if defined (MT_ZDO_CB_FUNC)

/***************************************************************************************************
 * @fn      MT_ZdoStateChangeCB
 *
 * @brief   Handle state change OSAL message from ZDO.
 *
 * @param   pMsg  - Message data
 *
 * @return  void
 */
void MT_ZdoStateChangeCB(osal_event_hdr_t *pMsg)
{
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                       MT_ZDO_STATE_CHANGE_IND, 1, &pMsg->status);
}

/***************************************************************************************************
 * @fn     MT_ZdoDirectCB()
 *
 * @brief  ZDO direct callback.  Build an MT message directly from the
 *         over-the-air ZDO message.
 *
 * @param  pData - Incoming AF frame.
 *
 * @return  none
 ***************************************************************************************************/
void MT_ZdoDirectCB( afIncomingMSGPacket_t *pData,  zdoIncomingMsg_t *inMsg )
{
  uint8 len, *pBuf;
  uint16 origClusterId;

  // save original value because MT_ZdoHandleExceptions() function could modify pData->clusterId
  origClusterId = pData->clusterId;

  // Is the message an exception or not a response?
  if ( MT_ZdoHandleExceptions( pData, inMsg ) || ( (origClusterId & ZDO_RESPONSE_BIT) == 0 ) )
  {
    return;  // Handled somewhere else or not needed.
  }

  /* ZDO data starts after one-byte sequence number and the msg buffer length includes
   * two bytes for srcAddr.
   */
  len = pData->cmd.DataLength - 1 + sizeof(uint16);

  if (NULL != (pBuf = (uint8 *)osal_mem_alloc(len)))
  {
    uint8 id = MT_ZDO_CID_TO_AREQ_ID(pData->clusterId);

    pBuf[0] = LO_UINT16(pData->srcAddr.addr.shortAddr);
    pBuf[1] = HI_UINT16(pData->srcAddr.addr.shortAddr);

    /* copy ZDO data, skipping one-byte sequence number */
    osal_memcpy(pBuf+2, (pData->cmd.Data + 1), pData->cmd.DataLength-1);

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO), id, len, pBuf);
    osal_mem_free(pBuf);
  }
}

/***************************************************************************************************
 * @fn     MT_ZdoHandleExceptions()
 *
 * @brief  Handles all messages that are an exception to the generic MT ZDO Response.
 *
 * @param  pData - Incoming AF frame.
 *
 * @return  TRUE if handled by this function, FALSE if not
 ***************************************************************************************************/
uint8 MT_ZdoHandleExceptions( afIncomingMSGPacket_t *pData, zdoIncomingMsg_t *inMsg )
{
  uint8 ret = TRUE;
  ZDO_NwkIEEEAddrResp_t *nwkRsp = NULL;
  ZDO_DeviceAnnce_t devAnnce;
  uint8 doDefault = FALSE;

  switch ( inMsg->clusterID )
  {
    case NWK_addr_rsp:
    case IEEE_addr_rsp:
      nwkRsp = ZDO_ParseAddrRsp( inMsg );
      MT_ZdoAddrRspCB( nwkRsp, inMsg->clusterID );
      if ( nwkRsp )
      {
        osal_mem_free( nwkRsp );
      }
      break;

    case Device_annce:
      ZDO_ParseDeviceAnnce( inMsg, &devAnnce );
      MT_ZdoEndDevAnnceCB( &devAnnce, inMsg->srcAddr.addr.shortAddr );
      break;

    case Simple_Desc_rsp:
      if ( pData->cmd.DataLength > 5 )
      {
        ret = FALSE;
      }
      else
      {
        doDefault = TRUE;
      }
      break;

    default:
      ret = FALSE;
      break;
  }

  if ( doDefault )
  {
    ret = FALSE;
    pData->clusterId = MtZdoDef_rsp;
    pData->cmd.DataLength = 2;
  }

  return ( ret );
}

/***************************************************************************************************
 * @fn      MT_ZdoAddrRspCB
 *
 * @brief   Handle IEEE or nwk address response OSAL message from ZDO.
 *
 * @param   pMsg  - Message data
 *
 * @return  void
 */
void MT_ZdoAddrRspCB( ZDO_NwkIEEEAddrResp_t *pMsg, uint16 clusterID )
{
  uint8   listLen, len, *pBuf;

  /* both ZDO_NwkAddrResp_t and ZDO_IEEEAddrResp_t must be the same */

  /* get length, sanity check length */
  listLen = pMsg->numAssocDevs;

  /* calculate msg length */
  len = MT_ZDO_ADDR_RSP_LEN + (listLen * sizeof(uint16));

  /* get buffer */
  if (NULL != (pBuf = (uint8 *)osal_mem_alloc(len)))
  {
    uint8 id = MT_ZDO_CID_TO_AREQ_ID(clusterID);
    uint8 *pTmp = pBuf;

    *pTmp++ = pMsg->status;

    osal_cpyExtAddr(pTmp, pMsg->extAddr);
    pTmp += Z_EXTADDR_LEN;

    *pTmp++ = LO_UINT16(pMsg->nwkAddr);
    *pTmp++ = HI_UINT16(pMsg->nwkAddr);

    *pTmp++ = pMsg->startIndex;
    *pTmp++ = listLen;

    MT_Word2Buf(pTmp, pMsg->devList, listLen);

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO), id, len, pBuf);
    osal_mem_free(pBuf);
  }
}

/***************************************************************************************************
 * @fn      MT_ZdoEndDevAnnceCB
 *
 * @brief   Handle end device announce OSAL message from ZDO.
 *
 * @param   pMsg  - Message data
 *
 * @return  void
 */
void MT_ZdoEndDevAnnceCB( ZDO_DeviceAnnce_t *pMsg, uint16 srcAddr )
{
  uint8 *pBuf;

  if (NULL != (pBuf = (uint8 *)osal_mem_alloc(MT_ZDO_END_DEVICE_ANNCE_IND_LEN)))
  {
    uint8 *pTmp = pBuf;

    *pTmp++ = LO_UINT16(srcAddr);
    *pTmp++ = HI_UINT16(srcAddr);

    *pTmp++ = LO_UINT16(pMsg->nwkAddr);
    *pTmp++ = HI_UINT16(pMsg->nwkAddr);

    osal_cpyExtAddr(pTmp, pMsg->extAddr);
    pTmp += Z_EXTADDR_LEN;

    *pTmp = pMsg->capabilities;

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                         MT_ZDO_END_DEVICE_ANNCE_IND,
                                         MT_ZDO_END_DEVICE_ANNCE_IND_LEN, pBuf);
    osal_mem_free(pBuf);
  }
}

/***************************************************************************************************
 * @fn      MT_ZdoSrcRtgCB
 *
 * @brief   Handle Src Route from ZDO.
 *
 * @param   pStr  - pointer to the data structure for the src route
 *
 * @return  void*
 */
void* MT_ZdoSrcRtgCB( void *pStr )
{
  uint8 len, *pBuf;
  zdoSrcRtg_t *pSrcRtg = pStr;

  // srcAddr (2) + relayCnt (1) + relayList( relaycnt * 2 )
  len = 2 + 1 + pSrcRtg->relayCnt * sizeof(uint16);

  if (NULL != (pBuf = (uint8 *)osal_mem_alloc(len)))
  {
    uint8 idx, *pTmp = pBuf;
    uint16 *pRelay;

    // Packet payload
    *pTmp++ = LO_UINT16(pSrcRtg->srcAddr);
    *pTmp++ = HI_UINT16(pSrcRtg->srcAddr);
    *pTmp++ = pSrcRtg->relayCnt;

    // Relay List
    if( ( pRelay = pSrcRtg->pRelayList ) != NULL )
    {
      for( idx = 0; idx < pSrcRtg->relayCnt; idx ++ )
      {
        *pTmp++ = LO_UINT16(*pRelay);
        *pTmp++ = HI_UINT16(*pRelay);
        pRelay++;
      }
    }
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                         MT_ZDO_SRC_RTG_IND, len, pBuf);
    osal_mem_free(pBuf);
  }

  return NULL;
}

/***************************************************************************************************
 * @fn          MT_ZdoConcentratorIndCB
 *
 * @brief       Handle the ZDO Concentrator Indication callback from the ZDO.
 *
 * @param       pStr - pointer to a parameter and a structure of parameters
 *
 * @return      NULL
 ***************************************************************************************************/
static void *MT_ZdoConcentratorIndCB(void *pStr)
{
  uint8 buf[MT_ZDO_CONCENTRATOR_IND_LEN], *pTmp = buf;
  zdoConcentratorInd_t *pInd = (zdoConcentratorInd_t *)pStr;

  *pTmp++ = LO_UINT16(pInd->nwkAddr);
  *pTmp++ = HI_UINT16(pInd->nwkAddr);
  pTmp = osal_memcpy(pTmp, pInd->extAddr, Z_EXTADDR_LEN);
  *pTmp = pInd->pktCost;

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                    MT_ZDO_CONCENTRATOR_IND_CB, MT_ZDO_CONCENTRATOR_IND_LEN, buf);
  return NULL;
}
#endif // MT_ZDO_CB_FUNC

/***************************************************************************************************
 * @fn      MT_ZdoSendMsgCB
 *
 * @brief   Proxy the ZDO_SendMsgCBs one message at a time.
 *
 * @param   pMsg  - Message data
 *
 * @return  void
 */
void MT_ZdoSendMsgCB(zdoIncomingMsg_t *pMsg)
{
  uint8 len = pMsg->asduLen + 9;
  uint8 *pBuf = (uint8 *)osal_mem_alloc(len);

  if (pBuf != NULL)
  {
    uint8 *pTmp = pBuf;

    // Assuming exclusive use of network short addresses.
    *pTmp++ = LO_UINT16(pMsg->srcAddr.addr.shortAddr);
    *pTmp++ = HI_UINT16(pMsg->srcAddr.addr.shortAddr);
    *pTmp++ = pMsg->wasBroadcast;
    *pTmp++ = LO_UINT16(pMsg->clusterID);
    *pTmp++ = HI_UINT16(pMsg->clusterID);
    *pTmp++ = pMsg->SecurityUse;
    *pTmp++ = pMsg->TransSeq;
    // Skipping asduLen since it can be deduced from the RPC packet length.
    *pTmp++ = LO_UINT16(pMsg->macDestAddr);
    *pTmp++ = HI_UINT16(pMsg->macDestAddr);
    (void)osal_memcpy(pTmp, pMsg->asdu, pMsg->asduLen);

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_ZDO),
                                         MT_ZDO_MSG_CB_INCOMING, len, pBuf);

    osal_mem_free(pBuf);
  }
}

#endif   /*ZDO Command Processing in MT*/
/***************************************************************************************************
***************************************************************************************************/
