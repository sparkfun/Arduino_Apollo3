/*************************************************************************************************/
/*!
 *  \file   app_db.c
 *
 *  \brief  Application framework device database example, using simple RAM-based storage.
 *
 *          $Date: 2017-03-21 16:17:43 -0500 (Tue, 21 Mar 2017) $
 *          $Revision: 11622 $
 *
 *  Copyright (c) 2011-2017 ARM Ltd., all rights reserved.
 *  ARM Ltd. confidential and proprietary.
 *
 *  IMPORTANT.  Your use of this file is governed by a Software License Agreement
 *  ("Agreement") that must be accepted in order to download or otherwise receive a
 *  copy of this file.  You may not use or copy this file for any purpose other than
 *  as described in the Agreement.  If you do not agree to all of the terms of the
 *  Agreement do not use this file and delete all copies in your possession or control;
 *  if you do not have a copy of the Agreement, you must contact ARM Ltd. prior
 *  to any use, copying or further distribution of this software.
 */
/*************************************************************************************************/

#include <string.h>
#include "am_mcu_apollo.h"
#include "wsf_types.h"
#include "wsf_assert.h"
#include "bda.h"
#include "app_api.h"
#include "app_main.h"
#include "app_db.h"
#include "app_cfg.h"

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! Database record */
typedef struct
{
  /*! Common for all roles */
  bdAddr_t    peerAddr;                     /*! Peer address */
  uint8_t     addrType;                     /*! Peer address type */
  dmSecIrk_t  peerIrk;                      /*! Peer IRK */
  dmSecCsrk_t peerCsrk;                     /*! Peer CSRK */
  uint8_t     keyValidMask;                 /*! Valid keys in this record */
  bool_t      inUse;                        /*! TRUE if record in use */
  bool_t      valid;                        /*! TRUE if record is valid */
  bool_t      peerAddedToRl;                /*! TRUE if peer device's been added to resolving list */
  bool_t      peerRpao;                     /*! TRUE if RPA Only attribute's present on peer device */

  /*! For slave local device */
  dmSecLtk_t  localLtk;                     /*! Local LTK */
  uint8_t     localLtkSecLevel;             /*! Local LTK security level */
  bool_t      peerAddrRes;                  /*! TRUE if address resolution's supported on peer device (master) */

  /*! For master local device */
  dmSecLtk_t  peerLtk;                      /*! Peer LTK */
  uint8_t     peerLtkSecLevel;              /*! Peer LTK security level */

  /*! for ATT server local device */
  uint16_t    cccTbl[APP_DB_NUM_CCCD];      /*! Client characteristic configuration descriptors */
  uint32_t    peerSignCounter;              /*! Peer Sign Counter */

  /*! for ATT client */
  uint16_t    hdlList[APP_DB_HDL_LIST_LEN]; /*! Cached handle list */
  uint8_t     discStatus;                   /*! Service discovery and configuration status */
} appDbRec_t;

/*! Database type */
typedef struct
{
  appDbRec_t  rec[APP_DB_NUM_RECS];               /*! Device database records */
  char        devName[ATT_DEFAULT_PAYLOAD_LEN];   /*! Device name */
  uint8_t     devNameLen;                         /*! Device name length */
} appDb_t;

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! Database */
static appDb_t appDb;

/*! When all records are allocated use this index to determine which to overwrite */
static appDbRec_t *pAppDbNewRec = appDb.rec;


//
// Copy Record list from NVM into the active record list, if any
//

appDbRec_t * pRecListNvmPointer = (appDbRec_t *)0x00070000; //temporarily put the data here

void AppCopyRecListInNvm(appDbRec_t *pRecord)
{
    appDbRec_t* pNvmRec =  pRecListNvmPointer;
    uint8_t i;  
    for(i=0;i<APP_DB_NUM_RECS;i++)
    {
        if(((*(uint32_t*)(pNvmRec->peerAddr) != 0xFFFFFFFF))&&(*(uint32_t*)(pRecListNvmPointer->peerAddr) != 0x00000000))
        {
            //valid record in NVM
            memcpy(pRecord, pNvmRec, sizeof(appDbRec_t));
            
            //pRecord->inUse = FALSE;
            //pRecord->valid = TRUE;
            
            pNvmRec++;
            pRecord++;
        }
        else
        {
            break;
        }
    }
}
// sizeof(appDbRec_t) = 176 bytes of buffer
uint8_t ui8DbRamBuffer[APP_DB_NUM_RECS * sizeof(appDbRec_t)];
uint16_t ui16DbRamBufferSize = APP_DB_NUM_RECS * sizeof(appDbRec_t);

