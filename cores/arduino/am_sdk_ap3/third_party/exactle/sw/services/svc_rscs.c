/*************************************************************************************************/
/*!
 *  \file   svc_rscs.h
 *
 *  \brief  Example Running Speed and Cadence Service Server implementation.
 *
 *          $Date: 2017-02-07 19:05:49 -0600 (Tue, 07 Feb 2017) $
 *          $Revision: 11110 $
 *  
 *  Copyright (c) 2016-2017 ARM Ltd., all rights reserved.
 *  ARM confidential and proprietary.
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

#include "wsf_types.h"
#include "att_api.h"
#include "att_uuid.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "svc_ch.h"
#include "svc_rscs.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef RSCS_SEC_PERMIT_READ
#define RSCS_SEC_PERMIT_READ (ATTS_PERMIT_READ | ATTS_PERMIT_READ_ENC)
#endif

/*! Characteristic write permissions */
#ifndef RSCS_SEC_PERMIT_WRITE
#define RSCS_SEC_PERMIT_WRITE  (ATTS_PERMIT_WRITE | ATTS_PERMIT_WRITE_ENC)
#endif

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* Running Speed service declaration */
static const uint8_t rscsValSvc[] = {UINT16_TO_BYTES(ATT_UUID_RUNNING_SPEED_SERVICE)};
static const uint16_t rscsLenSvc = sizeof(rscsValSvc);


/* Running Speed Feature characteristic */ 
static const uint8_t rscsValFeatureCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(RSCS_RSF_HDL), UINT16_TO_BYTES(ATT_UUID_RUNNING_SPEED_FEATURE)};
static const uint16_t rscsLenFeatureCh = sizeof(rscsValFeatureCh);

/* Running Speed Feature */
/* TODO: Set Supported Feature Bits */
static uint16_t rscsValFeature[] = {RSCS_ISLMS_FEATURE_BIT};
static const uint16_t rscsLenFeature = sizeof(rscsValFeature);


/* Running Speed Measurement characteristic */
static const uint8_t rscsValMeasurementCh[] = { ATT_PROP_NOTIFY, UINT16_TO_BYTES(RSCS_RSM_HDL), UINT16_TO_BYTES(ATT_UUID_RUNNING_SPEED_MEASUREMENT)};
static const uint16_t rscsLenMeasurementCh = sizeof(rscsValMeasurementCh);

/* Running Speed Measurement */
/* Note these are dummy values */
static const uint8_t rscsValMeasurement[] = { 0 };
static const uint16_t rscsLenMeasurement = CH_RSCS_MEASUREMENT_LEN;

/* Running Speed Measurement client characteristic configuration */
static uint8_t rscsValMeasurementChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t rscsLenMeasurementChCcc = sizeof(rscsValMeasurementChCcc);


/* Running Speed Sensor Location characteristic */
static const uint8_t rscsValLocationCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(RSCS_SL_HDL), UINT16_TO_BYTES(ATT_UUID_SENSOR_LOCATION)};
static const uint16_t rscsLenLocationCh = sizeof(rscsValLocationCh);

/* Running Speed Sensor Location */
static uint8_t rscsValLocation[] = { 0 };
static const uint16_t rscsLenLocation = sizeof(rscsValLocation);


/* Attribute list for RSCS group */
static const attsAttr_t rscsList[] =
{
  /* Running Speed Service declaration */
  {
    attPrimSvcUuid, 
    (uint8_t *) rscsValSvc,
    (uint16_t *) &rscsLenSvc, 
    sizeof(rscsValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* Running Speed Feature characteristic */ 
  {
    attChUuid,
    (uint8_t *)rscsValFeatureCh,
    (uint16_t *) &rscsLenFeatureCh,
    sizeof(rscsValFeatureCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Running Speed Feature value */
  {
    attRsfChUuid,
    (uint8_t *)rscsValFeature,
    (uint16_t *) &rscsLenFeature,
    sizeof(rscsValFeature),
    0,
    RSCS_SEC_PERMIT_READ
  },
  /* Running Speed Measurement characteristic */
  {
    attChUuid,
    (uint8_t *)rscsValMeasurementCh,
    (uint16_t *)&rscsLenMeasurementCh,
    sizeof(rscsValMeasurementCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Running Speed Measurement value */
  {
    attRsmChUuid,
    (uint8_t *)rscsValMeasurement,
    (uint16_t *)&rscsLenMeasurement,
    sizeof(rscsValMeasurement),
    0,
    0
  },
  /* Characteristic CCC descriptor */
  {
    attCliChCfgUuid,
    rscsValMeasurementChCcc,
    (uint16_t *)&rscsLenMeasurementChCcc,
    sizeof(rscsValMeasurementChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | RSCS_SEC_PERMIT_WRITE)
  },
  /* Running Speed Sensor Location characteristic */
  {
    attChUuid,
    (uint8_t *)rscsValLocationCh,
    (uint16_t *)&rscsLenLocationCh,
    sizeof(rscsValLocationCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Running Speed Sensor Location value */
  {
    attSlChUuid,
    (uint8_t *)rscsValLocation,
    (uint16_t *)&rscsLenLocation,
    sizeof(rscsValLocation),
    0,
    RSCS_SEC_PERMIT_READ
  },
};

/* RSCS group structure */
static attsGroup_t svcRscsGroup =
{
  NULL,
  (attsAttr_t *) rscsList,
  NULL,
  NULL,
  RSCS_START_HDL,
  RSCS_END_HDL
};

/*************************************************************************************************/
/*!
 *  \fn     SvcRscsAddGroup
 *        
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcRscsAddGroup(void)
{
  AttsAddGroup(&svcRscsGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcRscsRemoveGroup
 *        
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcRscsRemoveGroup(void)
{
  AttsRemoveGroup(RSCS_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcRscsCbackRegister
 *        
 *  \brief  Register callbacks for the service.
 *
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcRscsCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcRscsGroup.readCback = readCback;
  svcRscsGroup.writeCback = writeCback;
}
