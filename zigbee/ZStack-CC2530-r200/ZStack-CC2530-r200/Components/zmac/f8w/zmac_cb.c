/**************************************************************************************************
  Filename:       zmac_cb.c
  Revised:        $Date: 2011-05-24 10:23:19 -0700 (Tue, 24 May 2011) $
  Revision:       $Revision: 26064 $

  Description:    This file contains the NWK functions that the ZMAC calls


  Copyright 2005-2011 Texas Instruments Incorporated. All rights reserved.

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

/********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************/

#include "ZComDef.h"
#include "OSAL.h"
#include "ZMAC.h"
#include "MT_MAC.h"
#include "hal_mcu.h"

#if !defined NONWK
#include "nwk.h"
#include "nwk_bufs.h"
#include "ZGlobals.h"
#endif

#if defined( MACSIM )
  #include "mac_sim.h"
#endif

#include "mac_security.h"

#include "mac_main.h"
extern void *ZMac_ScanBuf;

/********************************************************************************************************
 *                                               CONSTANTS
 ********************************************************************************************************/

#if !defined NONWK
/* Lookup table for size of structures. Must match with the order of MAC callback events */
const uint8 CODE zmacCBSizeTable [] = {
  0,
  sizeof(ZMacAssociateInd_t),       // MAC_MLME_ASSOCIATE_IND      1   Associate indication
  sizeof(ZMacAssociateCnf_t),       // MAC_MLME_ASSOCIATE_CNF      2   Associate confirm
  0,                                // MAC_MLME_DISASSOCIATE_IND   3   Disassociate indication
  0,                                // MAC_MLME_DISASSOCIATE_CNF   4   Disassociate confirm
  sizeof(macMlmeBeaconNotifyInd_t), // MAC_MLME_BEACON_NOTIFY_IND  5   con notify indication
  sizeof(ZMacOrphanInd_t),          // MAC_MLME_ORPHAN_IND         6   Orphan indication
  sizeof(ZMacScanCnf_t),            // MAC_MLME_SCAN_CNF           7   Scan confirm
  sizeof(ZMacStartCnf_t),           // MAC_MLME_START_CNF          8   Start confirm
  0,                                // MAC_MLME_SYNC_LOSS_IND      9   Sync loss indication
  sizeof(ZMacPollCnf_t),            // MAC_MLME_POLL_CNF           10  Poll confirm
  sizeof(ZMacCommStatusInd_t),      // MAC_MLME_COMM_STATUS_IND    11  Comm status indication
  sizeof(ZMacDataCnf_t),            // MAC_MCPS_DATA_CNF           12  Data confirm
  sizeof(macMcpsDataInd_t),         // MAC_MCPS_DATA_IND           13  Data indication
  0,                                // MAC_MCPS_PURGE_CNF          14  Purge confirm
  0,                                // MAC_PWR_ON_CNF              15  Power on confirm
  sizeof(ZMacPollInd_t)             // MAC_MLME_POLL_IND           16  Poll indication
};
#endif /* !defined NONWK */

/********************************************************************************************************
 *                                               LOCALS
 ********************************************************************************************************/

/* LQI Adjustment Mode */
static ZMacLqiAdjust_t lqiAdjMode = LQI_ADJ_OFF;

#if !defined NONWK
/* LQI Adjustment Function */
static void ZMacLqiAdjust( uint8 corr, uint8* lqi );
#endif

/*********************************************************************
 * ZMAC Function Pointers
 */

/*
 * ZMac Application callback function. This function will be called
 * for every MAC message that is received over-the-air or generated
 * locally by MAC for the application.
 *
 * The callback function should return TRUE if it has handled the
 * MAC message and no further action should be taken with it. It
 * should return FALSE if it has not handled the MAC message and
 * normal processing should take place.
 *
 * NOTE: The processing in this function should be kept to the
 *       minimum.
 */
uint8 (*pZMac_AppCallback)( uint8 *msgPtr ) = (void*)NULL;


/*********************************************************************
 * ZMAC Functions
 */

/**************************************************************************************************
 * @fn       MAC_CbackEvent()
 *
 * @brief    convert MAC data confirm and indication to ZMac and send to NWK
 *
 * @param    pData - pointer to macCbackEvent_t
 *
 * @return   none
 *************************************************************************************************/
