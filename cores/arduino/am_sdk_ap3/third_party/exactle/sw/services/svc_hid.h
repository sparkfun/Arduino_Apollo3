/*************************************************************************************************/
/*!
 *  \file   svc_hid.h
 *        
 *  \brief  Human Interface Device service implementation.
 *
 *          $Date: 2017-02-08 14:15:55 -0600 (Wed, 08 Feb 2017) $
 *          $Revision: 11118 $
 *  
 *  Copyright (c) 2015-2016-2017 ARM Ltd., all rights reserved.
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

#ifndef SVC_HID_H
#define SVC_HID_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
Macros
**************************************************************************************************/

/* HID Service */
#define HID_SVC_UUID                  ATT_UUID_HID_SERVICE

/* HID Spec Version: 1.11 */
#define HID_VERSION                   0x0111

/* HID Report Types */
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

/* HID Protocol Mode Types */
#define HID_PROTOCOL_MODE_BOOT        0x00
#define HID_PROTOCOL_MODE_REPORT      0x01

/* HID Control Point Values */
#define HID_CONTROL_POINT_SUSPEND     0x00
#define HID_CONTROL_POINT_RESUME      0x01

/* Max length of the report map value */
#define HID_MAX_REPORT_MAP_LEN        512

/* Max length of an output report value */
#define HID_MAX_REPORT_LEN            32

/* Initial length of the report map value */
#define HID_INIT_REPORT_MAP_LEN       1

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Proprietary Service */
#define HID_START_HDL                 0x50
#define HID_END_HDL                   (HID_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/
/* Proprietary Service Handles Common to HID Devices */
enum
{
  HID_SVC_HDL = HID_START_HDL,        /* Proprietary Service Declaration */
  HID_INFO_CH_HDL,                    /* HID Information Characteristic Declaration */ 
  HID_INFO_HDL,                       /* HID Information Value */
  HID_REPORT_MAP_CH_HDL,              /* HID Report Map Characteristic Declaration */ 
  HID_REPORT_MAP_HDL,                 /* HID Report Map Value */ 
  HID_EXTERNAL_REPORT_HDL,            /* HID External Report Descriptor */ 
  HID_CONTROL_POINT_CH_HDL,           /* HID Control Point Characteristic Declaration */ 
  HID_CONTROL_POINT_HDL,              /* HID Control Point Value */ 
  HID_KEYBOARD_BOOT_IN_CH_HDL,        /* HID Keyboard Boot Input Characteristic Declaration */ 
  HID_KEYBOARD_BOOT_IN_HDL,           /* HID Keyboard Boot Input Value */
  HID_KEYBOARD_BOOT_IN_CH_CCC_HDL,    /* HID Keyboard Boot Input CCC Descriptor */
  HID_KEYBOARD_BOOT_OUT_CH_HDL,       /* HID Keyboard Boot Output Characteristic Declaration */ 
  HID_KEYBOARD_BOOT_OUT_HDL,          /* HID Keyboard Boot Output Value */
  HID_MOUSE_BOOT_IN_CH_HDL,           /* HID Mouse Boot Input Characteristic Declaration */ 
  HID_MOUSE_BOOT_IN_HDL,              /* HID Mouse Boot Input Value */
  HID_MOUSE_BOOT_IN_CH_CCC_HDL,       /* HID Mouse Boot Input CCC Descriptor */
  HID_INPUT_REPORT_1_CH_HDL,          /* HID Input Report Characteristic Declaration */ 
  HID_INPUT_REPORT_1_HDL,             /* HID Input Report Value */ 
  HID_INPUT_REPORT_1_CH_CCC_HDL,      /* HID Input Report CCC Descriptor */
  HID_INPUT_REPORT_1_REFERENCE_HDL,   /* HID Input Report Reference Descriptor */ 
  HID_INPUT_REPORT_2_CH_HDL,          /* HID Input Report Characteristic Declaration */ 
  HID_INPUT_REPORT_2_HDL,             /* HID Input Report Value */ 
  HID_INPUT_REPORT_2_CH_CCC_HDL,      /* HID Input Report CCC Descriptor */
  HID_INPUT_REPORT_2_REFERENCE_HDL,   /* HID Input Report Reference Descriptor */ 
  HID_INPUT_REPORT_3_CH_HDL,          /* HID Input Report Characteristic Declaration */ 
  HID_INPUT_REPORT_3_HDL,             /* HID Input Report Value */ 
  HID_INPUT_REPORT_3_CH_CCC_HDL,      /* HID Input Report CCC Descriptor */
  HID_INPUT_REPORT_3_REFERENCE_HDL,   /* HID Input Report Reference Descriptor */ 
  HID_OUTPUT_REPORT_CH_HDL,           /* HID Output Report Characteristic Declaration */ 
  HID_OUTPUT_REPORT_HDL,              /* HID Output Report Value */ 
  HID_OUTPUT_REPORT_REFERENCE_HDL,    /* HID Output Report Reference Descriptor */ 
  HID_FEATURE_REPORT_CH_HDL,          /* HID Feature Report Characteristic Declaration */ 
  HID_FEATURE_REPORT_HDL,             /* HID Feature Report Value */ 
  HID_FEATURE_REPORT_REFERENCE_HDL,   /* HID Feature Report Reference Descriptor */ 
  HID_PROTOCOL_MODE_CH_HDL,           /* HID Protocol Mode Characteristic Declaration */ 
  HID_PROTOCOL_MODE_HDL,              /* HID Protocol Mode Value */ 
  HID_MAX_HDL
};


/*************************************************************************************************/
/*!
 *  \fn     SvcHidAddGroup
 *        
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcHidAddGroup(void);

/*************************************************************************************************/
/*!
 *  \fn     SvcHidRemoveGroup
 *        
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcHidRemoveGroup(void);

/*************************************************************************************************/
/*!
 *  \fn     SvcHidRegister
 *        
 *  \brief  Register a read and write callback functions for the ATT Group.
 *
 *  \param  writeCb   Write callback function
 *  \param  readCb    Read callback function
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcHidRegister(attsWriteCback_t writeCb, attsReadCback_t readCb);

/*************************************************************************************************/
/*!
 *  \fn     SvcHidAddGroupDyn
 *
 *  \brief  Add the Hid Service using the dynamic attribute subsystem.
 *
 *  \return None.
 */
/*************************************************************************************************/
void *SvcHidAddGroupDyn(void);

#ifdef __cplusplus
}
#endif
 
#endif /* SVC_HID_H */

