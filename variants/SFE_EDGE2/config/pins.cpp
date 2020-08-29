/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "bridge/pins.h"

const pin_size_t variantPinCount = 4;

PinState variantPinStates[variantPinCount] = {
    {D16, 16, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D31, 31, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D45, 45, NULL, /*NULL, NULL, NULL,*/ NULL},
    {D44, 44, NULL, /*NULL, NULL, NULL,*/ NULL},
};
