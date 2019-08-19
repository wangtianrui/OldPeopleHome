/**************************************************************************************************
  Filename:       BindingTable.c
  Revised:        $Date: 2011-03-13 18:57:34 -0700 (Sun, 13 Mar 2011) $
  Revision:       $Revision: 25408 $

  Description:    Device binding table functions.


  Copyright 2004-2011 Texas Instruments Incorporated. All rights reserved.

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
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "nwk_globals.h"
#include "AddrMgr.h"
#include "BindingTable.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define NV_BIND_EMPTY   0xFF
#define NV_BIND_REC_SIZE (gBIND_REC_SIZE)
#define NV_BIND_ITEM_SIZE  (gBIND_REC_SIZE * gNWK_MAX_BINDING_ENTRIES)

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint8        srcEP;
  uint16       srcIndex;
  uint16       dstIndex;
  uint8        dstEP;
  uint8        dstAddrMode;
  uint8        clusterIDs;
  uint16*      clusterIDList;
} bindFields_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void BindAddrMgrCB( uint8 update, AddrMgrEntry_t *entryOld,
                    AddrMgrEntry_t *entryNew );
BindingEntry_t *bindFindEmpty( void );
uint16 bindingAddrMgsHelperFind( zAddrType_t *addr );
uint8 bindingAddrMgsHelperConvert( uint16 idx, zAddrType_t *addr );
void bindAddrMgrLocalLoad( void );
uint16 bindAddrIndexGet( zAddrType_t* addr );

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 bindAddrMgrLocalLoaded = FALSE;

/*********************************************************************
 * Function Pointers
 */

BindingEntry_t *(*pbindAddEntry)( byte srcEpInt,
                                  zAddrType_t *dstAddr, byte dstEpInt,
                                  byte numClusterIds, uint16 *clusterIds ) = (void*)NULL;
uint16 (*pbindNumOfEntries)( void ) = (void*)NULL;
void (*pbindRemoveDev)( zAddrType_t *Addr ) = (void*)NULL;
byte (*pBindInitNV)( void ) = (void*)NULL;
void (*pBindSetDefaultNV)( void ) = (void*)NULL;
uint16 (*pBindRestoreFromNV)( void ) = (void*)NULL;
void (*pBindWriteNV)( void ) = (void*)NULL;

#if ( ADDRMGR_CALLBACK_ENABLED == 1 )
/*********************************************************************
 * @fn      BindAddrMgrCB()
 *
 * @brief   Address Manager Callback function
 *
 * @param   update -
 * @param   entry -
 *
 * @return  pointer to
 */
void BindAddrMgrCB( uint8 update, AddrMgrEntry_t *entryNew,
                    AddrMgrEntry_t *entryOld )
{
  // Check for either deleted items or changed Extended (Duplicate) addresses
}
#endif // ( ADDRMGR_CALLBACK_ENABLED == 1 )

/*********************************************************************
 * @fn      InitBindingTable()
 *
 * @brief
 *
 *   This function is used to initialise the binding table
 *
 * @param   none
 *
 * @return  none
 */
void InitBindingTable( void )
{
  osal_memset( BindingTable, 0xFF, gBIND_REC_SIZE * gNWK_MAX_BINDING_ENTRIES );

  pbindAddEntry = bindAddEntry;
  pbindNumOfEntries = bindNumOfEntries;
  pbindRemoveDev = bindRemoveDev;
  pBindInitNV = BindInitNV;
  pBindSetDefaultNV = BindSetDefaultNV;
  pBindRestoreFromNV = BindRestoreFromNV;
  pBindWriteNV = BindWriteNV;

  bindAddrMgrLocalLoaded = FALSE;

#if ( ADDRMGR_CALLBACK_ENABLED == 1 )
  // Register with the address manager
  AddrMgrRegister( ADDRMGR_REG_BINDING, BindAddrMgrCB );
#endif
}

/*********************************************************************
 * @fn      bindFindEmpty()
 *
 * @brief   This function returns a pointer to an empty binding slot
 *
 * @param   none
 *
 * @return  pointer to binding table entry, NULL if not added
 */
