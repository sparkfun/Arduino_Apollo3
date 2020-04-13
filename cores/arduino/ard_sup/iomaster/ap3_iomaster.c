#include "am_mcu_apollo.h"


//*****************************************************************************
//
// Private Types. (Copied from am_hal_iom.c)
//
//*****************************************************************************

#ifdef __IAR_SYSTEMS_ICC__
#define AM_INSTR_CLZ(n)                     __CLZ(n)
#else
#define AM_INSTR_CLZ(n)                     __builtin_clz(n)
#endif

#define AM_HAL_MAGIC_IOM            0x123456
#define AM_HAL_IOM_CHK_HANDLE(h)    ((h) && ((am_hal_handle_prefix_t *)(h))->s.bInit && (((am_hal_handle_prefix_t *)(h))->s.magic == AM_HAL_MAGIC_IOM))

//
// Command Queue entry structure.
//
typedef struct
{
#if (AM_HAL_IOM_CQ == 1)
    uint32_t    ui32PAUSENAddr;
    uint32_t    ui32PAUSEENVal;
    uint32_t    ui32PAUSEN2Addr;
    uint32_t    ui32PAUSEEN2Val;
#endif
    uint32_t    ui32OFFSETHIAddr;
    uint32_t    ui32OFFSETHIVal;
    uint32_t    ui32DEVCFGAddr;
    uint32_t    ui32DEVCFGVal;
    uint32_t    ui32DMACFGdis1Addr;
    uint32_t    ui32DMACFGdis1Val;
    uint32_t    ui32DMATOTCOUNTAddr;
    uint32_t    ui32DMATOTCOUNTVal;
    uint32_t    ui32DMATARGADDRAddr;
    uint32_t    ui32DMATARGADDRVal;
    uint32_t    ui32DMACFGAddr;
    uint32_t    ui32DMACFGVal;
    // CMDRPT register has been repurposed for DCX
    uint32_t    ui32DCXAddr;
    uint32_t    ui32DCXVal;
    uint32_t    ui32CMDAddr;
    uint32_t    ui32CMDVal;
#if (AM_HAL_IOM_CQ == 1)
    uint32_t    ui32SETCLRAddr;
    uint32_t    ui32SETCLRVal;
#endif
} am_hal_iom_txn_cmdlist_t;

//
// Command Queue entry structure for Sequence Repeat
//
typedef struct
{
    uint32_t                    ui32PAUSENAddr;
    uint32_t                    ui32PAUSEENVal;
    uint32_t                    ui32PAUSEN2Addr;
    uint32_t                    ui32PAUSEEN2Val;
    uint32_t                    ui32SETCLRAddr;
    uint32_t                    ui32SETCLRVal;
} am_hal_iom_cq_loop_entry_t;

#define AM_HAL_IOM_MAX_PENDING_TRANSACTIONS      256 // Must be power of 2 for the implementation below
#define AM_HAL_IOM_CQUPD_INT_FLAG                (0x00000001)

typedef struct
{
    bool        bValid;
    uint32_t    regFIFOTHR;
    uint32_t    regDMATRIGEN;
    uint32_t    regCLKCFG;
    uint32_t    regSUBMODCTRL;
    uint32_t    regCQCFG;
    uint32_t    regCQADDR;
    uint32_t    regCQFLAGS;
    uint32_t    regCQPAUSEEN;
    uint32_t    regCQCURIDX;
    uint32_t    regCQENDIDX;
    uint32_t    regMSPICFG;
    uint32_t    regMI2CCFG;
    uint32_t    regINTEN;
} am_hal_iom_register_state_t;

typedef enum
{
    AM_HAL_IOM_SEQ_NONE,
    AM_HAL_IOM_SEQ_UNDER_CONSTRUCTION,
    AM_HAL_IOM_SEQ_RUNNING,
} am_hal_iom_seq_e;

