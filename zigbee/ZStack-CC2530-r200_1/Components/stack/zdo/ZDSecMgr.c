/******************************************************************************
  Filename:       ZDSecMgr.c
  Revised:        $Date: 2011-07-22 16:06:08 -0700 (Fri, 22 Jul 2011) $
  Revision:       $Revision: 26893 $

  Description:    The ZigBee Device Security Manager.


  Copyright 2005-2011 Texas Instruments Incorporated. All rights reserved.

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

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * INCLUDES
 */
#include "ZComdef.h"
#include "OSAL.h"
#include "OSAL_NV.h"
#include "ZGlobals.h"
#include "ssp.h"
#include "nwk_globals.h"
#include "nwk.h"
#include "NLMEDE.h"
#include "AddrMgr.h"
#include "AssocList.h"
#include "APSMEDE.h"
#include "ZDConfig.h"
#include "ZDSecMgr.h"

/******************************************************************************
 * CONSTANTS
 */
// maximum number of devices managed by this Security Manager
#if !defined ( ZDSECMGR_DEVICE_MAX )
  #define ZDSECMGR_DEVICE_MAX 3
#endif

// total number of preconfigured devices (EXT address, MASTER key)
//devtag.pro.security
//#define ZDSECMGR_PRECONFIG_MAX ZDSECMGR_DEVICE_MAX
#define ZDSECMGR_PRECONFIG_MAX 0

// maximum number of MASTER keys this device may hold
#define ZDSECMGR_MASTERKEY_MAX ZDSECMGR_DEVICE_MAX

// maximum number of LINK keys this device may store
#define ZDSECMGR_ENTRY_MAX ZDSECMGR_DEVICE_MAX

// total number of devices under control - authentication, SKKE, etc.
#define ZDSECMGR_CTRL_MAX ZDSECMGR_DEVICE_MAX

// total number of stored devices
#if !defined ( ZDSECMGR_STORED_DEVICES )
  #define ZDSECMGR_STORED_DEVICES 3
#endif

// Total number of preconfigured trust center link key
#if !defined ( ZDSECMGR_TC_DEVICE_MAX )
  #define ZDSECMGR_TC_DEVICE_MAX 1
#endif

#if ( ZDSECMGR_TC_DEVICE_MAX < 1 ) || ( ZDSECMGR_TC_DEVICE_MAX > 255 )
  #error "ZDSECMGR_TC_DEVICE_MAX shall be between 1 and 255 !"
#endif

#define ZDSECMGR_CTRL_NONE       0
#define ZDSECMGR_CTRL_INIT       1
#define ZDSECMGR_CTRL_TK_MASTER  2
#define ZDSECMGR_CTRL_SKKE_INIT  3
#define ZDSECMGR_CTRL_SKKE_WAIT  4
#define ZDSECMGR_CTRL_SKKE_DONE  5
#define ZDSECMGR_CTRL_SKKE_FAIL  6
#define ZDSECMGR_CTRL_TK_NWK     7

#define ZDSECMGR_CTRL_BASE_CNTR      1
#define ZDSECMGR_CTRL_SKKE_INIT_CNTR 1
#define ZDSECMGR_CTRL_TK_NWK_CNTR    1

// set SKA slot maximum
#define ZDSECMGR_SKA_SLOT_MAX 1

// APSME Stub Implementations
#define ZDSecMgrMasterKeyGet   APSME_MasterKeyGet
#define ZDSecMgrLinkKeySet     APSME_LinkKeySet
#define ZDSecMgrLinkKeyNVIdGet APSME_LinkKeyNVIdGet
#define ZDSecMgrKeyFwdToChild  APSME_KeyFwdToChild
#define ZDSecMgrIsLinkKeyValid APSME_IsLinkKeyValid

/******************************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint8 extAddr[Z_EXTADDR_LEN];
  uint8 key[SEC_KEY_LEN];
} ZDSecMgrPreConfigData_t;

typedef struct
{
  uint16 ami;
  uint8  key[SEC_KEY_LEN];
} ZDSecMgrMasterKeyData_t;

typedef struct
{
  uint16            ami;
  uint16            keyNvId;   // index to the Link Key table in NV
  ZDSecMgr_Authentication_Option authenticateOption;
} ZDSecMgrEntry_t;

typedef struct
{
  ZDSecMgrEntry_t* entry;
  uint16           parentAddr;
  uint8            secure;
  uint8            state;
  uint8            cntr;
} ZDSecMgrCtrl_t;

typedef struct
{
  uint16          nwkAddr;
  uint8*          extAddr;
  uint16          parentAddr;
  uint8           secure;
  uint8           devStatus;
  ZDSecMgrCtrl_t* ctrl;
} ZDSecMgrDevice_t;

/******************************************************************************
 * LOCAL VARIABLES
 */
#if 0 // Taken out because the following functionality is only used for test
      // purpose. A more efficient (above) way is used. It can be put
      // back in if customers request for a white/black list feature.
uint8 ZDSecMgrStoredDeviceList[ZDSECMGR_STORED_DEVICES][Z_EXTADDR_LEN] =
{
  { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};
#endif

uint8 ZDSecMgrTCExtAddr[Z_EXTADDR_LEN]=
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Key data is put in CONST area for security reasons
CONST uint8 ZDSecMgrTCMasterKey[SEC_KEY_LEN] =
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x89,0x67,0x45,0x23,0x01,0xEF,0xCD,0xAB};

uint8 ZDSecMgrTCAuthenticated = FALSE;
uint8 ZDSecMgrTCDataLoaded    = FALSE;

//devtag.pro.security - remove this
#if ( ZDSECMGR_PRECONFIG_MAX != 0 )
const ZDSecMgrPreConfigData_t ZDSecMgrPreConfigData[ZDSECMGR_PRECONFIG_MAX] =
{
  //---------------------------------------------------------------------------
  // DEVICE A
  //---------------------------------------------------------------------------
  {
    // extAddr
    {0x7C,0x01,0x12,0x13,0x14,0x15,0x16,0x17},

    // key
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
  },
  //---------------------------------------------------------------------------
  // DEVICE B
  //---------------------------------------------------------------------------
  {
    // extAddr
    {0x84,0x03,0x00,0x00,0x00,0x4B,0x12,0x00},

    // key
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
  },
  //---------------------------------------------------------------------------
  // DEVICE C
  //---------------------------------------------------------------------------
  {
    // extAddr
    {0x3E,0x01,0x12,0x13,0x14,0x15,0x16,0x17},

    // key
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
  },
};
#endif // ( ZDSECMGR_PRECONFIG_MAX != 0 )

ZDSecMgrEntry_t* ZDSecMgrEntries  = NULL;
ZDSecMgrCtrl_t*  ZDSecMgrCtrlData = NULL;
void ZDSecMgrAddrMgrUpdate( uint16 ami, uint16 nwkAddr );
void ZDSecMgrAddrMgrCB( uint8 update, AddrMgrEntry_t* newEntry, AddrMgrEntry_t* oldEntry );

uint8 ZDSecMgrPermitJoiningEnabled;
uint8 ZDSecMgrPermitJoiningTimed;

APSME_TCLinkKey_t TrustCenterLinkKey;

APSME_ApsLinkKeyFrmCntr_t ApsLinkKeyFrmCntr[ZDSECMGR_ENTRY_MAX];
APSME_TCLinkKeyFrmCntr_t TCLinkKeyFrmCntr[ZDSECMGR_TC_DEVICE_MAX];

/******************************************************************************
 * PRIVATE FUNCTIONS
 *
 *   ZDSecMgrMasterKeyInit
 *   ZDSecMgrAddrStore
 *   ZDSecMgrExtAddrStore
 *   ZDSecMgrExtAddrLookup
 *   ZDSecMgrMasterKeyLookup
 *   ZDSecMgrMasterKeyStore
 *   ZDSecMgrEntryInit
 *   ZDSecMgrEntryLookup
 *   ZDSecMgrEntryLookupAMI
 *   ZDSecMgrEntryLookupExt
 *   ZDSecMgrEntryLookupExtGetIndex
 *   ZDSecMgrEntryFree
 *   ZDSecMgrEntryNew
 *   ZDSecMgrCtrlInit
 *   ZDSecMgrCtrlRelease
 *   ZDSecMgrCtrlLookup
 *   ZDSecMgrCtrlSet
 *   ZDSecMgrCtrlAdd
 *   ZDSecMgrCtrlTerm
 *   ZDSecMgrCtrlReset
 *   ZDSecMgrMasterKeyLoad
 *   ZDSecMgrAppKeyGet
 *   ZDSecMgrAppKeyReq
 *   ZDSecMgrEstablishKey
 *   ZDSecMgrSendMasterKey
 *   ZDSecMgrSendNwkKey
 *   ZDSecMgrDeviceEntryRemove
 *   ZDSecMgrDeviceEntryAdd
 *   ZDSecMgrDeviceCtrlHandler
 *   ZDSecMgrDeviceCtrlSetup
 *   ZDSecMgrDeviceCtrlUpdate
 *   ZDSecMgrDeviceRemove
 *   ZDSecMgrDeviceValidateSKKE
 *   ZDSecMgrDeviceValidateRM
 *   ZDSecMgrDeviceValidateCM
 *   ZDSecMgrDeviceValidate
 *   ZDSecMgrDeviceJoin
 *   ZDSecMgrDeviceJoinDirect
 *   ZDSecMgrDeviceJoinFwd
 *   ZDSecMgrDeviceNew
 *   ZDSecMgrAssocDeviceAuth
 *   ZDSecMgrAuthInitiate
 *   ZDSecMgrAuthNwkKey
 *   APSME_TCLinkKeyInit
 *   APSME_IsDefaultTCLK
 */
//-----------------------------------------------------------------------------
// master key data
//-----------------------------------------------------------------------------
void ZDSecMgrMasterKeyInit( void );

//-----------------------------------------------------------------------------
// address management
//-----------------------------------------------------------------------------
ZStatus_t ZDSecMgrAddrStore( uint16 nwkAddr, uint8* extAddr, uint16* ami );
ZStatus_t ZDSecMgrExtAddrStore( uint16 nwkAddr, uint8* extAddr, uint16* ami );
ZStatus_t ZDSecMgrExtAddrLookup( uint8* extAddr, uint16* ami );

//-----------------------------------------------------------------------------
// MASTER key data
//-----------------------------------------------------------------------------
ZStatus_t ZDSecMgrMasterKeyLookup( uint16 ami, uint16* pKeyNvId );
ZStatus_t ZDSecMgrMasterKeyStore( uint16 ami, uint8* key );

//-----------------------------------------------------------------------------
// entry data
//-----------------------------------------------------------------------------
void ZDSecMgrEntryInit(uint8 state);
ZStatus_t ZDSecMgrEntryLookup( uint16 nwkAddr, ZDSecMgrEntry_t** entry );
ZStatus_t ZDSecMgrEntryLookupAMI( uint16 ami, ZDSecMgrEntry_t** entry );
ZStatus_t ZDSecMgrEntryLookupExt( uint8* extAddr, ZDSecMgrEntry_t** entry );
ZStatus_t ZDSecMgrEntryLookupExtGetIndex( uint8* extAddr, ZDSecMgrEntry_t** entry, uint16* entryIndex );
void ZDSecMgrEntryFree( ZDSecMgrEntry_t* entry );
ZStatus_t ZDSecMgrEntryNew( ZDSecMgrEntry_t** entry );
ZStatus_t ZDSecMgrAuthenticationSet( uint8* extAddr, ZDSecMgr_Authentication_Option option );
void ZDSecMgrApsLinkKeyInit(void);
#if defined NV_RESTORE
static void ZDSecMgrWriteNV(void);
static void ZDSecMgrRestoreFromNV(void);
#endif

//-----------------------------------------------------------------------------
// control data
//-----------------------------------------------------------------------------
void ZDSecMgrCtrlInit( void );
void ZDSecMgrCtrlRelease( ZDSecMgrCtrl_t* ctrl );
void ZDSecMgrCtrlLookup( ZDSecMgrEntry_t* entry, ZDSecMgrCtrl_t** ctrl );
void ZDSecMgrCtrlSet( ZDSecMgrDevice_t* device,
                      ZDSecMgrEntry_t*  entry,
                      ZDSecMgrCtrl_t*   ctrl );
ZStatus_t ZDSecMgrCtrlAdd( ZDSecMgrDevice_t* device, ZDSecMgrEntry_t*  entry );
void ZDSecMgrCtrlTerm( ZDSecMgrEntry_t* entry );
ZStatus_t ZDSecMgrCtrlReset( ZDSecMgrDevice_t* device,
                             ZDSecMgrEntry_t*  entry );

//-----------------------------------------------------------------------------
// key support
//-----------------------------------------------------------------------------
ZStatus_t ZDSecMgrMasterKeyLoad( uint8* extAddr, uint8* key );
ZStatus_t ZDSecMgrAppKeyGet( uint16  initNwkAddr,
                             uint8*  initExtAddr,
                             uint16  partNwkAddr,
                             uint8*  partExtAddr,
                             uint8** key,
                             uint8*  keyType );
