/**************************************************************************************************
  Filename:       zcl_closures.c
  Revised:        $Date: 2011-05-02 10:30:51 -0700 (Mon, 02 May 2011) $
  Revision:       $Revision: 25832 $

  Description:    Zigbee Cluster Library - Closures.


  Copyright 2006-2011 Texas Instruments Incorporated. All rights reserved.

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
#include "OSAL.h"
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_closures.h"

#if defined ( INTER_PAN )
  #include "stub_aps.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

typedef struct zclClosuresCBRec
{
  struct zclClosuresCBRec     *next;
  uint8                       endpoint; // Used to link it into the endpoint descriptor
  zclClosures_AppCallbacks_t  *CBs;     // Pointer to Callback function
} zclClosuresCBRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static zclClosuresCBRec_t *zclClosuresCBs = (zclClosuresCBRec_t *)NULL;
static uint8 zclClosuresPluginRegisted = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static ZStatus_t zclClosures_HdlIncoming( zclIncoming_t *pInMsg );
static ZStatus_t zclClosures_HdlInSpecificCommands( zclIncoming_t *pInMsg );
static zclClosures_AppCallbacks_t *zclClosures_FindCallbacks( uint8 endpoint );
static ZStatus_t zclClosures_ProcessInClosuresCmds( zclIncoming_t *pInMsg );

#ifdef ZCL_DOORLOCK
static ZStatus_t zclClosures_ProcessInDoorLock( zclIncoming_t *pInMsg,
                                                zclClosures_AppCallbacks_t *pCBs );
#endif //ZCL_DOORLOCK

#ifdef ZCL_WINDOWCOVERING
static ZStatus_t zclClosures_ProcessInWindowCovering( zclIncoming_t *pInMsg,
                                                      zclClosures_AppCallbacks_t *pCBs );
#endif //ZCL_WINDOWCOVERING

/*********************************************************************
 * @fn      zclClosures_RegisterCmdCallbacks
 *
 * @brief   Register an applications command callbacks
 *
 * @param   endpoint - application's endpoint
 * @param   callbacks - pointer to the callback record.
 *
 * @return  ZMemError if not able to allocate
 */
ZStatus_t zclClosures_RegisterCmdCallbacks( uint8 endpoint, zclClosures_AppCallbacks_t *callbacks )
{
  zclClosuresCBRec_t *pNewItem;
  zclClosuresCBRec_t *pLoop;

  // Register as a ZCL Plugin
  if ( !zclClosuresPluginRegisted )
  {
    zcl_registerPlugin( ZCL_CLUSTER_ID_CLOSURES_SHADE_CONFIG,
                        ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                        zclClosures_HdlIncoming );
    zclClosuresPluginRegisted = TRUE;
  }

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclClosuresCBRec_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclClosuresCBRec_t *)NULL;
  pNewItem->endpoint = endpoint;
  pNewItem->CBs = callbacks;

  // Find spot in list
  if ( zclClosuresCBs == NULL )
  {
    zclClosuresCBs = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = zclClosuresCBs;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }
  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclClosures_FindCallbacks
 *
 * @brief   Find the callbacks for an endpoint
 *
 * @param   endpoint
 *
 * @return  pointer to the callbacks
 */
static zclClosures_AppCallbacks_t *zclClosures_FindCallbacks( uint8 endpoint )
{
  zclClosuresCBRec_t *pCBs;
  
  pCBs = zclClosuresCBs;
  while ( pCBs )
  {
    if ( pCBs->endpoint == endpoint )
      return ( pCBs->CBs );
    pCBs = pCBs->next;
  }
  return ( (zclClosures_AppCallbacks_t *)NULL );
}