BindingEntry_t *bindFindEmpty( void )
{
  uint16 x;

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    // It's empty if the index is "Not Found"
    if ( BindingTable[x].srcEP == NV_BIND_EMPTY )
    {
      return ( &BindingTable[x] );
    }
  }

  return ( (BindingEntry_t *)NULL );
}

/*********************************************************************
 * @fn      bindNumOfEntries()
 *
 * @brief   This function returns the number of binding table entries.
 *          The return for this is the number of clusters in the
 *          table NOT the number of entries.
 *
 * @param   none
 *
 * @return  number of entries
 */
uint16 bindNumOfEntries( void )
{
  uint16 x;
  uint16 found;

  for ( found = 0, x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    // It's empty if the index is "Not Found"
    if ( BindingTable[x].srcEP != NV_BIND_EMPTY )
    {
      found += BindingTable[x].numClusterIds;
    }
  }

  return ( found );
}

/*********************************************************************
 * @fn      bindCapacity()
 *
 * @brief   This function returns the number of binding entries
 *          possible and used.
 *
 * @param   maxEntries - pointer to a place to put the max entries
 * @param   usedEntries - pointer to a place to put the number
 *               of used entries
 *
 * @return  none
 */
void bindCapacity( uint16 *maxEntries, uint16 *usedEntries  )
{
  uint16 x;
  uint16 used;

  for ( used = 0, x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    // It's empty if the index is "Not Found"
    if ( BindingTable[x].srcEP != NV_BIND_EMPTY )
    {
      used++;
    }
  }

  *maxEntries = gNWK_MAX_BINDING_ENTRIES;
  *usedEntries = used;
}

/*********************************************************************
 * @fn      bindAddEntry()
 *
 * @brief   This function is used to Add an entry to the binding table
 *
 * @param       srcAddr - source Address
 * @param       srcEpInt - source endpoint
 * @param       dstAddr - destination Address
 * @param       dstEpInt - destination endpoint
 * @param       numClusterIds - number of cluster Ids in the list
 * @param       clusterIds - pointer to the Object ID list
 *
 * @return  pointer to binding table entry, NULL if not added
 */
BindingEntry_t *bindAddEntry( byte srcEpInt,
                              zAddrType_t *dstAddr, byte dstEpInt,
                              byte numClusterIds, uint16 *clusterIds )
{
  uint8           index;
  BindingEntry_t* entry;
  bindFields_t    fields;

  // initialize results
  entry = NULL;

  // make sure local addresses have been loaded
  bindAddrMgrLocalLoad();

  // setup fields
  fields.dstIndex = bindAddrIndexGet( dstAddr );
  fields.srcEP    = srcEpInt;

  if ( dstAddr->addrMode == AddrGroup )
  {
    fields.dstAddrMode = DSTGROUPMODE_GROUP;
    fields.dstEP       = 0;
  }
  else
  {
    fields.dstAddrMode = DSTGROUPMODE_ADDR;
    fields.dstEP       = dstEpInt;
  }

  if ( fields.dstIndex != INVALID_NODE_ADDR  )
  {
    for ( index = 0; index < gNWK_MAX_BINDING_ENTRIES; index++ )
    {
      if ( ( fields.srcEP       == BindingTable[index].srcEP        ) &&
           ( fields.dstAddrMode == BindingTable[index].dstGroupMode ) &&
           ( fields.dstIndex    == BindingTable[index].dstIdx       ) &&
           ( fields.dstEP       == BindingTable[index].dstEP        )    )
      {
        entry = &BindingTable[index];

        // break from loop
        break;
      }
    }

    if ( entry != NULL )
    {
      // Loop through the cluster IDs
      for ( index = 0; index < numClusterIds; index++ )
      {
        // Found - is the cluster already defined?
        if ( bindIsClusterIDinList( entry, clusterIds[index] ) == FALSE )
        {
          // Nope, add this cluster
          if ( bindAddClusterIdToList( entry, clusterIds[index] ) == FALSE )
          {
            // Indicate error if cluster list was full
            entry = NULL;
          }
        }
      }
    }
    else
    {
      // Find an empty slot
      entry = bindFindEmpty();

      // Check against the maximum number allowed
      if ( entry != NULL )
      {
        // Add new entry
        entry->srcEP         = fields.srcEP;
        entry->dstGroupMode  = fields.dstAddrMode;
        entry->dstIdx        = fields.dstIndex;
        entry->dstEP         = fields.dstEP;

        if ( numClusterIds > gMAX_BINDING_CLUSTER_IDS )
        {
          numClusterIds = gMAX_BINDING_CLUSTER_IDS;
        }

        entry->numClusterIds = numClusterIds;

        osal_memcpy( entry->clusterIdList,
                     clusterIds,
                     numClusterIds * sizeof(uint16) );
      }
    }
  }

  return entry;
}

