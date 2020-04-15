#include "ap3_iomaster_types.h"
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
                    totper = (AM_HAL_IOM_48MHZ / (2 * fsel))/reqFreq - 1;
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

// SDK 2.4.2 fix... copying fullduplex function from previous SDK (2.2.0) (also need some supporting functions)
// Some declarations of functions that will be used from am_hal_iom.c (this is ugly c hacks... bear with me)
uint32_t internal_iom_get_int_err(uint32_t ui32Module, uint32_t ui32IntStatus);
uint32_t validate_transaction(am_hal_iom_state_t *pIOMState, am_hal_iom_transfer_t *psTransaction, bool bBlocking);
//*****************************************************************************
//
// Function to build the CMD value.
// Returns the CMD value, but does not set the CMD register.
//
// The OFFSETHI register must still be handled by the caller, e.g.
//      AM_REGn(IOM, ui32Module, OFFSETHI) = (uint16_t)(ui32Offset >> 8);
//
//*****************************************************************************
static uint32_t
build_cmd(uint32_t ui32CS,     uint32_t ui32Dir, uint32_t ui32Cont,
          uint32_t ui32Offset, uint32_t ui32OffsetCnt,
          uint32_t ui32nBytes)
{
    //
    // Initialize the CMD variable
    //
    uint32_t ui32Cmd = 0;

    //
    // If SPI, we'll need the chip select
    //
    ui32Cmd |= _VAL2FLD(IOM0_CMD_CMDSEL, ui32CS);

    //
    // Build the CMD with number of bytes and direction.
    //
    ui32Cmd |= _VAL2FLD(IOM0_CMD_TSIZE, ui32nBytes);

    if (ui32Dir == AM_HAL_IOM_RX)
    {
        ui32Cmd |= _VAL2FLD(IOM0_CMD_CMD, IOM0_CMD_CMD_READ);
    }
    else
    {
        ui32Cmd |= _VAL2FLD(IOM0_CMD_CMD, IOM0_CMD_CMD_WRITE);
    }

    ui32Cmd |= _VAL2FLD(IOM0_CMD_CONT, ui32Cont);

    //
    // Now add the OFFSETLO and OFFSETCNT information.
    //
    ui32Cmd |= _VAL2FLD(IOM0_CMD_OFFSETLO, (uint8_t)ui32Offset);
    ui32Cmd |= _VAL2FLD(IOM0_CMD_OFFSETCNT, ui32OffsetCnt);

    return ui32Cmd;
} // build_cmd()