void MAC_CbackEvent(macCbackEvent_t *pData)
#ifndef MT_MAC_CB_FUNC
{
#if !defined NONWK
  uint8 event = pData->hdr.event;
  uint16 tmp = zmacCBSizeTable[event];
  macCbackEvent_t *msgPtr;

  /* If the Network layer will handle a new MAC callback, a non-zero value must be entered in the
   * corresponding location in the zmacCBSizeTable[] - thus the table acts as "should handle"?
   */
  if (tmp == 0)
  {
    return;
  }

  // MAC_MCPS_DATA_IND is very special - it is the only event where the MAC does not free *pData.
  if ( event == MAC_MCPS_DATA_IND )
  {
    MAC_MlmeGetReq( MAC_SHORT_ADDRESS, &tmp );
    if ((tmp == INVALID_NODE_ADDR) || (tmp == NWK_BROADCAST_SHORTADDR_DEVALL) ||
        (pData->dataInd.msdu.len == 0))
    {
      mac_msg_deallocate( (uint8 **)&pData );
      return;
    }
    msgPtr = pData;
  }
  else
  {
    if (event == MAC_MLME_BEACON_NOTIFY_IND )
    {
      tmp += sizeof(macPanDesc_t) + pData->beaconNotifyInd.sduLength;
    }
    else if (event == MAC_MLME_SCAN_CNF)
    {
      if (pData->scanCnf.scanType == ZMAC_ED_SCAN)
      {
        tmp += ZMAC_ED_SCAN_MAXCHANNELS;
      }
      else
      {
        tmp += sizeof( ZMacPanDesc_t ) * pData->scanCnf.resultListSize;
      }
    }

    if ( !(msgPtr = (macCbackEvent_t *)osal_msg_allocate(tmp)) )
    {
      // Not enough memory. If data confirm - try again
      if ( event == MAC_MCPS_DATA_CNF )
      {
        halIntState_t intState;

        // This is not normally deallocated here because the pZMac_AppCallback()
        // application may need it.
        HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

        mac_msg_deallocate( (uint8**)&(pData->dataCnf.pDataReq) );
        if ( !(msgPtr = (macCbackEvent_t *)osal_msg_allocate(tmp)) )
        {
          // Still no allocation, something is wrong
          HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.
          return;
        }
        HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.
        pData->dataCnf.pDataReq = NULL;
      }
      else
      {
        // This message is dropped
        return;
      }
    }
    osal_memcpy(msgPtr, pData, zmacCBSizeTable[event]);
  }

  if ( event == MAC_MLME_BEACON_NOTIFY_IND )
  {
    macMlmeBeaconNotifyInd_t *pBeacon = (macMlmeBeaconNotifyInd_t*)msgPtr;

    osal_memcpy(pBeacon+1, pBeacon->pPanDesc, sizeof(macPanDesc_t));
    pBeacon->pPanDesc = (macPanDesc_t *)(pBeacon+1);
    osal_memcpy(pBeacon->pPanDesc+1, pBeacon->pSdu, pBeacon->sduLength);
    pBeacon->pSdu = (uint8 *)(pBeacon->pPanDesc+1);
  }
  else if (event == MAC_MLME_SCAN_CNF)
  {
    macMlmeScanCnf_t *pScan = (macMlmeScanCnf_t*)msgPtr;

    if (ZMac_ScanBuf != NULL)
    {
      void *pTmp = ZMac_ScanBuf;
      ZMac_ScanBuf = NULL;

      if (pScan->scanType == ZMAC_ED_SCAN)
      {
        pScan->result.pEnergyDetect = (uint8*) (pScan + 1);
        osal_memcpy(pScan->result.pEnergyDetect, pTmp, ZMAC_ED_SCAN_MAXCHANNELS);
      }
      else
      {
        pScan->result.pPanDescriptor = (macPanDesc_t*) (pScan + 1);
        osal_memcpy(pScan + 1, pTmp, sizeof( ZMacPanDesc_t ) * pScan->resultListSize);
      }

      osal_mem_free(pTmp);
    }
  }

  if ( ( pZMac_AppCallback == NULL ) || ( pZMac_AppCallback( (uint8 *)msgPtr ) == FALSE ) )
  {
    // Filter out non-zigbee packets
    if ( event == MAC_MCPS_DATA_IND )
    {
      uint8 fcFrameType = (pData->dataInd.msdu.p[0] & 0x03);
      uint8 fcProtoVer = ((pData->dataInd.msdu.p[0] >> 2) & 0x0F);
      uint8 fcReserve = (pData->dataInd.msdu.p[1] & 0xE0);
      if ( (fcFrameType > 0x01) || (fcProtoVer != _NIB.nwkProtocolVersion) || (fcReserve != 0)
          || (pData->dataInd.mac.srcAddr.addrMode != SADDR_MODE_SHORT) )
      {
        // Drop the message
        mac_msg_deallocate( (uint8 **)&pData );
        return;
      }
      else
      {
        macDataInd_t *pInd = &msgPtr->dataInd.mac;
        // See if LQI needs adjustment due to frame correlation
        ZMacLqiAdjust( pInd->correlation, &pInd->mpduLinkQuality );

        // Look for broadcast message that has a radius of greater 1
        if ( (pData->dataInd.mac.dstAddr.addr.shortAddr == 0xFFFF)
               && (pData->dataInd.msdu.p[6] > 1) )
        {
          // Send the messsage to a special broadcast queue
          if ( nwk_broadcastSend( (uint8 *)msgPtr ) != SUCCESS )
          {
            // Drop the message, too many broadcast messages to process
            mac_msg_deallocate( (uint8 **)&pData );
          }
          return;
        }
      }
    }
    else if ( event == MAC_MCPS_DATA_CNF )
    {
      macMcpsDataCnf_t *pCnf = &msgPtr->dataCnf;
      
      if (pCnf->pDataReq->internal.txOptions & MAC_TXOPTION_ACK)
      {
        // See if LQI needs adjustment due to frame correlation
        ZMacLqiAdjust( pCnf->correlation, &pCnf->mpduLinkQuality );
      }
    }

    // Application hasn't already processed this message. Send it to NWK task.
    osal_msg_send( NWK_TaskID, (uint8 *)msgPtr );
  }

  if ( event == MAC_MCPS_DATA_CNF )
  {
    // If the application needs 'pDataReq' then we cannot free it here.
    // The application must free it after using it. Note that 'pDataReq'
    // is of macMcpsDataReq_t (and not ZMacDataReq_t) type.

    mac_msg_deallocate( (uint8**)&(pData->dataCnf.pDataReq) );
  }
#endif
}
#else  // ifdef MT_MAC_CB_FUNC
{
  /* Check if MT has subscribed for this callback If so, pass it as an event to MonitorTest */
  switch (pData->hdr.event)
  {
    case MAC_MLME_ASSOCIATE_IND:
      if ( _macCallbackSub & CB_ID_NWK_ASSOCIATE_IND )
        nwk_MTCallbackSubNwkAssociateInd ( (ZMacAssociateInd_t *)pData );
      break;

    case MAC_MLME_ASSOCIATE_CNF:
      if ( _macCallbackSub & CB_ID_NWK_ASSOCIATE_CNF )
        nwk_MTCallbackSubNwkAssociateCnf ( (ZMacAssociateCnf_t *)pData );
      break;

    case MAC_MLME_DISASSOCIATE_IND:
      if ( _macCallbackSub & CB_ID_NWK_DISASSOCIATE_IND )
        nwk_MTCallbackSubNwkDisassociateInd ( (ZMacDisassociateInd_t *)pData );
      break;

    case MAC_MLME_DISASSOCIATE_CNF:
      if ( _macCallbackSub & CB_ID_NWK_DISASSOCIATE_CNF )
        nwk_MTCallbackSubNwkDisassociateCnf ( (ZMacDisassociateCnf_t *)pData );
      break;

    case MAC_MLME_BEACON_NOTIFY_IND:
      if ( _macCallbackSub & CB_ID_NWK_BEACON_NOTIFY_IND )
        nwk_MTCallbackSubNwkBeaconNotifyInd( (ZMacBeaconNotifyInd_t *)pData );
      break;

    case MAC_MLME_ORPHAN_IND:
      if ( _macCallbackSub & CB_ID_NWK_ORPHAN_IND )
        nwk_MTCallbackSubNwkOrphanInd( (ZMacOrphanInd_t *) pData );
      break;

    case MAC_MLME_SCAN_CNF:
      if ( _macCallbackSub & CB_ID_NWK_SCAN_CNF )
      {
        pData->scanCnf.result.pEnergyDetect = ZMac_ScanBuf;
        nwk_MTCallbackSubNwkScanCnf ( (ZMacScanCnf_t *) pData );
      }

      if (ZMac_ScanBuf != NULL)
      {
        void *pTmp = ZMac_ScanBuf;
        ZMac_ScanBuf = NULL;
        osal_mem_free(pTmp);
      }
      break;

    case MAC_MLME_START_CNF:
      if ( _macCallbackSub & CB_ID_NWK_START_CNF )
        nwk_MTCallbackSubNwkStartCnf ( pData->hdr.status );
      break;

    case MAC_MLME_SYNC_LOSS_IND:
      if ( _macCallbackSub & CB_ID_NWK_SYNC_LOSS_IND )
       nwk_MTCallbackSubNwkSyncLossInd( (ZMacSyncLossInd_t *) pData );
      break;

    case MAC_MLME_POLL_CNF:
      if ( _macCallbackSub & CB_ID_NWK_POLL_CNF )
         nwk_MTCallbackSubNwkPollCnf( pData->hdr.status );
      break;

    case MAC_MLME_COMM_STATUS_IND:
      if ( _macCallbackSub & CB_ID_NWK_COMM_STATUS_IND )
        nwk_MTCallbackSubCommStatusInd ( (ZMacCommStatusInd_t *) pData );
      break;

    case MAC_MCPS_DATA_CNF:
      mac_msg_deallocate((uint8**)&pData->dataCnf.pDataReq);

      if ( _macCallbackSub & CB_ID_NWK_DATA_CNF )
        nwk_MTCallbackSubNwkDataCnf( (ZMacDataCnf_t *) pData );
      break;

    case MAC_MCPS_DATA_IND:
      {
        /*
           Data Ind is unconventional: to save an alloc/copy, reuse the MAC
           buffer and re-organize the contents into ZMAC format.
        */
        ZMacDataInd_t *pDataInd = (ZMacDataInd_t *) pData;
        uint8 event, status, len, *msdu;

        /* Store parameters */
        event = pData->hdr.event;
        status = pData->hdr.status;
        len = pData->dataInd.msdu.len;
        msdu = pData->dataInd.msdu.p;

        /* Copy security fields */
        osal_memcpy(&pDataInd->Sec, &pData->dataInd.sec, sizeof(ZMacSec_t));
        
        /* Copy mac fields one by one since the two buffers overlap. */
        osal_memcpy(&pDataInd->SrcAddr, &pData->dataInd.mac.srcAddr, sizeof(zAddrType_t));
        osal_memcpy(&pDataInd->DstAddr, &pData->dataInd.mac.dstAddr, sizeof(zAddrType_t));
        pDataInd->Timestamp = pData->dataInd.mac.timestamp;
        pDataInd->Timestamp2 = pData->dataInd.mac.timestamp2;
        pDataInd->SrcPANId = pData->dataInd.mac.srcPanId;
        pDataInd->DstPANId = pData->dataInd.mac.dstPanId;
        pDataInd->mpduLinkQuality = pData->dataInd.mac.mpduLinkQuality;
        pDataInd->Correlation = pData->dataInd.mac.correlation;
        pDataInd->Rssi = pData->dataInd.mac.rssi;
        pDataInd->Dsn = pData->dataInd.mac.dsn;

        /* Restore parameters */
        pDataInd->hdr.Status = status;
        pDataInd->hdr.Event = event;
        pDataInd->msduLength = len;

        if (len)
          pDataInd->msdu = msdu;
        else
          pDataInd->msdu = NULL;

        if ( _macCallbackSub & CB_ID_NWK_DATA_IND )
          nwk_MTCallbackSubNwkDataInd ( pDataInd );
      }

      /* free buffer */
      mac_msg_deallocate( (uint8 **)&pData );
      break;

    case MAC_MCPS_PURGE_CNF:
      if ( _macCallbackSub & CB_ID_NWK_PURGE_CNF )
        nwk_MTCallbackSubNwkPurgeCnf( (ZMacPurgeCnf_t *) pData);
      break;

    default:
      break;
  }
}
#endif