/*********************************************************************
 * @fn      bindRemoveEntry
 *
 * @brief   Removes a binding table entry.
 *
 * @param   pBind - pointer to binding table entry to delete
 *
 * @return  TRUE if Removed, FALSE if not
 */
byte bindRemoveEntry( BindingEntry_t *pBind )
{
  osal_memset( pBind, 0xFF, gBIND_REC_SIZE );
  return ( TRUE );
}

/*********************************************************************
 * @fn      bindIsClusterIDinList()
 *
 * @brief   Is the clusterID in the clusterID list?
 *
 * @param   enter - binding table entry
 * @param   clusterId  - Cluster ID to look for
 *
 * @return  TRUE if found, FALSE if not found
 */
byte bindIsClusterIDinList( BindingEntry_t *entry, uint16 clusterId )
{
  uint8 x;

  if ( entry != NULL )
  {
    for ( x = 0; x < entry->numClusterIds; x++ )
    {
      if ( entry->clusterIdList[x] == clusterId )
      {
        return ( TRUE );
      }
    }
  }

  return ( FALSE );
}

/*********************************************************************
 * @fn      bindRemoveClusterIdFromList()
 *
 * @brief   Removes a ClusterID from a list of ClusterIDs.
 *
 * @param   enter - binding table entry
 * @param   clusterId  - Cluster ID to look for
 *
 * @return  TRUE if there are at least 1 clusterID left, FALSE if none
 */
byte bindRemoveClusterIdFromList( BindingEntry_t *entry, uint16 clusterId )
{
  byte x;
  uint16 *listPtr;
  byte numIds;

  if ( entry )
  {
    if ( entry->numClusterIds > 0 )
    {
      listPtr = entry->clusterIdList;
      numIds = entry->numClusterIds;

      // Copy the new list over
      for ( x = 0; x < numIds; x++ )
      {
        if ( entry->clusterIdList[x] != clusterId )
        {
          *listPtr++ = entry->clusterIdList[x];
        }
        else
        {
          entry->numClusterIds--;
          if ( entry->numClusterIds == 0 )
          {
            break;
          }
        }
      }
    }
  }

  if ( entry && (entry->numClusterIds > 0) )
  {
    return ( TRUE );
  }
  else
  {
    return ( FALSE );
  }
}

/*********************************************************************
 * @fn      bindAddClusterIdToList()
 *
 * @brief   Adds a ClusterID to a list of ClusterIDs.
 *
 * @param   enter - binding table entry
 * @param   clusterId  - Cluster ID to Add
 *
 * @return  TRUE if Added, FALSE if not
 */
byte bindAddClusterIdToList( BindingEntry_t *entry, uint16 clusterId )
{
  if ( entry && entry->numClusterIds < gMAX_BINDING_CLUSTER_IDS )
  {
    // Add the new one
    entry->clusterIdList[entry->numClusterIds] = clusterId;
    entry->numClusterIds++;
    return ( TRUE );
  }
  return ( FALSE );
}

/*********************************************************************
 * @fn      bindFindExisting
 *
 * @brief   Finds an existing src/epint to dst/epint bind record
 *
 * @param   srcAddr - Source address
 * @param   srcEpInt - Source Endpoint/Interface
 * @param   dstAddr - Destination address
 * @param   dstEpInt - Destination Endpoint/Interface
 *
 * @return  pointer to existing entry or NULL
 */
