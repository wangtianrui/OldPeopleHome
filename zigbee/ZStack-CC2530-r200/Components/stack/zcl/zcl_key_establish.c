/******************************************************************************
  Filename:       zcl_key_establish.c
  Revised:        $Date: 2011-07-18 15:10:51 -0700 (Mon, 18 Jul 2011) $
  Revision:       $Revision: 26841 $

  Description:    Zigbee Cluster Library - General Function Domain - key
                  establishment cluster.
                  This application receives ZCL messages and handles them
                  within the ZCL layer, without passing to application.


  Copyright 2007-2011 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License"). You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product. Other than for
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
******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "zcl.h"
#include "ZDApp.h"
#include "ssp_hash.h"
#include "AddrMgr.h"
#include "ZDSecMgr.h"
#include "APSMEDE.h"
#include "eccapi.h"
#include "zcl_key_establish.h"
#include "DebugTrace.h"
#include "se.h"

#if defined ( INTER_PAN )
  #include "stub_aps.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define KEY_ESTABLISHMENT_DEVICE_VERSION      0
#define KEY_ESTABLISHMENT_FLAGS               0
#define KEY_ESTABLISHMENT_SUITE               1  // For CBKE with ECMQV
#define KEY_ESTABLISHMENT_AVG_TIMEOUT         ( 2 * ( ZCL_KEY_ESTABLISHMENT_KEY_GENERATE_TIMEOUT + \
                                                  ZCL_KEY_ESTABLISHMENT_MAC_GENERATE_TIMEOUT ) )

#define ZCL_KEY_ESTABLISH_DEVICE_VERSION      0
#define ZCL_KEY_ESTABLISH_FLAGS               0

#define INVALID_TASK_ID                       0xFF
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// For debug and testing purpose, use a fixed ephermeral key pair instead
// of the randomly generated one.
#if defined (DEBUG_STATIC_ECC)
uint8 public1[22] = {
    0x03, 0x06, 0xAB, 0x52, 0x06, 0x22, 0x01, 0xD9,
    0x95, 0xB8, 0xB8, 0x59, 0x1F, 0x3F, 0x08, 0x6A,
    0x3A, 0x2E, 0x21, 0x4D, 0x84, 0x5E
  };
uint8 private1[21] = {
    0x03, 0xD4, 0x8C, 0x72, 0x10, 0xDD, 0xBC, 0xC4,
    0xFB, 0x2E, 0x5E, 0x7A, 0x0A, 0xA1, 0x6A, 0x0D,
    0xB8, 0x95, 0x40, 0x82, 0x0B
  };
uint8 public2[22] = {
    0x03, 0x00, 0xE1, 0x17, 0xC8, 0x6D, 0x0E, 0x7C,
    0xD1, 0x28, 0xB2, 0xF3, 0x4E, 0x90, 0x76, 0xCF,
    0xF2, 0x4A, 0xF4, 0x6D, 0x72, 0x88
  };
uint8 private2[21] = {
    0x00, 0x13, 0xD3, 0x6D, 0xE4, 0xB1, 0xEA, 0x8E,
    0x22, 0x73, 0x9C, 0x38, 0x13, 0x70, 0x82, 0x3F,
    0x40, 0x4B, 0xFF, 0x88, 0x62
  };
#endif


zclOptionRec_t zclKeyEstablish_Options[1] =
{
  {
    ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
    ( AF_ACK_REQUEST ),
  },
};

YieldFunc *zclKeyEstablish_YieldFunc = NULL;
uint8 zclKeyEstablish_YieldLevel = 0;

#if defined (NWK_AUTO_POLL)
uint16 zclSavedPollRate = POLL_RATE;
#endif

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
extern uint8* SSP_MemCpyReverse( uint8* dst, uint8* src, unsigned int len );

/*********************************************************************
 * LOCAL VARIABLES
 */
#if defined(ZCL_KEY_ESTABLISH)
static uint8 zcl_KeyEstablishment_TaskID;    // Task ID of the key Establishment cluster
#endif

/*********************************************************************
 * SIMPLE DESCRIPTOR
 */
// This is the Cluster ID List and should be filled with Application
// specific cluster IDs.
#define ZCL_KEY_ESTABLISH_MAX_INCLUSTERS       1
const cId_t zclKeyEstablish_InClusterList[ZCL_KEY_ESTABLISH_MAX_INCLUSTERS] =
{
  ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
};

#define ZCL_KEY_ESTABLISH_MAX_OUTCLUSTERS       1
const cId_t zclKeyEstablish_OutClusterList[ZCL_KEY_ESTABLISH_MAX_OUTCLUSTERS] =
{
  ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
};

SimpleDescriptionFormat_t zclKeyEstablish_SimpleDesc =
{
  ZCL_KEY_ESTABLISHMENT_ENDPOINT,          //  int Endpoint;
  ZCL_SE_PROFILE_ID,                       //  uint16 AppProfId[2];
  ZCL_SE_DEVICEID_ESP,                     //  uint16 AppDeviceId[2];
  ZCL_KEY_ESTABLISH_DEVICE_VERSION,        //  int   AppDevVer:4;
  ZCL_KEY_ESTABLISH_FLAGS,                 //  int   AppFlags:4;
  ZCL_KEY_ESTABLISH_MAX_INCLUSTERS,        //  byte  AppNumInClusters;
  (cId_t *)zclKeyEstablish_InClusterList,  //  byte *pAppInClusterList;
  ZCL_KEY_ESTABLISH_MAX_OUTCLUSTERS,       //  byte  AppNumInClusters;
  (cId_t *)zclKeyEstablish_OutClusterList  //  byte *pAppInClusterList;
};

#if defined (ZCL_KEY_ESTABLISH)
// Endpoint for Key Establishment Cluster
static endPointDesc_t zclKeyEstablish_Ep =
{
  ZCL_KEY_ESTABLISHMENT_ENDPOINT,                               // Test endpoint
  &zcl_TaskID,
  (SimpleDescriptionFormat_t *)&zclKeyEstablish_SimpleDesc,
  (afNetworkLatencyReq_t)0                           // No Network Latency req
};
#endif

// Pointer to the application sequence number for ZCL commands
#if defined ( ZCL_KEY_ESTABLISH)
static uint8 zclKeyEstablishPluginRegisted = FALSE;

static zclKeyEstablishRec_t keyEstablishRec[MAX_KEY_ESTABLISHMENT_REC_ENTRY];

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static ZStatus_t zclGeneral_KeyEstablish_HdlIncoming( zclIncoming_t *pInMsg );

static ZStatus_t zclGeneral_KeyEstablish_HdlInSpecificCommands( zclIncoming_t *pInMsg );

// Key Establish Cluster Command Processing functions
static ZStatus_t zclGeneral_ProcessInCmd_InitiateKeyEstablish( zclIncoming_t *pInMsg );
static ZStatus_t zclGeneral_ProcessInCmd_InitiateKeyEstablishRsp( zclIncoming_t *pInMsg );
static ZStatus_t zclGeneral_ProcessInCmd_EphemeralDataReq( zclIncoming_t *pInMsg );
static ZStatus_t zclGeneral_ProcessInCmd_EphemeralDataRsp( zclIncoming_t *pInMsg );
static ZStatus_t zclGeneral_ProcessInCmd_ConfirmKey( zclIncoming_t *pInMsg );
static ZStatus_t zclGeneral_ProcessInCmd_ConfirmKeyRsp( zclIncoming_t *pInMsg );
static ZStatus_t zclGeneral_ProcessInCmd_TerminateKeyEstablish( zclIncoming_t *pInMsg );

// Event driven key calculation function
static ZStatus_t zclGeneral_InitiateKeyEstablish_Cmd_CalculateKey(void);
static ZStatus_t zclGeneral_InitiateKeyEstablish_Rsp_CalculateKey(void);

// Key establishment rec table management function
static void zclGeneral_InitKeyEstablishRecTable( void );
static uint8 zclGeneral_GetKeyEstablishRecIndex( uint16 partnerAddress );
static uint8 zclGeneral_GetKeyEstablishRecIndex_State( KeyEstablishState_t state );
static uint8 zclGeneral_AddKeyEstablishRec( afAddrType_t *addr );
static void zclGeneral_AgeKeyEstablishRec( void );
static void zclGeneral_ResetKeyEstablishRec( uint8 index );

// Call back function supplying to ECC library
static int zclGeneral_KeyEstablishment_GetRandom(unsigned char *buffer, unsigned long len);
static int zclGeneral_KeyEstablishment_HashFunc(unsigned char *digest, unsigned long len, unsigned char *data);

// Security related functions
static void zclGeneral_KeyEstablishment_KeyDeriveFunction( uint8 *zData,
                                                           uint8 keyBitLen,
                                                           uint8 *keyBit );

static ZStatus_t zclGeneral_KeyEstablishment_GenerateMAC(uint8 recIndex,
                                                         uint8 ifMACu,
                                                         uint8 *MAC);

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Init
 *
 * @brief   Call to initialize the Key Establishment Task
 *
 * @param   task_id
 *
 * @return  none
 */

void zclGeneral_KeyEstablish_Init( uint8 task_id )
{
  zcl_KeyEstablishment_TaskID = task_id;

  // Register for the key establishment cluster endpoint
  afRegister( &zclKeyEstablish_Ep );

  zcl_registerClusterOptionList( ZCL_KEY_ESTABLISHMENT_ENDPOINT, 1,
                                 zclKeyEstablish_Options );

  // Register as a ZCL Plugin
  if ( !zclKeyEstablishPluginRegisted )
  {
    zcl_registerPlugin( ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                        ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                        zclGeneral_KeyEstablish_HdlIncoming );
    zclKeyEstablishPluginRegisted = TRUE;
  }

  // Initialize the keyEstablishRec table
  zclGeneral_InitKeyEstablishRecTable();
}

/*********************************************************************
 * @fn          zclKeyEstablish_event_loop
 *
 * @brief       Event Loop Processor for Key establish task.
 *
 * @param       task_id - TaskId
 *              events - events
 *
 * @return      none
 */