typedef struct
{
    uint32_t                    ui32OFFSETHIVal;
    uint32_t                    ui32DEVCFGVal;
    uint32_t                    ui32DMATOTCOUNTVal;
    uint32_t                    ui32DMATARGADDRVal;
    uint32_t                    ui32DMACFGVal;
    uint32_t                    ui32CMDVal;
    am_hal_iom_callback_t       pfnCallback;
    void                        *pCallbackCtxt;
} am_hal_iom_dma_entry_t;

typedef struct
{
    am_hal_handle_prefix_t  prefix;
    //
    // Physical module number.
    //
    uint32_t                ui32Module;

    //
    // Interface mode (SPI or I2C).
    //
    am_hal_iom_mode_e       eInterfaceMode;
    uint32_t                *pNBTxnBuf;
    uint32_t                ui32NBTxnBufLength;

    uint32_t                ui32UserIntCfg;
    uint32_t                ui32TxnInt;

    uint32_t                ui32LastIdxProcessed;
    uint32_t                ui32MaxTransactions;
    volatile uint32_t       ui32NumPendTransactions;
    //
    // Stores the CQ callbacks.
    //
    am_hal_iom_callback_t   pfnCallback[AM_HAL_IOM_MAX_PENDING_TRANSACTIONS];
    void                    *pCallbackCtxt[AM_HAL_IOM_MAX_PENDING_TRANSACTIONS];
#if (AM_HAL_IOM_CQ == 1)
    void                    *pCmdQHdl;
    // To support sequence
    am_hal_iom_seq_e        eSeq;
    bool                    bAutonomous;
    // This is used to track the number of transactions in a sequence
    uint32_t                ui32NumSeqTransactions;
    volatile bool           bRestart;
    uint32_t                block;
    // To support high priority transactions - out of band
    // High Priority DMA transactions
    volatile bool           bHP;
    uint32_t                ui32NumHPEntries;
    uint32_t                ui32NumHPPendingEntries;
    uint32_t                ui32MaxHPTransactions;
    uint32_t                ui32NextHPIdx;
    uint32_t                ui32LastHPIdxProcessed;
    am_hal_iom_dma_entry_t  *pHPTransactions;
#else
    uint32_t                ui32NextIdx;
    am_hal_iom_txn_cmdlist_t   *pTransactions;
#endif
    //
    // Delay timeout value.
    //
    uint32_t                waitTimeout;
    // Configured clock time
    uint32_t                ui32BitTimeTicks;

    am_hal_iom_register_state_t registerState;
    uint8_t                 dcx[AM_HAL_IOM_MAX_CS_SPI + 1];

} am_hal_iom_state_t;

//*****************************************************************************
// onebit()
//*****************************************************************************
//
// A power of 2?
// Return true if ui32Value has exactly 1 bit set, otherwise false.
//
static bool
onebit(uint32_t ui32Value)
{
    return ui32Value  &&  !(ui32Value & (ui32Value - 1));
} // onebit()

//*****************************************************************************
// compute_freq()
//*****************************************************************************
//
// Compute the interface frequency based on the given parameters
//
static uint32_t
compute_freq(uint32_t ui32HFRCfreqHz,
             uint32_t ui32Fsel, uint32_t ui32Div3,
             uint32_t ui32DivEn, uint32_t ui32TotPer)
{
    uint32_t ui32Denomfinal, ui32ClkFreq;

    ui32Denomfinal = ((1 << (ui32Fsel - 1)) * (1 + ui32Div3 * 2) * (1 + ui32DivEn * (ui32TotPer)));
    ui32ClkFreq = (ui32HFRCfreqHz) / ui32Denomfinal;                           // Compute the set frequency value
    ui32ClkFreq +=  (((ui32HFRCfreqHz) % ui32Denomfinal) > (ui32Denomfinal / 2)) ? 1 : 0;

    return ui32ClkFreq;
} // compute_freq()