/*********************************************************************
 * @fn      zclClosures_HdlIncoming
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library or Profile commands for attributes
 *          that aren't in the attribute list
 *
 * @param   pInMsg - pointer to the incoming message
 * @param   logicalClusterID
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclClosures_HdlIncoming(  zclIncoming_t *pInMsg )
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
      stat = zclClosures_HdlInSpecificCommands( pInMsg );
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
 * @fn      zclClosures_HdlInSpecificCommands
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclClosures_HdlInSpecificCommands( zclIncoming_t *pInMsg )
{
  ZStatus_t stat;
  zclClosures_AppCallbacks_t *pCBs;
  
  // make sure endpoint exists
  pCBs = zclClosures_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs == NULL )
    return ( ZFailure );
  
  switch ( pInMsg->msg->clusterId )				
  {
    case ZCL_CLOSURES_LOGICAL_CLUSTER_ID_SHADE_CONFIG:
      stat = zclClosures_ProcessInClosuresCmds( pInMsg );
      break;

    case ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK:
#ifdef ZCL_DOORLOCK
      stat = zclClosures_ProcessInDoorLock( pInMsg, pCBs );
#endif //ZCL_DOORLOCK
      break;

    case ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING:
#ifdef ZCL_WINDOWCOVERING
      stat = zclClosures_ProcessInWindowCovering( pInMsg, pCBs );
#endif //ZCL_WINDOWCOVERING
      break;

    default:
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclSS_ProcessInClosuresCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclClosures_ProcessInClosuresCmds( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZFailure;

  // there are no specific command for this cluster yet.
  // instead of suppressing a compiler warnings( for a code porting reasons )
  // fake unused call here and keep the code skeleton intact
  (void)pInMsg;
  if ( stat != ZFailure )
    zclClosures_FindCallbacks( 0 );

  return ( stat );
}

#ifdef ZCL_DOORLOCK
/*********************************************************************
 * @fn      zclClosures_ProcessInDoorLock
 *
 * @brief   Process in the received DoorLock Command.
 *
 * @param   pInMsg - pointer to the incoming message
 * @param   pCBs - pointer to the Application callback functions
 * 
 * @return  ZStatus_t
 */
static ZStatus_t zclClosures_ProcessInDoorLock( zclIncoming_t *pInMsg,
                                               zclClosures_AppCallbacks_t *pCBs )
{
  if ( zcl_ServerCmd( pInMsg->hdr.fc.direction ) )
  {
    switch(pInMsg->hdr.commandID)
    {
    case COMMAND_CLOSURES_LOCK_DOOR:
    case COMMAND_CLOSURES_UNLOCK_DOOR:
      if ( pCBs->pfnDoorLock )
      {
        pCBs->pfnDoorLock( pInMsg->hdr.commandID, &(pInMsg->msg->srcAddr), pInMsg->hdr.transSeqNum );
      }
      break;
      
    default:
      return ( ZCL_STATUS_UNSUP_CLUSTER_COMMAND );   // Error ignore the command
    }
  }
  else
  {
    switch(pInMsg->hdr.commandID)
    {
    case COMMAND_CLOSURES_LOCK_DOOR_RESPONSE:
    case COMMAND_CLOSURES_UNLOCK_DOOR_RESPONSE:
      if ( pCBs->pfnDoorLockRes )
      {
        pCBs->pfnDoorLockRes( pInMsg->hdr.commandID, &(pInMsg->msg->srcAddr), pInMsg->hdr.transSeqNum );
      }
      break;
      
    default:
      return ( ZCL_STATUS_UNSUP_CLUSTER_COMMAND );   // Error ignore the command
    }
  }

  return ( ZCL_STATUS_CMD_HAS_RSP );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockRequest
 *
 * @brief   Call to send out a Door Lock/ Door Unlock Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - either COMMAND_DOOR_LOCK or COMMAND_DOOR_UNLOCK
 * @param   clusterID - cluster whose commands and attributes lock and unlock 
                        the door
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_SendDoorLockRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                       uint8 cmd, uint16 clusterID,
                                       uint8 disableDefaultRsp, uint8 seqNum )
{
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 0, NULL );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockResponse
 *
 * @brief   Call to send out a Door Lock/ Door Unlock Response Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - either COMMAND_DOOR_LOCK or COMMAND_DOOR_UNLOCK
 * @param   clusterID - cluster whose commands and attributes lock and unlock 
                        the door
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * @param   status - status of the Door Lock/ Door Unlock Request Command
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_SendDoorLockResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                       uint8 cmd, uint16 clusterID,
                                       uint8 disableDefaultRsp, uint8 seqNum, 
                                       uint8 status )
{
  uint8 payload[DOORLOCK_RES_PAYLOAD_LEN];
  
  payload[0] = status;
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, 
                          DOORLOCK_RES_PAYLOAD_LEN, payload );
}
#endif //ZCL_DOORLOCK

