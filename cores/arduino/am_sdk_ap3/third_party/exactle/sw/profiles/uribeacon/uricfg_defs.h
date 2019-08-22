/*************************************************************************************************/
/*!
 *  \file   uricfg_defs.h
 *
 *  \brief  UriBeacon configuration service defines.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
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

#ifndef URICFG_DEFS_H
#define URICFG_DEFS_H

/**************************************************************************************************
 Macros
**************************************************************************************************/

/*! UriBeacon configuration service-related UUIDs */
#define URICFG_UUID_BYTES(id) 0xd8, 0x81, 0xc9, 0x1a, 0xb9, 0x99, \
                              0x96, 0xab, \
                              0xba, 0x40, \
                              0x86, 0x87, \
                              (id & 0xFF), ((id >> 8) & 0xFF), 0x0c, 0xee

/*! UriBeacon configuration service-related UUIDs */
enum
{
  URICFG_UUID_SVC              = 0x2080,
  URICFG_UUID_CHR_LOCKSTATE    = 0x2081,
  URICFG_UUID_CHR_LOCK         = 0x2082,
  URICFG_UUID_CHR_UNLOCK       = 0x2083,
  URICFG_UUID_CHR_URIDATA      = 0x2084,
  URICFG_UUID_CHR_URIFLAGS     = 0x2085,
  URICFG_UUID_CHR_TXPWRLEVELS  = 0x2086,
  URICFG_UUID_CHR_TXPWRMODE    = 0x2087,
  URICFG_UUID_CHR_BEACONPERIOD = 0x2088,
  URICFG_UUID_CHR_RESET        = 0x2089,
  URICFG_UUID_CHR_RESERVE      = 0x2090
};

/*! Transmit power modes */
enum
{
  URICFG_ATT_TXPWRMODE_LOWEST = 0,
  URICFG_ATT_TXPWRMODE_LOW    = 1,
  URICFG_ATT_TXPWRMODE_MEDIUM = 2,
  URICFG_ATT_TXPWRMODE_HIGH   = 3
};

/*! Sizes of attributes */
#define URICFG_MAXSIZE_URIDATA_ATT       18     /*! Size of URI data attribute */
#define URICFG_SIZE_TXPWRLEVELS_ATT      4      /*! Size of transmit power levels attribute */
#define URICFG_SIZE_LOCK_ATT             16     /*! Size of lock attribute */

/*! Beacon period attribute values */
#define URICFG_ATT_BEACONPERIOD_MIN      20     /*! Minimum period */
#define URICFG_ATT_BEACONPERIOD_MAX      10240  /*! Maximum period */
#define URICFG_ATT_BEACONPERIOD_DISABLE  0      /*! Value to disable beacon */

/*! Default (reset) values of attributes */
#define URICFG_ATT_URIFLAGS_DEFAULT      0x10
#define URICFG_ATT_TXPWRMODE_DEFAULT     URICFG_ATT_TXPWRMODE_LOW
#define URICFG_ATT_BEACONPERIOD_DEFAULT  1000   /*! Default beacon period in milliseconds (1 second) */
#define URICFG_ATT_LOCK_DEFAULT_BYTES    0x00, 0x00, 0x00, 0x00,  \
                                         0x00, 0x00, \
                                         0x00, 0x00, \
                                         0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00

/*! UriBeacon service UUID for advertising data */
#define URICFG_SERVICE_UUID              0xFEAA

#endif /* URICFG_DEFS_H */