uint16 zclKeyEstablish_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & KEY_ESTABLISHMENT_REC_AGING_EVT )
  {

    zclGeneral_AgeKeyEstablishRec();

    return ( events ^ KEY_ESTABLISHMENT_REC_AGING_EVT );
  }

  if ( events & KEY_ESTABLISHMENT_CMD_PROCESS_EVT )
  {
    zclGeneral_InitiateKeyEstablish_Cmd_CalculateKey();

    return ( events ^ KEY_ESTABLISHMENT_CMD_PROCESS_EVT );
  }

  if ( events & KEY_ESTABLISHMENT_RSP_PROCESS_EVT )
  {
    zclGeneral_InitiateKeyEstablish_Rsp_CalculateKey();
    return ( events ^ KEY_ESTABLISHMENT_RSP_PROCESS_EVT );
  }
  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_InitiateKeyEstablishment
 *
 * @brief   Call to initiate key establishment with partner device
 *
 * @param   appTaskID - task ID of the application that initates the key establish
 * @param   partnerAddr - short address and endpoint of the partner to establish key with
 * @param   seqNum - pointer to the sequence number of application (ZCL)
 *
 * @return  ZStatus_t ZSuccess or ZFailure
 */
ZStatus_t zclGeneral_KeyEstablish_InitiateKeyEstablishment(uint8 appTaskID,
                                                           afAddrType_t *partnerAddr,
                                                           uint8 seqNum)
{
  uint8 *implicitCert, index;

  // Assign the app seqnum pointer
  zcl_SeqNum = seqNum;

  // Start a new key establishment rec entry
  index = zclGeneral_AddKeyEstablishRec( partnerAddr );

  if( index < MAX_KEY_ESTABLISHMENT_REC_ENTRY ) // valid entry
  {
    keyEstablishRec[index].role = KEY_ESTABLISHMENT_INITIATOR;

    // Assign the application task ID that initiates the key establishment
    keyEstablishRec[index].appTaskID = appTaskID;
  }
  else
  {
    return ZFailure;
  }

  // Generate Ephemeral Public/Private Key Pair
  ZSE_ECCGenerateKey( ( unsigned char *)keyEstablishRec[index].pLocalEPrivateKey,
                     ( unsigned char *)keyEstablishRec[index].pLocalEPublicKey,
                     zclGeneral_KeyEstablishment_GetRandom,
                     zclKeyEstablish_YieldFunc, zclKeyEstablish_YieldLevel);

#if defined (DEBUG_STATIC_ECC)
  // For debug and testing purpose, use a fixed ephermeral key pair instead
  // of the randomly generated one.
  osal_memcpy( keyEstablishRec[index].pLocalEPrivateKey, private1, 21 );
  osal_memcpy( keyEstablishRec[index].pLocalEPublicKey, public1, 22 );
#endif

  keyEstablishRec[index].state = KeyEstablishState_InitiatePending;

  if ((implicitCert = osal_mem_alloc(ZCL_KE_IMPLICIT_CERTIFICATE_LEN)) == NULL)
  {
    return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
  }
  osal_nv_read(ZCD_NV_IMPLICIT_CERTIFICATE, 0, ZCL_KE_IMPLICIT_CERTIFICATE_LEN, implicitCert);

  // Send Initiate Key Establishment Command
  zclGeneral_KeyEstablish_Send_InitiateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
             partnerAddr,
             KEY_ESTABLISHMENT_SUITE,
             ZCL_KEY_ESTABLISHMENT_EKEY_GENERATE_TIMEOUT,
             ZCL_KEY_ESTABLISHMENT_MAC_GENERATE_TIMEOUT + ZCL_KEY_ESTABLISHMENT_KEY_GENERATE_TIMEOUT,
             implicitCert, TRUE, zcl_SeqNum++ );

  // Start the Key Establishment aging timer, this has to be started to make sure the record
  // is going to be deleted in the event that "Key Establishment Response" is not received for
  // any reason. This way we do not have hanging records in the table and memory leak issues.
  osal_start_reload_timer( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT,
                           KEY_ESTABLISHMENT_REC_AGING_INTERVAL );

  osal_mem_free(implicitCert);

#if defined (NWK_AUTO_POLL)
  // Start the key establishment process and set the Key Establish poll rate
  zclSavedPollRate = zgPollRate;
  NLME_SetPollRate(ZCL_KEY_ESTABLISH_POLL_RATE);