BindingEntry_t *bindFindExisting( byte srcEpInt,
                                  zAddrType_t *dstAddr, byte dstEpInt )
{
  uint16 dstIdx;
  uint16 x;

  // Find the records in the assoc list
  if ( dstAddr->addrMode == AddrGroup )
  {
    dstIdx = dstAddr->addr.shortAddr;
  }
  else
  {
    dstIdx = bindingAddrMgsHelperFind( dstAddr );
  }

  if ( dstIdx == INVALID_NODE_ADDR )
  {
    return ( (BindingEntry_t *)NULL );
  }

  // Start at the beginning
  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    if ( (BindingTable[x].srcEP == srcEpInt) )
    {
      if ( ((dstAddr->addrMode == AddrGroup)
              && (BindingTable[x].dstGroupMode == DSTGROUPMODE_GROUP)
              && (dstIdx == BindingTable[x].dstIdx))
         || ((dstAddr->addrMode != AddrGroup)
             && (BindingTable[x].dstGroupMode == DSTGROUPMODE_ADDR)
             && (dstIdx == BindingTable[x].dstIdx) && (BindingTable[x].dstEP == dstEpInt)) )
      {
        return ( &BindingTable[x] );
      }
    }
  }

  return ( (BindingEntry_t *)NULL );
}

/*********************************************************************
 * @fn       bindRemoveDev()
 *
 * @brief
 *
 *   Remove binds(s) associated to device address (destination).
 *   Updates binding table.
 *
 * @param   Addr - address of device
 *
 * @return  none
 */
void bindRemoveDev( zAddrType_t *Addr )
{
  uint16 idx;
  uint16 x;

  if ( Addr->addrMode == AddrGroup )
  {
    idx = Addr->addr.shortAddr;
  }
  else
  {
    idx = bindingAddrMgsHelperFind( Addr );
  }

  if ( idx == INVALID_NODE_ADDR )
  {
    return;
  }

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    if ( ((Addr->addrMode != AddrGroup) )
        || ((Addr->addrMode == AddrGroup) && (BindingTable[x].dstGroupMode == DSTGROUPMODE_GROUP)
            && (BindingTable[x].dstIdx == idx))
        || ((Addr->addrMode != AddrGroup) && (BindingTable[x].dstGroupMode == DSTGROUPMODE_ADDR)
            && (BindingTable[x].dstIdx == idx)) )
    {
      bindRemoveEntry( &BindingTable[x] );
    }
  }
}

/*********************************************************************
 * @fn       bindRemoveSrcDev()
 *
 * @brief
 *
 *   Remove binds(s) associated to device address (source).
 *   Updates binding table.
 *
 * @param   srcAddr - address of device
 * @param   ep - endpoint to remove, 0xFF is all endpoints
 *
 * @return  none
 */
void bindRemoveSrcDev( uint8 ep )
{
  uint16 x;

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    if ( (ep == 0xFF) || (ep == BindingTable[x].srcEP) )
    {
      bindRemoveEntry( &BindingTable[x] );
    }
  }
}

/*********************************************************************
 * @fn          bindNumBoundTo
 *
 * @brief       Calculate the number items this device is bound to.
 *              When srcMode is set to TRUE, discard what value devAddr
 *              has, it returns number count bound to the local dev.
 *
 * @param       devAddr - device Address
 * @param       devEP - endpoint
 * @param       srcMode - TRUE - assume devHandle is a source address
 *                        FALSE - destination address
 *
 * @return      status
 */
byte bindNumBoundTo( zAddrType_t *devAddr, byte devEpInt, byte srcMode )
{
  BindingEntry_t *pBind;
  uint16 idx;
  byte   num;
  uint16 x;

  // Init
  num = 0;

  if ( devAddr->addrMode == AddrGroup )
  {
    idx = devAddr->addr.shortAddr;
  }
  else
  {
    idx = bindingAddrMgsHelperFind( devAddr );
  }

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    pBind = &BindingTable[x];
    if ( srcMode )
    {
      if ( pBind->srcEP == devEpInt )
      {
        num++;
      }
    }
    else
    {
      if ( ((devAddr->addrMode == AddrGroup)
              && (pBind->dstGroupMode == DSTGROUPMODE_GROUP) && (pBind->dstIdx == idx))
          || ((devAddr->addrMode != AddrGroup) && (pBind->dstGroupMode == DSTGROUPMODE_ADDR)
                                && (pBind->dstIdx == idx) && (pBind->dstEP == devEpInt)) )
      {
        num++;
      }
    }
  }

  return num;
}

