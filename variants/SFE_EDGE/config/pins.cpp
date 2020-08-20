/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "bridge/pins.h"

const pin_size_t variantPinCount = 4;

PinState variantPinStates[variantPinCount] = {
    {D1, 1, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D38, 38, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D36, 36, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D3, 3, NULL, /*NULL, NULL, NULL,*/ NULL},
};
