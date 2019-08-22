/*************************************************************************************************/
/*!
 *  \file   wdxs_defs.h
 *
 *  \brief  Wireless Data Exchange Protocol Definitions.
 *
 *          $Date: 2017-03-09 12:18:38 -0600 (Thu, 09 Mar 2017) $
 *          $Revision: 11460 $
 *
 *  Copyright (c) 2013-2017 ARM Ltd., all rights reserved.
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

#ifndef WDXS_DEFS_H
#define WDXS_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Constant Definitions
**************************************************************************************************/

/*! WDXS File List Configuration */
#define WDXS_FLIST_HANDLE               0             /*! File List handle */
#define WDXS_FLIST_FORMAT_VER           1             /*! File List version */
#define WDXS_FLIST_HDR_SIZE             7             /*! File List header length */
#define WDXS_FLIST_RECORD_SIZE          40            /*! File List record length */
#define WDXS_FLIST_MAX_LEN              (WDXS_FLIST_HDR_SIZE + (WDXS_FLIST_RECORD_SIZE * (WSF_EFS_MAX_FILES-1)))

/*! Device configuration characteristic message header length */
#define WDXS_DC_HDR_LEN                 2

/*! Device configuration characteristic operations */
#define WDXS_DC_OP_GET                  0x01         /*! Get a parameter value */
#define WDXS_DC_OP_SET                  0x02         /*! Set a parameter value */
#define WDXS_DC_OP_UPDATE               0x03         /*! Send an update of a parameter value */

/*! Device control characteristic parameter IDs */
#define WDXS_DC_ID_CONN_UPDATE_REQ      0x01         /*! Connection Parameter Update Request */
#define WDXS_DC_ID_CONN_PARAM           0x02         /*! Current Connection Parameters */
#define WDXS_DC_ID_DISCONNECT_REQ       0x03         /*! Disconnect Request */
#define WDXS_DC_ID_CONN_SEC_LEVEL       0x04         /*! Connection Security Level */
#define WDXS_DC_ID_SECURITY_REQ         0x05         /*! Security Request */
#define WDXS_DC_ID_SERVICE_CHANGED      0x06         /*! Service Changed */
#define WDXS_DC_ID_DELETE_BONDS         0x07         /*! Delete Bonds */
#define WDXS_DC_ID_ATT_MTU              0x08         /*! Current ATT MTU */
#define WDXS_DC_ID_BATTERY_LEVEL        0x20         /*! Battery level */
#define WDXS_DC_ID_MODEL_NUMBER         0x21         /*! Device Model */
#define WDXS_DC_ID_FIRMWARE_REV         0x22         /*! Device Firmware Revision */
#define WDXS_DC_ID_ENTER_DIAGNOSTICS    0x23         /*! Enter Diagnostic Mode */
#define WDXS_DC_ID_DIAGNOSTICS_COMPLETE 0x24         /*! Diagnostic Complete */
#define WDXS_DC_ID_DISCONNECT_AND_RESET 0x25         /*! Disconnect and Reset */

/*! Device control parameter lengths */
#define WDXS_DC_LEN_DATA_FORMAT         1            /*! Data format */
#define WDXS_DC_LEN_SEC_LEVEL           1            /*! Security Level */
#define WDXS_DC_LEN_ATT_MTU             2            /*! ATT MTU */
#define WDXS_DC_LEN_BATTERY_LEVEL       1            /*! Battery level */
#define WDXS_DC_LEN_CONN_PARAM_REQ      8            /*! Connection parameter request */
#define WDXS_DC_LEN_CONN_PARAM          7            /*! Current connection parameters */
#define WDXS_DC_LEN_DIAG_COMPLETE       0            /*! Diagnostic complete */
#define WDXS_DC_LEN_DEVICE_MODEL        18           /*! Device Model */
#define WDXS_DC_LEN_FIRMWARE_REV        16           /*! Firmware Revision */

/*! File transfer control characteristic message header length */
#define WDXS_FTC_HDR_LEN                1
#define WDXS_FTC_HANDLE_LEN             2

