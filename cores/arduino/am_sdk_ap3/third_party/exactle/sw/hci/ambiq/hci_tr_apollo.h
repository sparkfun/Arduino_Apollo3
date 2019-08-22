//*****************************************************************************
//
//! @file hci_tr_apollo.h
//!
//! @brief Additional header information for the Apollo implementation of HCI.
//
//*****************************************************************************

#ifndef HCI_TR_APOLLO_H
#define HCI_TR_APOLLO_H

#ifdef __cplusplus
extern "C" 
{
#endif

//*****************************************************************************
//
// Function prototypes.
//
//*****************************************************************************
extern bool_t hciTrReceivingPacket(void);
extern uint16_t hciTrSerialRxIncoming(uint8_t *pBuf, uint16_t len);

#ifdef __cplusplus
};
#endif

#endif // HCI_TR_APOLLO_H
