//*****************************************************************************
//
// SYSCTRL
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_SYSCTRL_NUM_MODULES                   1
#define AM_REG_SYSCTRLn(n) \
    (REG_SYSCTRL_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// CMSIS-style defines.
//
//*****************************************************************************
#define SCB_CPACR_CP11_Pos                  22
#define SCB_CPACR_CP11_Msk                  0x00C00000
#define SCB_CPACR_CP10_Pos                  20
#define SCB_CPACR_CP10_Msk                  0x00300000