#endif

  return ZSuccess;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_InitiateKeyEstablishment
 *
 * @brief   Call to send out a Initiate Key Establishment Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   keyEstablishmentSuite - key establishment suite bitmap
 * @param   keyGenerateTime - how long it takes to generate key
 * @param   macGenerateTime - how long it takes to generate mac
 * @param   certificate - identity. For CBKE, it's the implicit certificate.
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_InitiateKeyEstablishment( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint16 keyEstablishmentSuite,
                                             uint8  keyGenerateTime,
                                             uint8  macGenerateTime,
                                             uint8 *certificate,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 *pBuf;
  uint8 status;
  uint8 bufLen;

  (void)srcEP; // Intentionally unreferenced parameter

  // keyEstablishmentSuite + eDataGenerateTime + macGenerateTime + certificate
  bufLen = 2 + 1 + 1 + ZCL_KE_IMPLICIT_CERTIFICATE_LEN;

  if ((buf = osal_mem_alloc(bufLen)) == NULL)
  {
    return ZMemError;
  }

  pBuf = buf;

  *pBuf++ = LO_UINT16( keyEstablishmentSuite );
  *pBuf++ = HI_UINT16( keyEstablishmentSuite );
  *pBuf++ = keyGenerateTime;
  *pBuf++ = macGenerateTime;
  osal_memcpy( pBuf, certificate, ZCL_KE_IMPLICIT_CERTIFICATE_LEN );

  status = zcl_SendCommand( ZCL_KEY_ESTABLISHMENT_ENDPOINT, dstAddr,
                           ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                           COMMAND_INITIATE_KEY_ESTABLISHMENT, TRUE,
                           ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp,
                           0, seqNum, bufLen, buf );

  osal_mem_free(buf);

  return status;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_EphemeralDataReq
 *
 * @brief   Call to send out a Ephemeral Data Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   eData - ephemeral data.
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_EphemeralDataReq( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *eData,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  return zcl_SendCommand( srcEP, dstAddr,
                           ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                           COMMAND_EPHEMERAL_DATA_REQUEST, TRUE,
                           ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp,
                           0, seqNum, ZCL_KE_CA_PUBLIC_KEY_LEN, eData );
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_ConfirmKey
 *
 * @brief   Call to send out a Confirm Key Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   mac - MAC.
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_ConfirmKey( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *mac,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  return (zcl_SendCommand(srcEP, dstAddr,
                           ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                           COMMAND_CONFIRM_KEY, TRUE,
                           ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp,
                           0, seqNum, KEY_ESTABLISH_MAC_LENGTH, mac ));
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment
 *
 * @brief   Call to send out a Terminate Key Establishment Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   status - status of the key establishment procedure.
 * @param   direction - client/server direction of the command
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( uint8 srcEP,
                                             afAddrType_t *dstAddr,
                                             TermKeyStatus_t status,
                                             uint8 waitTime,
                                             uint16 keyEstablishmentSuite, uint8 direction,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];

  buf[0] = status;
  buf[1] = waitTime;
  buf[2] = LO_UINT16(keyEstablishmentSuite);
  buf[3] = HI_UINT16(keyEstablishmentSuite);

  return zcl_SendCommand(srcEP, dstAddr,
                         ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                         COMMAND_TERMINATE_KEY_ESTABLISHMENT, TRUE,
                         direction, disableDefaultRsp,
                         0, seqNum, 4, buf );
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_InitiateKeyEstablishmentRsp
 *
 * @brief   Call to send out a Initiate Key Establishment Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   status - status of the key establishment response.
 * @param   keyEstablishmentSuite - requested key establishment suite bitmap
 * @param   keyGenerateTime - how long it takes to generate key
 * @param   macGenerateTime - how long it takes to generate mac
 * @param   certificate - identity. For CBKE, it's the implicit certificate.
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_InitiateKeyEstablishmentRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint16 keyEstablishmentSuite,
                                             uint8  keyGenerateTime,
                                             uint8  macGenerateTime,
                                             uint8 *certificate,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 bufLen;
  uint8 ret;
  uint8 *pBuf;

  bufLen = 2 + 1 + 1 + ZCL_KE_IMPLICIT_CERTIFICATE_LEN;

  if ((buf = osal_mem_alloc(bufLen)) == NULL)
  {
    return ZMemError;
  }

  pBuf = buf;

  *pBuf++ = LO_UINT16( keyEstablishmentSuite );
  *pBuf++ = HI_UINT16( keyEstablishmentSuite );
  *pBuf++ = keyGenerateTime;
  *pBuf++ = macGenerateTime;
  osal_memcpy( pBuf, certificate, ZCL_KE_IMPLICIT_CERTIFICATE_LEN );

  ret = zcl_SendCommand( srcEP, dstAddr,
                           ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                           COMMAND_INITIATE_KEY_ESTABLISHMENT_RESPONSE, TRUE,
                           ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp,
                           0, seqNum, bufLen, buf );
  osal_mem_free(buf);

  return ret;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_EphemeralDataRsp
 *
 * @brief   Call to send out a Ephemeral Data Response Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   eData - ephemeral data.
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_EphemeralDataRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *eData,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  return (zcl_SendCommand( srcEP, dstAddr,
                           ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                           COMMAND_EPHEMERAL_DATA_RESPONSE, TRUE,
                           ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp,
                           0, seqNum, ZCL_KE_CA_PUBLIC_KEY_LEN, eData ));
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_ConfirmKeyRsp
 *
 * @brief   Call to send out a Confirm Key Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   mac - MAC
 * @param   disableDefaultRsp - disable default response
 * @param   seqNum - ZCL sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclGeneral_KeyEstablish_Send_ConfirmKeyRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *mac,
                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  return (zcl_SendCommand(srcEP, dstAddr,
                           ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT,
                           COMMAND_CONFIRM_KEY_RESPONSE, TRUE,
                           ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp,
                           0, seqNum, KEY_ESTABLISH_MAC_LENGTH, mac ));
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_Send_ConfirmKeyRsp
 *
 * @brief   Register the user defined yielding function
 *
 * @param   yield - Pointer to a function to allow user defined yielding.
 *          YieldFunc may be NULL if yieldLevel is 0
 * @param   yieldLevel - The yield level determines how often the user defined
 *          yield function will be called. This is a number from 0 to 10.
 *          0 will never yield. 1 will yield the most often. 10 will yield the
 *          least often.
 */
void zclGeneral_KeyEstablishment_RegYieldCB( YieldFunc *pFnYield,
                                             uint8 yieldLevel )
{
  if( pFnYield == NULL )
  {
    zclKeyEstablish_YieldLevel = 0;
  }
  else
  {
    zclKeyEstablish_YieldFunc = pFnYield;
    zclKeyEstablish_YieldLevel = yieldLevel;
  }
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_HdlIncoming
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library or Profile commands
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclGeneral_KeyEstablish_HdlIncoming( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZSuccess;

#if defined ( INTER_PAN )
  if ( StubAPS_InterPan( pInMsg->msg->srcAddr.panId, pInMsg->msg->srcAddr.endPoint ) )
    return ( stat ); // Cluster not supported thru Inter-PAN
#endif

  if ( zcl_ClusterCmd( pInMsg->hdr.fc.type ) )
  {
    // Is this a manufacturer specific command?
    if ( pInMsg->hdr.fc.manuSpecific == 0 )
    {
      stat = zclGeneral_KeyEstablish_HdlInSpecificCommands( pInMsg );
    }
    else
    {
      // We don't support any manufacturer specific command.
      stat = ZFailure;
    }
  }
  else
  {
    // Handle all the normal (Read, Write...) commands -- should never get here
    stat = ZFailure;
  }
  return ( stat );
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablish_HdlInSpecificCommands
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclGeneral_KeyEstablish_HdlInSpecificCommands( zclIncoming_t *pInMsg )
{
  ZStatus_t stat;

  if ( zcl_ServerCmd( pInMsg->hdr.fc.direction ) )
  {
    // Commands received by Server
    switch ( pInMsg->hdr.commandID )
    {
      case COMMAND_INITIATE_KEY_ESTABLISHMENT:
        stat = zclGeneral_ProcessInCmd_InitiateKeyEstablish( pInMsg );
        break;

      case COMMAND_EPHEMERAL_DATA_REQUEST:
        stat = zclGeneral_ProcessInCmd_EphemeralDataReq( pInMsg );
        break;

      case COMMAND_CONFIRM_KEY:
        stat = zclGeneral_ProcessInCmd_ConfirmKey( pInMsg );
        break;

      case COMMAND_TERMINATE_KEY_ESTABLISHMENT:
        stat = zclGeneral_ProcessInCmd_TerminateKeyEstablish( pInMsg );
        break;

      default:
        stat = ZFailure;
        break;
    }
  }
  else
  {
    // Commands received by Client
    switch ( pInMsg->hdr.commandID )
    {
      case COMMAND_INITIATE_KEY_ESTABLISHMENT_RESPONSE:
        stat = zclGeneral_ProcessInCmd_InitiateKeyEstablishRsp( pInMsg );
        break;

      case COMMAND_EPHEMERAL_DATA_RESPONSE:
        stat = zclGeneral_ProcessInCmd_EphemeralDataRsp( pInMsg );
        break;

      case COMMAND_CONFIRM_KEY_RESPONSE:
        stat = zclGeneral_ProcessInCmd_ConfirmKeyRsp( pInMsg );
        break;

      case COMMAND_TERMINATE_KEY_ESTABLISHMENT:
        stat = zclGeneral_ProcessInCmd_TerminateKeyEstablish( pInMsg );
        break;

      default:
        stat = ZFailure;
        break;
    }
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_InitiateKeyEstablish
 *
 * @brief   Process the received Initiate Key Establishment Request.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_MALFORMED_COMMAND
 *                      ZCL_STATUS_CMD_HAS_RSP
 *                      ZCL_STATUS_SOFTWARE_FAILURE
 */
static ZStatus_t zclGeneral_ProcessInCmd_InitiateKeyEstablish( zclIncoming_t *pInMsg )
{
  TermKeyStatus_t status = TermKeyStatus_Success;
  uint16 remoteKeyEstablishmentSuite;
  uint8 *implicitCert = NULL;
  uint8 index = MAX_KEY_ESTABLISHMENT_REC_ENTRY;  // set to non valid value

  // Check the incoming packet length
  if ( pInMsg->pDataLen < PACKET_LEN_INITIATE_KEY_EST_REQ )
  {
    status = TermKeyStatus_BadMessage;
  }
  else
  {
    // Start a new key establishment rec entry
    index = zclGeneral_AddKeyEstablishRec( &pInMsg->msg->srcAddr );

    if( index == MAX_KEY_ESTABLISHMENT_REC_ENTRY ) // Fail to add an entry
    {
      status = TermKeyStatus_NoResources;
    }
    else
    {
      // Parse the incoming message
      // Copy the remote device certificate
      osal_memcpy(keyEstablishRec[index].pRemoteCertificate, &(pInMsg->pData[KEY_ESTABLISH_CERT_IDX]),
                  ZCL_KE_IMPLICIT_CERTIFICATE_LEN );

      // Verify the certificate issuer and key establishment suite
      remoteKeyEstablishmentSuite = BUILD_UINT16( pInMsg->pData[0], pInMsg->pData[1] );

      if ( remoteKeyEstablishmentSuite != KEY_ESTABLISHMENT_SUITE )
      {
        status = TermKeyStatus_UnSupportedSuite;
      }
      else
      {
        // continue parsing message
        // Save Ephemeral Data Generate Key and Confirm Key Time
        if (pInMsg->pData[2] >= KEY_ESTABLISHMENT_EPH_DATA_GEN_INVALID_TIME)
        {
          status = TermKeyStatus_BadMessage;
        }
        else
        {
          // continue parsing message
          keyEstablishRec[index].remoteEphDataGenTime = pInMsg->pData[2];

          if (pInMsg->pData[3] >= KEY_ESTABLISHMENT_CONF_KEY_GEN_INVALID_TIME)
          {
            status = TermKeyStatus_BadMessage;
          }
          else
          {
            // continue parsing message
            keyEstablishRec[index].remoteConfKeyGenTime = pInMsg->pData[3];

            if ((implicitCert = osal_mem_alloc(ZCL_KE_IMPLICIT_CERTIFICATE_LEN)) == NULL)
            {
              // Reset the entry
              zclGeneral_ResetKeyEstablishRec( index );

              return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
            }

            osal_nv_read(ZCD_NV_IMPLICIT_CERTIFICATE, 0, ZCL_KE_IMPLICIT_CERTIFICATE_LEN, implicitCert);

            if ( !osal_memcmp( &(keyEstablishRec[index].pRemoteCertificate[KEY_ESTABLISH_CERT_ISSUER_IDX]),
                               &(implicitCert[KEY_ESTABLISH_CERT_ISSUER_IDX]),
                               KEY_ESTABLISH_CERT_ISSUER_LENTGH ) )
            {
              status = TermKeyStatus_UnknowIssuer;
            }
          }
        }
      }
    } // end of parsing of the message
  }

  if ( status != TermKeyStatus_Success )
  {
    zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &pInMsg->msg->srcAddr,
                                                            status,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_SERVER_CLIENT_DIR,
                                                            FALSE, zcl_SeqNum++ );
    if ( implicitCert != NULL )
    {
      osal_mem_free(implicitCert);
    }

    // Reset the entry
    if ( index < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
    {
      zclGeneral_ResetKeyEstablishRec( index );
    }

    return ZCL_STATUS_CMD_HAS_RSP;
  }

  // Fill in partner's extended address
  SSP_MemCpyReverse( keyEstablishRec[index].partnerExtAddr,
                    &(keyEstablishRec[index].pRemoteCertificate[KEY_ESTABLISH_CERT_EXT_ADDR_IDX]),
                    Z_EXTADDR_LEN); // ID(L)

  // Change the state and wait for the Ephemeral Data Request
  keyEstablishRec[index].lastSeqNum = pInMsg->hdr.transSeqNum;
  keyEstablishRec[index].state = KeyEstablishState_EDataPending;
  keyEstablishRec[index].role = KEY_ESTABLISHMENT_RESPONDER;

  zclGeneral_KeyEstablish_Send_InitiateKeyEstablishmentRsp( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
            &pInMsg->msg->srcAddr,
            KEY_ESTABLISHMENT_SUITE,
            ZCL_KEY_ESTABLISHMENT_EKEY_GENERATE_TIMEOUT + ZCL_KEY_ESTABLISHMENT_KEY_GENERATE_TIMEOUT,
            ZCL_KEY_ESTABLISHMENT_MAC_GENERATE_TIMEOUT * 2 ,
            implicitCert, FALSE, pInMsg->hdr.transSeqNum );

  // The Request was processed successfuly, now the age timer needs to start based on the
  // remote Ephemeral Data Generate Time
  keyEstablishRec[index].age = keyEstablishRec[index].remoteEphDataGenTime;

  // Start the Ephemeral Data Generate aging timer
  osal_start_reload_timer( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT,
                           KEY_ESTABLISHMENT_REC_AGING_INTERVAL );

  osal_mem_free(implicitCert);

#if defined (NWK_AUTO_POLL)
  // For polling end device, set the Key Establishment poll rate
  zclSavedPollRate = zgPollRate;
  NLME_SetPollRate(ZCL_KEY_ESTABLISH_POLL_RATE);
#endif

  return ZCL_STATUS_CMD_HAS_RSP;
}

/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_EphemeralDataReq
 *
 * @brief   Process the received Ephemeral Data Request.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_MALFORMED_COMMAND
 *                      ZCL_STATUS_CMD_HAS_RSP
 */
static ZStatus_t zclGeneral_ProcessInCmd_EphemeralDataReq( zclIncoming_t *pInMsg )
{
  uint8 index;
  uint8 status = ZFailure;

  // Omit checking the incoming packet length

  // Stop the Ephemeral Data Generate timer because the message has been received
  osal_stop_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT );

  // Check state of the key establishment record. If not match, terminate the procedure
  if ( ( index = zclGeneral_GetKeyEstablishRecIndex( pInMsg->msg->srcAddr.addr.shortAddr ) )
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    if ( keyEstablishRec[index].role == KEY_ESTABLISHMENT_RESPONDER &&
         keyEstablishRec[index].state == KeyEstablishState_EDataPending )
    {
      status = ZSuccess;

      // Copy the remote device Ephemeral Public key
      osal_memcpy( keyEstablishRec[index].pRemotePublicKey,
              &(pInMsg->pData[0]),
              ZCL_KE_CA_PUBLIC_KEY_LEN );
    }
    else
    {
      // Reset the entry
      zclGeneral_ResetKeyEstablishRec( index );
    }
  }

  if( status != ZSuccess )
  {
    // Either the entry doesn't exist or in the wrong state, send termination back
    zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &pInMsg->msg->srcAddr,
                                                            TermKeyStatus_BadMessage,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_SERVER_CLIENT_DIR,
                                                            FALSE, zcl_SeqNum++ );

#if defined (NWK_AUTO_POLL)
  // Restore the saved poll rate for end device
  NLME_SetPollRate(zclSavedPollRate);
#endif
    return ZCL_STATUS_CMD_HAS_RSP;
  }

  // Generate Ephemeral Public/Private Key Pair
  ZSE_ECCGenerateKey( (unsigned char *)keyEstablishRec[index].pLocalEPrivateKey,
                    (unsigned char *)keyEstablishRec[index].pLocalEPublicKey,
                    zclGeneral_KeyEstablishment_GetRandom,
                    zclKeyEstablish_YieldFunc, zclKeyEstablish_YieldLevel );

#if defined (DEBUG_STATIC_ECC)

  // For debug and testing purpose, use a fixed ephermeral key pair instead
  // of the randomly generated one.
  osal_memcpy( keyEstablishRec[index].pLocalEPrivateKey, private2, 21 );
  osal_memcpy( keyEstablishRec[index].pLocalEPublicKey, public2, 22 );
#endif

  // Change the state and wait for the Key to be calculated
  keyEstablishRec[index].state = KeyEstablishState_KeyCalculatePending;

  osal_start_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_CMD_PROCESS_EVT,
                      KEY_ESTABLISHMENT_WAIT_PERIOD );


  return ZCL_STATUS_CMD_HAS_RSP;
}


/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_InitiateKeyEstablishRsp
 *
 * @brief   Process the received Initiate Key Establishment Response.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_MALFORMED_COMMAND
 *                      ZCL_STATUS_CMD_HAS_RSP
 *                      ZCL_STATUS_SOFTWARE_FAILURE
 */
static ZStatus_t zclGeneral_ProcessInCmd_InitiateKeyEstablishRsp( zclIncoming_t *pInMsg )
{
  TermKeyStatus_t keyStatus = TermKeyStatus_Success;
  uint16 remoteKeyEstablishmentSuite;
  uint8 index = MAX_KEY_ESTABLISHMENT_REC_ENTRY; // set to non valid value
  uint8 status = ZFailure;

  // Stop the Key Establishment aging timer because the message has been received
  osal_stop_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT );

  // Check the incoming packet length
  if ( pInMsg->pDataLen >= PACKET_LEN_INITIATE_KEY_EST_RSP )
  {
    // Check state of the key establishment record. If not match, terminate the procedure
    if ( ( index = zclGeneral_GetKeyEstablishRecIndex( pInMsg->msg->srcAddr.addr.shortAddr ) )
        < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
    {
      if ( keyEstablishRec[index].role == KEY_ESTABLISHMENT_INITIATOR &&
          keyEstablishRec[index].state == KeyEstablishState_InitiatePending )
      {
        status = ZSuccess;

        // Parse the incoming message
        // Copy the remote device certificate
        osal_memcpy( keyEstablishRec[index].pRemoteCertificate, &(pInMsg->pData[KEY_ESTABLISH_CERT_IDX]),
                     ZCL_KE_IMPLICIT_CERTIFICATE_LEN );

        SSP_MemCpyReverse( keyEstablishRec[index].partnerExtAddr,
                           &(keyEstablishRec[index].pRemoteCertificate[KEY_ESTABLISH_CERT_EXT_ADDR_IDX]),
                           Z_EXTADDR_LEN);
      }
      else
      {
        // Reset the entry from the rec table
        zclGeneral_ResetKeyEstablishRec( index );
      }
    }
  }

  if ( status == ZFailure )
  {
    keyStatus = TermKeyStatus_BadMessage;
  }
  else
  {
    uint8 *implicitCert;

    // Parse the incoming message
    // Verify the certificate issuer and key establishment suite
    remoteKeyEstablishmentSuite = BUILD_UINT16( pInMsg->pData[0], pInMsg->pData[1] );
    if ( remoteKeyEstablishmentSuite != KEY_ESTABLISHMENT_SUITE )
    {
      keyStatus = TermKeyStatus_UnSupportedSuite;
    }
    else
    {
      // continue parsing message

      // Save Ephemeral Data Generate Key and Confirm Key Time
      if (pInMsg->pData[2] >= KEY_ESTABLISHMENT_EPH_DATA_GEN_INVALID_TIME)
      {
        status = TermKeyStatus_BadMessage;
      }
      else
      {
        // continue parsing message

        keyEstablishRec[index].remoteEphDataGenTime = pInMsg->pData[2];

        if (pInMsg->pData[3] >= KEY_ESTABLISHMENT_CONF_KEY_GEN_INVALID_TIME)
        {
          status = TermKeyStatus_BadMessage;
        }
        else
        {
          // continue parsing message

          keyEstablishRec[index].remoteConfKeyGenTime = pInMsg->pData[3];

          if ((implicitCert = osal_mem_alloc(ZCL_KE_IMPLICIT_CERTIFICATE_LEN)) == NULL)
          {
            // Reset the entry
            zclGeneral_ResetKeyEstablishRec( index );

            return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
          }

          osal_nv_read(ZCD_NV_IMPLICIT_CERTIFICATE, 0, ZCL_KE_IMPLICIT_CERTIFICATE_LEN, implicitCert);

          if ( !osal_memcmp( &(keyEstablishRec[index].pRemoteCertificate[KEY_ESTABLISH_CERT_ISSUER_IDX]),
                            &(implicitCert[KEY_ESTABLISH_CERT_ISSUER_IDX]),
                            KEY_ESTABLISH_CERT_ISSUER_LENTGH ) )
          {
            keyStatus = TermKeyStatus_UnknowIssuer;
          }

          osal_mem_free(implicitCert);
        }
      }
    }
  } // end of parsing of the message

  if ( keyStatus == TermKeyStatus_Success )
  {
    keyEstablishRec[index].state = KeyEstablishState_EDataPending;

    // Send Ephemeral Data Request back
    zclGeneral_KeyEstablish_Send_EphemeralDataReq( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                  &pInMsg->msg->srcAddr,
                                                  keyEstablishRec[index].pLocalEPublicKey,
                                                  FALSE, zcl_SeqNum++ );

    // The Request was processed successfuly, now the age timer needs to start based on the
    // remote Ephemeral Data Generate Time
    keyEstablishRec[index].age = keyEstablishRec[index].remoteEphDataGenTime;

    // Start the Ephemeral Data Generate aging timer
    osal_start_reload_timer( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT,
                            KEY_ESTABLISHMENT_REC_AGING_INTERVAL );
  }
  else
  {
    zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                           &pInMsg->msg->srcAddr,
                                                           keyStatus,
                                                           KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                           KEY_ESTABLISHMENT_SUITE,
                                                           ZCL_FRAME_CLIENT_SERVER_DIR,
                                                           FALSE, zcl_SeqNum++ );

    // Reset the entry
    if ( index < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
    {
      zclGeneral_ResetKeyEstablishRec( index );
    }

#if defined (NWK_AUTO_POLL)
    // Restore the saved poll rate for end device
    NLME_SetPollRate(zclSavedPollRate);
#endif
  }

  return ZCL_STATUS_CMD_HAS_RSP;
}

/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_EphemeralDataRsp
 *
 * @brief   Process the received Initiate Key Establishment Response.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_MALFORMED_COMMAND
 *                      ZCL_STATUS_CMD_HAS_RSP
 *                      ZCL_STATUS_SOFTWARE_FAILURE
 */
static ZStatus_t zclGeneral_ProcessInCmd_EphemeralDataRsp( zclIncoming_t *pInMsg )
{
  uint8 index;
  uint8 status = ZFailure;

  // Stop the Ephemeral Data Generate timer because the message has been received
  osal_stop_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT );

  // Check state of the key establishment record. If not match, terminate the procedure
  if ( ( index = zclGeneral_GetKeyEstablishRecIndex( pInMsg->msg->srcAddr.addr.shortAddr ) )
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    if ( keyEstablishRec[index].role == KEY_ESTABLISHMENT_INITIATOR &&
         keyEstablishRec[index].state == KeyEstablishState_EDataPending )
    {
      status = ZSuccess;

      // Copy the remote device Ephemeral Public key
      osal_memcpy( keyEstablishRec[index].pRemotePublicKey,
                  &(pInMsg->pData[0]),
                  ZCL_KE_CA_PUBLIC_KEY_LEN );
    }
    else
    {
      // Reset the entry from the rec table
      zclGeneral_ResetKeyEstablishRec( index );
    }
  }

  if ( status == ZFailure )
  {
    // No existing record found or the record found has a wrong state, terminate the procedure
    zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &pInMsg->msg->srcAddr,
                                                            TermKeyStatus_BadMessage,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_CLIENT_SERVER_DIR,
                                                            FALSE, zcl_SeqNum++ );
#if defined (NWK_AUTO_POLL)
    // Restore the saved poll rate for end device
    NLME_SetPollRate(zclSavedPollRate);
#endif
  }
  else
  {
    keyEstablishRec[index].state = KeyEstablishState_KeyCalculatePending;

    osal_start_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_RSP_PROCESS_EVT,
                       KEY_ESTABLISHMENT_WAIT_PERIOD );

  }

  return ZCL_STATUS_CMD_HAS_RSP;
}

/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_ConfirmKey
 *
 * @brief   Process the received Confirm Key Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_CMD_HAS_RSP
 *                      ZCL_STATUS_SOFTWARE_FAILURE
 */
static ZStatus_t zclGeneral_ProcessInCmd_ConfirmKey( zclIncoming_t *pInMsg )
{
  uint8 index;
  uint8 status = ZFailure;
  uint8 MACu[KEY_ESTABLISH_MAC_KEY_LENGTH];
  uint8 MACv[KEY_ESTABLISH_MAC_KEY_LENGTH];
  TermKeyStatus_t keyStatus = TermKeyStatus_Success;

  // Stop the Config Key Generate aging timer because the message has been received
  osal_stop_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT );

  // Check state of the key establishment record. If not match, terminate the procedure
  if ( ( index = zclGeneral_GetKeyEstablishRecIndex( pInMsg->msg->srcAddr.addr.shortAddr ) )
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    if ( keyEstablishRec[index].role == KEY_ESTABLISHMENT_RESPONDER &&
         keyEstablishRec[index].state == KeyEstablishState_ConfirmPending )
    {
      status = ZSuccess;
    }
    else
    {
      // Reset the entry
      zclGeneral_ResetKeyEstablishRec( index );
    }
  }

  if ( status == ZFailure )
  {
    keyStatus = TermKeyStatus_BadMessage;
  }
  else
  {
    // Calculate MAC(U). Note that the zData is also pointing to the macKey
    zclGeneral_KeyEstablishment_GenerateMAC( index, TRUE, MACu );

    // Compare MAC(U) with MAC(V)
    if ( osal_memcmp( MACu, pInMsg->pData, KEY_ESTABLISH_MAC_LENGTH ) == TRUE )
    {
      // Send Confirm Key Response with Status - SUCCESS
      keyEstablishRec[index].state = KeyEstablishState_TerminationPending;

      // Store the key in the key table

      ZDSecMgrAddLinkKey( pInMsg->msg->srcAddr.addr.shortAddr,
                         keyEstablishRec[index].partnerExtAddr,
                         keyEstablishRec[index].pKey );

      // Calculate MAC(V) and send it back
      zclGeneral_KeyEstablishment_GenerateMAC( index, FALSE, MACv );

      zclGeneral_KeyEstablish_Send_ConfirmKeyRsp( pInMsg->msg->endPoint,
                                                 &pInMsg->msg->srcAddr,
                                                 MACv,
                                                 FALSE, pInMsg->hdr.transSeqNum );
    }
    else
    {
      keyStatus = TermKeyStatus_BadKeyConfirm;
    }

    // Reset the entry, at this point the Key Establishment process has
    // finished and the record is not needed anymore
    zclGeneral_ResetKeyEstablishRec( index );
  }

  if( keyStatus != TermKeyStatus_Success)
  {
    // If MAC(U) does not match MAC(V), send response with failure
    zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &pInMsg->msg->srcAddr,
                                                            keyStatus,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_SERVER_CLIENT_DIR,
                                                            FALSE, zcl_SeqNum++ );
  }