/*********************************************************************
 * @fn          bindNumReflections
 *
 * @brief       Counts the number of reflections needed for a
 *              endpoint and cluster ID combo.
 *
 * @param       ep - source endpoint
 * @param       clusterID - matching clusterID
 *
 * @return      number of reflections needed.
 */
uint16 bindNumReflections( uint8 ep, uint16 clusterID )
{
  uint16 x;
  BindingEntry_t *pBind;
  uint16 cnt = 0;
  uint8 bindEP;

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    pBind = &BindingTable[x];
    bindEP = pBind->srcEP;

    if ( (bindEP == ep) && (bindIsClusterIDinList( pBind, clusterID )) )
    {
      cnt++;
    }
  }

  return ( cnt );
}

/*********************************************************************
 * @fn          bindFind
 *
 * @brief       Finds the binding entry for the source address, endpoint
 *              and cluster ID passed in as a parameter.
 *
 * @param       ep - source endpoint
 * @param       clusterID - matching clusterID
 * @param       skip - number of matches to skip before returning
 *
 * @return      pointer to the binding table entry, NULL if not found
 */
BindingEntry_t *bindFind( uint8 ep, uint16 clusterID, uint8 skipping )
{
  BindingEntry_t *pBind;
  byte skipped = 0;
  uint16 x;

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    pBind = &BindingTable[x];

    if ( ( pBind->srcEP == ep) && bindIsClusterIDinList( pBind, clusterID ))
    {
      if ( skipped < skipping )
      {
        skipped++;
      }
      else
      {
        return ( pBind );
      }
    }
  }

  return ( (BindingEntry_t *)NULL );
}

/*********************************************************************
 * @fn          BindInitNV
 *
 * @brief       Initialize the Binding NV Item
 *
 * @param       none
 *
 * @return      ZSUCCESS if successful, NV_ITEM_UNINIT if item did not
 *              exist in NV, NV_OPER_FAILED if failure.
 */
byte BindInitNV( void )
{
  byte ret;

  // Initialize the device list
  ret = osal_nv_item_init( ZCD_NV_BINDING_TABLE,
                  (uint16)(gBIND_REC_SIZE + NV_BIND_ITEM_SIZE), NULL );

  if ( ret != ZSUCCESS )
  {
    BindSetDefaultNV();
  }

  return ( ret );
}

/*********************************************************************
 * @fn          BindSetDefaultNV
 *
 * @brief       Write the defaults to NV
 *
 * @param       none
 *
 * @return      none
 */
void BindSetDefaultNV( void )
{
  nvBindingHdr_t hdr;

  // Initialize the header
  hdr.numRecs = 0;

  // Save off the header
  osal_nv_write( ZCD_NV_BINDING_TABLE, 0, sizeof( nvBindingHdr_t ), &hdr );
}

/*********************************************************************
 * @fn          BindRestoreFromNV
 *
 * @brief       Restore the binding table from NV
 *
 * @param       none
 *
 * @return      Number of entries restored
 */
uint16 BindRestoreFromNV( void )
{
  uint16 x;
  nvBindingHdr_t hdr;
  BindingEntry_t bind;
  zAddrType_t dstAddr;
  uint16 numAdded = 0;

  if ( osal_nv_read( ZCD_NV_BINDING_TABLE, 0, sizeof(nvBindingHdr_t), &hdr ) == ZSuccess )
  {
    // Read in the device list
    for ( x = 0; x < hdr.numRecs; x++ )
    {
      if ( osal_nv_read( ZCD_NV_BINDING_TABLE,
                (uint16)(sizeof(nvBindingHdr_t) + (x * NV_BIND_REC_SIZE)),
                                  NV_BIND_REC_SIZE, &bind ) == ZSUCCESS )
        {
          dstAddr.addrMode = AddrNotPresent;

          // Find the addresses
          if ( bind.dstGroupMode )
          {
            // Setup the group address as the destination address.
            dstAddr.addrMode = AddrGroup;
            dstAddr.addr.shortAddr = bind.dstIdx;
          }
          else
          {
            // Find address index in address manager
            bindingAddrMgsHelperConvert( bind.dstIdx, &dstAddr );
          }

          if ( dstAddr.addrMode != AddrNotPresent )
          {
            bindAddEntry( bind.srcEP, &dstAddr,
                      bind.dstEP, bind.numClusterIds, bind.clusterIdList );
            numAdded++;
          }
      }
    }
  }
  return ( numAdded );
}