#ifdef ZCL_WINDOWCOVERING
/*********************************************************************
 * @fn      zclClosures_ProcessInWindowCovering
 *
 * @brief   Process in the received Window Covering cluster Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclClosures_ProcessInWindowCovering( zclIncoming_t *pInMsg,
                                                      zclClosures_AppCallbacks_t *pCBs )
{
  ZStatus_t status = ZCL_STATUS_SUCCESS;
  uint8 *pData = pInMsg->pData;
  
  if ( zcl_ServerCmd( pInMsg->hdr.fc.direction ) )
  {   
    switch ( pInMsg->hdr.commandID )
    {
    case COMMAND_CLOSURES_UP_OPEN:
      if ( pCBs->pfnWindowCoveringUpOpen )
      {
        pCBs->pfnWindowCoveringUpOpen();
      }
      break;
      
    case COMMAND_CLOSURES_DOWN_CLOSE:
      if ( pCBs->pfnWindowCoveringDownClose )
      {
        pCBs->pfnWindowCoveringDownClose();
      }
      break;
      
    case COMMAND_CLOSURES_STOP:
      if ( pCBs->pfnWindowCoveringStop )
      {
        pCBs->pfnWindowCoveringStop();
      }
      break;
      
    case COMMAND_CLOSURES_GO_TO_LIFT_SETPOINT:
      if ( pCBs->pfnWindowCoveringGotoLiftSetpoint )
      {
        status = pCBs->pfnWindowCoveringGotoLiftSetpoint( pData[0] );
      }
      break;
      
    case COMMAND_CLOSURES_GO_TO_LIFT_VALUE:
      if ( pCBs->pfnWindowCoveringGotoLiftValue )
      {
        if ( pCBs->pfnWindowCoveringGotoLiftValue( BUILD_UINT16( pData[0], pData[1] ) ) == FALSE )
        {
          status = ZCL_STATUS_INVALID_VALUE;
        }
      }
      break;
      
    case COMMAND_CLOSURES_GO_TO_LIFT_PERCENTAGE:
      if ( pCBs->pfnWindowCoveringGotoLiftPercentage )
      {
        if ( pCBs->pfnWindowCoveringGotoLiftPercentage( pData[0] ) == FALSE )
        {
          status = ZCL_STATUS_INVALID_VALUE;
        }
      }
      break;
      
    case COMMAND_CLOSURES_GO_TO_TILT_SETPOINT:
      if ( pCBs->pfnWindowCoveringGotoTiltSetpoint )
      {
        status = pCBs->pfnWindowCoveringGotoTiltSetpoint( pData[0] );
      }
      break;
      
    case COMMAND_CLOSURES_GO_TO_TILT_VALUE:
      if ( pCBs->pfnWindowCoveringGotoTiltValue )
      {
        if ( pCBs->pfnWindowCoveringGotoTiltValue( BUILD_UINT16( pData[0], pData[1] ) ) == FALSE )
        {
          status = ZCL_STATUS_INVALID_VALUE;
        }
      }
      break;
      
    case COMMAND_CLOSURES_GO_TO_TILT_PERCENTAGE:
      if ( pCBs->pfnWindowCoveringGotoTiltPercentage )
      {
        if ( pCBs->pfnWindowCoveringGotoTiltPercentage( pData[0] ) == FALSE )
        {
          status = ZCL_STATUS_INVALID_VALUE;
        }
      }
      break;
      
    case COMMAND_CLOSURES_PROGRAM_SETPOINT:
      if ( pCBs->pfnWindowCoveringProgramSetpoint )
      {
        programSetpointPayload_t setpoint;
        if( pInMsg->pDataLen == ZCL_WC_PROGRAMSETPOINTREQ_VER1_PAYLOADLEN )
        {
          setpoint.version = programSetpointVersion1;
          setpoint.setpointType = (setpointType_t)pData[0]; 
          setpoint.setpointIndex = pData[1];
          setpoint.setpointValue = BUILD_UINT16( pData[2], pData[3] );
        }
        else if( pInMsg->pDataLen == ZCL_WC_PROGRAMSETPOINTREQ_VER2_PAYLOADLEN )
        {
          setpoint.version = programSetpointVersion2;
          setpoint.setpointIndex = pData[0];
        }
        else
        {
          return ZCL_STATUS_INVALID_VALUE;
        }
        if ( pCBs->pfnWindowCoveringProgramSetpoint( &setpoint ) == FALSE )
        {
          status = ZCL_STATUS_INSUFFICIENT_SPACE;
        }
      }
      break;
      
    default:
      return( ZCL_STATUS_UNSUP_CLUSTER_COMMAND );
    }
  }
  // no Client command

  return ( status );
}

/*********************************************************************
 * @fn      zclClosures_WCSimpleRequest
 *
 * @brief   Call to send out a Window Covering command with no payload 
 *          as Up/Open, Down/Close or Stop
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_WCSimpleRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                       uint8 cmd, uint8 disableDefaultRsp, uint8 seqNum )
{
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 0, NULL );
}

/*********************************************************************
 * @fn      zclClosures_SendGoToSetpointRequest
 *
 * @brief   Call to send out a Go to Setpoint Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID for COMMAND_CLOSURES_GO_TO_LIFT_SETPOINT 
 *                or COMMAND_CLOSURES_GO_TO_TILT_SETPOINT
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * @param   SetpointIndex - payload
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_WCSendGoToSetpointRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                               uint8 cmd, uint8 disableDefaultRsp, 
                                               uint8 seqNum, uint8 SetpointIndex )
{
  uint8 buf[ZCL_WC_GOTOSETPOINTREQ_PAYLOADLEN];

  buf[0] = SetpointIndex;
  
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 
                          ZCL_WC_GOTOSETPOINTREQ_PAYLOADLEN, buf );
}


/*********************************************************************
 * @fn      zclClosures_WCSendGoToValueRequest
 *
 * @brief   Call to send out a Go to Value Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID for COMMAND_CLOSURES_GO_TO_LIFT_VALUE
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * @param   liftValue - payload
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_WCSendGoToValueRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                              uint8 cmd, uint8 disableDefaultRsp, 
                                              uint8 seqNum, uint16 Value )
{
  uint8 buf[ZCL_WC_GOTOVALUEREQ_PAYLOADLEN];

  buf[0] = LO_UINT16( Value );
  buf[1] = HI_UINT16( Value );
  
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 
                          ZCL_WC_GOTOVALUEREQ_PAYLOADLEN, buf );
}

/*********************************************************************
 * @fn      zclClosures_WCSendGoToPercentageRequest
 *
 * @brief   Call to send out a Go to Percentage Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID e.g. COMMAND_CLOSURES_GO_TO_LIFT_PERCENTAGE
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * @param   percentageLiftValue - payload
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_WCSendGoToPercentageRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                     uint8 cmd, uint8 disableDefaultRsp, 
                                                     uint8 seqNum, uint8 percentageValue )
{
  uint8 buf[ZCL_WC_GOTOPERCENTAGEREQ_PAYLOADLEN];

  buf[0] = percentageValue;
  
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 
                          ZCL_WC_GOTOPERCENTAGEREQ_PAYLOADLEN, buf );
}

/*********************************************************************
 * @fn      zclClosures_WCSendProgramSetpointRequest
 *
 * @brief   Call to send out a Program Setpoint Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID for COMMAND_CLOSURES_PROGRAM_SETPOINT
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 * @param   programSetpoint - contains payload information
 * 
 * @return  ZStatus_t
 */
ZStatus_t zclClosures_WCSendProgramSetpointRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                    uint8 cmd, uint8 disableDefaultRsp, 
                                                    uint8 seqNum, programSetpointPayload_t *programSetpoint )
{
  uint8 buf[ZCL_WC_PROGRAMSETPOINTREQ_VER1_PAYLOADLEN];
  uint8 len;
  
  if( programSetpoint->version == programSetpointVersion1 )
  {
    buf[0] = programSetpoint->setpointType;
    buf[1] = programSetpoint->setpointIndex;
    buf[2] = LO_UINT16( programSetpoint->setpointValue );
    buf[3] = HI_UINT16( programSetpoint->setpointValue );
    
    len = ZCL_WC_PROGRAMSETPOINTREQ_VER1_PAYLOADLEN;
  }
  else if( programSetpoint->version == programSetpointVersion2 )
  {
    buf[0] = programSetpoint->setpointIndex;
    
    len = ZCL_WC_PROGRAMSETPOINTREQ_VER2_PAYLOADLEN;
  }
  else
  {
    return( ZFailure );
  }
  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, len, buf );
}
#endif //ZCL_WINDOWCOVERING

/********************************************************************************************
*********************************************************************************************/