#if defined (NWK_AUTO_POLL)
  // Key Establishment Procedure complete. Restore the saved poll rate for end device
  NLME_SetPollRate(zclSavedPollRate);
#endif

  return ZCL_STATUS_CMD_HAS_RSP;
}

/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_ConfirmKeyRsp
 *
 * @brief   Process the received Confirm Key Response.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_MALFORMED_COMMAND
 *                      ZCL_STATUS_CMD_HAS_RSP
 *                      ZCL_STATUS_SOFTWARE_FAILURE
 */
static ZStatus_t zclGeneral_ProcessInCmd_ConfirmKeyRsp( zclIncoming_t *pInMsg )
{
  uint8 index;
  uint8 status = ZFailure;
  uint8 MACv[KEY_ESTABLISH_MAC_LENGTH];

  // Stop the Config Key Generate aging timer because the message has been received
  osal_stop_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT );

  // Check state of the key establishment record. If not match, terminate the procedure
  if ( ( index = zclGeneral_GetKeyEstablishRecIndex( pInMsg->msg->srcAddr.addr.shortAddr ) )
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    if ( keyEstablishRec[index].role == KEY_ESTABLISHMENT_INITIATOR &&
         keyEstablishRec[index].state == KeyEstablishState_ConfirmPending )
    {
      status = ZSuccess;
    }
    else
    {
      // Reset the entry from the rec table
      zclGeneral_ResetKeyEstablishRec( index );
    }
  }

  if ( status == ZFailure )
  {
    status = TermKeyStatus_BadMessage;
  }
  else
  {
    // Calculate MAC(V)
    zclGeneral_KeyEstablishment_GenerateMAC( index, FALSE, MACv);

    // Compare M(U) with M(V)
    if ( osal_memcmp( MACv, pInMsg->pData, KEY_ESTABLISH_MAC_LENGTH ) == TRUE )
    {
      status = TermKeyStatus_Success;

      // Store the link key
      ZDSecMgrAddLinkKey( pInMsg->msg->srcAddr.addr.shortAddr,
                     keyEstablishRec[index].partnerExtAddr,
                     keyEstablishRec[index].pKey );
    }
    else
    {
      // If MAC(U) does not match MAC(V), send response with failure
      status = TermKeyStatus_BadKeyConfirm;
    }
  }

  if( status != TermKeyStatus_Success )
  {
    zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &pInMsg->msg->srcAddr,
                                                            (TermKeyStatus_t)status,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_CLIENT_SERVER_DIR,
                                                            FALSE, zcl_SeqNum++ );
  }

  // Send Osal message to the application to indicate the completion
  if ( keyEstablishRec[index].appTaskID != INVALID_TASK_ID )
  {
    keyEstablishmentInd_t *ind;

    ind = (keyEstablishmentInd_t *)osal_msg_allocate( sizeof( keyEstablishmentInd_t ) );
    if ( ind )
    {
      ind->hdr.event = ZCL_KEY_ESTABLISH_IND;
      ind->hdr.status = status;

      // Clear remaining fields
      ind->waitTime = 0;
      ind->keyEstablishmentSuite = 0;

      osal_msg_send( keyEstablishRec[index].appTaskID, (uint8*)ind );
    }
  }

  // End of this transection. Reset the entry from the rec table
  zclGeneral_ResetKeyEstablishRec( index );