/*********************************************************************
 * @fn          BindWriteNV
 *
 * @brief       Save the Binding Table in NV
 *
 * @param       none
 *
 * @return      none
 */
void BindWriteNV( void )
{
  BindingEntry_t *pBind;
  BindingEntry_t bind;
  nvBindingHdr_t hdr;
  uint16 x;

  hdr.numRecs = 0;

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    pBind = &BindingTable[x];

    if ( pBind->srcEP != NV_BIND_EMPTY )
    {
      osal_memcpy( &bind, pBind, gBIND_REC_SIZE );

      // Save the record to NV
      osal_nv_write( ZCD_NV_BINDING_TABLE,
              (uint16)((sizeof(nvBindingHdr_t)) + (hdr.numRecs * NV_BIND_REC_SIZE)),
                      NV_BIND_REC_SIZE, &bind );
      hdr.numRecs++;
    }
  }

  // Save off the header
  osal_nv_write( ZCD_NV_BINDING_TABLE, 0, sizeof(nvBindingHdr_t), &hdr );
}

/*********************************************************************
 * @fn          bindUpdateAddr
 *
 * @brief       Update the network address in the binding table.
 *
 * @param       oldAddr - old network address
 * @param       newAddr - new network address
 *
 * @return      none
 */
void bindUpdateAddr( uint16 oldAddr, uint16 newAddr )
{
  uint16 oldIdx;
  uint16 newIdx;
  zAddrType_t addr;
  uint16 x;
  BindingEntry_t *pBind;

  addr.addrMode = Addr16Bit;
  addr.addr.shortAddr = oldAddr;
  oldIdx = bindingAddrMgsHelperFind( &addr );
  addr.addr.shortAddr = newAddr;
  newIdx = bindingAddrMgsHelperFind( &addr );

  for ( x = 0; x < gNWK_MAX_BINDING_ENTRIES; x++ )
  {
    pBind = &BindingTable[x];

    if ( pBind->dstIdx == oldIdx )
    {
      pBind->dstIdx = newIdx;
    }
  }
}

/*********************************************************************
 * @fn      bindingAddrMgsHelperFind
 *
 * @brief   Turns an zAddrType_t to an Addr Manager index
 *
 * @param   addr - zAddrType_t
 *
 * @return  INVALID_NODE_ADDR if not found, otherwise an index
 */
uint16 bindingAddrMgsHelperFind( zAddrType_t *addr )
{
  AddrMgrEntry_t entry;

  // Resolve addresses with the address manager
  entry.user = ADDRMGR_USER_BINDING;
  if ( addr->addrMode == Addr16Bit )
  {
    entry.nwkAddr = addr->addr.shortAddr;
    AddrMgrEntryLookupNwk( &entry );
  }
  else
  {
    AddrMgrExtAddrSet( entry.extAddr, addr->addr.extAddr );
    AddrMgrEntryLookupExt( &entry );
  }

  return ( entry.index );
}

/*********************************************************************
 * @fn      bindingAddrMgsHelperConvert
 *
 * @brief   Convert an index into an zAddrType_t
 *
 * @param   idx -
 * @param   addr - zAddrType_t
 *
 * @return  TRUE if found, FALSE if not
 */
uint8 bindingAddrMgsHelperConvert( uint16 idx, zAddrType_t *addr )
{
  AddrMgrEntry_t entry;
  uint8 stat;

  // Resolve addresses with the address manager
  entry.user = ADDRMGR_USER_BINDING;
  entry.index = idx;
  stat = AddrMgrEntryGet( &entry );
  if ( stat )
  {
    addr->addrMode = Addr64Bit;
    osal_cpyExtAddr( addr->addr.extAddr, entry.extAddr );
  }

  return ( stat );
}