static void
internal_iom_reset_on_error(am_hal_iom_state_t  *pIOMState, uint32_t ui32IntMask)
{
    uint32_t iterationsToWait = 2 * pIOMState->ui32BitTimeTicks; // effectively > 6 clocks
    uint32_t ui32Module = pIOMState->ui32Module;
    uint32_t curIntCfg = IOMn(ui32Module)->INTEN;
    IOMn(ui32Module)->INTEN = 0;

    // Disable interrupts temporarily
    if (ui32IntMask & AM_HAL_IOM_INT_DERR)
    {
        if ((IOMn(ui32Module)->DMACFG & IOM0_DMACFG_DMADIR_Msk) == _VAL2FLD(IOM0_DMACFG_DMADIR, IOM0_DMACFG_DMADIR_M2P))
        {
            // Write
            uint32_t dummy = 0xDEADBEEF;
            uint32_t numBytesRemaining = IOMn(ui32Module)->DMATOTCOUNT;

            while (numBytesRemaining)
            {
                if (IOMn(ui32Module)->FIFOPTR_b.FIFO0REM >= 4)
                {
                    // Write one 4-byte word to FIFO
                    IOMn(ui32Module)->FIFOPUSH = dummy;
                    if (numBytesRemaining > 4)
                    {
                        numBytesRemaining -= 4;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            // Now wait for command to finish
            while ((IOMn(ui32Module)->STATUS & (IOM0_STATUS_IDLEST_Msk | IOM0_STATUS_CMDACT_Msk)) != IOM0_STATUS_IDLEST_Msk);
        }
        else
        {
            // Read
            // Let command finish
            while (IOMn(ui32Module)->STATUS_b.CMDACT)
            {
                while (IOMn(ui32Module)->FIFOPTR_b.FIFO1SIZ >= 4)
                {
                    // Read one 4-byte word from FIFO
                    IOMn(ui32Module)->FIFOPOP;
#if MANUAL_POP
                    IOMn(ui32Module)->FIFOPOP = 0x11111111;
#endif
                }
            }
            // Now wait for command to finish
            while ((IOMn(ui32Module)->STATUS & (IOM0_STATUS_IDLEST_Msk | IOM0_STATUS_CMDACT_Msk)) != IOM0_STATUS_IDLEST_Msk);
            // Flush any remaining data from FIFO
            while  (IOMn(ui32Module)->FIFOPTR_b.FIFO1SIZ)
            {
                while (IOMn(ui32Module)->FIFOPTR_b.FIFO1SIZ >= 4)
                {
                    // Read one 4-byte word from FIFO
                    IOMn(ui32Module)->FIFOPOP;
#if MANUAL_POP
                    IOMn(ui32Module)->FIFOPOP = 0x11111111;
#endif
                }
            }
        }
    }
    if (ui32IntMask & AM_HAL_IOM_INT_NAK)
    {
        //
        // Wait for Idle
        //
        while ((IOMn(ui32Module)->STATUS & (IOM0_STATUS_IDLEST_Msk | IOM0_STATUS_CMDACT_Msk)) != IOM0_STATUS_IDLEST_Msk);

        //
        // Reset Submodule & FIFO
        //
        // Disable the submodules
        //
        IOMn(ui32Module)->SUBMODCTRL_b.SMOD1EN = 0;
        // Reset Fifo
        IOMn(ui32Module)->FIFOCTRL_b.FIFORSTN = 0;

        // Wait for few IO clock cycles
        am_hal_flash_delay(iterationsToWait);

        IOMn(ui32Module)->FIFOCTRL_b.FIFORSTN = 1;

        // Enable submodule
        IOMn(ui32Module)->SUBMODCTRL_b.SMOD1EN = 1;
    }

    IOMn(ui32Module)->INTCLR = AM_HAL_IOM_INT_ALL;

    // Restore interrupts
    IOMn(ui32Module)->INTEN = curIntCfg;
}

//*****************************************************************************
//
//! @brief Perform a simple full-duplex transaction to the SPI interface.
//!
//! This function performs SPI full-duplex operation to a selected SPI device.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32TxData array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui32TxData array.
//!
//! @return returns AM_HAL_IOM_SUCCESS on successful execution.
//
//*****************************************************************************
uint32_t
am_hal_iom_spi_blocking_fullduplex(void *pHandle,
                                   am_hal_iom_transfer_t *psTransaction)
{
    uint32_t ui32Cmd, ui32Offset, ui32OffsetCnt, ui32Dir, ui32Cont;
    uint32_t ui32FifoRem, ui32FifoSiz;
    uint32_t ui32Bytes;
    uint32_t ui32RxBytes;
    uint32_t ui32IntConfig;
    uint32_t *pui32TxBuffer;
    uint32_t *pui32RxBuffer;
    am_hal_iom_state_t *pIOMState = (am_hal_iom_state_t*)pHandle;
    uint32_t ui32Module;
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    bool     bCmdCmp = false;
    uint32_t numWait = 0;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( !AM_HAL_IOM_CHK_HANDLE(pHandle) )
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }

    if ( !psTransaction )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if ( psTransaction->eDirection != AM_HAL_IOM_FULLDUPLEX )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    //
    // Validate parameters
    //
    ui32Status = validate_transaction(pIOMState, psTransaction, true);

    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        return ui32Status;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    ui32Module = pIOMState->ui32Module;
    ui32Offset = psTransaction->ui32Instr;
    ui32OffsetCnt = psTransaction->ui32InstrLen;
    ui32Bytes = psTransaction->ui32NumBytes;
    ui32Dir = psTransaction->eDirection;
    ui32Cont = psTransaction->bContinue ? 1 : 0;
    pui32RxBuffer = psTransaction->pui32RxBuffer;
    pui32TxBuffer = psTransaction->pui32TxBuffer;

    //
    // Make sure any previous non-blocking transfers have completed.
    //
    ui32Status = am_hal_flash_delay_status_check(pIOMState->waitTimeout,
                                                 (uint32_t)&pIOMState->ui32NumPendTransactions,
                                                 0xFFFFFFFF,
                                                 0,
                                                 true);
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        return ui32Status;
    }

    //
    // Make sure any previous blocking transfer has been completed.
    // This check is required to make sure previous transaction has cleared if the blocking call
    // finished with a timeout
    //
    ui32Status = am_hal_flash_delay_status_check(pIOMState->waitTimeout,
                            (uint32_t)&IOMn(ui32Module)->STATUS,
                            (IOM0_STATUS_IDLEST_Msk | IOM0_STATUS_CMDACT_Msk),
                            IOM0_STATUS_IDLEST_Msk,
                            true);

    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        return ui32Status;
    }

    //
    // Disable interrupts so that we don't get any undesired interrupts.
    //
    ui32IntConfig = IOMn(ui32Module)->INTEN;

    //
    // Disable IOM interrupts as we'll be polling
    //
    IOMn(ui32Module)->INTEN = 0;

    //
    // Clear interrupts
    //
    IOMn(ui32Module)->INTCLR = AM_HAL_IOM_INT_ALL;

    //
    // Set the dev addr (either 7 or 10 bit as configured in MI2CCFG).
    //
    IOMn(ui32Module)->DEVCFG = psTransaction->uPeerInfo.ui32I2CDevAddr;
    // CMDRPT register has been repurposed for DCX
    // Set the DCX
    IOMn(ui32Module)->DCX = pIOMState->dcx[psTransaction->uPeerInfo.ui32SpiChipSelect];

    //
    // Build the CMD value
    //

    ui32Cmd = pIOMState->eInterfaceMode == AM_HAL_IOM_SPI_MODE ?
              psTransaction->uPeerInfo.ui32SpiChipSelect : 0;
    ui32Cmd = build_cmd(ui32Cmd, ui32Dir,  ui32Cont, ui32Offset, ui32OffsetCnt, ui32Bytes);

    //
    // Set the OFFSETHI register.
    //
    IOMn(ui32Module)->OFFSETHI = (uint16_t)(ui32Offset >> 8);

    //
    // Set FULLDUPLEX mode
    //
    IOMn(ui32Module)->MSPICFG |= _VAL2FLD(IOM0_MSPICFG_FULLDUP, 1);

    //
    // Start the transfer
    //
    IOMn(ui32Module)->CMD = ui32Cmd;

    ui32Bytes = psTransaction->ui32NumBytes;
    ui32RxBytes = ui32Bytes;

    //
    // Start a loop to catch the Rx data.
    //
    //
    // Keep looping until we're out of bytes to send or command complete (error).
    //
    while (ui32Bytes || ui32RxBytes)
    {
        //
        // Limit the wait to reasonable limit - instead of blocking forever
        //
        numWait = 0;
        ui32FifoRem = IOMn(ui32Module)->FIFOPTR_b.FIFO0REM;
        ui32FifoSiz = IOMn(ui32Module)->FIFOPTR_b.FIFO1SIZ;

        while ((ui32FifoRem < 4) &&
               (ui32FifoSiz < 4))
        {
            if (numWait++ < AM_HAL_IOM_MAX_BLOCKING_WAIT)
            {
                if (bCmdCmp && (ui32RxBytes > ui32FifoSiz))
                {
                    //
                    // No more data expected. Get out of the loop
                    //
                    break;
                }
                am_hal_flash_delay( FLASH_CYCLES_US(1) );
            }
            else
            {
                //
                // We've waited long enough - get out!
                //
                break;
            }
            bCmdCmp     = IOMn(ui32Module)->INTSTAT_b.CMDCMP;
            ui32FifoRem = IOMn(ui32Module)->FIFOPTR_b.FIFO0REM;
            ui32FifoSiz = IOMn(ui32Module)->FIFOPTR_b.FIFO1SIZ;
        }
        if (bCmdCmp || ((ui32FifoRem < 4) && (ui32FifoSiz < 4)))
        {
            //
            // Something went wrong - bail out
            //
            break;
        }

        while ((ui32FifoRem >= 4) && ui32Bytes)
        {
            IOMn(ui32Module)->FIFOPUSH = *pui32TxBuffer++;
            ui32FifoRem -= 4;
            if (ui32Bytes >= 4)
            {
                ui32Bytes -= 4;
            }
            else
            {
                ui32Bytes = 0;
            }
        }
        while ((ui32FifoSiz >= 4) && ui32RxBytes)
        {
            //
            // Safe to read the FIFO, read 4 bytes
            //
            *pui32RxBuffer++ = IOMn(ui32Module)->FIFOPOP;
#if MANUAL_POP
            IOMn(ui32Module)->FIFOPOP = 0x11111111;
#endif
            ui32FifoSiz -= 4;
            if (ui32RxBytes >= 4)
            {
                ui32RxBytes -= 4;
            }
            else
            {
                ui32RxBytes = 0;
            }
        }
    }

    //
    // Make sure transfer is completed.
    //
    ui32Status = am_hal_flash_delay_status_check(AM_HAL_IOM_MAX_BLOCKING_WAIT,
                            (uint32_t)&IOMn(ui32Module)->STATUS,
                            (IOM0_STATUS_IDLEST_Msk | IOM0_STATUS_CMDACT_Msk),
                            IOM0_STATUS_IDLEST_Msk,
                            true);

    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        return ui32Status;
    }

    ui32Status = internal_iom_get_int_err(ui32Module, 0);

    if (ui32Status == AM_HAL_STATUS_SUCCESS)
    {
        if (ui32Bytes)
        {
            // Indicates transaction did not finish for some reason
            ui32Status = AM_HAL_STATUS_FAIL;
        }
    }
    else
    {
        // Do Error recovery
        // Reset Submodule & FIFO
        internal_iom_reset_on_error(pIOMState, IOMn(ui32Module)->INTSTAT);
    }

    //
    // Clear interrupts
    // Re-enable IOM interrupts.
    //
    IOMn(ui32Module)->INTCLR = AM_HAL_IOM_INT_ALL;
    IOMn(ui32Module)->INTEN = ui32IntConfig;

    //
    // Return the status.
    //
    return ui32Status;

}