#if defined (NWK_AUTO_POLL)
  // Key Establishment Procedure complete. Restore the saved poll rate for end device
  NLME_SetPollRate(zclSavedPollRate);
#endif
  return ZCL_STATUS_CMD_HAS_RSP;
}

/*********************************************************************
 * @fn      zclGeneral_ProcessInCmd_TerminateKeyEstablish
 *
 * @brief   Process the received Terminate Key Establishment Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZSuccess @ Success
 */
static ZStatus_t zclGeneral_ProcessInCmd_TerminateKeyEstablish( zclIncoming_t *pInMsg )
{
  uint8 index;

  // Find the key establishment record and delete the record entry.
  if ( ( index = zclGeneral_GetKeyEstablishRecIndex( pInMsg->msg->srcAddr.addr.shortAddr ) )
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    if ( keyEstablishRec[index].appTaskID != INVALID_TASK_ID )
    {
      keyEstablishmentInd_t *ind;

      // Send osal message to the application
      ind = (keyEstablishmentInd_t *)osal_msg_allocate( sizeof( keyEstablishmentInd_t ) );
      if ( ind )
      {
        ind->hdr.event = ZCL_KEY_ESTABLISH_IND;
        ind->hdr.status = pInMsg->pData[0];
        ind->waitTime = pInMsg->pData[1];
        ind->keyEstablishmentSuite = BUILD_UINT16( pInMsg->pData[2], pInMsg->pData[3] );
        osal_msg_send( keyEstablishRec[index].appTaskID, (uint8*)ind );
      }
    }
    // In either case, remove the entry from the rec table
    zclGeneral_ResetKeyEstablishRec( index );

#if defined (NWK_AUTO_POLL)
    // Restore the saved poll rate for end device
    NLME_SetPollRate(zclSavedPollRate);
#endif
  }
  return ZSuccess;
}

/*********************************************************************
 * @fn      zclGeneral_InitiateKeyEstablish_Cmd_CalculateKey
 *
 * @brief   Calculate the Key using ECC library upon receipt of Initiate
            Key Establishment Command.
 *
 * @param   none
 *
 * @return  ZStatus_t - ZFailure @ Entry pending key calculation not found
 *                      ZSuccess
 */
