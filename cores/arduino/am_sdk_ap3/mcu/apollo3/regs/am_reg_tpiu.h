//*****************************************************************************
//
// TPIU
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_TPIU_NUM_MODULES                      1
#define AM_REG_TPIUn(n) \
    (REG_TPIU_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// CMSIS-style defines.
//
//*****************************************************************************
#define TPI_CSPSR_CWIDTH_1BIT      1
#define TPI_SPPR_TXMODE_UART       2
#define TPI_ITCTRL_Mode_NORMAL     0
