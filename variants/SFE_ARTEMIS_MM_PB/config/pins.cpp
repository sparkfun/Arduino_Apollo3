/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "bridge/pins.h"

const pin_size_t variantPinCount = 41;

PinState variantPinStates[variantPinCount] = {
    {LED1,          19, NULL, /*NULL, NULL, NULL,*/ NULL},
    {AUD_OUT,       18, NULL, /*NULL, NULL, NULL,*/ NULL}, // only unique pins here, so AUD_MCLK == AUD_OUT on this board
    {AUD_IN,        11, NULL, /*NULL, NULL, NULL,*/ NULL},
    {AUD_LRCLK,     36, NULL, /*NULL, NULL, NULL,*/ NULL},
    {AUD_BCLK,      37, NULL, /*NULL, NULL, NULL,*/ NULL},
    {I2C_SCL,       39, NULL, /*NULL, NULL, NULL,*/ NULL},
    {I2C_SDA,       40, NULL, /*NULL, NULL, NULL,*/ NULL},
    {I2C_INT,        2, NULL, /*NULL, NULL, NULL,*/ NULL},
    {I2C1_SCL,       8, NULL, /*NULL, NULL, NULL,*/ NULL},
    {I2C1_SDA,       9, NULL, /*NULL, NULL, NULL,*/ NULL},
    {BATT_VIN,      31, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SDIO_CLK,       5, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SDIO_CMD,       7, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SDIO_DATA0,     6, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SDIO_DATA1,    26, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SDIO_DATA2,     4, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SDIO_DATA3,    23, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SPI_SDI,       43, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SPI_SDO,       38, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SPI_CLK,       42, NULL, /*NULL, NULL, NULL,*/ NULL},
    {SPI_CS,        41, NULL, /*NULL, NULL, NULL,*/ NULL},
    {A0,            32, NULL, /*NULL, NULL, NULL,*/ NULL},
    {A1,            35, NULL, /*NULL, NULL, NULL,*/ NULL},
    {PWM0,          44, NULL, /*NULL, NULL, NULL,*/ NULL},
    {PWM1,          45, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D0,             0, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D1,             1, NULL, /*NULL, NULL, NULL,*/ NULL},
    {TX1,           12, NULL, /*NULL, NULL, NULL,*/ NULL},
    {RX1,           25, NULL, /*NULL, NULL, NULL,*/ NULL},
    {RTS1,          10, NULL, /*NULL, NULL, NULL,*/ NULL},
    {CTS1,          17, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G0,            16, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G1,            33, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G2,            34, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G3,            27, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G4,            28, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G5,            29, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G6,            14, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G7,            15, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G9,            12, NULL, /*NULL, NULL, NULL,*/ NULL},
    {G10,           13, NULL, /*NULL, NULL, NULL,*/ NULL},
};