void ZDSecMgrAppKeyReq( ZDO_RequestKeyInd_t* ind );
ZStatus_t ZDSecMgrEstablishKey( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrSendMasterKey( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrSendNwkKey( ZDSecMgrDevice_t* device );
void ZDSecMgrNwkKeyInit(uint8 setDefault);

//-----------------------------------------------------------------------------
// device entry
//-----------------------------------------------------------------------------
void ZDSecMgrDeviceEntryRemove( ZDSecMgrEntry_t* entry );
ZStatus_t ZDSecMgrDeviceEntryAdd( ZDSecMgrDevice_t* device, uint16 ami );

//-----------------------------------------------------------------------------
// device control
//-----------------------------------------------------------------------------
void ZDSecMgrDeviceCtrlHandler( ZDSecMgrDevice_t* device );
void ZDSecMgrDeviceCtrlSetup( ZDSecMgrDevice_t* device );
void ZDSecMgrDeviceCtrlUpdate( uint8* extAddr, uint8 state );

//-----------------------------------------------------------------------------
// device management
//-----------------------------------------------------------------------------
void ZDSecMgrDeviceRemove( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceValidateSKKE( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceValidateRM( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceValidateCM( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceValidate( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceJoin( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceJoinDirect( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceJoinFwd( ZDSecMgrDevice_t* device );
ZStatus_t ZDSecMgrDeviceNew( ZDSecMgrDevice_t* device );

//-----------------------------------------------------------------------------
// association management
//-----------------------------------------------------------------------------
void ZDSecMgrAssocDeviceAuth( associated_devices_t* assoc );

//-----------------------------------------------------------------------------
// authentication management
//-----------------------------------------------------------------------------
void ZDSecMgrAuthInitiate( uint8* responder );
void ZDSecMgrAuthNwkKey( void );

//-----------------------------------------------------------------------------
// APSME function
//-----------------------------------------------------------------------------
void APSME_TCLinkKeyInit( uint8 setDefault );
uint8 APSME_IsDefaultTCLK( uint8 *extAddr );

/******************************************************************************
 * @fn          ZDSecMgrMasterKeyInit                     ]
 *
 * @brief       Initialize master key data in NV
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrMasterKeyInit( void )
{
  uint16 index;
  ZDSecMgrMasterKeyData_t   masterKeyData;

  masterKeyData.ami = INVALID_NODE_ADDR;

  osal_memset( &masterKeyData.key, 0x00, SEC_KEY_LEN );

  for ( index = 0; index < ZDSECMGR_MASTERKEY_MAX; index++ )
  {
    if ( osal_nv_item_init( (ZCD_NV_MASTER_KEY_DATA_START + index),
                       sizeof(ZDSecMgrMasterKeyData_t), &masterKeyData) == SUCCESS)
    {
      // the item already exists in NV just needs to be set to default values
      osal_nv_write( (ZCD_NV_MASTER_KEY_DATA_START + index), 0,
                      sizeof(ZDSecMgrMasterKeyData_t), &masterKeyData );
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrAddrStore
 *
 * @brief       Store device addresses.
 *
 * @param       nwkAddr - [in] NWK address
 * @param       extAddr - [in] EXT address
 * @param       ami     - [out] Address Manager index
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrAddrStore( uint16 nwkAddr, uint8* extAddr, uint16* ami )
{
  ZStatus_t      status;
  AddrMgrEntry_t entry;


  // add entry
  entry.user    = ADDRMGR_USER_SECURITY;
  entry.nwkAddr = nwkAddr;
  AddrMgrExtAddrSet( entry.extAddr, extAddr );

  if ( AddrMgrEntryUpdate( &entry ) == TRUE )
  {
    // return successful results
    *ami   = entry.index;
    status = ZSuccess;
  }
  else
  {
    // return failed results
    *ami   = entry.index;
    status = ZNwkUnknownDevice;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrExtAddrStore
 *
 * @brief       Store EXT address.
 *
 * @param       extAddr - [in] EXT address
 * @param       ami     - [out] Address Manager index
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrExtAddrStore( uint16 nwkAddr, uint8* extAddr, uint16* ami )
{
  ZStatus_t      status;
  AddrMgrEntry_t entry;


  // add entry
  entry.user    = ADDRMGR_USER_SECURITY;
  entry.nwkAddr = nwkAddr;
  AddrMgrExtAddrSet( entry.extAddr, extAddr );

  if ( AddrMgrEntryUpdate( &entry ) == TRUE )
  {
    // return successful results
    *ami   = entry.index;
    status = ZSuccess;
  }
  else
  {
    // return failed results
    *ami   = entry.index;
    status = ZNwkUnknownDevice;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrExtAddrLookup
 *
 * @brief       Lookup index for specified EXT address.
 *
 * @param       extAddr - [in] EXT address
 * @param       ami     - [out] Address Manager index
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrExtAddrLookup( uint8* extAddr, uint16* ami )
{
  ZStatus_t      status;
  AddrMgrEntry_t entry;


  // lookup entry
  entry.user = ADDRMGR_USER_SECURITY;
  AddrMgrExtAddrSet( entry.extAddr, extAddr );

  if ( AddrMgrEntryLookupExt( &entry ) == TRUE )
  {
    // return successful results
    *ami   = entry.index;
    status = ZSuccess;
  }
  else
  {
    // return failed results
    *ami   = entry.index;
    status = ZNwkUnknownDevice;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrMasterKeyLookup
 *
 * @brief       Lookup MASTER key for specified address index.
 *
 * @param       ami - [in] Address Manager index
 * @param       pKeyNvId - [out] MASTER key NV ID
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrMasterKeyLookup( uint16 ami, uint16* pKeyNvId )
{
  uint16 index;
  ZDSecMgrMasterKeyData_t masterKeyData;


  for ( index = 0; index < ZDSECMGR_MASTERKEY_MAX ; index++ )
  {
    // Read entry index of the Master key table from NV
    osal_nv_read( (ZCD_NV_MASTER_KEY_DATA_START + index), 0,
                  sizeof(ZDSecMgrMasterKeyData_t), &masterKeyData );

    if ( masterKeyData.ami == ami )
    {
      // return successful results
      *pKeyNvId   = ZCD_NV_MASTER_KEY_DATA_START + index;

      // clear copy of key in RAM
      osal_memset(&masterKeyData, 0x00, sizeof(ZDSecMgrMasterKeyData_t));

      return ZSuccess;
    }
  }

  *pKeyNvId = SEC_NO_KEY_NV_ID;

  // clear copy of key in RAM
  osal_memset(&masterKeyData, 0x00, sizeof(ZDSecMgrMasterKeyData_t));

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrMasterKeyStore
 *
 * @brief       Store MASTER key for specified address index.
 *
 * @param       ami - [in] Address Manager index
 * @param       key - [in] valid key to store
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrMasterKeyStore( uint16 ami, uint8* key )
{
  uint16    index;
  ZDSecMgrMasterKeyData_t   masterKeyData;


  for ( index = 0; index < ZDSECMGR_MASTERKEY_MAX ; index++ )
  {
    // Read entry index of the Master key table from NV
    osal_nv_read( (ZCD_NV_MASTER_KEY_DATA_START + index), 0,
                   sizeof(ZDSecMgrMasterKeyData_t), &masterKeyData );

    if ( masterKeyData.ami == INVALID_NODE_ADDR )
    {
      // store EXT address index
      masterKeyData.ami = ami;

      if ( key != NULL )
      {
        osal_memcpy( masterKeyData.key, key,  SEC_KEY_LEN );
      }
      else
      {
        osal_memset( masterKeyData.key, 0, SEC_KEY_LEN );
      }

      // set new values in NV
      osal_nv_write( (ZCD_NV_MASTER_KEY_DATA_START + index), 0,
                      sizeof(ZDSecMgrMasterKeyData_t), &masterKeyData );

      // clear copy of key in RAM
      osal_memset( &masterKeyData, 0x00, sizeof(ZDSecMgrMasterKeyData_t) );

      // return successful results
      return ZSuccess;
    }
  }

  // clear copy of key in RAM
  osal_memset( &masterKeyData, 0x00, sizeof(ZDSecMgrMasterKeyData_t) );

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrEntryInit
 *
 * @brief       Initialize entry sub module
 *
 * @param       state - device initialization state
 *
 * @return      none
 */
void ZDSecMgrEntryInit(uint8 state)
{
  if (ZDSecMgrEntries == NULL)
  {
    uint16 index;

    if ((ZDSecMgrEntries = osal_mem_alloc(sizeof(ZDSecMgrEntry_t) * ZDSECMGR_ENTRY_MAX)) == NULL)
    {
      return;
    }

    for (index = 0; index < ZDSECMGR_ENTRY_MAX; index++)
    {
      ZDSecMgrEntries[index].ami = INVALID_NODE_ADDR;

      ZDSecMgrEntries[index].keyNvId = SEC_NO_KEY_NV_ID;
    }
  }

#if defined NV_RESTORE
  if (state == ZDO_INITDEV_RESTORED_NETWORK_STATE)
  {
    ZDSecMgrRestoreFromNV();
  }
#else
  (void)state;
#endif
}

/******************************************************************************
 * @fn          ZDSecMgrEntryLookup
 *
 * @brief       Lookup entry index using specified NWK address.
 *
 * @param       nwkAddr - [in] NWK address
 * @param       entry   - [out] valid entry
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrEntryLookup( uint16 nwkAddr, ZDSecMgrEntry_t** entry )
{
  uint16         index;
  AddrMgrEntry_t addrMgrEntry;

  // initialize results
  *entry = NULL;

  // verify data is available
  if ( ZDSecMgrEntries != NULL )
  {
    addrMgrEntry.user    = ADDRMGR_USER_SECURITY;
    addrMgrEntry.nwkAddr = nwkAddr;

    if ( AddrMgrEntryLookupNwk( &addrMgrEntry ) == TRUE )
    {
      for ( index = 0; index < ZDSECMGR_ENTRY_MAX ; index++ )
      {
        if ( addrMgrEntry.index == ZDSecMgrEntries[index].ami )
        {
          // return successful results
          *entry = &ZDSecMgrEntries[index];

          // break from loop
          return ZSuccess;
        }
      }
    }
  }

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrEntryLookupAMI
 *
 * @brief       Lookup entry using specified address index
 *
 * @param       ami   - [in] Address Manager index
 * @param       entry - [out] valid entry
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrEntryLookupAMI( uint16 ami, ZDSecMgrEntry_t** entry )
{
  uint16 index;

  // initialize results
  *entry = NULL;

  // verify data is available
  if ( ZDSecMgrEntries != NULL )
  {
    for ( index = 0; index < ZDSECMGR_ENTRY_MAX ; index++ )
    {
      if ( ZDSecMgrEntries[index].ami == ami )
      {
        // return successful results
        *entry = &ZDSecMgrEntries[index];

        // break from loop
        return ZSuccess;
      }
    }
  }

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrEntryLookupExt
 *
 * @brief       Lookup entry index using specified EXT address.
 *
 * @param       extAddr - [in] EXT address
 * @param       entry   - [out] valid entry
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrEntryLookupExt( uint8* extAddr, ZDSecMgrEntry_t** entry )
{
  ZStatus_t status;
  uint16    ami;

  // initialize results
  *entry = NULL;
  status = ZNwkUnknownDevice;

  // lookup address index
  if ( ZDSecMgrExtAddrLookup( extAddr, &ami ) == ZSuccess )
  {
    status = ZDSecMgrEntryLookupAMI( ami, entry );
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrEntryLookupExtGetIndex
 *
 * @brief       Lookup entry index using specified EXT address.
 *
 * @param       extAddr - [in] EXT address
 * @param       entryIndex - [out] valid index to the entry table
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrEntryLookupExtGetIndex( uint8* extAddr, ZDSecMgrEntry_t** entry, uint16* entryIndex )
{
  uint16 ami;
  uint16 index;

  // lookup address index
  if ( ZDSecMgrExtAddrLookup( extAddr, &ami ) == ZSuccess )
  {
    // verify data is available
    if ( ZDSecMgrEntries != NULL )
    {
      for ( index = 0; index < ZDSECMGR_ENTRY_MAX ; index++ )
      {
        if ( ZDSecMgrEntries[index].ami == ami )
        {
          // return successful results
          *entry = &ZDSecMgrEntries[index];
          *entryIndex = index;

          // break from loop
          return ZSuccess;
        }
      }
    }
  }

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrEntryFree
 *
 * @brief       Free entry.
 *
 * @param       entry - [in] valid entry
 *
 * @return      ZStatus_t
 */
void ZDSecMgrEntryFree( ZDSecMgrEntry_t* entry )
{
  APSME_LinkKeyData_t   *pApsLinkKey = NULL;

  pApsLinkKey = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

  if (pApsLinkKey != NULL)
  {
    osal_memset( pApsLinkKey, 0x00, sizeof(APSME_LinkKeyData_t) );

    // Clear the APS Link key in NV
    osal_nv_write( entry->keyNvId, 0,
                        sizeof(APSME_LinkKeyData_t), pApsLinkKey);

    // set entry to invalid Key
    entry->keyNvId = SEC_NO_KEY_NV_ID;

    osal_mem_free(pApsLinkKey);
  }

  // marking the entry as INVALID_NODE_ADDR
  entry->ami = INVALID_NODE_ADDR;
}

/******************************************************************************
 * @fn          ZDSecMgrEntryNew
 *
 * @brief       Get a new entry.
 *
 * @param       entry - [out] valid entry
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrEntryNew( ZDSecMgrEntry_t** entry )
{
  uint16 index;

  // initialize results
  *entry = NULL;

  // verify data is available
  if ( ZDSecMgrEntries != NULL )
  {
    // find available entry
    for ( index = 0; index < ZDSECMGR_ENTRY_MAX ; index++ )
    {
      if ( ZDSecMgrEntries[index].ami == INVALID_NODE_ADDR )
      {
        // return successful result
        *entry = &ZDSecMgrEntries[index];

        // Set the authentication option to default
        ZDSecMgrEntries[index].authenticateOption = ZDSecMgr_Not_Authenticated;

        // break from loop
        return ZSuccess;
      }
    }
  }

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlInit
 *
 * @brief       Initialize control sub module
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrCtrlInit( void )
{
  uint16 size;
  uint16 index;

  // allocate entry data
  size = (short)( sizeof(ZDSecMgrCtrl_t) * ZDSECMGR_CTRL_MAX );

  ZDSecMgrCtrlData = osal_mem_alloc( size );

  // initialize data
  if ( ZDSecMgrCtrlData != NULL )
  {
    for( index = 0; index < ZDSECMGR_CTRL_MAX; index++ )
    {
      ZDSecMgrCtrlData[index].state = ZDSECMGR_CTRL_NONE;
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlRelease
 *
 * @brief       Release control data.
 *
 * @param       ctrl - [in] valid control data
 *
 * @return      none
 */
void ZDSecMgrCtrlRelease( ZDSecMgrCtrl_t* ctrl )
{
  // should always be enough entry control data
  ctrl->state = ZDSECMGR_CTRL_NONE;
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlLookup
 *
 * @brief       Lookup control data.
 *
 * @param       entry - [in] valid entry data
 * @param       ctrl  - [out] control data - NULL if not found
 *
 * @return      none
 */
void ZDSecMgrCtrlLookup( ZDSecMgrEntry_t* entry, ZDSecMgrCtrl_t** ctrl )
{
  uint16 index;

  // initialize search results
  *ctrl = NULL;

  // verify data is available
  if ( ZDSecMgrCtrlData != NULL )
  {
    for ( index = 0; index < ZDSECMGR_CTRL_MAX; index++ )
    {
      // make sure control data is in use
      if ( ZDSecMgrCtrlData[index].state != ZDSECMGR_CTRL_NONE )
      {
        // check for entry match
        if ( ZDSecMgrCtrlData[index].entry == entry )
        {
          // return this control data
          *ctrl = &ZDSecMgrCtrlData[index];

          // break from loop
          return;
        }
      }
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlSet
 *
 * @brief       Set control data.
 *
 * @param       device - [in] valid device data
 * @param       entry  - [in] valid entry data
 * @param       ctrl   - [in] valid control data
 *
 * @return      none
 */
void ZDSecMgrCtrlSet( ZDSecMgrDevice_t* device,
                      ZDSecMgrEntry_t*  entry,
                      ZDSecMgrCtrl_t*   ctrl )
{
  // set control date
  ctrl->parentAddr = device->parentAddr;
  ctrl->secure     = device->secure;
  ctrl->entry      = entry;
  ctrl->state      = ZDSECMGR_CTRL_INIT;
  ctrl->cntr       = 0;

  // set device pointer
  device->ctrl = ctrl;
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlAdd
 *
 * @brief       Add control data.
 *
 * @param       device - [in] valid device data
 * @param       entry  - [in] valid entry data
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrCtrlAdd( ZDSecMgrDevice_t* device, ZDSecMgrEntry_t*  entry )
{
  uint16 index;

  // verify data is available
  if ( ZDSecMgrCtrlData != NULL )
  {
    // look for an empty slot
    for ( index = 0; index < ZDSECMGR_CTRL_MAX; index++ )
    {
      if ( ZDSecMgrCtrlData[index].state == ZDSECMGR_CTRL_NONE )
      {
        // return successful results
        ZDSecMgrCtrlSet( device, entry, &ZDSecMgrCtrlData[index] );

        // break from loop
        return ZSuccess;
      }
    }
  }

  return ZNwkUnknownDevice;
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlTerm
 *
 * @brief       Terminate device control.
 *
 * @param       entry - [in] valid entry data
 *
 * @return      none
 */
void ZDSecMgrCtrlTerm( ZDSecMgrEntry_t* entry )
{
  ZDSecMgrCtrl_t* ctrl;

  // remove device from control data
  ZDSecMgrCtrlLookup ( entry, &ctrl );

  if ( ctrl != NULL )
  {
    ZDSecMgrCtrlRelease ( ctrl );
  }
}

/******************************************************************************
 * @fn          ZDSecMgrCtrlReset
 *
 * @brief       Reset control data.
 *
 * @param       device - [in] valid device data
 * @param       entry  - [in] valid entry data
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrCtrlReset( ZDSecMgrDevice_t* device, ZDSecMgrEntry_t* entry )
{
  ZStatus_t       status;
  ZDSecMgrCtrl_t* ctrl;

  // initialize results
  status = ZNwkUnknownDevice;

  // look for a match for the entry
  ZDSecMgrCtrlLookup( entry, &ctrl );

  if ( ctrl != NULL )
  {
    ZDSecMgrCtrlSet( device, entry, ctrl );

    status = ZSuccess;
  }
  else
  {
    status = ZDSecMgrCtrlAdd( device, entry );
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrMasterKeyLoad
 *
 * @brief       Load the MASTER key for device with specified EXT
 *              address.
 *
 * @param       extAddr - [in] EXT address of device
 * @param       key     - [in] MASTER key shared with device
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrMasterKeyLoad( uint8* extAddr, uint8* key )
{
  ZStatus_t status;
  uint16 ami;
  uint16 keyNvId;

  // set status based on policy
  status = ZDSecMgrExtAddrLookup( extAddr, &ami );

  if ( status == ZSuccess )
  {
    // get the address NV ID
    if ( ZDSecMgrMasterKeyLookup( ami, &keyNvId ) == ZSuccess )
    {
      // overwrite old key in NV
      osal_nv_write( keyNvId, osal_offsetof(ZDSecMgrMasterKeyData_t, key),
                     SEC_KEY_LEN, key );
    }
    else
    {
      // store new key -- NULL will zero key
      status = ZDSecMgrMasterKeyStore( ami, key );
    }
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrAppKeyGet
 *
 * @brief       get an APP key - option APP(MASTER or LINK) key
 *
 * @param       initNwkAddr - [in] NWK address of initiator device
 * @param       initExtAddr - [in] EXT address of initiator device
 * @param       partNwkAddr - [in] NWK address of partner device
 * @param       partExtAddr - [in] EXT address of partner device
 * @param       key         - [out] APP(MASTER or LINK) key
 * @param       keyType     - [out] APP(MASTER or LINK) key type
 *
 * @return      ZStatus_t
 */
uint8 ZDSecMgrAppKeyType = KEY_TYPE_APP_LINK;    // Set the default key type
                                                 // to KEY_TYPE_APP_LINK since
                                                 // only specific requirement
                                                 // right now comes from SE profile

ZStatus_t ZDSecMgrAppKeyGet( uint16  initNwkAddr,
                             uint8*  initExtAddr,
                             uint16  partNwkAddr,
                             uint8*  partExtAddr,
                             uint8** key,
                             uint8*  keyType )
{
  // Intentionally unreferenced parameters
  (void)initNwkAddr;
  (void)initExtAddr;
  (void)partNwkAddr;
  (void)partExtAddr;

  //---------------------------------------------------------------------------
  // note:
  // should use a robust mechanism to generate keys, for example
  // combine EXT addresses and call a hash function
  //---------------------------------------------------------------------------
  SSP_GetTrueRand( SEC_KEY_LEN, *key );

  *keyType = ZDSecMgrAppKeyType;

  return ZSuccess;
}

/******************************************************************************
 * @fn          ZDSecMgrAppKeyReq
 *
 * @brief       Process request for APP key between two devices.
 *
 * @param       device - [in] ZDO_RequestKeyInd_t, request info
 *
 * @return      none
 */
void ZDSecMgrAppKeyReq( ZDO_RequestKeyInd_t* ind )
{
  APSME_TransportKeyReq_t req;
  uint8                   initExtAddr[Z_EXTADDR_LEN];
  uint16                  partNwkAddr;
  uint8                   key[SEC_KEY_LEN];


  // validate initiator and partner
  if ( ( APSME_LookupNwkAddr( ind->partExtAddr, &partNwkAddr ) == TRUE ) &&
       ( APSME_LookupExtAddr( ind->srcAddr, initExtAddr ) == TRUE      )   )
  {
    // point the key to some memory
    req.key = key;

    // get an APP key - option APP (MASTER or LINK) key
    if ( ZDSecMgrAppKeyGet( ind->srcAddr,
                            initExtAddr,
                            partNwkAddr,
                            ind->partExtAddr,
                            &req.key,
                            &req.keyType ) == ZSuccess )
    {
      // always secure
      req.nwkSecure = TRUE;
      req.apsSecure = TRUE;
      req.tunnel    = NULL;

      // send key to initiator device
      req.dstAddr   = ind->srcAddr;
      req.extAddr   = ind->partExtAddr;
      req.initiator = TRUE;
      APSME_TransportKeyReq( &req );

      // send key to partner device
      req.dstAddr   = partNwkAddr;
      req.extAddr   = initExtAddr;
      req.initiator = FALSE;

      APSME_TransportKeyReq( &req );

      // clear copy of key in RAM
      osal_memset( key, 0x00, SEC_KEY_LEN);

    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrEstablishKey
 *
 * @brief       Start SKKE with device joining network.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrEstablishKey( ZDSecMgrDevice_t* device )
{
  ZStatus_t               status;
  APSME_EstablishKeyReq_t req;


  req.respExtAddr = device->extAddr;
  req.method      = APSME_SKKE_METHOD;

  if ( device->parentAddr == NLME_GetShortAddr() )
  {
    req.dstAddr   = device->nwkAddr;
    //devtag.0604.todo - remove obsolete
    req.apsSecure = FALSE;
    req.nwkSecure = FALSE;
  }
  else
  {
    req.dstAddr   = device->parentAddr;
    //devtag.0604.todo - remove obsolete
    req.apsSecure = TRUE;
    req.nwkSecure = TRUE;
  }

  status = APSME_EstablishKeyReq( &req );

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrSendMasterKey
 *
 * @brief       Send MASTER key to device joining network.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrSendMasterKey( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;
  APSME_TransportKeyReq_t req;
  uint16 keyNvId;
  uint8 masterKey[SEC_KEY_LEN];


  req.keyType = KEY_TYPE_TC_MASTER;
  req.extAddr = device->extAddr;
  req.tunnel  = NULL;

  if ( ZDSecMgrMasterKeyLookup( device->ctrl->entry->ami, &keyNvId ) == ZSuccess )
  {
    osal_nv_read( keyNvId, osal_offsetof(ZDSecMgrMasterKeyData_t, key),
                  SEC_KEY_LEN, masterKey );
  }
  else
  {
    // in case read from NV fails
    osal_memset( masterKey, 0x00, SEC_KEY_LEN);
  }

  req.key = masterKey;

  //check if using secure hop to to parent
  if ( device->parentAddr != NLME_GetShortAddr() )
  {
    //send to parent with security
    req.dstAddr   = device->parentAddr;
    req.nwkSecure = TRUE;
    req.apsSecure = TRUE;
  }
  else
  {
    //direct with no security
    req.dstAddr   = device->nwkAddr;
    req.nwkSecure = FALSE;
    req.apsSecure = FALSE;
  }

  status = APSME_TransportKeyReq( &req );

  // clear copy of key in RAM
  osal_memset( masterKey, 0x00, SEC_KEY_LEN);

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrSendNwkKey
 *
 * @brief       Send NWK key to device joining network.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrSendNwkKey( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;
  APSME_TransportKeyReq_t req;
  APSDE_FrameTunnel_t tunnel;
  nwkKeyDesc tmpKey;

  req.dstAddr   = device->nwkAddr;
  req.extAddr   = device->extAddr;

  if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
  {
    req.keyType   = KEY_TYPE_NWK_HIGH;
  }
  else
  {
    req.keyType   = KEY_TYPE_NWK;
  }

  // get the Active Key into a local variable
  if( NLME_ReadNwkKeyInfo( 0, sizeof(tmpKey), &tmpKey,
                           ZCD_NV_NWK_ACTIVE_KEY_INFO ) != SUCCESS )
  {
    // set key data to all 0s if NV read fails
    osal_memset(&tmpKey, 0x00, sizeof(tmpKey));
  }

  if ( (ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH)
      || (ZG_CHECK_SECURITY_MODE == ZG_SECURITY_SE_STANDARD) )
  {
    // set values
    req.keySeqNum = tmpKey.keySeqNum;
    req.key       = tmpKey.key;

    //devtag.pro.security.todo - make sure that if there is no link key the NWK
    //key isn't used to secure the frame at the APS layer -- since the receiving
    //device may not have a NWK key yet
    req.apsSecure = TRUE;

    // check if using secure hop to to parent
    if ( device->parentAddr == NLME_GetShortAddr() )
    {
      req.nwkSecure = FALSE;
      req.tunnel    = NULL;
    }
    else
    {
      req.nwkSecure   = TRUE;
      req.tunnel      = &tunnel;
      req.tunnel->tna = device->parentAddr;
      req.tunnel->dea = device->extAddr;
    }
  }
  else
  {
    // default values
    //devtag.0604.verify
    req.nwkSecure = TRUE;
    req.apsSecure = FALSE;
    req.tunnel    = NULL;

    if ( device->parentAddr != NLME_GetShortAddr() )
    {
      req.dstAddr = device->parentAddr;
    }

    // special cases
    //devtag.0604.todo - modify to preconfig flag
    if ( device->secure == FALSE )
    {
      req.keySeqNum = tmpKey.keySeqNum;
      req.key       = tmpKey.key;

      // check if using secure hop to to parent
      if ( device->parentAddr == NLME_GetShortAddr() )
      {
        req.nwkSecure = FALSE;
      }
    }
    else
    {
      req.key       = NULL;
      req.keySeqNum = 0;
    }
  }

  status = APSME_TransportKeyReq( &req );

  // clear copy of key in RAM before return
  osal_memset( &tmpKey, 0x00, sizeof(nwkKeyDesc) );

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceEntryRemove
 *
 * @brief       Remove device entry.
 *
 * @param       entry - [in] valid entry
 *
 * @return      none
 */
void ZDSecMgrDeviceEntryRemove( ZDSecMgrEntry_t* entry )
{
  // terminate device control
  if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
  {
    ZDSecMgrCtrlTerm( entry );
  }

  // remove device from entry data
  ZDSecMgrEntryFree( entry );

}

/******************************************************************************
 * @fn          ZDSecMgrDeviceRemoveByExtAddr
 *
 * @brief       Remove device entry by its ext address.
 *
 * @param       pAddr - pointer to the extended address
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceRemoveByExtAddr( uint8 *pAddr )
{
  ZDSecMgrEntry_t *pEntry;
  uint8           retValue;

  retValue = (uint8)ZDSecMgrEntryLookupExt( pAddr, &pEntry );

  if( retValue == ZSuccess )
  {
    ZDSecMgrDeviceEntryRemove( pEntry );
  }

  return retValue;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceEntryAdd
 *
 * @brief       Add entry.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 * @param       ami    - [in] Address Manager index
 *
 * @return      ZStatus_t
 */
void ZDSecMgrAddrMgrUpdate( uint16 ami, uint16 nwkAddr )
{
  AddrMgrEntry_t entry;

  // get the ami data
  entry.user  = ADDRMGR_USER_SECURITY;
  entry.index = ami;

  AddrMgrEntryGet( &entry );

  // check if NWK address is same
  if ( entry.nwkAddr != nwkAddr )
  {
    // update NWK address
    entry.nwkAddr = nwkAddr;

    AddrMgrEntryUpdate( &entry );
  }
}

ZStatus_t ZDSecMgrDeviceEntryAdd( ZDSecMgrDevice_t* device, uint16 ami )
{
  ZStatus_t        status;
  ZDSecMgrEntry_t* entry;

  // initialize as unknown until completion
  status = ZNwkUnknownDevice;

  device->ctrl = NULL;

  // make sure not already registered
  if ( ZDSecMgrEntryLookup( device->nwkAddr, &entry ) == ZSuccess )
  {
    // verify that address index is same
    if ( entry->ami != ami )
    {
      // remove conflicting entry
      ZDSecMgrDeviceEntryRemove( entry );

      if ( ZDSecMgrEntryLookupAMI( ami, &entry ) == ZSuccess )
      {
        // update NWK address
        ZDSecMgrAddrMgrUpdate( ami, device->nwkAddr );
      }
    }
  }
  else if ( ZDSecMgrEntryLookupAMI( ami, &entry ) == ZSuccess )
  {
    // update NWK address
    ZDSecMgrAddrMgrUpdate( ami, device->nwkAddr );
  }

  // check if a new entry needs to be created
  if ( entry == NULL )
  {
    // get new entry
    if ( ZDSecMgrEntryNew( &entry ) == ZSuccess )
    {
      // reset entry lkd

      // finish setting up entry
      entry->ami = ami;

      // update NWK address
      ZDSecMgrAddrMgrUpdate( ami, device->nwkAddr );

      // enter new device into device control
      if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
      {
        status = ZDSecMgrCtrlAdd( device, entry );
      }
      else
      {
        status = ZSuccess;
      }
    }
  }
  else
  {
    // reset entry lkd

    // reset entry in entry control
    if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
    {
      status = ZDSecMgrCtrlReset( device, entry );
    }
    else
    {
      status = ZSuccess;
    }
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceCtrlHandler
 *
 * @brief       Device control handler.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      none
 */
void ZDSecMgrDeviceCtrlHandler( ZDSecMgrDevice_t* device )
{
  uint8 state;
  uint8 cntr;

  state = device->ctrl->state;
  cntr  = ZDSECMGR_CTRL_BASE_CNTR;

  switch ( state )
  {
    case ZDSECMGR_CTRL_TK_MASTER:
      if ( ZDSecMgrSendMasterKey( device ) == ZSuccess )
      {
        state = ZDSECMGR_CTRL_SKKE_INIT;
        cntr  = ZDSECMGR_CTRL_SKKE_INIT_CNTR;
      }
      break;

    case ZDSECMGR_CTRL_SKKE_INIT:
      if ( ZDSecMgrEstablishKey( device ) == ZSuccess )
      {
        state = ZDSECMGR_CTRL_SKKE_WAIT;
      }
      break;

    case ZDSECMGR_CTRL_SKKE_WAIT:
      // continue to wait for SKA control timeout
      break;

    case ZDSECMGR_CTRL_TK_NWK:
      if ( ZDSecMgrSendNwkKey( device ) == ZSuccess )
      {
        state = ZDSECMGR_CTRL_NONE;
      }
      break;

    default:
      state = ZDSECMGR_CTRL_NONE;
      break;
  }

  if ( state != ZDSECMGR_CTRL_NONE )
  {
    device->ctrl->state = state;
    device->ctrl->cntr  = cntr;

    osal_start_timerEx(ZDAppTaskID, ZDO_SECMGR_EVENT, 100 );
  }
  else
  {
    ZDSecMgrCtrlRelease( device->ctrl );
  }
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceCtrlSetup
 *
 * @brief       Setup device control.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
void ZDSecMgrDeviceCtrlSetup( ZDSecMgrDevice_t* device )
{
  if ( device->ctrl != NULL )
  {
    if ( device->secure == FALSE )
    {
      // send the master key data to the joining device
      device->ctrl->state = ZDSECMGR_CTRL_TK_MASTER;
    }
    else
    {
      // start SKKE
      device->ctrl->state = ZDSECMGR_CTRL_SKKE_INIT;
    }

    ZDSecMgrDeviceCtrlHandler( device );
  }
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceCtrlUpdate
 *
 * @brief       Update control data.
 *
 * @param       extAddr - [in] EXT address
 * @param       state   - [in] new control state
 *
 * @return      none
 */
void ZDSecMgrDeviceCtrlUpdate( uint8* extAddr, uint8 state )
{
  ZDSecMgrEntry_t* entry;
  ZDSecMgrCtrl_t*  ctrl;

  // lookup device entry data
  (void)ZDSecMgrEntryLookupExt( extAddr, &entry );

  if ( entry != NULL )
  {
    // lookup device control data
    ZDSecMgrCtrlLookup( entry, &ctrl );

    // make sure control data is valid
    if ( ctrl != NULL )
    {
      // possible state transitions
      if ( ctrl->state == ZDSECMGR_CTRL_SKKE_WAIT )
      {
        if ( state == ZDSECMGR_CTRL_SKKE_DONE )
        {
          // send the network key
          ctrl->state = ZDSECMGR_CTRL_TK_NWK;
          ctrl->cntr  = ZDSECMGR_CTRL_TK_NWK_CNTR;
        }
        else if ( state == ZDSECMGR_CTRL_SKKE_FAIL )
        {
          // force default timeout in order to cleanup control logic
          ctrl->state = ZDSECMGR_CTRL_SKKE_FAIL;
          ctrl->cntr  = ZDSECMGR_CTRL_BASE_CNTR;
        }
      }
      // timer should be active
    }
  }
}

void APSME_SKA_TimerExpired( uint8 initiator, uint8* partExtAddr );
void APSME_SKA_TimerExpired( uint8 initiator, uint8* partExtAddr )
{
  if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
  {
    if ( initiator == TRUE )
    {
      ZDSecMgrDeviceCtrlUpdate( partExtAddr, ZDSECMGR_CTRL_SKKE_FAIL );
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceRemove
 *
 * @brief       Remove device from network.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      none
 */
void ZDSecMgrDeviceRemove( ZDSecMgrDevice_t* device )
{
  APSME_RemoveDeviceReq_t remDevReq;
  NLME_LeaveReq_t         leaveReq;
  associated_devices_t*   assoc;

  // check if parent, remove the device
  if ( device->parentAddr == NLME_GetShortAddr() )
  {
    // this is the parent of the device
    leaveReq.extAddr        = device->extAddr;
    leaveReq.removeChildren = FALSE;
    leaveReq.rejoin         = FALSE;

    // find child association
    assoc = AssocGetWithExt( device->extAddr );

    if ( ( assoc != NULL                            ) &&
         ( assoc->nodeRelation >= CHILD_RFD         ) &&
         ( assoc->nodeRelation <= CHILD_FFD_RX_IDLE )    )
    {
      // check if associated device is authenticated
      if ( assoc->devStatus & DEV_SEC_AUTH_STATUS )
      {
        leaveReq.silent = FALSE;
      }
      else
      {
        leaveReq.silent = TRUE;
      }

      NLME_LeaveReq( &leaveReq );
    }
  }
  else
  {
    // this is not the parent of the device
    remDevReq.parentAddr   = device->parentAddr;
    remDevReq.childExtAddr = device->extAddr;

    APSME_RemoveDeviceReq( &remDevReq );
  }
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceValidateSKKE
 *
 * @brief       Decide whether device is allowed for SKKE.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceValidateSKKE( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;
  uint16 ami;
  uint16 keyNvId;

  // get EXT address
  status = ZDSecMgrExtAddrLookup( device->extAddr, &ami );

  if ( status == ZSuccess )
  {
    // get MASTER key
    status = ZDSecMgrMasterKeyLookup( ami, &keyNvId );

    if ( status == ZSuccess )
    {
      status = ZDSecMgrDeviceEntryAdd( device, ami );
    }
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceValidateRM (RESIDENTIAL MODE)
 *
 * @brief       Decide whether device is allowed.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceValidateRM( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;

  status = ZSuccess;

  (void)device;  // Intentionally unreferenced parameter

  // For test purpose, turning off the zgSecurePermitJoin flag will force
  // the trust center to reject any newly joining devices by sending
  // Remove-device to the parents.
  if ( zgSecurePermitJoin == FALSE )
  {
    status = ZNwkUnknownDevice;
  }



#if 0  // Taken out because the following functionality is only used for test
       // purpose. A more efficient (above) way is used. It can be put
       // back in if customers request for a white/black list feature.
       // ZDSecMgrStoredDeviceList[] is defined in ZDSecMgr.c

  // The following code processes the device black list (stored device list)
  // If the joining device is not part of the forbidden device list
  // Return ZSuccess. Otherwise, return ZNwkUnknownDevice. The trust center
  // will send Remove-device and ban the device from joining.

  uint8     index;
  uint8*    restricted;

  // Look through the stored device list - used for restricted devices
  for ( index = 0; index < ZDSECMGR_STORED_DEVICES; index++ )
  {
    restricted = ZDSecMgrStoredDeviceList[index];

    if ( AddrMgrExtAddrEqual( restricted, device->extAddr )  == TRUE )
    {
      // return as unknown device in regards to validation
      status = ZNwkUnknownDevice;

      // break from loop
      index = ZDSECMGR_STORED_DEVICES;
    }
  }

#endif

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceValidateCM (COMMERCIAL MODE)
 *
 * @brief       Decide whether device is allowed.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
//devtag.pro.security
ZStatus_t ZDSecMgrDeviceValidateCM( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;
  uint16    ami;
  uint8     key[SEC_KEY_LEN];

  // implement EXT address and MASTER key policy here -- the total number of
  // Security Manager entries should never exceed the number of EXT addresses
  // and MASTER keys available

  // set status based on policy
  //status = ZNwkUnknownDevice;

  // set status based on policy
  status = ZSuccess; // ZNwkUnknownDevice;

  // get key based on policy
  osal_memcpy( key, ZDSecMgrTCMasterKey, SEC_KEY_LEN);

  // if policy, store new EXT address
  status = ZDSecMgrAddrStore( device->nwkAddr, device->extAddr, &ami );

  // set the key
  ZDSecMgrMasterKeyLoad( device->extAddr, key );

  // if EXT address and MASTER key available -- add device
  if ( status == ZSuccess )
  {
    // add device to internal data - with control
    status = ZDSecMgrDeviceEntryAdd( device, ami );
  }

  // remove copy of key in RAM
  osal_memset( key, 0x00, SEC_KEY_LEN );

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceValidate
 *
 * @brief       Decide whether device is allowed.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceValidate( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;


  if ( ZDSecMgrPermitJoiningEnabled == TRUE )
  {
    // device may be joining with a secure flag but it is ultimately the Trust
    // Center that decides -- check if expected pre configured device --
    // override settings
    if ( zgPreConfigKeys == TRUE )
    {
      device->secure = TRUE;
    }
    else
    {
      device->secure = FALSE;
    }

    if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
    {
      status = ZDSecMgrDeviceValidateCM( device );
    }
    else // ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_RESIDENTIAL )
    {
      status = ZDSecMgrDeviceValidateRM( device );
    }
  }
  else
  {
    status = ZNwkUnknownDevice;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceJoin
 *
 * @brief       Try to join this device.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceJoin( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;
  uint16    ami;

  // attempt to validate device
  status = ZDSecMgrDeviceValidate( device );

  if ( status == ZSuccess )
  {
    if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
    {
      ZDSecMgrDeviceCtrlSetup( device );
    }
    else // ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_RESIDENTIAL )
    {
      // Add the device to the address manager
      ZDSecMgrAddrStore( device->nwkAddr, device->extAddr, &ami );
      //send the nwk key data to the joining device
      status = ZDSecMgrSendNwkKey( device );
    }
  }
  else
  {
    // not allowed, remove the device
    ZDSecMgrDeviceRemove( device );
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceJoinDirect
 *
 * @brief       Try to join this device as a direct child.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceJoinDirect( ZDSecMgrDevice_t* device )
{
  ZStatus_t status;

  status = ZDSecMgrDeviceJoin( device );

  if ( status == ZSuccess )
  {
    // set association status to authenticated
    ZDSecMgrAssocDeviceAuth( AssocGetWithShort( device->nwkAddr ) );
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceJoinFwd
 *
 * @brief       Forward join to Trust Center.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceJoinFwd( ZDSecMgrDevice_t* device )
{
  ZStatus_t               status;
  APSME_UpdateDeviceReq_t req;

  // forward any joining device to the Trust Center -- the Trust Center will
  // decide if the device is allowed to join
  status = ZSuccess;

  // forward authorization to the Trust Center
  req.dstAddr    = APSME_TRUSTCENTER_NWKADDR;
  req.devAddr    = device->nwkAddr;
  req.devExtAddr = device->extAddr;

  // set security status, option for router to reject if policy set
  if ( (device->devStatus & DEV_HIGH_SEC_STATUS) )
  {
    if ( device->devStatus & DEV_REJOIN_STATUS )
    {
      if ( device->secure == TRUE )
      {
        req.status = APSME_UD_HIGH_SECURED_REJOIN;
      }
      else
      {
        req.status = APSME_UD_HIGH_UNSECURED_REJOIN;
      }
    }
    else
    {
      req.status = APSME_UD_HIGH_UNSECURED_JOIN;
    }
  }
  else
  {
    if ( device->devStatus & DEV_REJOIN_STATUS )
    {
      if ( device->secure == TRUE )
      {
        req.status = APSME_UD_STANDARD_SECURED_REJOIN;
      }
      else
      {
        req.status = APSME_UD_STANDARD_UNSECURED_REJOIN;
      }
    }
    else
    {
      req.status = APSME_UD_STANDARD_UNSECURED_JOIN;
    }
  }

  if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
  {
    req.apsSecure = TRUE;
  }
  else
  {
    req.apsSecure = FALSE;
  }

  // send and APSME_UPDATE_DEVICE request to the trust center
  status = APSME_UpdateDeviceReq( &req );

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrDeviceNew
 *
 * @brief       Process a new device.
 *
 * @param       device - [in] ZDSecMgrDevice_t, device info
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrDeviceNew( ZDSecMgrDevice_t* joiner )
{
  ZStatus_t status;

  if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
  {
    // try to join this device
    status = ZDSecMgrDeviceJoinDirect( joiner );
  }
  else
  {
    status = ZDSecMgrDeviceJoinFwd( joiner );
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrAssocDeviceAuth
 *
 * @brief       Set associated device status to authenticated
 *
 * @param       assoc - [in, out] associated_devices_t
 *
 * @return      none
 */
void ZDSecMgrAssocDeviceAuth( associated_devices_t* assoc )
{
  if ( assoc != NULL )
  {
    assoc->devStatus |= DEV_SEC_AUTH_STATUS;
  }
}

/******************************************************************************
 * @fn          ZDSecMgrAuthInitiate
 *
 * @brief       Initiate entity authentication
 *
 * @param       responder - [in] responder EXT address
 *
 * @return      none
 */
void ZDSecMgrAuthInitiate( uint8* responder )
{
  APSME_AuthenticateReq_t req;


  // make sure NWK address is available
  if ( APSME_LookupNwkAddr( responder, &req.nwkAddr ) )
  {
    // set request fields
    req.extAddr   = responder;
    req.action    = APSME_EA_INITIATE;
    req.challenge = NULL;

    // start EA processing
    APSME_AuthenticateReq( &req );
  }
}

/******************************************************************************
 * @fn          ZDSecMgrAuthNwkKey
 *
 * @brief       Handle next step in authentication process
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrAuthNwkKey()
{
  if ( devState == DEV_END_DEVICE_UNAUTH )
  {
    if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
    {
      uint8 parent[Z_EXTADDR_LEN];

      // get parent's EXT address
      NLME_GetCoordExtAddr( parent );

      // begin entity authentication with parent
      ZDSecMgrAuthInitiate( parent );
    }
    else
    {
      // inform ZDO that device has been authenticated
      osal_set_event ( ZDAppTaskID, ZDO_DEVICE_AUTH );
    }
  }
}

/******************************************************************************
 * PUBLIC FUNCTIONS
 */
/******************************************************************************
 * @fn          ZDSecMgrInit
 *
 * @brief       Initialize ZigBee Device Security Manager.
 *
 * @param       state - device initialization state
 *
 * @return      none
 */
#if ( ADDRMGR_CALLBACK_ENABLED == 1 )
void ZDSecMgrAddrMgrCB( uint8 update, AddrMgrEntry_t* newEntry, AddrMgrEntry_t* oldEntry );
void ZDSecMgrAddrMgrCB( uint8           update,
                        AddrMgrEntry_t* newEntry,
                        AddrMgrEntry_t* oldEntry )
{
  (void)update;
  (void)newEntry;
  (void)oldEntry;
}
#endif // ( ADDRMGR_CALLBACK_ENABLED == 1 )

void ZDSecMgrInit(uint8 state)
{
  if ( (ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH)
      || (ZG_CHECK_SECURITY_MODE == ZG_SECURITY_SE_STANDARD) )
  {
    // initialize sub modules
    ZDSecMgrEntryInit(state);

    if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
    {
      ZDSecMgrCtrlInit();
    }

    // register with Address Manager
#if ( ADDRMGR_CALLBACK_ENABLED == 1 )
    AddrMgrRegister( ADDRMGR_REG_SECURITY, ZDSecMgrAddrMgrCB );
#endif
  }

  if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
  {
    // configure SKA slot data
    APSME_SKA_SlotInit( ZDSECMGR_SKA_SLOT_MAX );
  }

  if ( ZG_SECURE_ENABLED )
  {
    if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
    {
      // setup joining permissions
      ZDSecMgrPermitJoiningEnabled = TRUE;
      ZDSecMgrPermitJoiningTimed   = FALSE;
    }
  }

  // configure security based on security mode and type of device
  ZDSecMgrConfig();
}

/******************************************************************************
 * @fn          ZDSecMgrConfig
 *
 * @brief       Configure ZigBee Device Security Manager.
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrConfig( void )
{
  if ( ZG_SECURE_ENABLED )
  {
    SSP_Init();

    if ( (ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH)
        || (ZG_CHECK_SECURITY_MODE == ZG_SECURITY_SE_STANDARD) )
    {
      if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
      {
        // COMMERCIAL MODE - COORDINATOR DEVICE
        APSME_SecurityCM_CD();
      }
      else if ( ZSTACK_ROUTER_BUILD )
      {
        // COMMERCIAL MODE - ROUTER DEVICE
        APSME_SecurityCM_RD();
      }
      else
      {
        // COMMERCIAL MODE - END DEVICE
        APSME_SecurityCM_ED();
      }
    }
    else // ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_RESIDENTIAL )
    {
      if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
      {
        // RESIDENTIAL MODE - COORDINATOR DEVICE
        APSME_SecurityRM_CD();
      }
      else if ( ZSTACK_ROUTER_BUILD )
      {
        // RESIDENTIAL MODE - ROUTER DEVICE
        APSME_SecurityRM_RD();
      }
      else
      {
        // RESIDENTIAL MODE - END DEVICE
        APSME_SecurityRM_ED();
      }
    }
  }
  else
  {
    // NO SECURITY
    APSME_SecurityNM();
  }
}

/******************************************************************************
 * @fn          ZDSecMgrPermitJoining
 *
 * @brief       Process request to change joining permissions.
 *
 * @param       duration - [in] timed duration for join in seconds
 *                         - 0x00 not allowed
 *                         - 0xFF allowed without timeout
 *
 * @return      uint8 - success(TRUE:FALSE)
 */
uint8 ZDSecMgrPermitJoining( uint8 duration )
{
  uint8 accept;

  ZDSecMgrPermitJoiningTimed = FALSE;

  if ( duration > 0 )
  {
    ZDSecMgrPermitJoiningEnabled = TRUE;

    if ( duration != 0xFF )
    {
      ZDSecMgrPermitJoiningTimed = TRUE;
    }
  }
  else
  {
    ZDSecMgrPermitJoiningEnabled = FALSE;
  }

  accept = TRUE;

  return accept;
}

/******************************************************************************
 * @fn          ZDSecMgrPermitJoiningTimeout
 *
 * @brief       Process permit joining timeout
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrPermitJoiningTimeout( void )
{
  if ( ZDSecMgrPermitJoiningTimed == TRUE )
  {
    ZDSecMgrPermitJoiningEnabled = FALSE;
    ZDSecMgrPermitJoiningTimed   = FALSE;
  }
}

/******************************************************************************
 * @fn          ZDSecMgrNewDeviceEvent
 *
 * @brief       Process a the new device event, if found reset new device
 *              event/timer.
 *
 * @param       none
 *
 * @return      uint8 - found(TRUE:FALSE)
 */
uint8 ZDSecMgrNewDeviceEvent( void )
{
  uint8                 found;
  ZDSecMgrDevice_t      device;
  AddrMgrEntry_t        addrEntry;
  associated_devices_t* assoc;
  ZStatus_t             status;

  // initialize return results
  found = FALSE;

  // look for device in the security init state
  assoc = AssocMatchDeviceStatus( DEV_SEC_INIT_STATUS );

  if ( assoc != NULL )
  {
    // device found
    found = TRUE;

    // check for preconfigured security
    if ( zgPreConfigKeys == TRUE )
    {
      // set association status to authenticated
      ZDSecMgrAssocDeviceAuth( assoc );
    }

    // set up device info
    addrEntry.user  = ADDRMGR_USER_DEFAULT;
    addrEntry.index = assoc->addrIdx;
    AddrMgrEntryGet( &addrEntry );

    device.nwkAddr    = assoc->shortAddr;
    device.extAddr    = addrEntry.extAddr;
    device.parentAddr = NLME_GetShortAddr();
    device.secure     = FALSE;
    device.devStatus  = assoc->devStatus;

    // process new device
    status = ZDSecMgrDeviceNew( &device );

    if ( status == ZSuccess )
    {
      assoc->devStatus &= ~DEV_SEC_INIT_STATUS;
    }
    else if ( status == ZNwkUnknownDevice )
    {
      AssocRemove( addrEntry.extAddr );
    }
  }

  return found;
}

/******************************************************************************
 * @fn          ZDSecMgrEvent
 *
 * @brief       Handle ZDO Security Manager event/timer(ZDO_SECMGR_EVENT).
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrEvent( void )
{
  uint8            action;
  uint8            restart;
  uint16           index;
  AddrMgrEntry_t   entry;
  ZDSecMgrDevice_t device;

  // verify data is available
  if ( ZDSecMgrCtrlData != NULL )
  {
    action  = FALSE;
    restart = FALSE;

    // update all the counters
    for ( index = 0; index < ZDSECMGR_ENTRY_MAX; index++ )
    {
      if ( ZDSecMgrCtrlData[index].state !=  ZDSECMGR_CTRL_NONE )
      {
        if ( ZDSecMgrCtrlData[index].cntr != 0 )
        {
          ZDSecMgrCtrlData[index].cntr--;
        }

        if ( ( action == FALSE ) && ( ZDSecMgrCtrlData[index].cntr == 0 ) )
        {
          action = TRUE;

          // update from control data
          device.parentAddr = ZDSecMgrCtrlData[index].parentAddr;
          device.secure     = ZDSecMgrCtrlData[index].secure;
          device.ctrl       = &ZDSecMgrCtrlData[index];

          // set the user and address index
          entry.user  = ADDRMGR_USER_SECURITY;
          entry.index = ZDSecMgrCtrlData[index].entry->ami;

          // get the address data
          AddrMgrEntryGet( &entry );

          // set device address data
          device.nwkAddr = entry.nwkAddr;
          device.extAddr = entry.extAddr;

          // update from entry data
          ZDSecMgrDeviceCtrlHandler( &device );
        }
        else
        {
          restart = TRUE;
        }
      }
    }

    // check for timer restart
    if ( restart == TRUE )
    {
      osal_start_timerEx(ZDAppTaskID, ZDO_SECMGR_EVENT, 100 );
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrEstablishKeyCfm
 *
 * @brief       Process the ZDO_EstablishKeyCfm_t message.
 *
 * @param       cfm - [in] ZDO_EstablishKeyCfm_t confirmation
 *
 * @return      none
 */
void ZDSecMgrEstablishKeyCfm( ZDO_EstablishKeyCfm_t* cfm )
{
  // send the NWK key
  if ( ( ZG_BUILD_COORDINATOR_TYPE ) && ( ZG_DEVICE_COORDINATOR_TYPE ) )
  {
    // update control for specified EXT address
    ZDSecMgrDeviceCtrlUpdate( cfm->partExtAddr, ZDSECMGR_CTRL_SKKE_DONE );
  }
  else
  {
    // this should be done when receiving the NWK key
    // if devState ==
    //if ( devState == DEV_END_DEVICE_UNAUTH )
        //osal_set_event( ZDAppTaskID, ZDO_DEVICE_AUTH );

    // if not in joining state -- this should trigger an event for an
    // end point that requested SKKE
    // if ( devState == DEV_END_DEVICE )
   //       devState == DEV_ROUTER;

  }
}

uint8 ZDSecMgrTCExtAddrCheck( uint8* extAddr );
uint8 ZDSecMgrTCExtAddrCheck( uint8* extAddr )
{
  uint8  match;
  uint8  lookup[Z_EXTADDR_LEN];

  match = FALSE;

  if ( AddrMgrExtAddrLookup( APSME_TRUSTCENTER_NWKADDR, lookup ) )
  {
    match = AddrMgrExtAddrEqual( lookup, extAddr );
  }

  return match;
}

void ZDSecMgrTCDataLoad( uint8* extAddr );
void ZDSecMgrTCDataLoad( uint8* extAddr )
{
  uint16 ami;
  uint16 keyNvId;
  uint8 masterKey[SEC_KEY_LEN];

  if ( !ZDSecMgrTCDataLoaded )
  {
    if ( ZDSecMgrAddrStore( APSME_TRUSTCENTER_NWKADDR, extAddr, &ami ) == ZSuccess )
    {
      // if preconfigured load key
      if ( zgPreConfigKeys == TRUE )
      {
        if ( ZDSecMgrMasterKeyLookup( ami, &keyNvId ) != ZSuccess )
        {
          // temporary copy
          osal_memcpy( masterKey, ZDSecMgrTCMasterKey, SEC_KEY_LEN);

          ZDSecMgrMasterKeyStore( ami, masterKey );

          // remove copy of key in RAM
          osal_memset( masterKey, 0x00, SEC_KEY_LEN );
        }
      }
    }

    ZDSecMgrTCDataLoaded = TRUE;
  }
}

/******************************************************************************
 * @fn          ZDSecMgrEstablishKeyInd
 *
 * @brief       Process the ZDO_EstablishKeyInd_t message.
 *
 * @param       ind - [in] ZDO_EstablishKeyInd_t indication
 *
 * @return      none
 */
void ZDSecMgrEstablishKeyInd( ZDO_EstablishKeyInd_t* ind )
{
  ZDSecMgrDevice_t        device;
  APSME_EstablishKeyRsp_t rsp;


  // load Trust Center data if needed
  ZDSecMgrTCDataLoad( ind->initExtAddr );

  if ( ZDSecMgrTCExtAddrCheck( ind->initExtAddr ) )
  {
    //IF (ind->srcAddr == APSME_TRUSTCENTER_NWKADDR)
    //OR
    //!ZDSecMgrTCAuthenticated
    //devtag.0604.critical
        //how is the parentAddr used here

    // initial SKKE from Trust Center via parent
    device.nwkAddr    = APSME_TRUSTCENTER_NWKADDR;
    device.parentAddr = ind->srcAddr;
  }
  else
  {
    // Trust Center direct or E2E SKKE
    device.nwkAddr    = ind->srcAddr;
    device.parentAddr = INVALID_NODE_ADDR;
  }

  device.extAddr = ind->initExtAddr;
  //devtag.pro.security.0724.todo - verify usage
  device.secure  = ind->nwkSecure;

  // validate device for SKKE
  if ( ZDSecMgrDeviceValidateSKKE( &device ) == ZSuccess )
  {
    rsp.accept = TRUE;
  }
  else
  {
    rsp.accept = FALSE;
  }

  rsp.dstAddr     = ind->srcAddr;
  rsp.initExtAddr = &ind->initExtAddr[0];
  //devtag.0604.todo - remove obsolete
  rsp.apsSecure   = ind->apsSecure;
  rsp.nwkSecure   = ind->nwkSecure;

  APSME_EstablishKeyRsp( &rsp );
}

//devtag.pro.security
#if 0
void ZDSecMgrEstablishKeyInd( ZDO_EstablishKeyInd_t* ind )
{
  ZDSecMgrDevice_t        device;
  APSME_EstablishKeyRsp_t rsp;


  device.extAddr = ind->initExtAddr;
  device.secure  = ind->secure;

  if ( ind->secure == FALSE )
  {
    // SKKE from Trust Center is not secured between child and parent
    device.nwkAddr    = APSME_TRUSTCENTER_NWKADDR;
    device.parentAddr = ind->srcAddr;
  }
  else
  {
    // SKKE from initiator should be secured
    device.nwkAddr    = ind->srcAddr;
    device.parentAddr = INVALID_NODE_ADDR;
  }

  rsp.dstAddr     = ind->srcAddr;
  rsp.initExtAddr = &ind->initExtAddr[0];
  rsp.secure      = ind->secure;

  // validate device for SKKE
  if ( ZDSecMgrDeviceValidateSKKE( &device ) == ZSuccess )
  {
    rsp.accept = TRUE;
  }
  else
  {
    rsp.accept = FALSE;
  }

  APSME_EstablishKeyRsp( &rsp );
}
#endif

/******************************************************************************
 * @fn          ZDSecMgrTransportKeyInd
 *
 * @brief       Process the ZDO_TransportKeyInd_t message.
 *
 * @param       ind - [in] ZDO_TransportKeyInd_t indication
 *
 * @return      none
 */
void ZDSecMgrTransportKeyInd( ZDO_TransportKeyInd_t* ind )
{
  uint8 index;
  uint8 zgPreConfigKey[SEC_KEY_LEN];

  // load Trust Center data if needed
  ZDSecMgrTCDataLoad( ind->srcExtAddr );

  if ( ind->keyType == KEY_TYPE_TC_MASTER )
  {
    if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
    //ZDSecMgrTCMasterKey( ind );
    {
      if ( zgPreConfigKeys != TRUE )
      {
        // devtag.pro.security.todo - check if Trust Center address is configured and correct
        ZDSecMgrMasterKeyLoad( ind->srcExtAddr, ind->key );
      }
      else
      {
        // error condition - reject key
      }
    }
  }
  else if ( ( ind->keyType == KEY_TYPE_NWK      ) ||
            ( ind->keyType == 6                 ) ||
            ( ind->keyType == KEY_TYPE_NWK_HIGH )    )
  {
    // check for dummy NWK key (all zeros)
    for ( index = 0;
          ( (index < SEC_KEY_LEN) && (ind->key[index] == 0) );
          index++ );

    if ( index == SEC_KEY_LEN )
    {
      // load preconfigured key - once!!
      if ( !_NIB.nwkKeyLoaded )
      {
        ZDSecMgrReadKeyFromNv(ZCD_NV_PRECFGKEY, zgPreConfigKey);
        SSP_UpdateNwkKey( zgPreConfigKey, 0 );
        SSP_SwitchNwkKey( 0 );

        // clear local copy of key
        osal_memset(zgPreConfigKey, 0x00, SEC_KEY_LEN);
      }
    }
    else
    {
      SSP_UpdateNwkKey( ind->key, ind->keySeqNum );
      if ( !_NIB.nwkKeyLoaded )
      {
        SSP_SwitchNwkKey( ind->keySeqNum );
      }
    }

    // handle next step in authentication process
    ZDSecMgrAuthNwkKey();
  }
  else if ( ind->keyType == KEY_TYPE_TC_LINK )
  {
    if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
    {
      // This should not happen because TCLK should not be Tx
    }
  }
  else if ( ind->keyType == KEY_TYPE_APP_MASTER )
  {
    if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
    {
      uint16           ami;
      AddrMgrEntry_t   entry;
      ZDSecMgrEntry_t* entryZD;

      ZDSecMgrExtAddrLookup( ind->srcExtAddr, &ami );

      if ( ind->initiator == TRUE )
      {
        // get the ami data
        entry.user  = ADDRMGR_USER_SECURITY;
        entry.index = ami;
        AddrMgrEntryGet( &entry );

        if ( entry.nwkAddr != INVALID_NODE_ADDR )
        {
          APSME_EstablishKeyReq_t req;
          ZDSecMgrMasterKeyLoad( ind->srcExtAddr, ind->key );

          ZDSecMgrEntryLookupAMI( ami, &entryZD );

          if ( entryZD == NULL )
          {
            // get new entry
            if ( ZDSecMgrEntryNew( &entryZD ) == ZSuccess )
            {
              // finish setting up entry
              entryZD->ami = ami;
            }
          }

          req.respExtAddr = ind->srcExtAddr;
          req.method      = APSME_SKKE_METHOD;
          req.dstAddr     = entry.nwkAddr;
          req.apsSecure   = FALSE;
          req.nwkSecure   = TRUE;
          APSME_EstablishKeyReq( &req );
        }
      }
      else
      {
        if ( ami == INVALID_NODE_ADDR )
        {
          // store new EXT address
          ZDSecMgrAddrStore( INVALID_NODE_ADDR, ind->srcExtAddr, &ami );
        }

        ZDSecMgrMasterKeyLoad( ind->srcExtAddr, ind->key );
      }
    }
  }
  else if ( ind->keyType == KEY_TYPE_APP_LINK )
  {
    if ( ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH ) ||
         ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_SE_STANDARD ) )
    {
      uint16           ami;
      ZDSecMgrEntry_t* entry;

      // get the address index
      if ( ZDSecMgrExtAddrLookup( ind->srcExtAddr, &ami ) != ZSuccess )
      {
        // store new EXT address
        ZDSecMgrAddrStore( INVALID_NODE_ADDR, ind->srcExtAddr, &ami );
        ZDP_NwkAddrReq( ind->srcExtAddr, ZDP_ADDR_REQTYPE_SINGLE, 0, 0 );
      }

      ZDSecMgrEntryLookupAMI( ami, &entry );

      if ( entry == NULL )
      {
        // get new entry
        if ( ZDSecMgrEntryNew( &entry ) == ZSuccess )
        {
          // finish setting up entry
          entry->ami = ami;
        }
      }

      ZDSecMgrLinkKeySet( ind->srcExtAddr, ind->key );

#if defined NV_RESTORE
      ZDSecMgrWriteNV();  // Write the control record for the new established link key to NV.
#endif
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrUpdateDeviceInd
 *
 * @brief       Process the ZDO_UpdateDeviceInd_t message.
 *
 * @param       ind - [in] ZDO_UpdateDeviceInd_t indication
 *
 * @return      none
 */
void ZDSecMgrUpdateDeviceInd( ZDO_UpdateDeviceInd_t* ind )
{
  ZDSecMgrDevice_t device;

  device.nwkAddr    = ind->devAddr;
  device.extAddr    = ind->devExtAddr;
  device.parentAddr = ind->srcAddr;

  //if ( ( ind->status == APSME_UD_SECURED_JOIN   ) ||
  //     ( ind->status == APSME_UD_UNSECURED_JOIN )   )
  //{
  //  if ( ind->status == APSME_UD_SECURED_JOIN )
  //  {
  //    device.secure = TRUE;
  //  }
  //  else
  //  {
  //    device.secure = FALSE;
  //  }

    // try to join this device
    ZDSecMgrDeviceJoin( &device );
  //}
}

/******************************************************************************
 * @fn          ZDSecMgrRemoveDeviceInd
 *
 * @brief       Process the ZDO_RemoveDeviceInd_t message.
 *
 * @param       ind - [in] ZDO_RemoveDeviceInd_t indication
 *
 * @return      none
 */
void ZDSecMgrRemoveDeviceInd( ZDO_RemoveDeviceInd_t* ind )
{
  ZDSecMgrDevice_t device;

  // only accept from Trust Center
  if ( ind->srcAddr == APSME_TRUSTCENTER_NWKADDR )
  {
    // look up NWK address
    if ( APSME_LookupNwkAddr( ind->childExtAddr, &device.nwkAddr ) == TRUE )
    {
      device.parentAddr = NLME_GetShortAddr();
      device.extAddr    = ind->childExtAddr;

      // remove device
      ZDSecMgrDeviceRemove( &device );
    }
  }
}

/******************************************************************************
 * @fn          ZDSecMgrRequestKeyInd
 *
 * @brief       Process the ZDO_RequestKeyInd_t message.
 *
 * @param       ind - [in] ZDO_RequestKeyInd_t indication
 *
 * @return      none
 */
void ZDSecMgrRequestKeyInd( ZDO_RequestKeyInd_t* ind )
{
  if ( ind->keyType == KEY_TYPE_NWK )
  {
  }
  else if ( ind->keyType == KEY_TYPE_APP_MASTER )
  {
    ZDSecMgrAppKeyReq( ind );
  }
  else if ( ind->keyType == KEY_TYPE_TC_LINK )
  {
  }
  //else ignore
}

/******************************************************************************
 * @fn          ZDSecMgrSwitchKeyInd
 *
 * @brief       Process the ZDO_SwitchKeyInd_t message.
 *
 * @param       ind - [in] ZDO_SwitchKeyInd_t indication
 *
 * @return      none
 */
void ZDSecMgrSwitchKeyInd( ZDO_SwitchKeyInd_t* ind )
{
  SSP_SwitchNwkKey( ind->keySeqNum );

  // Save if nv
  ZDApp_NVUpdate();
}

/******************************************************************************
 * @fn          ZDSecMgrAuthenticateInd
 *
 * @brief       Process the ZDO_AuthenticateInd_t message.
 *
 * @param       ind - [in] ZDO_AuthenticateInd_t indication
 *
 * @return      none
 */
void ZDSecMgrAuthenticateInd( ZDO_AuthenticateInd_t* ind )
{
  APSME_AuthenticateReq_t req;
  AddrMgrEntry_t          entry;

  // update the address manager
  //---------------------------------------------------------------------------
  // note:
  // required for EA processing, but ultimately EA logic could also use the
  // neighbor table to look up addresses -- also(IF using EA) the neighbor
  // table is supposed to have authentication states for neighbors
  //---------------------------------------------------------------------------
  entry.user    = ADDRMGR_USER_SECURITY;
  entry.nwkAddr = ind->aps.initNwkAddr;
  AddrMgrExtAddrSet( entry.extAddr, ind->aps.initExtAddr );

  if ( AddrMgrEntryUpdate( &entry ) == TRUE )
  {
    // set request fields
    req.nwkAddr   = ind->aps.initNwkAddr;
    req.extAddr   = ind->aps.initExtAddr;
    req.action    = APSME_EA_ACCEPT;
    req.challenge = ind->aps.challenge;

    // start EA processing
    APSME_AuthenticateReq( &req );
  }
}

/******************************************************************************
 * @fn          ZDSecMgrAuthenticateCfm
 *
 * @brief       Process the ZDO_AuthenticateCfm_t message.
 *
 * @param       cfm - [in] ZDO_AuthenticateCfm_t confirmation
 *
 * @return      none
 */
void ZDSecMgrAuthenticateCfm( ZDO_AuthenticateCfm_t* cfm )
{
  if ( cfm->aps.status == ZSuccess )
  {
    if ( ( cfm->aps.initiator == TRUE ) && ( devState == DEV_END_DEVICE_UNAUTH ) )
    {
      // inform ZDO that device has been authenticated
      osal_set_event ( ZDAppTaskID, ZDO_DEVICE_AUTH );
    }
  }
}

#if ( ZG_BUILD_COORDINATOR_TYPE )
/******************************************************************************
 * @fn          ZDSecMgrUpdateNwkKey
 *
 * @brief       Load a new NWK key and trigger a network wide update.
 *
 * @param       key       - [in] new NWK key
 * @param       keySeqNum - [in] new NWK key sequence number
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrUpdateNwkKey( uint8* key, uint8 keySeqNum, uint16 dstAddr )
{
  ZStatus_t               status;
  APSME_TransportKeyReq_t req;

  // initialize common elements of local variables
  if ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH )
  {
    req.keyType   = KEY_TYPE_NWK_HIGH;
  }
  else
  {
    req.keyType   = KEY_TYPE_NWK;
  }

  req.dstAddr   = dstAddr;
  req.keySeqNum = keySeqNum;
  req.key       = key;
  req.extAddr   = NULL;
  req.nwkSecure = TRUE;
  req.apsSecure = TRUE;
  req.tunnel    = NULL;

  if (( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH ) ||
      ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_SE_STANDARD ))
  {
    ZDSecMgrEntry_t*        entry;
    uint16                  index;
    AddrMgrEntry_t          addrEntry;

    addrEntry.user = ADDRMGR_USER_SECURITY;

    status = ZFailure;

    // verify data is available
    if ( ZDSecMgrEntries != NULL )
    {
      // find available entry
      for ( index = 0; index < ZDSECMGR_ENTRY_MAX ; index++ )
      {
        if ( ZDSecMgrEntries[index].ami != INVALID_NODE_ADDR )
        {
          // return successful result
          entry = &ZDSecMgrEntries[index];

          // get NWK address
          addrEntry.index = entry->ami;
          if ( AddrMgrEntryGet( &addrEntry ) == TRUE )
          {
            req.dstAddr = addrEntry.nwkAddr;
            req.extAddr = addrEntry.extAddr;
            status = APSME_TransportKeyReq( &req );
          }
        }
      }
    }
  }
  else // ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_RESIDENTIAL )
  {
    status = APSME_TransportKeyReq( &req );
  }

  SSP_UpdateNwkKey( key, keySeqNum );

  // Save if nv
  ZDApp_NVUpdate();

  return status;
}
#endif // ( ZG_BUILD_COORDINATOR_TYPE )

#if ( ZG_BUILD_COORDINATOR_TYPE )
/******************************************************************************
 * @fn          ZDSecMgrSwitchNwkKey
 *
 * @brief       Causes the NWK key to switch via a network wide command.
 *
 * @param       keySeqNum - [in] new NWK key sequence number
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrSwitchNwkKey( uint8 keySeqNum, uint16 dstAddr )
{
  ZStatus_t            status;
  APSME_SwitchKeyReq_t req;

  // initialize common elements of local variables
  req.dstAddr = dstAddr;
  req.keySeqNum = keySeqNum;

  if (( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_PRO_HIGH ) ||
      ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_SE_STANDARD ))
  {
    ZDSecMgrEntry_t*     entry;
    uint16               index;
    AddrMgrEntry_t       addrEntry;

    addrEntry.user = ADDRMGR_USER_SECURITY;

    status = ZFailure;

    // verify data is available
    if ( ZDSecMgrEntries != NULL )
    {
      // find available entry
      for ( index = 0; index < ZDSECMGR_ENTRY_MAX ; index++ )
      {
        if ( ZDSecMgrEntries[index].ami != INVALID_NODE_ADDR )
        {
          // return successful result
          entry = &ZDSecMgrEntries[index];

          // get NWK address
          addrEntry.index = entry->ami;

          if ( AddrMgrEntryGet( &addrEntry ) == TRUE )
          {
            req.dstAddr = addrEntry.nwkAddr;
            status = APSME_SwitchKeyReq( &req );
          }
        }
      }
    }
  }
  else // ( ZG_CHECK_SECURITY_MODE == ZG_SECURITY_RESIDENTIAL )
  {
    status = APSME_SwitchKeyReq( &req );
  }

  SSP_SwitchNwkKey( keySeqNum );

  // Save if nv
  ZDApp_NVUpdate();

  return status;
}
#endif // ( ZG_BUILD_COORDINATOR_TYPE )

/******************************************************************************
 * @fn          ZDSecMgrRequestAppKey
 *
 * @brief       Request an application key with partner.
 *
 * @param       partExtAddr - [in] partner extended address
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrRequestAppKey( uint8 *partExtAddr )
{
  ZStatus_t status;
  APSME_RequestKeyReq_t req;

  req.dstAddr = 0;
  req.keyType = KEY_TYPE_APP_MASTER;

  req.partExtAddr = partExtAddr;
  status = APSME_RequestKeyReq( &req );

  return status;
}

#if ( ZG_BUILD_JOINING_TYPE )
/******************************************************************************
 * @fn          ZDSecMgrSetupPartner
 *
 * @brief       Setup for application key partner.
 *
 * @param       partNwkAddr - [in] partner network address
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrSetupPartner( uint16 partNwkAddr, uint8* partExtAddr )
{
  AddrMgrEntry_t entry;
  ZStatus_t      status;

  status = ZFailure;

  // update the address manager
  entry.user    = ADDRMGR_USER_SECURITY;
  entry.nwkAddr = partNwkAddr;
  AddrMgrExtAddrSet( entry.extAddr, partExtAddr );

  if ( AddrMgrEntryUpdate( &entry ) == TRUE )
  {
    status = ZSuccess;

    // check for address discovery
    if ( partNwkAddr == INVALID_NODE_ADDR )
    {
      status = ZDP_NwkAddrReq( partExtAddr, ZDP_ADDR_REQTYPE_SINGLE, 0, 0 );
    }
    else if ( !AddrMgrExtAddrValid( partExtAddr ) )
    {
      status = ZDP_IEEEAddrReq( partNwkAddr, ZDP_ADDR_REQTYPE_SINGLE, 0, 0 );
    }
  }

  return status;
}
#endif // ( ZG_BUILD_JOINING_TYPE )

#if ( ZG_BUILD_COORDINATOR_TYPE )
/******************************************************************************
 * @fn          ZDSecMgrAppKeyTypeSet
 *
 * @brief       Set application key type.
 *
 * @param       keyType - [in] application key type (KEY_TYPE_APP_MASTER@2 or
 *                                                   KEY_TYPE_APP_LINK@3
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrAppKeyTypeSet( uint8 keyType )
{
  if ( keyType == KEY_TYPE_APP_LINK )
  {
    ZDSecMgrAppKeyType = KEY_TYPE_APP_LINK;
  }
  else
  {
    ZDSecMgrAppKeyType = KEY_TYPE_APP_MASTER;
  }

  return ZSuccess;
}
#endif

/******************************************************************************
 * ZigBee Device Security Manager - Stub Implementations
 */
/******************************************************************************
 * @fn          ZDSecMgrMasterKeyGet (stubs APSME_MasterKeyGet)
 *
 * @brief       Get MASTER key for specified EXT address.
 *
 * @param       extAddr - [in] EXT address
 * @param       pKeyNvId - [out] MASTER key NV ID
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrMasterKeyGet( uint8* extAddr, uint16* pKeyNvId )
{
  ZStatus_t status;
  uint16 ami;

  // lookup entry for specified EXT address
  status = ZDSecMgrExtAddrLookup( extAddr, &ami );

  if ( status == ZSuccess )
  {
    ZDSecMgrMasterKeyLookup( ami, pKeyNvId );
  }
  else
  {
    *pKeyNvId = SEC_NO_KEY_NV_ID;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrLinkKeySet (stubs APSME_LinkKeySet)
 *
 * @brief       Set <APSME_LinkKeyData_t> for specified NWK address.
 *
 * @param       extAddr - [in] EXT address
 * @param       data    - [in] APSME_LinkKeyData_t
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrLinkKeySet( uint8* extAddr, uint8* key )
{
  ZStatus_t status;
  ZDSecMgrEntry_t* entry;
  APSME_LinkKeyData_t *pApsLinkKey = NULL;
  uint16 Index;

  // lookup entry index for specified EXT address
  status = ZDSecMgrEntryLookupExtGetIndex( extAddr, &entry, &Index );

  if ( status == ZSuccess )
  {
    // point to NV item
    entry->keyNvId = ZCD_NV_APS_LINK_KEY_DATA_START + Index;

    pApsLinkKey = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

    if (pApsLinkKey != NULL)
    {
      // read the key form NV, keyNvId must be ZCD_NV_APS_LINK_KEY_DATA_START based
      osal_nv_read( entry->keyNvId, 0,
                   sizeof(APSME_LinkKeyData_t), pApsLinkKey );

      // set new values of the key
      osal_memcpy( pApsLinkKey->key, key, SEC_KEY_LEN );
      pApsLinkKey->rxFrmCntr = 0;
      pApsLinkKey->txFrmCntr = 0;

      osal_nv_write( entry->keyNvId, 0,
                    sizeof(APSME_LinkKeyData_t), pApsLinkKey );

      // clear copy of key in RAM
      osal_memset(pApsLinkKey, 0x00, sizeof(APSME_LinkKeyData_t));

      osal_mem_free(pApsLinkKey);

      // set initial values for counters in RAM
      ApsLinkKeyFrmCntr[entry->keyNvId - ZCD_NV_APS_LINK_KEY_DATA_START].txFrmCntr = 0;
      ApsLinkKeyFrmCntr[entry->keyNvId - ZCD_NV_APS_LINK_KEY_DATA_START].rxFrmCntr = 0;
    }
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrAuthenticationSet
 *
 * @brief       Mark the specific device as authenticated or not
 *
 * @param       extAddr - [in] EXT address
 * @param       option  - [in] authenticated or not
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrAuthenticationSet( uint8* extAddr, ZDSecMgr_Authentication_Option option )
{
  ZStatus_t        status;
  ZDSecMgrEntry_t* entry;


  // lookup entry index for specified EXT address
  status = ZDSecMgrEntryLookupExt( extAddr, &entry );

  if ( status == ZSuccess )
  {
    entry->authenticateOption = option;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrAuthenticationCheck
 *
 * @brief       Check if the specific device has been authenticated or not
 *              For non-trust center device, always return TRUE
 *
 * @param       shortAddr - [in] short address
 *
 * @return      TRUE @ authenticated with CBKE
 *              FALSE @ not authenticated
 */

uint8 ZDSecMgrAuthenticationCheck( uint16 shortAddr )
{
#if defined (TC_LINKKEY_JOIN)

  ZDSecMgrEntry_t* entry;
  uint8 extAddr[Z_EXTADDR_LEN];

  // If the local device is not the trust center, always return TRUE
  if ( NLME_GetShortAddr() != zgTrustCenterAddr )
  {
    return TRUE;
  }
  // Otherwise, check the authentication option
  else if ( AddrMgrExtAddrLookup( shortAddr, extAddr ) )
  {
    // lookup entry index for specified EXT address
    if ( ZDSecMgrEntryLookupExt( extAddr, &entry ) == ZSuccess )
    {
      if ( entry->authenticateOption != ZDSecMgr_Not_Authenticated )
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }
  return FALSE;

#else
  (void)shortAddr;  // Intentionally unreferenced parameter

  // For non AMI/SE Profile, perform no check and always return TRUE.
  return TRUE;

#endif // TC_LINKKEY_JOIN
}

/******************************************************************************
 * @fn          ZDSecMgrLinkKeyNVIdGet (stubs APSME_LinkKeyNVIdGet)
 *
 * @brief       Get Key NV ID for specified NWK address.
 *
 * @param       extAddr - [in] EXT address
 * @param       keyNvId - [out] NV ID
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrLinkKeyNVIdGet(uint8* extAddr, uint16 *pKeyNvId)
{
  ZStatus_t status;
  ZDSecMgrEntry_t* entry;

  // lookup entry index for specified NWK address
  status = ZDSecMgrEntryLookupExt( extAddr, &entry );

  if ( status == ZSuccess )
  {
    // return the index to the NV table
    *pKeyNvId = entry->keyNvId;
  }
  else
  {
    *pKeyNvId = SEC_NO_KEY_NV_ID;
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrIsLinkKeyValid (stubs APSME_IsLinkKeyValid)
 *
 * @brief       Verifies if Link Key in NV has been set.
 *
 * @param       extAddr - [in] EXT address
 *
 * @return      TRUE - Link Key has been established
 *              FALSE - Link Key in NV has default value.
 */
uint8 ZDSecMgrIsLinkKeyValid(uint8* extAddr)
{
  APSME_LinkKeyData_t *pKeyData = NULL;
  uint16 apsLinkKeyNvId;
  uint8 nullKey[SEC_KEY_LEN];
  uint8 status = FALSE;

  // initialize default vealue to compare to
  osal_memset(nullKey, 0x00, SEC_KEY_LEN);

  // check for APS link NV ID
  APSME_LinkKeyNVIdGet( extAddr, &apsLinkKeyNvId );

  if (apsLinkKeyNvId != SEC_NO_KEY_NV_ID )
  {
    pKeyData = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

    if (pKeyData != NULL)
    {
      // retrieve key from NV
      if ( osal_nv_read( apsLinkKeyNvId, 0,
                        sizeof(APSME_LinkKeyData_t), pKeyData) == ZSUCCESS)
      {
        // if stored key is different than default value, then a key has been established
        if (!osal_memcmp(pKeyData, nullKey, SEC_KEY_LEN))
        {
          status = TRUE;
        }
      }
    }
  }

  return status;
}

/******************************************************************************
 * @fn          ZDSecMgrKeyFwdToChild (stubs APSME_KeyFwdToChild)
 *
 * @brief       Verify and process key transportation to child.
 *
 * @param       ind - [in] APSME_TransportKeyInd_t
 *
 * @return      uint8 - success(TRUE:FALSE)
 */
uint8 ZDSecMgrKeyFwdToChild( APSME_TransportKeyInd_t* ind )
{
  // verify from Trust Center
  if ( ind->srcAddr == APSME_TRUSTCENTER_NWKADDR )
  {
    // check for initial NWK key
    if ( ( ind->keyType == KEY_TYPE_NWK      ) ||
         ( ind->keyType == 6                 ) ||
         ( ind->keyType == KEY_TYPE_NWK_HIGH )    )
    {
      // set association status to authenticated
      ZDSecMgrAssocDeviceAuth( AssocGetWithExt( ind->dstExtAddr ) );
    }

    return TRUE;
  }

  return FALSE;
}

/******************************************************************************
 * @fn          ZDSecMgrAddLinkKey
 *
 * @brief       Add the application link key to ZDSecMgr. Also mark the device
 *              as authenticated in the authenticateOption. Note that this function
 *              is hardwared to CBKE right now.
 *
 * @param       shortAddr - short address of the partner device
 * @param       extAddr - extended address of the partner device
 * @param       key - link key
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrAddLinkKey( uint16 shortAddr, uint8 *extAddr, uint8 *key)
{
  uint16           ami;
  ZDSecMgrEntry_t* entry;

  /* Store the device address in the addr manager */
  if( ZDSecMgrAddrStore( shortAddr, extAddr, &ami ) != ZSuccess )
  {
    /* Adding to Addr Manager fails */
    return ZFailure;
  }

  /* Lookup entry using specified address index */
  ZDSecMgrEntryLookupAMI( ami, &entry );

  // If no existing entry, create one
  if ( entry == NULL )
  {
    if ( ZDSecMgrEntryNew( &entry ) == ZSuccess )
    {
      entry->ami = ami;
    }
    else
    {
      /* Security Manager full */
      return ZBufferFull;
    }
  }
  // Write the link key
  APSME_LinkKeySet( extAddr, key );

#if defined (TC_LINKKEY_JOIN)
  // Mark the device as authenticated.
  ZDSecMgrAuthenticationSet( extAddr, ZDSecMgr_Authenticated_CBCK );
#endif

#if defined NV_RESTORE
  ZDSecMgrWriteNV();  // Write the new established link key to NV.
#endif

  return ZSuccess;
}

#if defined ( NV_RESTORE )
/******************************************************************************
 * @fn          ZDSecMgrInitNV
 *
 * @brief       Initialize the SecMgr entry data in NV with all values set to 0
 *
 * @param       none
 *
 * @return      uint8 - <osal_nv_item_init> return codes
 */
uint8 ZDSecMgrInitNV(void)
{

  uint8 rtrn = osal_nv_item_init(ZCD_NV_APS_LINK_KEY_TABLE,
                (sizeof(nvDeviceListHdr_t) + (sizeof(ZDSecMgrEntry_t) * ZDSECMGR_ENTRY_MAX)), NULL);

  // If the item does not already exist, set all values to 0
  if (rtrn != SUCCESS)
  {
    nvDeviceListHdr_t hdr;
    hdr.numRecs = 0;
    osal_nv_write(ZCD_NV_APS_LINK_KEY_TABLE, 0, sizeof(nvDeviceListHdr_t), &hdr);
  }

  return rtrn;
}
#endif // NV_RESTORE

#if defined ( NV_RESTORE )
/*********************************************************************
 * @fn      ZDSecMgrWriteNV()
 *
 * @brief   Save off the APS link key list to NV
 *
 * @param   none
 *
 * @return  none
 */
static void ZDSecMgrWriteNV( void )
{
  uint16 i;
  nvDeviceListHdr_t hdr;

  hdr.numRecs = 0;

  if (ZDSecMgrEntries != NULL)
  {
    for ( i = 0; i < ZDSECMGR_ENTRY_MAX; i++ )
    {
      if ( ZDSecMgrEntries[i].ami != INVALID_NODE_ADDR )
      {
        // Save off the record
        osal_nv_write( ZCD_NV_APS_LINK_KEY_TABLE,
                (uint16)((sizeof(nvDeviceListHdr_t)) + (hdr.numRecs * sizeof(ZDSecMgrEntry_t))),
                        sizeof(ZDSecMgrEntry_t), &ZDSecMgrEntries[i] );
        hdr.numRecs++;
      }
    }
  }

  // Save off the header
  osal_nv_write( ZCD_NV_APS_LINK_KEY_TABLE, 0, sizeof( nvDeviceListHdr_t ), &hdr );
}
#endif // NV_RESTORE

#if defined ( NV_RESTORE )
/******************************************************************************
 * @fn          ZDSecMgrRestoreFromNV
 *
 * @brief       Restore the APS Link Key entry data from NV. It does not restore
 *              the key data itself as they remain in NV until they are used.
 *              Only list data is restored.
 *
 * @param       none
 *
 * @return      None.
 */
static void ZDSecMgrRestoreFromNV( void )
{
  nvDeviceListHdr_t hdr;
  APSME_LinkKeyData_t *pApsLinkKey = NULL;

  if ((osal_nv_read(ZCD_NV_APS_LINK_KEY_TABLE, 0, sizeof(nvDeviceListHdr_t), &hdr) == ZSUCCESS) &&
      (hdr.numRecs <= ZDSECMGR_ENTRY_MAX))
  {
    uint8 x;

    pApsLinkKey = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

    for (x = 0; x < hdr.numRecs; x++)
    {
      if ( osal_nv_read( ZCD_NV_APS_LINK_KEY_TABLE,
                        (uint16)(sizeof(nvDeviceListHdr_t) + (x * sizeof(ZDSecMgrEntry_t))),
                        sizeof(ZDSecMgrEntry_t), &ZDSecMgrEntries[x] ) == SUCCESS )
      {
        // update data only for valid entries
        if ( ZDSecMgrEntries[x].ami != INVALID_NODE_ADDR )
        {
          if (pApsLinkKey != NULL)
          {
            // read the key form NV, keyNvId must be ZCD_NV_APS_LINK_KEY_DATA_START based
            osal_nv_read( ZDSecMgrEntries[x].keyNvId, 0,
                         sizeof(APSME_LinkKeyData_t), pApsLinkKey );

            // set new values for the counter
            pApsLinkKey->txFrmCntr += ( MAX_APS_FRAMECOUNTER_CHANGES + 1 );

            // restore values for counters in RAM
            ApsLinkKeyFrmCntr[ZDSecMgrEntries[x].keyNvId - ZCD_NV_APS_LINK_KEY_DATA_START].txFrmCntr =
                                            pApsLinkKey->txFrmCntr;

            ApsLinkKeyFrmCntr[ZDSecMgrEntries[x].keyNvId - ZCD_NV_APS_LINK_KEY_DATA_START].rxFrmCntr =
                                            pApsLinkKey->rxFrmCntr;

            osal_nv_write( ZDSecMgrEntries[x].keyNvId, 0,
                          sizeof(APSME_LinkKeyData_t), pApsLinkKey );

            // clear copy of key in RAM
            osal_memset(pApsLinkKey, 0x00, sizeof(APSME_LinkKeyData_t));
          }
        }
      }
    }

    if (pApsLinkKey != NULL)
    {
      osal_mem_free(pApsLinkKey);
    }
  }
}
#endif // NV_RESTORE

/*********************************************************************
 * @fn          ZDSecMgrSetDefaultNV
 *
 * @brief       Write the defaults to NV for Entry table and for APS key data table
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrSetDefaultNV( void )
{
  uint16 i;
  nvDeviceListHdr_t hdr;
  ZDSecMgrEntry_t secMgrEntry;
  APSME_LinkKeyData_t *pApsLinkKey = NULL;

  // Initialize the header
  hdr.numRecs = 0;

  // clear the header
  osal_nv_write(ZCD_NV_APS_LINK_KEY_TABLE, 0, sizeof(nvDeviceListHdr_t), &hdr);

  osal_memset( &secMgrEntry, 0x00, sizeof(ZDSecMgrEntry_t) );

  for ( i = 0; i < ZDSECMGR_ENTRY_MAX; i++ )
  {
    // Clear the record
    osal_nv_write( ZCD_NV_APS_LINK_KEY_TABLE,
                (uint16)((sizeof(nvDeviceListHdr_t)) + (i * sizeof(ZDSecMgrEntry_t))),
                        sizeof(ZDSecMgrEntry_t), &secMgrEntry );
  }

  pApsLinkKey = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

  if (pApsLinkKey != NULL)
  {
    osal_memset( pApsLinkKey, 0x00, sizeof(APSME_LinkKeyData_t) );

    for ( i = 0; i < ZDSECMGR_ENTRY_MAX; i++ )
    {
      // Clear the record
      osal_nv_write( (ZCD_NV_APS_LINK_KEY_DATA_START + i), 0,
                    sizeof(APSME_LinkKeyData_t), pApsLinkKey);
    }

    osal_mem_free(pApsLinkKey);
  }
}

/******************************************************************************
 * @fn          ZDSecMgrAPSRemove
 *
 * @brief       Remove device from network.
 *
 * @param       nwkAddr - device's NWK address
 * @param       extAddr - device's Extended address
 * @param       parentAddr - parent's NWK address
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDSecMgrAPSRemove( uint16 nwkAddr, uint8 *extAddr, uint16 parentAddr )
{
  ZDSecMgrDevice_t device;

  if ( ( nwkAddr == INVALID_NODE_ADDR ) ||
       ( extAddr == NULL )              ||
       ( parentAddr == INVALID_NODE_ADDR ) )
  {
    return ( ZFailure );
  }

  device.nwkAddr = nwkAddr;
  device.extAddr = extAddr;
  device.parentAddr = parentAddr;

  // remove device
  ZDSecMgrDeviceRemove( &device );

  return ( ZSuccess );
}

/******************************************************************************
 * @fn          APSME_TCLinkKeyInit
 *
 * @brief       Initialize the NV table for preconfigured TC link key
 *
 *              When zgUseDefaultTCL is set to TRUE, the default preconfig
 *              Trust Center Link Key is written to NV. A single tclk is used
 *              by all devices joining the network.
 *
 * @param       setDefault - TRUE to set default values
 *
 * @return      none
 */
void APSME_TCLinkKeyInit(uint8 setDefault)
{
  uint8             i;
  APSME_TCLinkKey_t tcLinkKey;
  uint8             rtrn;

  // Initialize all NV items for preconfigured TCLK
  for( i = 0; i < ZDSECMGR_TC_DEVICE_MAX; i++ )
  {
    // Making sure data is cleared for every key all the time
    osal_memset( &tcLinkKey, 0x00, sizeof(APSME_TCLinkKey_t) );

    // Initialize first element of the table with the default TCLK
    if((i == 0) && ( zgUseDefaultTCLK == TRUE ))
    {
      osal_memset( tcLinkKey.extAddr, 0xFF, Z_EXTADDR_LEN );
      osal_memcpy( tcLinkKey.key, defaultTCLinkKey, SEC_KEY_LEN);
    }

    // If the item doesn't exist in NV memory, create and initialize
    // it with the default value passed in, either defaultTCLK or 0
    rtrn = osal_nv_item_init( (ZCD_NV_TCLK_TABLE_START + i),
                               sizeof(APSME_TCLinkKey_t), &tcLinkKey);

    if (rtrn == SUCCESS)
    {
      // set the Frame counters to 0 to existing keys in NV
      osal_nv_read( ( ZCD_NV_TCLK_TABLE_START + i), 0,
                     sizeof(APSME_TCLinkKey_t), &tcLinkKey );

#if defined ( NV_RESTORE )
      if (setDefault == TRUE)
      {
        // clear the value stored in NV
        tcLinkKey.txFrmCntr = 0;
      }
      else
      {
        // increase the value stored in NV
        tcLinkKey.txFrmCntr += ( MAX_TCLK_FRAMECOUNTER_CHANGES + 1 );
      }
#else
      // Clear the counters if NV_RESTORE is not enabled and this NV item
      // already existed in the NV memory
      tcLinkKey.txFrmCntr = 0;
      tcLinkKey.rxFrmCntr = 0;
#endif  // NV_RESTORE

      osal_nv_write( ( ZCD_NV_TCLK_TABLE_START + i), 0,
                      sizeof(APSME_TCLinkKey_t), &tcLinkKey );

      // set initial values for counters in RAM
      TCLinkKeyFrmCntr[i].txFrmCntr = tcLinkKey.txFrmCntr;
      TCLinkKeyFrmCntr[i].rxFrmCntr = tcLinkKey.rxFrmCntr;
    }
  }

  // clear copy of key in RAM
  osal_memset( &tcLinkKey, 0x00, sizeof(APSME_TCLinkKey_t) );

}

/******************************************************************************
 * @fn          APSME_TCLinkKeySync
 *
 * @brief       Sync Trust Center LINK key data.
 *
 * @param       srcAddr - [in] srcAddr
 * @param       si      - [in, out] SSP_Info_t
 *
 * @return      ZStatus_t
 */
ZStatus_t APSME_TCLinkKeySync( uint16 srcAddr, SSP_Info_t* si )
{
  uint8 i;
  ZStatus_t status = ZSecNoKey;
  APSME_TCLinkKey_t tcLinkKey;
  uint32 *tclkRxFrmCntr;

  // Look up the IEEE address of the trust center if it's available
  if ( AddrMgrExtAddrValid( si->extAddr ) == FALSE )
  {
    APSME_LookupExtAddr( srcAddr, si->extAddr );
  }

  // Look up the TC link key associated with the device
  // or the default TC link key (extAddr is all FFs), whichever is found
  for( i = 0; i < ZDSECMGR_TC_DEVICE_MAX; i++ )
  {
    // Read entry i of the TC link key table from NV
    osal_nv_read( (ZCD_NV_TCLK_TABLE_START + i), 0,
                 sizeof(APSME_TCLinkKey_t), &tcLinkKey );

    if( AddrMgrExtAddrEqual(si->extAddr, tcLinkKey.extAddr) ||
        APSME_IsDefaultTCLK(tcLinkKey.extAddr))
    {
      tclkRxFrmCntr = &TCLinkKeyFrmCntr[i].rxFrmCntr;

      // verify that the incoming frame counter is valid
      if ( si->frmCntr >= *tclkRxFrmCntr )
      {
        // set the keyNvId to use
        si->keyNvId = (ZCD_NV_TCLK_TABLE_START + i);

        // update the rx frame counter
        *tclkRxFrmCntr = si->frmCntr + 1;

        status = ZSuccess;
      }
      else
      {
        status = ZSecOldFrmCount;
      }
      // break from the loop
      break;
    }
  }

  // clear copy of key in RAM
  osal_memset( &tcLinkKey, 0x00, sizeof(APSME_TCLinkKey_t) );

  return status;
}

/******************************************************************************
 * @fn          APSME_TCLinkKeyLoad
 *
 * @brief       Load Trust Center LINK key data.
 *
 * @param       dstAddr - [in] dstAddr
 * @param       si      - [in, out] SSP_Info_t
 *
 * @return      ZStatus_t
 */
ZStatus_t APSME_TCLinkKeyLoad( uint16 dstAddr, SSP_Info_t* si )
{
  uint8 i;
  ZStatus_t status = ZSecNoKey;
  APSME_TCLinkKey_t tcLinkKey;
  AddrMgrEntry_t addrEntry;
  uint32 *tclkTxFrmCntr;
  uint8 extAddrFound;
  uint8 defaultTCLKIdx = ZDSECMGR_TC_DEVICE_MAX;

  // Look up the ami of the srcAddr if available
  addrEntry.user    = ADDRMGR_USER_DEFAULT;
  addrEntry.nwkAddr = dstAddr;

  APSME_LookupExtAddr( dstAddr, si->extAddr );

  extAddrFound = AddrMgrExtAddrValid( si->extAddr );

  // Look up the TC link key associated with the device
  // or the master TC link key (ami = 0xFFFF), whichever is found
  for( i = 0; i < ZDSECMGR_TC_DEVICE_MAX; i++ )
  {
    // Read entry i of the TC link key table from NV
    osal_nv_read( (ZCD_NV_TCLK_TABLE_START + i), 0,
                 sizeof(APSME_TCLinkKey_t), &tcLinkKey );

    if( extAddrFound && AddrMgrExtAddrEqual(si->extAddr, tcLinkKey.extAddr) )
    {
      status = ZSuccess;

      break; // break from the loop
    }

    if ( APSME_IsDefaultTCLK(tcLinkKey.extAddr) )
    {
      if ( !extAddrFound )
      {
        status = ZSuccess;

        break; // break from the loop
      }

      // Remember the default TCLK index
      defaultTCLKIdx = i;
    }
  }

  if ( (status != ZSuccess) && (defaultTCLKIdx < ZDSECMGR_TC_DEVICE_MAX) )
  {
    // Exact match was not found; use the default TC Link Key
    i = defaultTCLKIdx;
    status = ZSuccess;
  }

  if ( status == ZSuccess )
  {
    tclkTxFrmCntr = &TCLinkKeyFrmCntr[i].txFrmCntr;

    // set the keyNvId to use
    si->keyNvId = (ZCD_NV_TCLK_TABLE_START + i);

    // update link key related fields
    si->keyID   = SEC_KEYID_LINK;
    si->frmCntr = *tclkTxFrmCntr;

    // update outgoing frame counter
    (*tclkTxFrmCntr)++;

#if defined ( NV_RESTORE )
    // write periodically to NV
    if ( !(*tclkTxFrmCntr % MAX_TCLK_FRAMECOUNTER_CHANGES) )
    {
      // set the flag to write key to NV
      TCLinkKeyFrmCntr[i].pendingFlag = TRUE;

      // Notify the ZDApp that the frame counter has changed.
      osal_set_event( ZDAppTaskID, ZDO_TCLK_FRAMECOUNTER_CHANGE );
    }
#endif
  }

  // If no TC link key found, remove the device from the address manager
  if ( (status != ZSuccess) && (AddrMgrEntryLookupNwk(&addrEntry) == TRUE) )
  {
    AddrMgrEntryRelease( &addrEntry );
  }

    // clear copy of key in RAM
  osal_memset( &tcLinkKey, 0x00, sizeof(APSME_TCLinkKey_t) );

  return status;
}

/******************************************************************************
 * @fn          APSME_IsDefaultTCLK
 *
 * @brief       Return TRUE or FALSE based on the extended address.  If the
 *              input ext address is all FFs, it means the trust center link
 *              assoiciated with the address is the default trust center link key
 *
 * @param       extAddr - [in] extended address
 *
 * @return      uint8 TRUE/FALSE
 */
uint8 APSME_IsDefaultTCLK( uint8 *extAddr )
{
  uint8 i = 0;

  if( extAddr == NULL )
  {
    return FALSE;
  }

  while( i++ < Z_EXTADDR_LEN )
  {
    if( *extAddr++ != 0xFF )
    {
      return FALSE;
    }
  }

  return TRUE;
}

/******************************************************************************
 * @fn          ZDSecMgrNwkKeyInit
 *
 * @brief       Initialize the NV items for
 *                  ZCD_NV_NWKKEY,
 *                  ZCD_NV_NWK_ACTIVE_KEY_INFO and
 *                  ZCD_NV_NWK_ALTERN_KEY_INFO
 *
 * @param       setDefault
 *
 * @return      none
 */
void ZDSecMgrNwkKeyInit(uint8 setDefault)
{
  uint8 status;
  nwkKeyDesc nwkKey;

  // Initialize NV items for NWK key, this structure contains the frame counter
  // and is only used when NV_RESTORE is enabled
  nwkActiveKeyItems keyItems;

  osal_memset( &keyItems, 0, sizeof( nwkActiveKeyItems ) );

  status = osal_nv_item_init( ZCD_NV_NWKKEY, sizeof(nwkActiveKeyItems), (void *)&keyItems );

#if defined ( NV_RESTORE )
  // reset the values of NV items if NV_RESTORE is not enabled
  if ((status == SUCCESS) && (setDefault == TRUE))
  {
    // clear NV data to default values
    osal_nv_write( ZCD_NV_NWKKEY, 0, sizeof(nwkActiveKeyItems), &keyItems );
  }
#else
  (void)setDefault;   // to eliminate compiler warning

  // reset the values of NV items if NV_RESTORE is not enabled
  if (status == SUCCESS)
  {
    osal_nv_write( ZCD_NV_NWKKEY, 0, sizeof(nwkActiveKeyItems), &keyItems );
  }
#endif // defined (NV_RESTORE)

  // Initialize NV items for NWK Active and Alternate keys. These items are used
  // all the time, independently of NV_RESTORE being set or not
  osal_memset( &nwkKey, 0x00, sizeof(nwkKey) );

  status = osal_nv_item_init( ZCD_NV_NWK_ACTIVE_KEY_INFO, sizeof(nwkKey), &nwkKey);

#if defined ( NV_RESTORE )
  // reset the values of NV items if NV_RESTORE is not enabled
  if ((status == SUCCESS) && (setDefault == TRUE))
  {
    // clear NV data to default values
    osal_nv_write( ZCD_NV_NWK_ACTIVE_KEY_INFO, 0, sizeof(nwkKey), &nwkKey );
  }
#else
  // reset the values of NV items if NV_RESTORE is not enabled
  if (status == SUCCESS)
  {
    osal_nv_write( ZCD_NV_NWK_ACTIVE_KEY_INFO, 0, sizeof(nwkKey), &nwkKey );
  }
#endif // defined (NV_RESTORE)

  status = osal_nv_item_init( ZCD_NV_NWK_ALTERN_KEY_INFO, sizeof(nwkKey), &nwkKey );

#if defined ( NV_RESTORE )
  // reset the values of NV items if NV_RESTORE is not enabled
  if ((status == SUCCESS) && (setDefault == TRUE))
  {
    // clear NV data to default values
    osal_nv_write( ZCD_NV_NWK_ALTERN_KEY_INFO, 0, sizeof(nwkKey), &nwkKey );
  }
#else
  // reset the values of NV items if NV_RESTORE is not enabled
  if (status == SUCCESS)
  {
    osal_nv_write( ZCD_NV_NWK_ALTERN_KEY_INFO, 0, sizeof(nwkKey), &nwkKey );
  }
#endif // defined (NV_RESTORE)

}

/*********************************************************************
 * @fn          ZDSecMgrReadKeyFromNv
 *
 * @brief       Looks for a specific key in NV based on Index value
 *
 * @param   keyNvId - Index of key to look in NV
 *                    valid values are:
 *                    ZCD_NV_NWK_ACTIVE_KEY_INFO
 *                    ZCD_NV_NWK_ALTERN_KEY_INFO
 *                    ZCD_NV_TCLK_TABLE_START + <offset_in_table>
 *                    ZCD_NV_APS_LINK_KEY_DATA_START + <offset_in_table>
 *                    ZCD_NV_MASTER_KEY_DATA_START + <offset_in_table>
 *                    ZCD_NV_PRECFGKEY
 *
 * @param  *keyinfo - Data is read into this buffer.
 *
 * @return  SUCCESS if NV data was copied to the keyinfo parameter .
 *          Otherwise, NV_OPER_FAILED for failure.
 */
ZStatus_t ZDSecMgrReadKeyFromNv(uint16 keyNvId, void *keyinfo)
{
  if ((keyNvId == ZCD_NV_NWK_ACTIVE_KEY_INFO) ||
      (keyNvId == ZCD_NV_NWK_ALTERN_KEY_INFO))
  {
    // get NWK active or alternate key from NV
    return (osal_nv_read(keyNvId,
                         osal_offsetof(nwkKeyDesc, key),
                         SEC_KEY_LEN,
                         keyinfo));
  }
  else if ((keyNvId >= ZCD_NV_TCLK_TABLE_START) &&
           (keyNvId < (ZCD_NV_TCLK_TABLE_START + ZDSECMGR_TC_DEVICE_MAX)))
  {
    // Read entry keyNvId of the TC link key table from NV. keyNvId should be
    // ZCD_NV_TCLK_TABLE_START + <offset_in_table>
    return (osal_nv_read(keyNvId,
                         osal_offsetof(APSME_TCLinkKey_t, key),
                         SEC_KEY_LEN,
                         keyinfo));
  }
  else if ((keyNvId >= ZCD_NV_APS_LINK_KEY_DATA_START) &&
           (keyNvId < (ZCD_NV_APS_LINK_KEY_DATA_START + ZDSECMGR_ENTRY_MAX)))
  {
    // Read entry keyNvId of the APS link key table from NV. keyNvId should be
    // ZCD_NV_APS_LINK_KEY_DATA_START + <offset_in_table>
    return (osal_nv_read(keyNvId,
                         osal_offsetof(APSME_LinkKeyData_t, key),
                         SEC_KEY_LEN,
                         keyinfo));
  }
  else if ((keyNvId >= ZCD_NV_MASTER_KEY_DATA_START) &&
           (keyNvId < (ZCD_NV_MASTER_KEY_DATA_START + ZDSECMGR_MASTERKEY_MAX)))
  {
    // Read entry keyNvId of the MASTER key table from NV. keyNvId should be
    // ZCD_NV_MASTER_KEY_DATA_START + <offset_in_table>
    return (osal_nv_read(keyNvId,
                         osal_offsetof(ZDSecMgrMasterKeyData_t, key),
                         SEC_KEY_LEN,
                         keyinfo));
  }
  else if (keyNvId == ZCD_NV_PRECFGKEY)
  {
    // Read entry keyNvId of the Preconfig key from NV.
    return (osal_nv_read(keyNvId,
                         0,
                         SEC_KEY_LEN,
                         keyinfo));
  }

  return NV_OPER_FAILED;
}

/******************************************************************************
 * @fn          ZDSecMgrApsLinkKeyInit
 *
 * @brief       Initialize the NV table for Application link keys
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrApsLinkKeyInit(void)
{
  APSME_LinkKeyData_t pApsLinkKey;
  uint8 i;
  uint8 status;

  // Initialize all NV items for APS link key, if not exist already.
  osal_memset( &pApsLinkKey, 0x00, sizeof(APSME_LinkKeyData_t) );

  for( i = 0; i < ZDSECMGR_ENTRY_MAX; i++ )
  {
    status = osal_nv_item_init( (ZCD_NV_APS_LINK_KEY_DATA_START + i),
                               sizeof(APSME_LinkKeyData_t), &pApsLinkKey );

#if defined ( NV_RESTORE )
    (void)status;   // to eliminate compiler warning
#else
    // reset the values of NV items if NV_RESTORE is not enabled
    if (status == SUCCESS)
    {
      osal_nv_write( (ZCD_NV_APS_LINK_KEY_DATA_START + i), 0,
                    sizeof(APSME_LinkKeyData_t), &pApsLinkKey );

    }
#endif // defined (NV_RESTORE)
  }
}

/******************************************************************************
 * @fn          ZDSecMgrInitNVKeyTables
 *
 * @brief       Initialize the NV table for All keys: NWK, Master, TCLK and APS
 *
 * @param       setDefault - TRUE to set default values
 *
 * @return      none
 */
void ZDSecMgrInitNVKeyTables(uint8 setDefault)
{
  ZDSecMgrNwkKeyInit(setDefault);
  ZDSecMgrMasterKeyInit();
  ZDSecMgrApsLinkKeyInit();
  APSME_TCLinkKeyInit(setDefault);
}

/******************************************************************************
 * @fn          ZDSecMgrSaveApsLinkKey
 *
 * @brief       Save APS Link Key to NV. It will loop through all the keys
 *              to see which one to save.
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrSaveApsLinkKey(void)
{
  APSME_LinkKeyData_t *pKeyData = NULL;
  int i;

  pKeyData = (APSME_LinkKeyData_t *)osal_mem_alloc(sizeof(APSME_LinkKeyData_t));

  if (pKeyData != NULL)
  {
    // checks all pending flags to know which one to save
    for (i = 0; i < ZDSECMGR_ENTRY_MAX; i++)
    {
      if (ApsLinkKeyFrmCntr[i].pendingFlag == TRUE)
      {
        // retrieve key from NV
        if (osal_nv_read(ZCD_NV_APS_LINK_KEY_DATA_START + i, 0,
                         sizeof(APSME_LinkKeyData_t), pKeyData) == SUCCESS)
        {
          pKeyData->txFrmCntr = ApsLinkKeyFrmCntr[i].txFrmCntr;
          pKeyData->rxFrmCntr = ApsLinkKeyFrmCntr[i].rxFrmCntr;

          // Write the APS link key back to the NV
          osal_nv_write(ZCD_NV_APS_LINK_KEY_DATA_START + i, 0,
                        sizeof(APSME_LinkKeyData_t), pKeyData);

          // clear the pending write flag
          ApsLinkKeyFrmCntr[i].pendingFlag = FALSE;
        }
      }
    }

    // clear copy of key in RAM
    osal_memset( pKeyData, 0x00, sizeof(APSME_LinkKeyData_t) );

    osal_mem_free(pKeyData);
  }
}

/******************************************************************************
 * @fn          ZDSecMgrSaveTCLinkKey
 *
 * @brief       Save TC Link Key to NV. It will loop through all the keys
 *              to see which one to save.
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrSaveTCLinkKey(void)
{
  APSME_TCLinkKey_t *pKeyData = NULL;
  uint16 i;

  pKeyData = (APSME_TCLinkKey_t *)osal_mem_alloc(sizeof(APSME_TCLinkKey_t));

  if (pKeyData != NULL)
  {
    for( i = 0; i < ZDSECMGR_TC_DEVICE_MAX; i++ )
    {
      if (TCLinkKeyFrmCntr[i].pendingFlag == TRUE)
      {
        if (osal_nv_read(ZCD_NV_TCLK_TABLE_START + i, 0,
                         sizeof(APSME_TCLinkKey_t), pKeyData) == SUCCESS)
        {
          pKeyData->txFrmCntr = TCLinkKeyFrmCntr[i].txFrmCntr;
          pKeyData->rxFrmCntr = TCLinkKeyFrmCntr[i].rxFrmCntr;

          // Write the TC link key back to the NV
          osal_nv_write(ZCD_NV_TCLK_TABLE_START + i, 0,
                        sizeof(APSME_TCLinkKey_t), pKeyData);

          // clear the pending write flag
          TCLinkKeyFrmCntr[i].pendingFlag = FALSE;
        }
      }
    }
      // clear copy of key in RAM
    osal_memset( pKeyData, 0x00, sizeof(APSME_TCLinkKey_t) );

    osal_mem_free(pKeyData);
  }
}

#if defined ( ZBA_FALLBACK_NWKKEY )
/******************************************************************************
 * @fn          ZDSecMgrFallbackNwkKey
 *
 * @brief       Use the ZBA fallback network key.
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrFallbackNwkKey( void )
{
  if ( !_NIB.nwkKeyLoaded )
  {
    uint8 fallbackKey[SEC_KEY_LEN];

    ZDSecMgrReadKeyFromNv( ZCD_NV_PRECFGKEY, fallbackKey );
    SSP_UpdateNwkKey( fallbackKey, 0);
    SSP_SwitchNwkKey( 0 );

    // clear local copy of key
    osal_memset( fallbackKey, 0x00, SEC_KEY_LEN );

    // handle next step in authentication process
    ZDSecMgrAuthNwkKey();
  }
}
#endif // defined ( ZBA_FALLBACK_NWKKEY )

#if defined ( NV_RESTORE )
/******************************************************************************
 * @fn          ZDSecMgrClearNVKeyValues
 *
 * @brief       If NV_RESTORE is enabled and the status of the network needs
 *              default values this fuction clears ZCD_NV_NWKKEY,
 *              ZCD_NV_NWK_ACTIVE_KEY_INFO and ZCD_NV_NWK_ALTERN_KEY_INFO link
 *
 * @param       none
 *
 * @return      none
 */
void ZDSecMgrClearNVKeyValues(void)
{
  nwkActiveKeyItems keyItems;
  nwkKeyDesc nwkKey;

  osal_memset(&keyItems, 0x00, sizeof(nwkActiveKeyItems));

  osal_nv_write(ZCD_NV_NWKKEY, 0, sizeof(nwkActiveKeyItems), &keyItems);

  // Initialize NV items for NWK Active and Alternate keys.
  osal_memset( &nwkKey, 0x00, sizeof(nwkKeyDesc) );

  osal_nv_write(ZCD_NV_NWK_ACTIVE_KEY_INFO, 0, sizeof(nwkKeyDesc), &nwkKey);

  osal_nv_write(ZCD_NV_NWK_ALTERN_KEY_INFO, 0, sizeof(nwkKeyDesc), &nwkKey);
}
#endif // defined ( NV_RESTORE )

/******************************************************************************
******************************************************************************/
