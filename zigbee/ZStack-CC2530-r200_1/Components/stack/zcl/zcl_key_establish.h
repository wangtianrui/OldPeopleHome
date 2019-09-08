/**************************************************************************************************
  Filename:       zcl_key_establish.h
  Revised:        $Date: 2011-07-13 11:41:07 -0700 (Wed, 13 Jul 2011) $
  Revision:       $Revision: 26769 $

  Description:    This file contains the ZCL General Function Domain, key
                  establishment cluster definitions.


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
#ifndef ZCL_KEY_ESTABLISH_H
#define ZCL_KEY_ESTABLISH_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "zcl.h"
#include "eccapi.h"

/*********************************************************************
 * CONSTANTS
 */

#define ZCL_KEY_ESTABLISHMENT_ENDPOINT                  10  // Reserved endpoint for key establishment cluster

// Key Establishment Task Events
#define KEY_ESTABLISHMENT_REC_AGING_EVT                 0x01
#define KEY_ESTABLISHMENT_CMD_PROCESS_EVT               0x02
#define KEY_ESTABLISHMENT_RSP_PROCESS_EVT               0x04
#define KEY_ESTABLISHMENT_REC_AGING_INTERVAL            1000  // in ms
#define KEY_ESTABLISHMENT_WAIT_PERIOD                   500

// Key Establishment Cluster Attributes
#define ATTRID_KEY_ESTABLISH_SUITE                       0x0000
#define CERTIFICATE_BASED_KEY_ESTABLISHMENT              0x0000

// Key Establishment Cluster Command ID
#define COMMAND_INITIATE_KEY_ESTABLISHMENT               0x00
#define COMMAND_EPHEMERAL_DATA_REQUEST                   0x01
#define COMMAND_CONFIRM_KEY                              0x02
#define COMMAND_TERMINATE_KEY_ESTABLISHMENT              0x03
#define COMMAND_INITIATE_KEY_ESTABLISHMENT_RESPONSE      0x00
#define COMMAND_EPHEMERAL_DATA_RESPONSE                  0x01
#define COMMAND_CONFIRM_KEY_RESPONSE                     0x02

#define COMMAND_GET_LINK_KEY                             0x04

// Command Packet Length
#define PACKET_LEN_INITIATE_KEY_EST_REQ                 (4+ZCL_KE_IMPLICIT_CERTIFICATE_LEN)
#define PACKET_LEN_INITIATE_KEY_EST_RSP                 (4+ZCL_KE_IMPLICIT_CERTIFICATE_LEN)

// Length of the implicit certificate
#define KEY_ESTABLISH_SHARED_SECRET_LENGTH               21
#define KEY_ESTABLISH_KEY_DATA_LENGTH                    16
#define KEY_ESTABLISH_MAC_KEY_LENGTH                     16
#define KEY_ESTABLISH_MAC_LENGTH                         16
#define KEY_ESTABLISH_POINT_ORDER_SIZE                   21
#define KEY_ESTABLISH_AES_MMO_HASH_SIZE                  16

#define KEY_ESTABLISH_CERT_EXT_ADDR_IDX                  22
#define KEY_ESTABLISH_CERT_ISSUER_IDX                    30
#define KEY_ESTABLISH_CERT_IDX                           4
#define KEY_ESTABLISH_CERT_ISSUER_LENTGH                 Z_EXTADDR_LEN

// Max number of entries in the Key Establishment Rec Table
#define MAX_KEY_ESTABLISHMENT_REC_ENTRY                  2

#define INVALID_PARTNER_ADDR                             0xFFFE

// Key Establishment Device Role
#define KEY_ESTABLISHMENT_INITIATOR                      0
#define KEY_ESTABLISHMENT_RESPONDER                      1

#define KEY_ESTABLISHMENT_REC_EXPIRY_TIME                0xFF  // In seconds
#define KEY_ESTABLISHMENT_EPH_DATA_GEN_INVALID_TIME      0xFF  // In seconds
#define KEY_ESTABLISHMENT_CONF_KEY_GEN_INVALID_TIME      0xFF  // In seconds

// The time out for generating the key bits and mac (in sec)
#if !defined ( ZCL_KEY_ESTABLISHMENT_KEY_GENERATE_TIMEOUT )
#define ZCL_KEY_ESTABLISHMENT_KEY_GENERATE_TIMEOUT       4
#endif
#if !defined ( ZCL_KEY_ESTABLISHMENT_MAC_GENERATE_TIMEOUT )
#define ZCL_KEY_ESTABLISHMENT_MAC_GENERATE_TIMEOUT       1
#endif
#if !defined ( ZCL_KEY_ESTABLISHMENT_EKEY_GENERATE_TIMEOUT )
#define ZCL_KEY_ESTABLISHMENT_EKEY_GENERATE_TIMEOUT      1
#endif