static ZStatus_t zclGeneral_InitiateKeyEstablish_Cmd_CalculateKey( void )
{
  uint8 zData[KEY_ESTABLISH_SHARED_SECRET_LENGTH];
  uint8 *caPublicKey, *devicePrivateKey, *keyBit;
  uint8 index, status, tmp;

  // It is possible to have multiple entries in the keyCalulationPending state.
  // Here we assume the partner that starts the key establishment procedure earlier
  // will have a smaller index in the table.
  // However, this might not apply due to different processing capability of
  // different processors.
  if ( (index = zclGeneral_GetKeyEstablishRecIndex_State( KeyEstablishState_KeyCalculatePending ))
       >= MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    return ZFailure;
  }

  if ((caPublicKey = osal_mem_alloc(ZCL_KE_CA_PUBLIC_KEY_LEN)) == NULL)
  {
    // Reset the entry
    zclGeneral_ResetKeyEstablishRec( index );

    return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
  }
  if ((devicePrivateKey = osal_mem_alloc(ZCL_KE_DEVICE_PRIVATE_KEY_LEN)) == NULL)
  {
    osal_mem_free(caPublicKey);

    // Reset the entry
    zclGeneral_ResetKeyEstablishRec( index );

    return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
  }

  osal_nv_read(ZCD_NV_CA_PUBLIC_KEY, 0, ZCL_KE_CA_PUBLIC_KEY_LEN, caPublicKey);
  osal_nv_read(ZCD_NV_DEVICE_PRIVATE_KEY, 0, ZCL_KE_DEVICE_PRIVATE_KEY_LEN, devicePrivateKey);

  // Turn off the radio
  tmp = FALSE;
  ZMacSetReq( ZMacRxOnIdle, &tmp );

  status = ZSE_ECCKeyBitGenerate( devicePrivateKey, keyEstablishRec[index].pLocalEPrivateKey,
                    keyEstablishRec[index].pLocalEPublicKey,
                    keyEstablishRec[index].pRemoteCertificate,
                    keyEstablishRec[index].pRemotePublicKey,
                    caPublicKey, zData,
                    zclGeneral_KeyEstablishment_HashFunc,
                    zclKeyEstablish_YieldFunc, zclKeyEstablish_YieldLevel);
  tmp = TRUE;
  ZMacSetReq( ZMacRxOnIdle, &tmp );  // Turn the radio back on

  osal_mem_free(caPublicKey);
  osal_mem_free(devicePrivateKey);

  if( status == MCE_SUCCESS )
  {
    // Allocate buffer to store KDF(Z) = MacKey || KeyData
    if ( (keyBit = osal_mem_alloc( KEY_ESTABLISH_KEY_DATA_LENGTH +
                                 KEY_ESTABLISH_MAC_KEY_LENGTH)) == NULL )
    {
      // Reset the entry
      zclGeneral_ResetKeyEstablishRec( index );

      return  ZCL_STATUS_SOFTWARE_FAILURE; // Memory allocation failure
    }

    // Derive the keying data using KDF function
    zclGeneral_KeyEstablishment_KeyDeriveFunction(zData,
                                                  KEY_ESTABLISH_SHARED_SECRET_LENGTH,
                                                  keyBit );

    // Save the derived 128-bit key and macKey
    osal_memcpy( keyEstablishRec[index].pMacKey, keyBit, KEY_ESTABLISH_MAC_KEY_LENGTH );
    osal_memcpy( keyEstablishRec[index].pKey, &(keyBit[KEY_ESTABLISH_MAC_KEY_LENGTH]),
                KEY_ESTABLISH_KEY_DATA_LENGTH);
    osal_mem_free( keyBit );

    // Key Bit generation success, send Ephemeral Data Response back
    zclGeneral_KeyEstablish_Send_EphemeralDataRsp( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                   &(keyEstablishRec[index].dstAddr),
                                                   keyEstablishRec[index].pLocalEPublicKey,
                                                   FALSE, keyEstablishRec[index].lastSeqNum );

    // The Request was processed successfuly, now the age timer needs to start based on the
    // remote Config Key Generate
    keyEstablishRec[index].age = keyEstablishRec[index].remoteConfKeyGenTime;

    // Start the Config Key Generate aging timer
    osal_start_reload_timer( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT,
                             KEY_ESTABLISHMENT_REC_AGING_INTERVAL );
  }
  else
  {
    // Key Bit generation failure. Send terminate key command
     zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &(keyEstablishRec[index].dstAddr),
                                                            TermKeyStatus_BadKeyConfirm,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_SERVER_CLIENT_DIR,
                                                            FALSE, zcl_SeqNum++ );
    // Reset the entry
    zclGeneral_ResetKeyEstablishRec( index );

#if defined (NWK_AUTO_POLL)
    // Restore the saved poll rate for end device
    NLME_SetPollRate(zclSavedPollRate);
#endif

     return ZFailure;
  }

  keyEstablishRec[index].state = KeyEstablishState_ConfirmPending;
  return ZSuccess;
}

/*********************************************************************
 * @fn      zclGeneral_InitiateKeyEstablish_Rsp_CalculateKey
 *
 * @brief   Calculate the Key using ECC library upon receipt of
 *          Ephemeral Data Response.
 *
 * @param   none
 *
 * @return  ZStatus_t - ZFailure @ Unsupported
 *                      ZCL_STATUS_MALFORMED_COMMAND
 *                      ZCL_STATUS_CMD_HAS_RSP
 */
static ZStatus_t zclGeneral_InitiateKeyEstablish_Rsp_CalculateKey( void )
{
  uint8 zData[KEY_ESTABLISH_SHARED_SECRET_LENGTH];
  uint8 MACu[KEY_ESTABLISH_MAC_LENGTH];
  uint8 *caPublicKey, *devicePrivateKey, *keyBit;
  uint8 index, ret, tmp, currentRxState;

  // It is possible to have multiple entries in the keyCalulationPending state.
  // Here we assume the partner that starts the key establishment procedure earlier
  // will have a smaller index in the table.
  // However, this might not apply due to different processing capability of
  // different processors.
  if ( (index = zclGeneral_GetKeyEstablishRecIndex_State( KeyEstablishState_KeyCalculatePending ))
       >= MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    return ZFailure;
  }

  if ((caPublicKey = osal_mem_alloc(ZCL_KE_CA_PUBLIC_KEY_LEN)) == NULL)
  {
    // Reset the entry from the rec table
    zclGeneral_ResetKeyEstablishRec( index );

    return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
  }
  if ((devicePrivateKey = osal_mem_alloc(ZCL_KE_DEVICE_PRIVATE_KEY_LEN)) == NULL)
  {
    // Reset the entry from the rec table
    zclGeneral_ResetKeyEstablishRec( index );

    osal_mem_free(caPublicKey);
    return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
  }
  osal_nv_read(ZCD_NV_CA_PUBLIC_KEY, 0, ZCL_KE_CA_PUBLIC_KEY_LEN, caPublicKey);
  osal_nv_read(ZCD_NV_DEVICE_PRIVATE_KEY, 0, ZCL_KE_DEVICE_PRIVATE_KEY_LEN, devicePrivateKey);

  ZMacGetReq( ZMacRxOnIdle, &currentRxState );  // Save current radio state
  // Turn off the radio before the key bit generation, in order to avoid
  // incoming messages accumulation by interrupts during the long process time.
  tmp = FALSE;
  ZMacSetReq( ZMacRxOnIdle, &tmp );

  // Generate the Key Bits
  ret = ZSE_ECCKeyBitGenerate( devicePrivateKey, keyEstablishRec[index].pLocalEPrivateKey,
                             keyEstablishRec[index].pLocalEPublicKey,
                             keyEstablishRec[index].pRemoteCertificate,
                             keyEstablishRec[index].pRemotePublicKey,
                             caPublicKey, zData,
                             zclGeneral_KeyEstablishment_HashFunc,
                             zclKeyEstablish_YieldFunc, zclKeyEstablish_YieldLevel);

  ZMacSetReq( ZMacRxOnIdle, &currentRxState );  // Resume saved radio state

  osal_mem_free(caPublicKey);
  osal_mem_free(devicePrivateKey);

  if ( ret != MCE_SUCCESS )
  {
    // Key Bit generation failure. Send terminate key command
     zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                                            &(keyEstablishRec[index].dstAddr),
                                                            TermKeyStatus_BadKeyConfirm,
                                                            KEY_ESTABLISHMENT_AVG_TIMEOUT,
                                                            KEY_ESTABLISHMENT_SUITE,
                                                            ZCL_FRAME_CLIENT_SERVER_DIR,
                                                            FALSE, zcl_SeqNum++ );
     // Reset the entry from the rec table
     zclGeneral_ResetKeyEstablishRec( index );

#if defined (NWK_AUTO_POLL)
    // Restore the saved poll rate for end device
    NLME_SetPollRate(zclSavedPollRate);
#endif

    return ZFailure;
  }
  else
  {
    // Allocate buffer to store KDF(Z) = MacKey || KeyData
    if ( (keyBit = osal_mem_alloc( KEY_ESTABLISH_KEY_DATA_LENGTH +
                                 KEY_ESTABLISH_MAC_KEY_LENGTH)) == NULL )
    {
      // Reset the entry from the rec table
      zclGeneral_ResetKeyEstablishRec( index );

      return  ZCL_STATUS_SOFTWARE_FAILURE; // Memory allocation failure
    }

    // Derive the keying data using KDF function
    zclGeneral_KeyEstablishment_KeyDeriveFunction(zData,
                                                  KEY_ESTABLISH_SHARED_SECRET_LENGTH,
                                                  keyBit );

    // Save the derived 128-bit keyData
    osal_memcpy( keyEstablishRec[index].pMacKey, keyBit, KEY_ESTABLISH_KEY_DATA_LENGTH);
    osal_memcpy( keyEstablishRec[index].pKey, &(keyBit[KEY_ESTABLISH_MAC_KEY_LENGTH]),
                 KEY_ESTABLISH_KEY_DATA_LENGTH);

    // Calculate MAC(U). Note that the keyBit is also pointing to the macKey
    zclGeneral_KeyEstablishment_GenerateMAC( index, TRUE, MACu );
    osal_mem_free( keyBit );

    // Send MAC(U) to the Partner
    zclGeneral_KeyEstablish_Send_ConfirmKey( ZCL_KEY_ESTABLISHMENT_ENDPOINT,
                                             &(keyEstablishRec[index].dstAddr),
                                             MACu,
                                             FALSE, zcl_SeqNum++ );

    // The Request was processed successfuly, now the age timer needs to start based on the
    // remote Config Key Generate
    keyEstablishRec[index].age = keyEstablishRec[index].remoteConfKeyGenTime;

    // Start the Config Key Generate aging timer
    osal_start_reload_timer( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT,
                             KEY_ESTABLISHMENT_REC_AGING_INTERVAL );

    keyEstablishRec[index].state = KeyEstablishState_ConfirmPending;

    return ZSuccess;
  }
}