//*****************************************************************************
//
// iom_get_interface_clock_cfg()
//
// Returns the proper settings for the CLKCFG register.
//
// ui32FreqHz - The desired interface frequency in Hz.
//
// Given a desired serial interface clock frequency, this function computes
// the appropriate settings for the various fields in the CLKCFG register
// and returns the 32-bit value that should be written to that register.
// The actual interface frequency may be slightly lower than the specified
// frequency, but the actual frequency is also returned.
//
// Note A couple of criteria that this algorithm follow are:
//  1. For power savings, choose the highest FSEL possible.
//  2. Use DIV3 when possible rather than DIVEN.
//
// Return An unsigned 64-bit value.
// The lower 32-bits represent the value to use to set CLKCFG.
// The upper 32-bits represent the actual frequency (in Hz) that will result
// from setting CLKCFG with the lower 32-bits.
//
// 0 (64 bits) = error. Note that the caller must check the entire 64 bits.
// It is not an error if only the low 32-bits are 0 (this is a valid value).
// But the entire 64 bits returning 0 is an error.
//!
//*****************************************************************************
static
uint64_t iom_get_interface_clock_cfg(uint32_t ui32FreqHz, uint32_t ui32Phase )
{
    uint32_t ui32Fsel, ui32Div3, ui32DivEn, ui32TotPer, ui32LowPer;
    uint32_t ui32Denom, ui32v1, ui32Denomfinal, ui32ClkFreq, ui32ClkCfg;
    uint32_t ui32HFRCfreqHz;
    int32_t i32Div, i32N;

    if ( ui32FreqHz == 0 )
    {
        return 0;
    }

    //
    // Set the HFRC clock frequency.
    //
    ui32HFRCfreqHz = AM_HAL_CLKGEN_FREQ_MAX_HZ;

    //
    // Compute various parameters used for computing the optimal CLKCFG setting.
    //
    i32Div = (ui32HFRCfreqHz / ui32FreqHz) + ((ui32HFRCfreqHz % ui32FreqHz) ? 1 : 0);    // Round up (ceiling)

    //
    // Compute N (count the number of LS zeros of Div) = ctz(Div) = log2(Div & (-Div))
    //
    i32N = 31 - AM_INSTR_CLZ((i32Div & (-i32Div)));

    if ( i32N > 6 )
    {
        i32N = 6;
    }

    ui32Div3 = ( (ui32FreqHz < (ui32HFRCfreqHz / 16384))            ||
                 ( ((ui32FreqHz >= (ui32HFRCfreqHz / 3))    &&
                    (ui32FreqHz <= ((ui32HFRCfreqHz / 2) - 1)) ) ) ) ? 1 : 0;
    ui32Denom = ( 1 << i32N ) * ( 1 + (ui32Div3 * 2) );
    ui32TotPer = i32Div / ui32Denom;
    ui32TotPer += (i32Div % ui32Denom) ? 1 : 0;
    ui32v1 = 31 - AM_INSTR_CLZ(ui32TotPer);     // v1 = log2(TotPer)
    ui32Fsel = (ui32v1 > 7) ? ui32v1 + i32N - 7 : i32N;
    ui32Fsel++;

    if ( ui32Fsel > 7 )
    {
        //
        // This is an error, can't go that low.
        //
        return 0;
    }

    if ( ui32v1 > 7 )
    {
        ui32DivEn = ui32TotPer;     // Save TotPer for the round up calculation
        ui32TotPer = ui32TotPer>>(ui32v1-7);
        ui32TotPer += ((ui32DivEn) % (1 << (ui32v1 - 7))) ? 1 : 0;
    }

    ui32DivEn = ( (ui32FreqHz >= (ui32HFRCfreqHz / 4)) ||
                  ((1 << (ui32Fsel - 1)) == i32Div) ) ? 0 : 1;

    if (ui32Phase == 1)
    {
        ui32LowPer = (ui32TotPer - 2) / 2;          // Longer high phase
    }
    else
    {
        ui32LowPer = (ui32TotPer - 1) / 2;          // Longer low phase
    }

    ui32ClkCfg = _VAL2FLD(IOM0_CLKCFG_FSEL,   ui32Fsel)     |
                 _VAL2FLD(IOM0_CLKCFG_DIV3,   ui32Div3)     |
                 _VAL2FLD(IOM0_CLKCFG_DIVEN,  ui32DivEn)    |
                 _VAL2FLD(IOM0_CLKCFG_LOWPER, ui32LowPer)   |
                 _VAL2FLD(IOM0_CLKCFG_TOTPER, ui32TotPer - 1);

    //
    // Now, compute the actual frequency, which will be returned.
    //
    ui32ClkFreq = compute_freq(ui32HFRCfreqHz, ui32Fsel, ui32Div3, ui32DivEn, ui32TotPer - 1);

    //
    // Determine if the actual frequency is a power of 2 (MHz).
    //
    if ( (ui32ClkFreq % 250000) == 0 )
    {
        //
        // If the actual clock frequency is a power of 2 ranging from 250KHz up,
        // we can simplify the CLKCFG value using DIV3 (which also results in a
        // better duty cycle).
        //
        ui32Denomfinal = ui32ClkFreq / (uint32_t)250000;

        if ( onebit(ui32Denomfinal) )
        {
            //
            // These configurations can be simplified by using DIV3.  Configs
            // using DIV3 have a 50% duty cycle, while those from DIVEN will
            // have a 66/33 duty cycle.
            //
            ui32TotPer = ui32LowPer = ui32DivEn = 0;
            ui32Div3 = 1;

            //
            // Now, compute the return values.
            //
            ui32ClkFreq = compute_freq(ui32HFRCfreqHz, ui32Fsel, ui32Div3, ui32DivEn, ui32TotPer);

    ui32ClkCfg = _VAL2FLD(IOM0_CLKCFG_FSEL,   ui32Fsel)     |
                 _VAL2FLD(IOM0_CLKCFG_DIV3,   1)            |
                 _VAL2FLD(IOM0_CLKCFG_DIVEN,  0)            |
                 _VAL2FLD(IOM0_CLKCFG_LOWPER, 0)            |
                 _VAL2FLD(IOM0_CLKCFG_TOTPER, 0);
        }
    }

    return ( ((uint64_t)ui32ClkFreq) << 32) | (uint64_t)ui32ClkCfg;

} //iom_get_interface_clock_cfg()

