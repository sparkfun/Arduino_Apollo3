/*************************************************************************************************/
/*!
 *  \file   svc_cscs.h
 *
 *  \brief  Example Cycling Speed and Cadence Service Server implementation.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
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
#include "svc_cscs.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef CSCS_SEC_PERMIT_READ
#define CSCS_SEC_PERMIT_READ (ATTS_PERMIT_READ | ATTS_PERMIT_READ_ENC)
#endif

/*! Characteristic write permissions */
#ifndef CSCS_SEC_PERMIT_WRITE
#define CSCS_SEC_PERMIT_WRITE  (ATTS_PERMIT_WRITE | ATTS_PERMIT_WRITE_ENC)
#endif

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* Cycling Speed service declaration */
static const uint8_t cscsValSvc[] = {UINT16_TO_BYTES(ATT_UUID_CYCLING_SPEED_SERVICE)};
static const uint16_t cscsLenSvc = sizeof(cscsValSvc);


/* Cycling Speed Feature characteristic */ 
static const uint8_t cscsValFeatureCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(CSCS_CSF_HDL), UINT16_TO_BYTES(ATT_UUID_CYCLING_SPEED_FEATURE)};
static const uint16_t cscsLenFeatureCh = sizeof(cscsValFeatureCh);

/* Cycling Speed Feature */
/* TODO: Set Supported Feature Bits */
static uint16_t cscsValFeature[] = {CSCS_WRDS_FEATURE_BIT};
static const uint16_t cscsLenFeature = sizeof(cscsValFeature);


/* Cycling Speed Measurement characteristic */
static const uint8_t cscsValMeasurementCh[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(CSCS_CSM_HDL), UINT16_TO_BYTES(ATT_UUID_CYCLING_SPEED_MEASUREMENT)};
static const uint16_t cscsLenMeasurementCh = sizeof(cscsValMeasurementCh);

/* Cycling Speed Measurement */
/* Note these are dummy values */
static const uint8_t cscsValMeasurement[] = { 0 };
static const uint16_t cscsLenMeasurement = CH_CSCS_MEASUREMENT_LEN;

/* Cycling Speed Measurement client characteristic configuration */
static uint8_t cscsValMeasurementChCcc[] = {UINT16_TO_BYTES(0x0000) };
static const uint16_t cscsLenMeasurementChCcc = sizeof(cscsValMeasurementChCcc);


/* Cycling Speed Sensor Location characteristic */
static const uint8_t cscsValLocationCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(CSCS_SL_HDL), UINT16_TO_BYTES(ATT_UUID_SENSOR_LOCATION)};
static const uint16_t cscsLenLocationCh = sizeof(cscsValLocationCh);

/* Cycling Speed Sensor Location */
/* Note these are dummy values */
static uint8_t cscsValLocation[] = { 0 };
static const uint16_t cscsLenLocation = sizeof(cscsValLocation);


/* Attribute list for CSCS group */
static const attsAttr_t cscsList[] =
{
  /* Cycling Speed Service declaration */
  {
    attPrimSvcUuid, 
    (uint8_t *) cscsValSvc,
    (uint16_t *) &cscsLenSvc, 
    sizeof(cscsValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* Cycling Speed Feature characteristic */ 
  {
    attChUuid,
    (uint8_t *)cscsValFeatureCh,
    (uint16_t *) &cscsLenFeatureCh,
    sizeof(cscsValFeatureCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Cycling Speed Feature value */
  {
    attCsfChUuid,
    (uint8_t *)cscsValFeature,
    (uint16_t *) &cscsLenFeature,
    sizeof(cscsValFeature),
    0,
    CSCS_SEC_PERMIT_READ
  },
  /* Cycling Speed Measurement characteristic */
  {
    attChUuid,
    (uint8_t *)cscsValMeasurementCh,
    (uint16_t *)&cscsLenMeasurementCh,
    sizeof(cscsValMeasurementCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Cycling Speed Measurement value */
  {
    attCsmChUuid,
    (uint8_t *)cscsValMeasurement,
    (uint16_t *)&cscsLenMeasurement,
    sizeof(cscsValMeasurement),
    0,
    0
  },
  /* Characteristic CCC descriptor */
  {
    attCliChCfgUuid,
    cscsValMeasurementChCcc,
    (uint16_t *)&cscsLenMeasurementChCcc,
    sizeof(cscsValMeasurementChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | CSCS_SEC_PERMIT_WRITE)
  },
  /* Cycling Speed Sensor Location characteristic */
  {
    attChUuid,
    (uint8_t *)cscsValLocationCh,
    (uint16_t *)&cscsLenLocationCh,
    sizeof(cscsValLocationCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Cycling Speed Sensor Location value */
  {
    attSlChUuid,
    (uint8_t *)cscsValLocation,
    (uint16_t *)&cscsLenLocation,
    sizeof(cscsValLocation),
    0,
    CSCS_SEC_PERMIT_READ
  },
};

/* CSCS group structure */
static attsGroup_t svcCscsGroup =
{
  NULL,
  (attsAttr_t *) cscsList,
  NULL,
  NULL,
  CSCS_START_HDL,
  CSCS_END_HDL
};

/*************************************************************************************************/
/*!
 *  \fn     SvcCscsAddGroup
 *        
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCscsAddGroup(void)
{
  AttsAddGroup(&svcCscsGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcCscsRemoveGroup
 *        
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCscsRemoveGroup(void)
{
  AttsRemoveGroup(CSCS_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcCscsCbackRegister
 *        
 *  \brief  Register callbacks for the service.
 *
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCscsCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcCscsGroup.readCback = readCback;
  svcCscsGroup.writeCback = writeCback;
}
