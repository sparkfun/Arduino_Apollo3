#ifndef _BLE_EXAMPLE_H_
#define _BLE_EXAMPLE_H_

#include "Arduino.h"

#define DEBUG
#define SERIAL_PORT Serial

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_buf.h"

#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"

#include "am_mcu_apollo.h"
#include "am_util.h"

#include "nus_api.h"
#include "app_ui.h"

#include "wsf_msg.h"

#ifdef __cplusplus
}
#endif


//*****************************************************************************
//
// Forward declarations.
//
//*****************************************************************************
void exactle_stack_init(void);
void scheduler_timer_init(void);
void update_scheduler_timers(void);
void set_next_wakeup(void);
void button_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void AppUiBtnTest(uint8_t btn);

void setAdvName(const char* str);

#endif // _BLE_EXAMPLE_H_