static void updateRecordInNVM(uint32_t* pDest, uint32_t* pSrc, uint32_t* pFlashAddr)
{
    uint16_t ui16Size = sizeof(appDbRec_t);

    // read the flash out
    memcpy(ui8DbRamBuffer, pFlashAddr, ui16DbRamBufferSize);
    // update the record
    memcpy((uint8_t *)pDest, (uint8_t *)pSrc, ui16Size);
    // erase the page
    uint32_t ui32CurrentPage =  AM_HAL_FLASH_ADDR2PAGE((uint32_t)pFlashAddr);
    uint32_t ui32CurrentBlock = AM_HAL_FLASH_ADDR2INST((uint32_t)pFlashAddr);

    uint32_t ui32Critical = am_hal_interrupt_master_disable();
    am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                                ui32CurrentBlock, ui32CurrentPage);
    // program the data back
    am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                              (uint32_t *)ui8DbRamBuffer,
                              (uint32_t *)pFlashAddr,
                              ui16DbRamBufferSize%4?((ui16DbRamBufferSize/4) + 1):(ui16DbRamBufferSize/4));
    am_hal_interrupt_master_set(ui32Critical);
}

int32_t AppStorePairingInfoInNVM(appDbHdl_t hdl)
{
    appDbRec_t* pNvmRec =  pRecListNvmPointer;
    appDbRec_t* pRamBufRec = (appDbRec_t*)ui8DbRamBuffer;
    
    int i;  


    for(i=0;i<APP_DB_NUM_RECS;i++)
    {
        if((*(uint32_t*)(pNvmRec->peerAddr) != 0xFFFFFFFF))
        {
            if(BdaCmp(((appDbRec_t*)hdl)->peerAddr, pNvmRec->peerAddr))
            {
                updateRecordInNVM((uint32_t*)&pRamBufRec[i], (uint32_t*)((appDbRec_t*)hdl)->peerAddr, (uint32_t*)pRecListNvmPointer);
                return true;
            }
            //skip
            pNvmRec++;
        }
        else
        {
            am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                                      (uint32_t *)((appDbRec_t*)hdl)->peerAddr,
                                      (uint32_t *)pNvmRec,
                                      (sizeof(appDbRec_t)%4?((sizeof(appDbRec_t)/4) + 1):(sizeof(appDbRec_t)/4)));
            return true;
        }
    }

    // if we get here, the record NVM are full
    // function spec: erase all the record and record the current one
    // erase the page
    uint32_t ui32CurrentPage =  AM_HAL_FLASH_ADDR2PAGE((uint32_t)pRecListNvmPointer);
    uint32_t ui32CurrentBlock = AM_HAL_FLASH_ADDR2INST((uint32_t)pRecListNvmPointer);

    uint32_t ui32Critical = am_hal_interrupt_master_disable();
    am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                                ui32CurrentBlock, ui32CurrentPage);
    // program the data back
    am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                              (uint32_t *)((appDbRec_t*)hdl)->peerAddr,
                              (uint32_t *)pRecListNvmPointer,
                              (sizeof(appDbRec_t)%4?((sizeof(appDbRec_t)/4) + 1):(sizeof(appDbRec_t)/4)));
    am_hal_interrupt_master_set(ui32Critical);

    return false;

}
/*************************************************************************************************/
/*!
 *  \fn     AppDbInit()
 *        
 *  \brief  Initialize the device database.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbInit(void)
{
#ifdef AM_BLE_USE_NVM
    AppCopyRecListInNvm(pAppDbNewRec);
#endif
    return;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbNewRecord
 *        
 *  \brief  Create a new device database record.
 *
 *  \param  addrType  Address type.
 *  \param  pAddr     Peer device address.
 *
 *  \return Database record handle.
 */