/********************************************************************************************************
 * @fn      MAC_CbackCheckPending
 *
 * @brief   Return number of pending indirect msg
 *
 * @param   None
 *
 * @return  Number of indirect msg holding
 ********************************************************************************************************/
uint8 MAC_CbackCheckPending(void)
{
#if !defined (NONWK)
  if ( ZSTACK_ROUTER_BUILD )
  {
    return (nwkDB_ReturnIndirectHoldingCnt());
  }
  else
  {
    return (0);
  }
#else
  return (0);
#endif
}

/********************************************************************************************************
 * @fn      ZMacLqiAdjustMode
 *
 * @brief   Sets/return LQI adjust mode
 *
 * @param   mode - LQI_ADJ_GET = return current mode only
 *                 LQI_ADJ_OFF = disable LQI adjusts
 *                 LQI_ADJ_MODEx = set to LQI adjust MODEx
 *
 * @return  current LQI adjust mode
 ********************************************************************************************************/
ZMacLqiAdjust_t ZMacLqiAdjustMode( ZMacLqiAdjust_t mode )
{
  if ( mode != LQI_ADJ_GET )
  {
    lqiAdjMode = mode;
  }
  return ( lqiAdjMode );
}

#if !defined NONWK
/********************************************************************************************************
 * @fn      ZMacLqiAdjust
 *
 * @brief   Adjust LQI according to correlation value
 *
 * @notes - the IEEE 802.15.4 specification provides some general statements on
 *          the subject of LQI. Section 6.7.8: "The minimum and maximum LQI values
 *          (0x00 and 0xFF) should be associated with the lowest and highest IEEE
 *          802.15.4 signals detectable by the receiver, and LQ values should be
 *          uniformly distributed between these two limits." Section E.2.3: "The
 *          LQI (see 6.7.8) measures the received energy and/or SNR for each
 *          received packet. When energy level and SNR information are combined,
 *          they can indicate whether a corrupt packet resulted from low signal
 *          strength or from high signal strength plus interference."
 *        - LQI Adjustment Mode1 provided below is a simple algorithm to use the
 *          packet correlation value (related to SNR) to scale incoming LQI value
 *          (related to signal strength) to 'derate' noisy packets.
 *        - LQI Adjustment Mode2 provided below is a location for a developer to
 *          implement their own proprietary LQI adjustment algorithm.
 *
 * @param   corr - packet correlation value
 * @param   lqi  - ptr to link quality (scaled rssi)
 *
 * @return  *lqi - adjusted link quality
 ********************************************************************************************************/
static void ZMacLqiAdjust( uint8 corr, uint8 *lqi )
{
  if ( lqiAdjMode != LQI_ADJ_OFF )
  {
    uint16 adjLqi = *lqi;

    // Keep correlation within theoretical limits
    if ( corr < LQI_CORR_MIN )
    {
       corr = LQI_CORR_MIN;
    }
    else if ( corr > LQI_CORR_MAX )
    {
       corr = LQI_CORR_MAX;
    }

    if ( lqiAdjMode == LQI_ADJ_MODE1 )
    {
      /* MODE1 - linear scaling of incoming LQI with a "correlation percentage"
                 which is computed from the incoming correlation value between
                 theorectical minimum/maximum values. This is a very simple way
                 of 'derating' the incoming LQI as correlation value drops. */
      adjLqi = (adjLqi * (corr - LQI_CORR_MIN)) / (LQI_CORR_MAX - LQI_CORR_MIN);
    }
    else if ( lqiAdjMode == LQI_ADJ_MODE2 )
    {
      /* MODE2 - location for developer to implement a proprietary algorithm */
    }

    // Replace incoming LQI with scaled value
    *lqi = (adjLqi > 255) ? 255 : (uint8)adjLqi;
  }
}
#endif