/*********************************************************************
 * @fn      zclGeneral_InitKeyEstablishRecTable
 *
 * @brief   Initializae key establishment record table entries.
 *
 * @param   none
 *
 * @return  none
 */
static void zclGeneral_InitKeyEstablishRecTable( void )
{
  uint8 i;

  for ( i = 0; i < MAX_KEY_ESTABLISHMENT_REC_ENTRY; i++ )
  {
    keyEstablishRec[i].dstAddr.addrMode = afAddrNotPresent;
    keyEstablishRec[i].dstAddr.addr.shortAddr = INVALID_PARTNER_ADDR;
    keyEstablishRec[i].state = KeyEstablishState_Idle;
    keyEstablishRec[i].appTaskID = INVALID_TASK_ID;
    keyEstablishRec[i].remoteEphDataGenTime = KEY_ESTABLISHMENT_EPH_DATA_GEN_INVALID_TIME;
    keyEstablishRec[i].remoteConfKeyGenTime = KEY_ESTABLISHMENT_CONF_KEY_GEN_INVALID_TIME;
  }
}

/*********************************************************************
 * @fn      zclGeneral_GetKeyEstablishRecIndex
 *
 * @brief   Get the index of a particular key establishment record.
 *          If the input is INVALID_PARTNER_ADDR, return an empty slot.
 *
 * @param   partnerAddress - address of the partner that the local device
 *                           is establishing key with.
 *
 * @return   index of the record
 */