// The poll rate for end device is set to this value
// during the key establishment procedure
#if !defined (ZCL_KEY_ESTABLISH_POLL_RATE)
#define ZCL_KEY_ESTABLISH_POLL_RATE                      1000
#endif

/*********************************************************************
 * TYPEDEFS
 */

// The format of a Key Establishment Record
typedef struct
{
  afAddrType_t dstAddr;             // Partner Address
  uint8  lastSeqNum;
  uint8  appTaskID;                 // Task ID of the application that initiates key establishment
  uint8  partnerExtAddr[Z_EXTADDR_LEN];
  uint8  role;                      // 0 @ initiator
                                    // 1 @ responder
  uint8  age;                       // Age in seconds
  uint8  state;                     // State

  // Key information
  uint8  *pLocalEPrivateKey;
  uint8  *pLocalEPublicKey;
  uint8  *pRemotePublicKey;
  uint8  *pRemoteCertificate;

  uint8  *pKey;
  uint8  *pMacKey;

  uint8  remoteEphDataGenTime;      // partner Ephemeral Data Generate Time
  uint8  remoteConfKeyGenTime;      // partner Confirm Key Generate Time

} zclKeyEstablishRec_t;

// Key Establishment Procedure internal State
typedef enum
{
  KeyEstablishState_Idle = 0,
  KeyEstablishState_InitiatePending,   // Waiting for Initiate Key Establishment Rsp
  KeyEstablishState_EDataPending,      // Waiting for the Ephemeral data
  KeyEstablishState_KeyCalculatePending,// Waiting for the key to be calcuated
  KeyEstablishState_ConfirmPending,     // Waiting for Confirm Response
  KeyEstablishState_TerminationPending // Waiting for Terminate command
} KeyEstablishState_t;

// Terminate Key Establishment Status
typedef enum
{
  TermKeyStatus_Success = 0,
  TermKeyStatus_UnknowIssuer,
  TermKeyStatus_BadKeyConfirm,
  TermKeyStatus_BadMessage,
  TermKeyStatus_NoResources,
  TermKeyStatus_UnSupportedSuite
} TermKeyStatus_t;


// Osal message format of indication for key establishment completion
typedef struct
{
  osal_event_hdr_t hdr;
  uint8 waitTime;
  uint16 keyEstablishmentSuite;
} keyEstablishmentInd_t;


/*********************************************************************
 * FUNCTION MACROS
 */


/*********************************************************************
 * FUNCTIONS
 */
/*
 * Key Establishment Task initialization function
 */
extern void zclGeneral_KeyEstablish_Init( uint8 task_id );

/*
 * Event process for Key Establishment Task
 */
extern uint16 zclKeyEstablish_event_loop( uint8 task_id, uint16 events );

/*
 * Call to initiate key establishment procedure with partner device
 */
extern ZStatus_t zclGeneral_KeyEstablish_InitiateKeyEstablishment(uint8 appTaskID,
                                                           afAddrType_t *partnerAddr,
                                                           uint8 seqNum);
/*
 * Send Initiate Key Establishment Command
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_InitiateKeyEstablishment( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint16 keyEstablishmentSuite,
                                             uint8  keyGenerateTime,
                                             uint8  macGenerateTime,
                                             uint8 *certificate,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Ephemeral Data Request
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_EphemeralDataReq( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *eData,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Confirm Key Command
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_ConfirmKey( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *mac,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Terminate Key Establishment Command
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_TerminateKeyEstablishment( uint8 srcEP,
                                             afAddrType_t *dstAddr,
                                             TermKeyStatus_t status,
                                             uint8 waitTime,
                                             uint16 keyEstablishmentSuite, uint8 direction,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Initiate Key Establishment Response
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_InitiateKeyEstablishmentRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint16 keyEstablishmentSuite,
                                             uint8  keyGenerateTime,
                                             uint8  macGenerateTime,
                                             uint8 *certificate,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Ephemeral Data Response
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_EphemeralDataRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *eData,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Confirm Key Response
 */
extern ZStatus_t zclGeneral_KeyEstablish_Send_ConfirmKeyRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                             uint8 *mac,
                                             uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Sign the message using static private key
 */
extern ZStatus_t zclGeneral_KeyEstablishment_ECDSASign( uint8 *input,  uint8 inputLen,
                                             uint8 *output);

/*
 * Verify the signature of the message digest
 */
extern ZStatus_t zclGeneral_KeyEstablishment_ECDSAVerify( uint8 *input,  uint8 inputLen,
                                             uint8 *signature);

/*
 * Register the user defined yielding function
 */
extern void zclGeneral_KeyEstablishment_RegYieldCB( YieldFunc *pFnYield,
                                                        uint8 yieldLevel );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_KEY_ESTABLISH_H */