/*************************************************************************************************/
appDbHdl_t AppDbNewRecord(uint8_t addrType, uint8_t *pAddr)
{
  appDbRec_t  *pRec = appDb.rec;
  uint8_t     i;
  
  /* find a free record */
  for (i = APP_DB_NUM_RECS; i > 0; i--, pRec++)
  {
    if (!pRec->inUse)
    {
      break;
    }
  }
  
  /* if all records were allocated */
  if (i == 0)
  {
    /* overwrite a record */
    pRec = pAppDbNewRec;
    
    /* get next record to overwrite */
    pAppDbNewRec++;
    if (pAppDbNewRec == &appDb.rec[APP_DB_NUM_RECS])
    {
      pAppDbNewRec = appDb.rec;
    }
  }
  
  /* initialize record */
  memset(pRec, 0, sizeof(appDbRec_t));
  pRec->inUse = TRUE;
  pRec->addrType = addrType;
  BdaCpy(pRec->peerAddr, pAddr);
  pRec->peerAddedToRl = FALSE;
  pRec->peerRpao = FALSE;

  return (appDbHdl_t) pRec;
}

/*************************************************************************************************/
/*!
*  \fn     AppDbGetNextRecord
*
*  \brief  Get the next database record for a given record. For the first record, the function
*          should be called with 'hdl' set to 'APP_DB_HDL_NONE'.
*
*  \param  hdl  Database record handle.
*
*  \return Next record handle found. APP_DB_HDL_NONE, otherwise.
*/
/*************************************************************************************************/
appDbHdl_t AppDbGetNextRecord(appDbHdl_t hdl)
{
  appDbRec_t  *pRec;

  /* if first record is requested */
  if (hdl == APP_DB_HDL_NONE)
  {
    pRec = appDb.rec;
  }
  /* if valid record passed in */
  else if (AppDbRecordInUse(hdl))
  {
    pRec = (appDbRec_t *)hdl;
    pRec++;
  }
  /* invalid record passed in */
  else
  {
    return APP_DB_HDL_NONE;
  }

  /* look for next valid record */
  while (pRec < &appDb.rec[APP_DB_NUM_RECS])
  {
    /* if record is in use */
    if (pRec->inUse && pRec->valid)
    {
      /* record found */
      return (appDbHdl_t)pRec;
    }

    /* look for next record */
    pRec++;
  }

  /* end of records */
  return APP_DB_HDL_NONE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbDeleteRecord
 *        
 *  \brief  Delete a new device database record.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbDeleteRecord(appDbHdl_t hdl)
{
  ((appDbRec_t *) hdl)->inUse = FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbValidateRecord
 *        
 *  \brief  Validate a new device database record.  This function is called when pairing is
 *          successful and the devices are bonded.
 *
 *  \param  hdl       Database record handle.
 *  \param  keyMask   Bitmask of keys to validate.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbValidateRecord(appDbHdl_t hdl, uint8_t keyMask)
{
  ((appDbRec_t *) hdl)->valid = TRUE;
  ((appDbRec_t *) hdl)->keyValidMask = keyMask;

#ifdef AM_BLE_USE_NVM
  AppStorePairingInfoInNVM(hdl);
#endif
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbCheckValidRecord
 *        
 *  \brief  Check if a record has been validated.  If it has not, delete it.  This function
 *          is typically called when the connection is closed.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbCheckValidRecord(appDbHdl_t hdl)
{
  if (((appDbRec_t *) hdl)->valid == FALSE)
  {
    AppDbDeleteRecord(hdl);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbInvalidateRecord
 *        
 *  \brief  Invalidate a new device database record.  This function is called when application
 *          needs to delete bonding information with peer device.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbInvalidateRecord(appDbHdl_t hdl)
{
    if(((appDbRec_t *) hdl)->valid == true)
    {
        ((appDbRec_t *) hdl)->valid = false;
        ((appDbRec_t *) hdl)->keyValidMask = 0;
    }
}
/*************************************************************************************************/
/*!
*  \fn     AppDbRecordInUse
*
*  \brief  Check if a database record is in use.

*  \param  hdl       Database record handle.
*
*  \return TURE if record in use. FALSE, otherwise.
*/
/*************************************************************************************************/
bool_t AppDbRecordInUse(appDbHdl_t hdl)
{
  appDbRec_t  *pRec = appDb.rec;
  uint8_t     i;

  /* see if record is in database record list */
  for (i = APP_DB_NUM_RECS; i > 0; i--, pRec++)
  {
    if (pRec->inUse && pRec->valid && (pRec == ((appDbRec_t *)hdl)))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbCheckBonded
 *        
 *  \brief  Check if there is a stored bond with any device.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return TRUE if a bonded device is found, FALSE otherwise.
 */
/*************************************************************************************************/
bool_t AppDbCheckBonded(void)
{
  appDbRec_t  *pRec = appDb.rec;
  uint8_t     i;
  
  /* find a record */
  for (i = APP_DB_NUM_RECS; i > 0; i--, pRec++)
  {
    if (pRec->inUse)
    {
      return TRUE;
    }
  }
  
  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbDeleteAllRecords
 *        
 *  \brief  Delete all database records.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbDeleteAllRecords(void)
{
  appDbRec_t  *pRec = appDb.rec;
  uint8_t     i;

  /* set in use to false for all records */
  for (i = APP_DB_NUM_RECS; i > 0; i--, pRec++)
  {
    pRec->inUse = FALSE;
  }  
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbFindByAddr
 *        
 *  \brief  Find a device database record by peer address.
 *
 *  \param  addrType  Address type.
 *  \param  pAddr     Peer device address.
 *
 *  \return Database record handle or APP_DB_HDL_NONE if not found.
 */
/*************************************************************************************************/
appDbHdl_t AppDbFindByAddr(uint8_t addrType, uint8_t *pAddr)
{
  appDbRec_t  *pRec = appDb.rec;
  uint8_t     peerAddrType = DmHostAddrType(addrType);
  uint8_t     i;
  
  /* find matching record */
  for (i = APP_DB_NUM_RECS; i > 0; i--, pRec++)
  {
    if (pRec->inUse && (pRec->addrType == peerAddrType) && BdaCmp(pRec->peerAddr, pAddr))
    {
      return (appDbHdl_t) pRec;
    }
  }
  
  return APP_DB_HDL_NONE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbFindByLtkReq
 *        
 *  \brief  Find a device database record by data in an LTK request.
 *
 *  \param  encDiversifier  Encryption diversifier associated with key.
 *  \param  pRandNum        Pointer to random number associated with key.
 *
 *  \return Database record handle or APP_DB_HDL_NONE if not found.
 */
/*************************************************************************************************/
appDbHdl_t AppDbFindByLtkReq(uint16_t encDiversifier, uint8_t *pRandNum)
{
  appDbRec_t  *pRec = appDb.rec;
  uint8_t     i;
  
  /* find matching record */
  for (i = APP_DB_NUM_RECS; i > 0; i--, pRec++)
  {
    if (pRec->inUse && (pRec->localLtk.ediv == encDiversifier) &&
        (memcmp(pRec->localLtk.rand, pRandNum, SMP_RAND8_LEN) == 0))
    {
      return (appDbHdl_t) pRec;
    }
  }
  
  return APP_DB_HDL_NONE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetKey
 *        
 *  \brief  Get a key from a device database record.
 *
 *  \param  hdl       Database record handle.
 *  \param  type      Type of key to get.
 *  \param  pSecLevel If the key is valid, the security level of the key.
 *
 *  \return Pointer to key if key is valid or NULL if not valid.
 */
/*************************************************************************************************/
dmSecKey_t *AppDbGetKey(appDbHdl_t hdl, uint8_t type, uint8_t *pSecLevel)
{
  dmSecKey_t *pKey = NULL;
  
  /* if key valid */
  if ((type & ((appDbRec_t *) hdl)->keyValidMask) != 0)
  {
    switch(type)
    {
      case DM_KEY_LOCAL_LTK:
        *pSecLevel = ((appDbRec_t *) hdl)->localLtkSecLevel;
        pKey = (dmSecKey_t *) &((appDbRec_t *) hdl)->localLtk;
        break;

      case DM_KEY_PEER_LTK:
        *pSecLevel = ((appDbRec_t *) hdl)->peerLtkSecLevel;
        pKey = (dmSecKey_t *) &((appDbRec_t *) hdl)->peerLtk;
        break;

      case DM_KEY_IRK:
        pKey = (dmSecKey_t *)&((appDbRec_t *)hdl)->peerIrk;
        break;

      case DM_KEY_CSRK:
        pKey = (dmSecKey_t *)&((appDbRec_t *)hdl)->peerCsrk;
        break;
        
      default:
        break;
    }
  }
  
  return pKey;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetKey
 *        
 *  \brief  Set a key in a device database record.
 *
 *  \param  hdl       Database record handle.
 *  \param  pKey      Key data.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetKey(appDbHdl_t hdl, dmSecKeyIndEvt_t *pKey)
{
  switch(pKey->type)
  {
    case DM_KEY_LOCAL_LTK:
      ((appDbRec_t *) hdl)->localLtkSecLevel = pKey->secLevel;
      ((appDbRec_t *) hdl)->localLtk = pKey->keyData.ltk;
      break;

    case DM_KEY_PEER_LTK:
      ((appDbRec_t *) hdl)->peerLtkSecLevel = pKey->secLevel;
      ((appDbRec_t *) hdl)->peerLtk = pKey->keyData.ltk;
      break;

    case DM_KEY_IRK:
      ((appDbRec_t *)hdl)->peerIrk = pKey->keyData.irk;

      /* make sure peer record is stored using its identity address */
      ((appDbRec_t *)hdl)->addrType = pKey->keyData.irk.addrType;
      BdaCpy(((appDbRec_t *)hdl)->peerAddr, pKey->keyData.irk.bdAddr);
      break;

    case DM_KEY_CSRK:
      ((appDbRec_t *)hdl)->peerCsrk = pKey->keyData.csrk;

      /* sign counter must be initialized to zero when CSRK is generated */
      ((appDbRec_t *)hdl)->peerSignCounter = 0;
      break;
      
    default:
      break;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetCccTbl
 *        
 *  \brief  Get the client characteristic configuration descriptor table.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Pointer to client characteristic configuration descriptor table.
 */
/*************************************************************************************************/
uint16_t *AppDbGetCccTbl(appDbHdl_t hdl)
{
  return ((appDbRec_t *) hdl)->cccTbl;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetCccTblValue
 *        
 *  \brief  Set a value in the client characteristic configuration table.
 *
 *  \param  hdl       Database record handle.
 *  \param  idx       Table index.
 *  \param  value     client characteristic configuration value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetCccTblValue(appDbHdl_t hdl, uint16_t idx, uint16_t value)
{
  WSF_ASSERT(idx < APP_DB_NUM_CCCD);
  
  ((appDbRec_t *) hdl)->cccTbl[idx] = value;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetDiscStatus
 *        
 *  \brief  Get the discovery status.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Discovery status.
 */
/*************************************************************************************************/
uint8_t AppDbGetDiscStatus(appDbHdl_t hdl)
{
  return ((appDbRec_t *) hdl)->discStatus;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetDiscStatus
 *        
 *  \brief  Set the discovery status.
 *
 *  \param  hdl       Database record handle.
 *  \param  state     Discovery status.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetDiscStatus(appDbHdl_t hdl, uint8_t status)
{
  ((appDbRec_t *) hdl)->discStatus = status;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetHdlList
 *        
 *  \brief  Get the cached handle list.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Pointer to handle list.
 */
/*************************************************************************************************/
uint16_t *AppDbGetHdlList(appDbHdl_t hdl)
{
  return ((appDbRec_t *) hdl)->hdlList;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetHdlList
 *        
 *  \brief  Set the cached handle list.
 *
 *  \param  hdl       Database record handle.
 *  \param  pHdlList  Pointer to handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetHdlList(appDbHdl_t hdl, uint16_t *pHdlList)
{
  memcpy(((appDbRec_t *) hdl)->hdlList, pHdlList, sizeof(((appDbRec_t *) hdl)->hdlList));
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetDevName
 *        
 *  \brief  Get the device name.
 *
 *  \param  pLen      Returned device name length.
 *
 *  \return Pointer to UTF-8 string containing device name or NULL if not set.
 */
/*************************************************************************************************/
char *AppDbGetDevName(uint8_t *pLen)
{
  /* if first character of name is NULL assume it is uninitialized */
  if (appDb.devName[0] == 0)
  {
    *pLen = 0;
    return NULL;
  }
  else
  {
    *pLen = appDb.devNameLen;
    return appDb.devName;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetDevName
 *        
 *  \brief  Set the device name.
 *
 *  \param  len       Device name length.
 *  \param  pStr      UTF-8 string containing device name.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetDevName(uint8_t len, char *pStr)
{
  /* check for maximum device length */
  len = (len <= sizeof(appDb.devName)) ? len : sizeof(appDb.devName);
  
  memcpy(appDb.devName, pStr, len);
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetPeerAddrRes
 *
 *  \brief  Get address resolution attribute value read from a peer device.
 *
 *  \param  hdl        Database record handle.
 *
 *  \return TRUE if address resolution is supported in peer device. FALSE, otherwise.
 */
/*************************************************************************************************/
bool_t AppDbGetPeerAddrRes(appDbHdl_t hdl)
{
  return ((appDbRec_t *)hdl)->peerAddrRes;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetPeerAddrRes
 *
 *  \brief  Set address resolution attribute value for a peer device.
 *
 *  \param  hdl        Database record handle.
 *  \param  addrRes    Address resolution attribue value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerAddrRes(appDbHdl_t hdl, uint8_t addrRes)
{
  ((appDbRec_t *)hdl)->peerAddrRes = addrRes;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetPeerSignCounter
 *
 *  \brief  Get sign counter for a peer device.
 *
 *  \param  hdl        Database record handle.
 *
 *  \return Sign counter for peer device.
 */
/*************************************************************************************************/
uint32_t AppDbGetPeerSignCounter(appDbHdl_t hdl)
{
  return ((appDbRec_t *)hdl)->peerSignCounter;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetPeerSignCounter
 *
 *  \brief  Set sign counter for a peer device.
 *
 *  \param  hdl          Database record handle.
 *  \param  signCounter  Sign counter for peer device.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerSignCounter(appDbHdl_t hdl, uint32_t signCounter)
{
  ((appDbRec_t *)hdl)->peerSignCounter = signCounter;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetPeerAddedToRl
 *
 *  \brief  Get the peer device added to resolving list flag value.
 *
 *  \param  hdl        Database record handle.
 *
 *  \return TRUE if peer device's been added to resolving list. FALSE, otherwise.
 */
/*************************************************************************************************/
bool_t AppDbGetPeerAddedToRl(appDbHdl_t hdl)
{
  return ((appDbRec_t *)hdl)->peerAddedToRl;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetPeerAddedToRl
 *
 *  \brief  Set the peer device added to resolving list flag to a given value.
 *
 *  \param  hdl           Database record handle.
 *  \param  peerAddedToRl Peer device added to resolving list flag value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerAddedToRl(appDbHdl_t hdl, bool_t peerAddedToRl)
{
  ((appDbRec_t *)hdl)->peerAddedToRl = peerAddedToRl;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbGetPeerRpao
 *
 *  \brief  Get the resolvable private address only attribute flag for a given peer device.
 *
 *  \param  hdl        Database record handle.
 *
 *  \return TRUE if RPA Only attribute is present on peer device. FALSE, otherwise.
 */
/*************************************************************************************************/
bool_t AppDbGetPeerRpao(appDbHdl_t hdl)
{
  return ((appDbRec_t *)hdl)->peerRpao;
}

/*************************************************************************************************/
/*!
 *  \fn     AppDbSetPeerRpao
 *
 *  \brief  Set the resolvable private address only attribute flag for a given peer device.
 *
 *  \param  hdl        Database record handle.
 *  \param  peerRpao   Resolvable private address only attribute flag value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerRpao(appDbHdl_t hdl, bool_t peerRpao)
{
  ((appDbRec_t *)hdl)->peerRpao = peerRpao;
}