static uint8 zclGeneral_GetKeyEstablishRecIndex( uint16 partnerAddress )
{
  uint8 i;

  // Find an existing entry or vacant entry, depends on what DstAddress is
  for ( i = 0; i < MAX_KEY_ESTABLISHMENT_REC_ENTRY ; i++ )
  {
    if ( keyEstablishRec[i].dstAddr.addr.shortAddr == partnerAddress )
    {
      // entry found
      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      zclGeneral_GetKeyEstablishRecIndex
 *
 * @brief   Get the index of a particular key establishment record.
 *          If the input is INVALID_PARTNER_ADDR, return an empty slot.
 *
 * @param   state - state to find.
 *
 * @return   index of the record
 */
static uint8 zclGeneral_GetKeyEstablishRecIndex_State( KeyEstablishState_t state )
{
  uint8 i;

  // Find an existing entry or vacant entry, depends on what DstAddress is
  for ( i = 0; i < MAX_KEY_ESTABLISHMENT_REC_ENTRY ; i++ )
  {
    if ( keyEstablishRec[i].state == state )
    {
      // entry found
      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      zclGeneral_AddKeyEstablishRec
 *
 * @brief   Add a new key establishment record. If one already exist,
 *          remove the existng entry. After initialization, fill in
 *          partner short address and extended address. If partner extended
 *          address not available, return failure.
 *
 * @param   addr - address of the partner
 *
 * @return  index - 0 ~ (MAX_KEY_ESTABLISHMENT_REC_ENTRY-1) @ success
 *                  MAX_KEY_ESTABLISHMENT_REC_ENTRY @ failure due to rec
 *                  table full or partner IEEE address not available
 */
static uint8 zclGeneral_AddKeyEstablishRec( afAddrType_t *addr )
{
  uint8 index;
  uint8 *pBuf;

  // Search for all current key establishment record
  // If not found, create a new entry
  if ( ( index = zclGeneral_GetKeyEstablishRecIndex(addr->addr.shortAddr) )
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    // expire the existing entry for this address
    zclGeneral_ResetKeyEstablishRec( index );
  }

  // Create a new Entry
  if ( (index = zclGeneral_GetKeyEstablishRecIndex(INVALID_PARTNER_ADDR))
      < MAX_KEY_ESTABLISHMENT_REC_ENTRY )
  {
    osal_memset( &(keyEstablishRec[index]), 0, sizeof ( zclKeyEstablishRec_t ) );

    // Fill in initial values
    keyEstablishRec[index].dstAddr.addrMode = addr->addrMode;
    keyEstablishRec[index].dstAddr.addr.shortAddr = addr->addr.shortAddr;
    keyEstablishRec[index].dstAddr.endPoint = addr->endPoint;
    keyEstablishRec[index].age = KEY_ESTABLISHMENT_REC_EXPIRY_TIME;
    keyEstablishRec[index].state = KeyEstablishState_Idle;

    // extAddr will be unknown when the initator first initiates the key establishment
    // It will be filled in later after the remote certificate is received.

    // Allocate memory for the rest of the fields
    if( ( pBuf = osal_mem_alloc( ZCL_KE_DEVICE_PRIVATE_KEY_LEN +
                               ZCL_KE_CA_PUBLIC_KEY_LEN +
                               ZCL_KE_CA_PUBLIC_KEY_LEN +
                               ZCL_KE_IMPLICIT_CERTIFICATE_LEN +
                               KEY_ESTABLISH_KEY_DATA_LENGTH +
                               KEY_ESTABLISH_MAC_KEY_LENGTH )) != NULL )
    {
      keyEstablishRec[index].pLocalEPrivateKey = pBuf;
      pBuf += ZCL_KE_DEVICE_PRIVATE_KEY_LEN;
      keyEstablishRec[index].pLocalEPublicKey = pBuf;
      pBuf += ZCL_KE_CA_PUBLIC_KEY_LEN;
      keyEstablishRec[index].pRemotePublicKey = pBuf;
      pBuf += ZCL_KE_CA_PUBLIC_KEY_LEN;
      keyEstablishRec[index].pRemoteCertificate = pBuf;
      pBuf += ZCL_KE_IMPLICIT_CERTIFICATE_LEN;
      keyEstablishRec[index].pKey = pBuf;
      pBuf += KEY_ESTABLISH_KEY_DATA_LENGTH;
      keyEstablishRec[index].pMacKey = pBuf;

    }
    else
    {
      return ZMemError;
    }
  }

  return index;
}

/*********************************************************************
 * @fn      zclGeneral_AgeKeyEstablishRec
 *
 * @brief   Function to age Key Establish Rec. This function is called
 *          as event handler for KEY_ESTABLISHMENT_REC_AGING_EVT every
 *          second.
 *
 * @param   none
 *
 * @return  none
 */
static void zclGeneral_AgeKeyEstablishRec( void )
{
  uint8 i;
  bool recFound = FALSE;

  for ( i = 0; i < MAX_KEY_ESTABLISHMENT_REC_ENTRY; i++ )
  {
    // Only age valid rec entry
    if (keyEstablishRec[i].dstAddr.addrMode == afAddrNotPresent)
    {
      continue;
    }

    if (--(keyEstablishRec[i].age) == 0)
    {
      // Reset this table entry
      zclGeneral_ResetKeyEstablishRec( i );
    }
    else
    {
       recFound = TRUE;
    }
  }

  if ( recFound == FALSE )
  {
    osal_stop_timerEx( zcl_KeyEstablishment_TaskID, KEY_ESTABLISHMENT_REC_AGING_EVT );
  }
}

/*********************************************************************
 * @fn      zclGeneral_ResetKeyEstablishRec
 *
 * @brief   Reset specified key establishment record to initial value.
 *
 * @param   index - index of table entry to reset
 *
 * @return   ZStatus_t - ZSuccess or ZFailure
 */
static void zclGeneral_ResetKeyEstablishRec( uint8 index )
{
  uint8 *pKeys;

  pKeys = keyEstablishRec[index].pLocalEPrivateKey;
  if ( pKeys != NULL )
  {
    // All "Key infomation" was allocated in one block,
    // Clear the allocated memory to remove all copies of keys,
    osal_memset( pKeys, 0, ZCL_KE_DEVICE_PRIVATE_KEY_LEN +
                          ZCL_KE_CA_PUBLIC_KEY_LEN +
                          ZCL_KE_CA_PUBLIC_KEY_LEN +
                          ZCL_KE_IMPLICIT_CERTIFICATE_LEN +
                          KEY_ESTABLISH_KEY_DATA_LENGTH +
                          KEY_ESTABLISH_MAC_KEY_LENGTH );
    // Free the allocated memory
    osal_mem_free( pKeys );
  }

  // Reset the table entry to initial state
  osal_memset( &(keyEstablishRec[index]), 0, sizeof( zclKeyEstablishRec_t ) );
  keyEstablishRec[index].dstAddr.addrMode = afAddrNotPresent;
  keyEstablishRec[index].dstAddr.addr.shortAddr = INVALID_PARTNER_ADDR;
  keyEstablishRec[index].remoteEphDataGenTime = KEY_ESTABLISHMENT_EPH_DATA_GEN_INVALID_TIME;
  keyEstablishRec[index].remoteConfKeyGenTime = KEY_ESTABLISHMENT_CONF_KEY_GEN_INVALID_TIME;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablishment_GetRandom
 *
 * @brief   Fill in a buffer with random numbers
 *
 * @param   buffer - output buffer
 *          len - length of the buffer
 *
 * @return  MCE_SUCCESS indicates success
 */
static int zclGeneral_KeyEstablishment_GetRandom(unsigned char *buffer, unsigned long len)
{
  uint8 *pBuf;

  pBuf = buffer;

  // Input to SSP_GetTrueRandAES assumes len <= SEC_KEY_LEN
  // Therefore, call SSP_GetTrueRandAES multiple times to
  // fill out the buffer.
  while( len > SEC_KEY_LEN )
  {
    SSP_GetTrueRandAES( SEC_KEY_LEN, pBuf );
    len -= SEC_KEY_LEN;
    pBuf += SEC_KEY_LEN;
  }
  SSP_GetTrueRandAES( len, pBuf );
  return MCE_SUCCESS;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablishment_HashFunc
 *
 * @brief   Hash Function
 *
 * @param   digest - output buffer 16 bytes
 *          len - length of the input buffer
 *          data - input buffer
 *
 * @return  MCE_SUCCESS indicates success
 */
static int zclGeneral_KeyEstablishment_HashFunc(unsigned char *digest, unsigned long len, unsigned char *data)
{
  len *= 8;  // Convert to bit length
  sspMMOHash( NULL, 0, data, (uint16)len, digest );
  return MCE_SUCCESS;
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablishment_KeyDeriveFunction
 *
 * @brief   Key Derive Function (ANSI X9.63).
 *          Note this is not a generalized KDF. It only applies to the KDF
 *          specified in ZigBee SE profile. Only the first two hashed keys
 *          are calculated and concatenated.
 *
 * @param   zData - input shared secret (length = KEY_ESTABLISH_SHARED_SECRET_LENGTH)
 *          keyBitLen - input key data length
 *          keyBit - output buffer ( 16*2 bytes)
 *
 * @return  none
 */
static void zclGeneral_KeyEstablishment_KeyDeriveFunction( uint8 *zData,
                                                           uint8 keyBitLen,
                                                           uint8 *keyBit )
{
  uint8 hashCounter[4] = {0x00, 0x00, 0x00, 0x01};
  uint8 hashedData[KEY_ESTABLISH_SHARED_SECRET_LENGTH + 4];
  uint8 bitLen;

  bitLen = (keyBitLen + 4 ) * 8;

  // Calculate K1: Ki = Hash(Z || Counter1 )
  osal_memcpy( hashedData, zData, KEY_ESTABLISH_SHARED_SECRET_LENGTH );
  osal_memcpy( &(hashedData[KEY_ESTABLISH_SHARED_SECRET_LENGTH]), hashCounter, 4);

  sspMMOHash(NULL, 0, hashedData, bitLen, keyBit);

  // Indrement the counter
  hashedData[KEY_ESTABLISH_SHARED_SECRET_LENGTH + 3] = 0x02;

  sspMMOHash(NULL, 0, hashedData, bitLen, &(keyBit[KEY_ESTABLISH_KEY_DATA_LENGTH]));
}

/*********************************************************************
 * @fn      zclGeneral_KeyEstablishment_GenerateMAC
 *
 * @brief   Key Derive Function (ANSI X9.63).
 *          Note this is not a generalized KDF. It only applies to the KDF
 *          specified in ZigBee SE profile. Only the first two hashed keys
 *          are calculated and concatenated.
 *
 * @param   recIndex - input key establishment record index
 *          ifMACu - use M(U) if TRUE, otherwise M(V)
 *          MAC - output buffer ( 16 bytes )
 *
 * @return  ZStatus_t - success
 */
static ZStatus_t zclGeneral_KeyEstablishment_GenerateMAC(uint8 recIndex,
                                                         uint8 ifMACu,
                                                         uint8 *MAC)
{
  uint8 i;
  uint8 M;
  uint8 *hashBuf;
  uint16 bufLen;

  // Assumption for M(U) and M(V) is: M(U) = 0x02, M(V) = 0x03
  if( ifMACu == TRUE )
  {
    M = 0x02;  // Assumption
  }
  else
  {
    M = 0x03;  // Assumption
  }

  // At this point, it is assumed the device has already
  // obtained the IEEE address of the partner device.
  for ( i = 0; i < Z_EXTADDR_LEN; i++ )
  {
    if ( keyEstablishRec[recIndex].partnerExtAddr[i] != 0 )
    {
      break;
    }
  }
  if ( i == Z_EXTADDR_LEN )
  {
    return ZFailure;  // Partner IEEE address not available, return failure.
  }

  // MAC(U) = MAC(MacKey) { M(U) || ID(U) || ID(V) || E(U) || E(V) }
  bufLen = (1 + (Z_EXTADDR_LEN * 2) + (ZCL_KE_CA_PUBLIC_KEY_LEN * 2));
  if( ( hashBuf = osal_mem_alloc( (bufLen) )) == NULL )
  {
    return ZMemError;  // Memory allocation error
  }

  // Fill in the buffer
  hashBuf[0] = M;  // M(U)
  bufLen = bufLen * 8;  // Convert to bitlength

  if ( (keyEstablishRec[recIndex].role == KEY_ESTABLISHMENT_INITIATOR && ifMACu == TRUE) ||
       (keyEstablishRec[recIndex].role == KEY_ESTABLISHMENT_RESPONDER && ifMACu == FALSE))
  {
    // MAC = MAC(MacKey) { M() || ID(L) || ID(R) || E(L) || E(R) }
    // L - Local, R - Remote
    SSP_MemCpyReverse( &(hashBuf[1]), NLME_GetExtAddr(), Z_EXTADDR_LEN); // ID(U)
    SSP_MemCpyReverse( &(hashBuf[1+Z_EXTADDR_LEN]), keyEstablishRec[recIndex].partnerExtAddr,
                Z_EXTADDR_LEN); // ID(V)
    osal_memcpy( &(hashBuf[1 + (2 * Z_EXTADDR_LEN)]),                               // E(U)
                keyEstablishRec[recIndex].pLocalEPublicKey,
                ZCL_KE_CA_PUBLIC_KEY_LEN );
    osal_memcpy( &(hashBuf[1 + (2 * Z_EXTADDR_LEN) + ZCL_KE_CA_PUBLIC_KEY_LEN]), // E(V)
                keyEstablishRec[recIndex].pRemotePublicKey, ZCL_KE_CA_PUBLIC_KEY_LEN );

    SSP_KeyedHash (hashBuf, bufLen, keyEstablishRec[recIndex].pMacKey, MAC);
  }
  else
  {
    // MAC = MAC(MacKey) { M() || ID(R) || ID(L) || E(R) || E(L) }
    // L - Local, R - Remote
    SSP_MemCpyReverse( &(hashBuf[1]), keyEstablishRec[recIndex].partnerExtAddr,
                Z_EXTADDR_LEN); // ID(R)
    SSP_MemCpyReverse( &(hashBuf[1 + Z_EXTADDR_LEN]), NLME_GetExtAddr(), Z_EXTADDR_LEN); // ID(L)
    osal_memcpy( &(hashBuf[ 1 + (2 * Z_EXTADDR_LEN)]), // E(R)
                keyEstablishRec[recIndex].pRemotePublicKey,
                ZCL_KE_CA_PUBLIC_KEY_LEN );
    osal_memcpy( &(hashBuf[1 + (2 * Z_EXTADDR_LEN) + ZCL_KE_CA_PUBLIC_KEY_LEN]),                               // E(U)
                keyEstablishRec[recIndex].pLocalEPublicKey,
                ZCL_KE_CA_PUBLIC_KEY_LEN );
    SSP_KeyedHash (hashBuf, bufLen, keyEstablishRec[recIndex].pMacKey, MAC);
  }

  osal_mem_free(hashBuf);
  return ZSuccess;
}


/*********************************************************************
 * @fn      zclGeneral_KeyEstablishment_ECDSASign
 *
 * @brief    Creates an ECDSA signature of a message digest.
 *
 * @param   input - input data buffer
 *          inputLen - byte length of the input buffer
 *          output - output buffer ( 21x2 bytes )
 *
 * @return  ZStatus_t - success
 */
ZStatus_t zclGeneral_KeyEstablishment_ECDSASign( uint8 *input,  uint8 inputLen,
                                             uint8 *output)
{
  uint8 msgDigest[KEY_ESTABLISH_AES_MMO_HASH_SIZE];
  uint16 bitLen = inputLen * 8;
  uint8 status;
  uint8 *devicePrivateKey;

  if ((devicePrivateKey = osal_mem_alloc(ZCL_KE_DEVICE_PRIVATE_KEY_LEN)) == NULL)
  {
    return ZCL_STATUS_SOFTWARE_FAILURE;  // Memory allocation failure.
  }
  osal_nv_read(ZCD_NV_DEVICE_PRIVATE_KEY, 0, ZCL_KE_DEVICE_PRIVATE_KEY_LEN, devicePrivateKey);

  // First hash the input buffer
  sspMMOHash(NULL, 0, input, bitLen, msgDigest);

  status = ZSE_ECDSASign( (unsigned char*)devicePrivateKey, (unsigned char*)msgDigest,
                zclGeneral_KeyEstablishment_GetRandom,
               (unsigned char*)output, (unsigned char*)output + KEY_ESTABLISH_POINT_ORDER_SIZE,
               zclKeyEstablish_YieldFunc, zclKeyEstablish_YieldLevel );

  osal_mem_free(devicePrivateKey);

  if (status == MCE_SUCCESS )
  {
    return ZSuccess;
  }

  return ZFailure;
}


/*********************************************************************
 * @fn      zclGeneral_KeyEstablishment_ECDSAVerify
 *
 * @brief    Verify an ECDSA signature of a message digest.
 *
 * @param   input - input data buffer
 *          inputLen - byte length of the input buffer
 *          signature - input signature ( 21x2 bytes )
 *
 * @return  ZSuccess - success verify
 *          ZFailure - fail to verify
 */
ZStatus_t zclGeneral_KeyEstablishment_ECDSAVerify( uint8 *input,  uint8 inputLen,
                                             uint8 *signature)
{

  uint8 msgDigest[KEY_ESTABLISH_AES_MMO_HASH_SIZE];
  uint16 bitLen;
  uint8 ret;

  bitLen = inputLen * 8;

  // First hash the input buffer
  sspMMOHash(NULL, 0, input, bitLen, msgDigest);

  ret = ZSE_ECDSAVerify((unsigned char*)NULL, (unsigned char*)msgDigest,
             (unsigned char*)signature, (unsigned char*)signature + KEY_ESTABLISH_POINT_ORDER_SIZE,
             zclKeyEstablish_YieldFunc, zclKeyEstablish_YieldLevel );

  if ( ret == MCE_SUCCESS )
  {
    return ZSuccess;
  }

  return ZFailure;
}
#endif // ZCL_KEY_ESTABLISH

/***************************************************************************
****************************************************************************/
