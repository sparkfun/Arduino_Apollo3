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
//#include "app_api.h" // oops, i deleted this one... let's see if we can get by without it
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"

#include "am_mcu_apollo.h"
#include "am_util.h"



void setup() {
  // put your setup code here, to run once:

  HciDrvRadioBoot(0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