/*! File transfer control characteristic operations */
#define WDXS_FTC_OP_NONE                0x00        /*! No operation */
#define WDXS_FTC_OP_GET_REQ             0x01        /*! Get a file from the server */
#define WDXS_FTC_OP_GET_RSP             0x02        /*! File get response */
#define WDXS_FTC_OP_PUT_REQ             0x03        /*! Put a file to the server */
#define WDXS_FTC_OP_PUT_RSP             0x04        /*! File put response */
#define WDXS_FTC_OP_ERASE_REQ           0x05        /*! Erase a file on the server */
#define WDXS_FTC_OP_ERASE_RSP           0x06        /*! File erase response */
#define WDXS_FTC_OP_VERIFY_REQ          0x07        /*! Verify a file (e.g. check its CRC) */
#define WDXS_FTC_OP_VERIFY_RSP          0x08        /*! File verify response */
#define WDXS_FTC_OP_ABORT               0x09        /*! Abort a get, put, or list operation in progress */
#define WDXS_FTC_OP_EOF                 0x0a        /*! End of file reached */

/*! File transfer control permissions */
#define WDXS_FTC_GET_PERMITTED          0x01        /*! File Get Permitted */
#define WDXS_FTC_PUT_PERMITTED          0x02        /*! File Put Permitted */
#define WDXS_FTC_ERASE_PERMITTED        0x04        /*! File Erase Permitted */
#define WDXS_FTC_VERIFY_PERMITTED       0x08        /*! File Verify Permitted */

/*! File transfer control characteristic status */
#define WDXS_FTC_ST_SUCCESS             0           /*! Success */
#define WDXS_FTC_ST_INVALID_OP_FILE     1           /*! Invalid operation for this file */
#define WDXS_FTC_ST_INVALID_HANDLE      2           /*! Invalid file handle */
#define WDXS_FTC_ST_INVALID_OP_DATA     3           /*! Invalid operation data */
#define WDXS_FTC_ST_IN_PROGRESS         4           /*! Operation in progress */
#define WDXS_FTC_ST_VERIFICATION        5           /*! Verification failure */

/*! File transfer control transport */
#define WDXS_FTC_TRANSPORT_TYPE         0           /*! Transport Type */
#define WDXS_FTC_TRANSPORT_ID           0x0030      /*! Transport ID */

/*! File transfer data characteristic message header length */
#define WDXS_FTD_HDR_LEN                0

/*! Authentication message header length */
#define WDXS_AU_HDR_LEN                 1

/*! Authentication characteristic operations */
#define WDXS_AU_OP_START                0x01        /*! Authentication start */
#define WDXS_AU_OP_CHALLENGE            0x02        /*! Authentication challenge */
#define WDXS_AU_OP_REPLY                0x03        /*! Authentication reply */

/*! Proprietary ATT error codes */
#define WDXS_APP_AUTH_REQUIRED          0x80        /*! Application authentication required */
#define WDXS_AU_ST_INVALID_MESSAGE      0x81        /*! Authentication invalid message */
#define WDXS_AU_ST_INVALID_STATE        0x82        /*! Authentication invalid state */
#define WDXS_AU_ST_AUTH_FAILED          0x83        /*! Authentication failed */

/*! Authentication characteristic authentication level  */
#define WDXS_AU_LVL_NONE                0x00        /*! None */
#define WDXS_AU_LVL_USER                0x01        /*! User level */
#define WDXS_AU_LVL_MAINT               0x02        /*! Maintenance level */
#define WDXS_AU_LVL_DEBUG               0x03        /*! Debug level */

/*! Authenttication characteristic message parameter lengths */
#define WDXS_AU_MSG_HDR_LEN             1           /*! Message header length */
#define WDXS_AU_PARAM_LEN_START         2           /*! Authentication start */
#define WDXS_AU_PARAM_LEN_CHALLENGE     16          /*! Authentication challenge */
#define WDXS_AU_PARAM_LEN_REPLY         8           /*! Authentication reply */

/*! Authenttication characteristic random number and key lengths */
#define WDXS_AU_RAND_LEN                16          /*! Authentication Random challenge length (bytes)*/
#define WDXS_AU_KEY_LEN                 16          /*! Authentication Key length (bytes) */
#define WDXS_AU_HASH_LEN                8           /*! Authentication Hash length (bytes) */

/* WDXS Media Types */
#define WDXS_FLASH_MEDIA                0
#define WDXS_OTA_MEDIA                  1
#define WDXS_RAM_MEDIA                  2
#define WDXS_STREAM_MEDIA               3

/* WDXS File Transfer Control Command Message Lengths */
#define WDXS_FTC_ABORT_LEN              3
#define WDXS_FTC_ERASE_LEN              3
#define WDXS_FTC_VERIFY_LEN             3
#define WDXS_FTC_PUT_LEN                16
#define WDXS_FTC_GET_LEN                12

#ifdef __cplusplus
}
#endif

#endif /* WDXS_DEFS_H */
