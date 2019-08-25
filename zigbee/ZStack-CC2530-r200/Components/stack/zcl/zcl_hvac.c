/**************************************************************************************************
  Filename:       zcl_hvac.c
  Revised:        $Date: 2011-05-19 11:53:12 -0700 (Thu, 19 May 2011) $
  Revision:       $Revision: 26031 $

  Description:    Zigbee Cluster Library - HVAC


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
#include "zcl_hvac.h"

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
typedef struct zclHVACCBRec
{
  struct zclHVACCBRec     *next;
  uint8                   endpoint; // Used to link it into the endpoint descriptor
  zclHVAC_AppCallbacks_t  *CBs;     // Pointer to Callback function
} zclHVACCBRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static zclHVACCBRec_t *zclHVACCBs = (zclHVACCBRec_t *)NULL;
static uint8 zclHVACPluginRegisted = FALSE;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static ZStatus_t zclHVAC_HdlIncoming( zclIncoming_t *pInMsg );
static ZStatus_t zclHVAC_HdlInSpecificCommands( zclIncoming_t *pInMsg );
static zclHVAC_AppCallbacks_t *zclHVAC_FindCallbacks( uint8 endpoint );

static ZStatus_t zclHVAC_ProcessInPumpCmds( zclIncoming_t *pInMsg );
static ZStatus_t zclHVAC_ProcessInThermostatCmds( zclIncoming_t *pInMsg, zclHVAC_AppCallbacks_t *pCBs );

/*********************************************************************
 * @fn      zclHVAC_RegisterCmdCallbacks
 *
 * @brief   Register an applications command callbacks
 *
 * @param   endpoint - application's endpoint
 * @param   callbacks - pointer to the callback record.
 *
 * @return  ZMemError if not able to allocate
 */
ZStatus_t zclHVAC_RegisterCmdCallbacks( uint8 endpoint, zclHVAC_AppCallbacks_t *callbacks )
{
  zclHVACCBRec_t *pNewItem;
  zclHVACCBRec_t *pLoop;

  // Register as a ZCL Plugin
  if ( !zclHVACPluginRegisted )
  {
    zcl_registerPlugin( ZCL_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL,
                        ZCL_CLUSTER_ID_HAVC_USER_INTERFACE_CONFIG,
                        zclHVAC_HdlIncoming );
    zclHVACPluginRegisted = TRUE;
  }

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclHVACCBRec_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclHVACCBRec_t *)NULL;
  pNewItem->endpoint = endpoint;
  pNewItem->CBs = callbacks;

  // Find spot in list
  if ( zclHVACCBs == NULL )
  {
    zclHVACCBs = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = zclHVACCBs;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }
  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclHVAC_SendSetpointRaiseLower
 *
 * @brief   Call to send out a Setpoint Raise/Lower Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   mode - which setpoint is to be configured
 * @param   amount - amount setpoint(s) are to be increased (or decreased) by,
 *                   in steps of 0.1°C
 * @param   seqNum - transaction sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclHVAC_SendSetpointRaiseLower( uint8 srcEP, afAddrType_t *dstAddr,
                                          uint8 mode, int8 amount, 
                                          uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];

  buf[0] = mode;
  buf[1] = amount;

  return zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_HAVC_THERMOSTAT,
                          COMMAND_THERMOSTAT_SETPOINT_RAISE_LOWER, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclHVAC_FindCallbacks
 *
 * @brief   Find the callbacks for an endpoint
 *
 * @param   endpoint
 *
 * @return  pointer to the callbacks
 */
static zclHVAC_AppCallbacks_t *zclHVAC_FindCallbacks( uint8 endpoint )
{
  zclHVACCBRec_t *pCBs;
  pCBs = zclHVACCBs;
  while ( pCBs )
  {
    if ( pCBs->endpoint == endpoint )
      return ( pCBs->CBs );
    pCBs = pCBs->next;
  }
  return ( (zclHVAC_AppCallbacks_t *)NULL );
}

/*********************************************************************
 * @fn      zclHVAC_HdlIncoming
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library or Profile commands for attributes
 *          that aren't in the attribute list
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_HdlIncoming( zclIncoming_t *pInMsg )
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
      stat = zclHVAC_HdlInSpecificCommands( pInMsg );
    }
    else
    {
      // We don't support any manufacturer specific command -- ignore it.
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
 * @fn      zclHVAC_HdlInSpecificCommands
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_HdlInSpecificCommands( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZSuccess;
  zclHVAC_AppCallbacks_t *pCBs;
  
  // make sure endpoint exists
  pCBs = (void*)zclHVAC_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs == NULL )
    return ( ZFailure );
  
  switch ( pInMsg->msg->clusterId )				
  {
    case ZCL_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL:
      stat = zclHVAC_ProcessInPumpCmds( pInMsg );
      break;

    case ZCL_CLUSTER_ID_HAVC_THERMOSTAT:
      stat = zclHVAC_ProcessInThermostatCmds( pInMsg, pCBs );
      break;
 
    default:
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclHVAC_ProcessInPumpCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_ProcessInPumpCmds( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZFailure;

  // there are no specific command for this cluster yet.
  // instead of suppressing a compiler warnings( for a code porting reasons )
  // fake unused call here and keep the code skeleton intact
 (void)pInMsg;
  if ( stat != ZFailure )
    zclHVAC_FindCallbacks( 0 );

  return ( stat );
}

/*********************************************************************
 * @fn      zclHVAC_ProcessInThermostatCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_ProcessInThermostatCmds( zclIncoming_t *pInMsg,
                                                  zclHVAC_AppCallbacks_t *pCBs )
{
  if  ( pInMsg->hdr.commandID != COMMAND_THERMOSTAT_SETPOINT_RAISE_LOWER )
    return (ZFailure);   // Error ignore the command

  if ( pCBs->pfnHVAC_SetpointRaiseLower )
  {
    zclCmdThermostatSetpointRaiseLowerPayload_t cmd;
    
    cmd.mode = pInMsg->pData[0];
    cmd.amount = pInMsg->pData[1];
    
    pCBs->pfnHVAC_SetpointRaiseLower( &cmd );
  }
  
  return ( ZSuccess );
}

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/****************************************************************************
****************************************************************************/