//*****************************************************************************
//
// IOM configuration function.
// Additional functionality added over Ambiq HAL version (am_hal_iom_configure)
// - Clock frequency now supports arbitrary values. Best configuration computed and applied automatically
//
//*****************************************************************************
uint32_t
ap3_iom_configure(void *pHandle, am_hal_iom_config_t *psConfig)
{
    uint32_t ui32ClkCfg;
    am_hal_iom_state_t *pIOMState = (am_hal_iom_state_t*)pHandle;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (!AM_HAL_IOM_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }

    //
    // Validate the parameters
    //
    if ( (pHandle == NULL)      ||
         (psConfig == NULL)     ||
         (pIOMState->ui32Module >= AM_REG_IOM_NUM_MODULES) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    // Configure not allowed in Enabled state
    if (pIOMState->prefix.s.bEnable)
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    ui32Module = pIOMState->ui32Module;
    //
    // Save the interface mode and chip select in the global handle.
    //
    pIOMState->eInterfaceMode = psConfig->eInterfaceMode;

    //
    // Set the IOM read/write FIFO thresholds to default values.
    //
    IOMn(ui32Module)->FIFOTHR =
        _VAL2FLD(IOM0_FIFOTHR_FIFORTHR, 16) |
        _VAL2FLD(IOM0_FIFOTHR_FIFOWTHR, 16);

    if ( psConfig->eInterfaceMode == AM_HAL_IOM_SPI_MODE )
    {
#ifndef AM_HAL_DISABLE_API_VALIDATION
        //
        // Validate the SPI mode
        //
        if ( psConfig->eSpiMode > AM_HAL_IOM_SPI_MODE_3 )
        {
            return AM_HAL_STATUS_INVALID_ARG;
        }
        if (psConfig->ui32ClockFreq > AM_HAL_IOM_MAX_FREQ)
        {
            return AM_HAL_STATUS_INVALID_ARG;
        }
#endif // AM_HAL_DISABLE_API_VALIDATION

        //
        // Determine the CLKCFG value for SPI.
        //
        ui32ClkCfg = iom_get_interface_clock_cfg(psConfig->ui32ClockFreq, (psConfig->eSpiMode & 2) >> 1);

        //
        // Set the SPI configuration.
        //
        IOMn(ui32Module)->MSPICFG =
            ( ((psConfig->eSpiMode << IOM0_MSPICFG_SPOL_Pos) & (IOM0_MSPICFG_SPHA_Msk | IOM0_MSPICFG_SPOL_Msk))  |
             _VAL2FLD(IOM0_MSPICFG_FULLDUP, 0)                              |
             _VAL2FLD(IOM0_MSPICFG_WTFC,    IOM0_MSPICFG_WTFC_DIS)          |
             _VAL2FLD(IOM0_MSPICFG_RDFC,    IOM0_MSPICFG_RDFC_DIS)          |
             _VAL2FLD(IOM0_MSPICFG_MOSIINV, IOM0_MSPICFG_MOSIINV_NORMAL)    |
             _VAL2FLD(IOM0_MSPICFG_WTFCIRQ, IOM0_MSPICFG_WTFCIRQ_MISO)      |
             _VAL2FLD(IOM0_MSPICFG_WTFCPOL, IOM0_MSPICFG_WTFCPOL_HIGH)      |
             _VAL2FLD(IOM0_MSPICFG_RDFCPOL, IOM0_MSPICFG_RDFCPOL_HIGH)      |
             _VAL2FLD(IOM0_MSPICFG_SPILSB,  IOM0_MSPICFG_SPILSB_MSB)        |
             _VAL2FLD(IOM0_MSPICFG_DINDLY,  0)                              |
             _VAL2FLD(IOM0_MSPICFG_DOUTDLY, 0)                              |
             _VAL2FLD(IOM0_MSPICFG_MSPIRST, 0) );
    }
    else if ( psConfig->eInterfaceMode == AM_HAL_IOM_I2C_MODE )
    {

        switch (psConfig->ui32ClockFreq)
        {
            case AM_HAL_IOM_100KHZ:
                //
                // settings below should give ~100 kHz
                //
                ui32ClkCfg = _VAL2FLD(IOM0_CLKCFG_TOTPER, 0x77)                     |
                             _VAL2FLD(IOM0_CLKCFG_LOWPER, 0x3B)                     |
                             _VAL2FLD(IOM0_CLKCFG_DIVEN, IOM0_CLKCFG_DIVEN_EN)      |
                             _VAL2FLD(IOM0_CLKCFG_DIV3, IOM0_CLKCFG_DIV3_DIS)       |
                             _VAL2FLD(IOM0_CLKCFG_FSEL, IOM0_CLKCFG_FSEL_HFRC_DIV2) |
                             _VAL2FLD(IOM0_CLKCFG_IOCLKEN, 1);
                IOMn(ui32Module)->MI2CCFG = _VAL2FLD(IOM0_MI2CCFG_STRDIS, 0)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_SMPCNT, 3)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_SDAENDLY, 15)                         |
                                            _VAL2FLD(IOM0_MI2CCFG_SCLENDLY, 0)                          |
                                            _VAL2FLD(IOM0_MI2CCFG_MI2CRST, 1)                           |
                                            _VAL2FLD(IOM0_MI2CCFG_SDADLY, 3)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_ARBEN, IOM0_MI2CCFG_ARBEN_ARBDIS)     |
                                            _VAL2FLD(IOM0_MI2CCFG_I2CLSB, IOM0_MI2CCFG_I2CLSB_MSBFIRST) |
                                            _VAL2FLD(IOM0_MI2CCFG_ADDRSZ, IOM0_MI2CCFG_ADDRSZ_ADDRSZ7);
                break;
            case AM_HAL_IOM_400KHZ:
                //
                // settings below should give ~400 kHz
                //
                ui32ClkCfg = _VAL2FLD(IOM0_CLKCFG_TOTPER, 0x1D)                     |
                             _VAL2FLD(IOM0_CLKCFG_LOWPER, 0x0E)                     |
                             _VAL2FLD(IOM0_CLKCFG_DIVEN, IOM0_CLKCFG_DIVEN_EN)      |
                             _VAL2FLD(IOM0_CLKCFG_DIV3, IOM0_CLKCFG_DIV3_DIS)       |
                             _VAL2FLD(IOM0_CLKCFG_FSEL, IOM0_CLKCFG_FSEL_HFRC_DIV2) |
                             _VAL2FLD(IOM0_CLKCFG_IOCLKEN, 1);
                IOMn(ui32Module)->MI2CCFG = _VAL2FLD(IOM0_MI2CCFG_STRDIS, 0)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_SMPCNT, 3)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_SDAENDLY, 15)                         |
                                            _VAL2FLD(IOM0_MI2CCFG_SCLENDLY, 2)                          |
                                            _VAL2FLD(IOM0_MI2CCFG_MI2CRST, 1)                           |
                                            _VAL2FLD(IOM0_MI2CCFG_SDADLY, 3)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_ARBEN, IOM0_MI2CCFG_ARBEN_ARBDIS)     |
                                            _VAL2FLD(IOM0_MI2CCFG_I2CLSB, IOM0_MI2CCFG_I2CLSB_MSBFIRST) |
                                            _VAL2FLD(IOM0_MI2CCFG_ADDRSZ, IOM0_MI2CCFG_ADDRSZ_ADDRSZ7);
                break;
            case AM_HAL_IOM_1MHZ:
                //
                // settings below should give ~860 kHz
                //
                ui32ClkCfg = _VAL2FLD(IOM0_CLKCFG_TOTPER, 0x06)                     |
                             _VAL2FLD(IOM0_CLKCFG_LOWPER, 0x03)                     |
                             _VAL2FLD(IOM0_CLKCFG_DIVEN, IOM0_CLKCFG_DIVEN_EN)      |
                             _VAL2FLD(IOM0_CLKCFG_DIV3, IOM0_CLKCFG_DIV3_DIS)       |
                             _VAL2FLD(IOM0_CLKCFG_FSEL, IOM0_CLKCFG_FSEL_HFRC_DIV4) |
                             _VAL2FLD(IOM0_CLKCFG_IOCLKEN, 1);
                IOMn(ui32Module)->MI2CCFG = _VAL2FLD(IOM0_MI2CCFG_STRDIS, 0)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_SMPCNT, 0x21)                         |
                                            _VAL2FLD(IOM0_MI2CCFG_SDAENDLY, 3)                          |
                                            _VAL2FLD(IOM0_MI2CCFG_SCLENDLY, 0)                          |
                                            _VAL2FLD(IOM0_MI2CCFG_MI2CRST, 1)                           |
                                            _VAL2FLD(IOM0_MI2CCFG_SDADLY, 0)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_ARBEN, IOM0_MI2CCFG_ARBEN_ARBDIS)     |
                                            _VAL2FLD(IOM0_MI2CCFG_I2CLSB, IOM0_MI2CCFG_I2CLSB_MSBFIRST) |
                                            _VAL2FLD(IOM0_MI2CCFG_ADDRSZ, IOM0_MI2CCFG_ADDRSZ_ADDRSZ7);
                break;
            default:
            {
                //Calculate TOTPER and FSEL based on requested frequency
                uint32_t reqFreq = psConfig->ui32ClockFreq;
                uint32_t fsel = 2;
                uint32_t totper = 0;
                for( ; fsel < 128 ; fsel = fsel * 2)
                {
                    //IOM and HFRC are not affected by burst mode
                    totper = (48000000 / (2 * fsel))/reqFreq - 1;
                    if(totper < 256) break;
                }

                if(fsel == 128)
                {
                    //If fsel is too large, return with error
                    return AM_HAL_STATUS_INVALID_ARG;
                }

                uint32_t fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV2;

                if(fsel == 2)
                    fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV2;
                else if(fsel == 4)
                    fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV4;
                else if(fsel == 8)
                    fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV8;
                else if(fsel == 16)
                    fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV16;
                else if(fsel == 32)
                    fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV32;
                else if(fsel == 64)
                    fsel_bitvalue = IOM0_CLKCFG_FSEL_HFRC_DIV64;

                ui32ClkCfg = _VAL2FLD(IOM0_CLKCFG_TOTPER, totper)                     |
                            _VAL2FLD(IOM0_CLKCFG_LOWPER, totper/2)                     |
                            _VAL2FLD(IOM0_CLKCFG_DIVEN, IOM0_CLKCFG_DIVEN_EN)      |
                            _VAL2FLD(IOM0_CLKCFG_DIV3, IOM0_CLKCFG_DIV3_DIS)       |
                            _VAL2FLD(IOM0_CLKCFG_FSEL, fsel_bitvalue) |
                            _VAL2FLD(IOM0_CLKCFG_IOCLKEN, 1);
                IOMn(ui32Module)->MI2CCFG = _VAL2FLD(IOM0_MI2CCFG_STRDIS, 0)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_SMPCNT, 0x21)                         |
                                            _VAL2FLD(IOM0_MI2CCFG_SDAENDLY, 3)                          |
                                            _VAL2FLD(IOM0_MI2CCFG_SCLENDLY, 0)                          |
                                            _VAL2FLD(IOM0_MI2CCFG_MI2CRST, 1)                           |
                                            _VAL2FLD(IOM0_MI2CCFG_SDADLY, 0)                            |
                                            _VAL2FLD(IOM0_MI2CCFG_ARBEN, IOM0_MI2CCFG_ARBEN_ARBDIS)     |
                                            _VAL2FLD(IOM0_MI2CCFG_I2CLSB, IOM0_MI2CCFG_I2CLSB_MSBFIRST) |
                                            _VAL2FLD(IOM0_MI2CCFG_ADDRSZ, IOM0_MI2CCFG_ADDRSZ_ADDRSZ7);
                break;

            }
        }

    }
    else
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    //
    // Enable and set the clock configuration.
    //
    ui32ClkCfg |= _VAL2FLD(IOM0_CLKCFG_IOCLKEN, 1);
    IOMn(ui32Module)->CLKCFG = ui32ClkCfg;

    pIOMState->ui32BitTimeTicks = AM_HAL_CLKGEN_FREQ_MAX_HZ / psConfig->ui32ClockFreq;

    //
    // Set the delay timeout value to the default maximum value.
    //
    pIOMState->waitTimeout = 1000;

    pIOMState->pNBTxnBuf = psConfig->pNBTxnBuf;
    pIOMState->ui32NBTxnBufLength = psConfig->ui32NBTxnBufLength;
    // Disable the DCX
    for (uint8_t i = 0; i <= AM_HAL_IOM_MAX_CS_SPI; i++)
    {
        pIOMState->dcx[i] = 0;
    }

    //
    // Return the status.
    //
    return status;

} // am_hal_iom_configure()