/*********************************************************************
 * @fn      bindingAddrMgsHelperConvertShort
 *
 * @brief   Convert an index into a short address
 *
 * @param   idx -
 *
 * @return  INVALID_NODE_ADDR if not available, otherwise the short address
 */
uint16 bindingAddrMgsHelperConvertShort( uint16 idx )
{
  AddrMgrEntry_t entry;

  // Resolve addresses with the address manager
  entry.user = ADDRMGR_USER_BINDING;
  entry.index = idx;
  AddrMgrEntryGet( &entry );

  return ( entry.nwkAddr );
}

/*********************************************************************
 * @fn      bindAddrMgrLocalLoad
 *
 * @brief   Load local(self and parent) address information into
 *          Address Manager
 *
 * @param   none
 *
 * @return  none
 */
void bindAddrMgrLocalLoad( void )
{
  AddrMgrEntry_t entry;
  uint16         parent;

  // add "local"(self and parent) address informtion into the Address
  // Manager
  if ( bindAddrMgrLocalLoaded == FALSE )
  {
    // add the device's address information
    entry.user    = ADDRMGR_USER_BINDING;
    entry.nwkAddr = _NIB.nwkDevAddress;
    AddrMgrExtAddrSet( entry.extAddr, NLME_GetExtAddr() );
    AddrMgrEntryUpdate( &entry );

    // make sure parent address is valid
    parent = NLME_GetCoordShortAddr();
    if ( ( parent != entry.nwkAddr     ) &&
         ( parent != INVALID_NODE_ADDR )    )
    {
      // add the parent's address information
      entry.nwkAddr = parent;
      NLME_GetCoordExtAddr( entry.extAddr );
      AddrMgrEntryUpdate( &entry );
    }

    bindAddrMgrLocalLoaded = TRUE;
  }
}

/*********************************************************************
 * @fn      bindAddrIndexGet
 *
 * @brief   Get bind address index.
 *
 * @param   addr - <zAddrType_t>
 *
 * @return  (uint16) address index
 */
uint16 bindAddrIndexGet( zAddrType_t* addr )
{
  AddrMgrEntry_t entry;
  uint8          update;

  update = FALSE;

  // sync binding addresses with the address manager
  entry.user = ADDRMGR_USER_BINDING;

  if ( addr->addrMode == Addr16Bit )
  {
    entry.nwkAddr = addr->addr.shortAddr;

    if ( AddrMgrEntryLookupNwk( &entry ) == FALSE )
    {
      update = TRUE;
    }
  }
  else if ( addr->addrMode == Addr64Bit )
  {
    AddrMgrExtAddrSet( entry.extAddr, addr->addr.extAddr );

    if ( AddrMgrEntryLookupExt( &entry ) == FALSE )
    {
      update = TRUE;
    }
  }
  else if ( addr->addrMode == AddrGroup )
  {
    entry.index = addr->addr.shortAddr;
  }
  else
  {
    entry.index = INVALID_NODE_ADDR;
  }

  if ( update )
  {
    AddrMgrEntryUpdate( &entry );
  }

  return entry.index;
}

/*********************************************************************
 * @fn      GetBindingTableEntry
 *
 * @brief   Get a pointer to the Nth valid binding table entry.
 *
 * @param   Nth valid entry being requested.
 *
 * @return  The Nth valid binding table entry.
 */
BindingEntry_t *GetBindingTableEntry( uint16 Nth )
{
  BindingEntry_t *rtrn = NULL;

#if defined ( REFLECTOR )
  uint16 idx, cnt = 0;

  for ( idx = 0; idx < gNWK_MAX_BINDING_ENTRIES; idx++ )
  {
    if ( BindingTable[idx].srcEP != NV_BIND_EMPTY )
    {
      if ( cnt++ == Nth )
      {
        rtrn = BindingTable+idx;
        break;
      }
    }
  }
#else
  (void)Nth;
#endif

  return rtrn;
}

/*********************************************************************
*********************************************************************/
