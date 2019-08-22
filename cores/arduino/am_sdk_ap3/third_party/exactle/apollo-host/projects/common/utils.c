/*************************************************************************************************/
/*!
 *  \file   utils.c
 *
 *  \brief  Utility functions.
 *
 *          $Date: 2016-03-29 14:55:12 -0700 (Tue, 29 Mar 2016) $
 *          $Revision: 6524 $
 *
 *  Copyright (c) 2015 ARM, Ltd., all rights reserved.
 *  ARM confidential and proprietary.
 *
 *  IMPORTANT.  Your use of this file is governed by a Software License Agreement
 *  ("Agreement") that must be accepted in order to download or otherwise receive a
 *  copy of this file.  You may not use or copy this file for any purpose other than
 *  as described in the Agreement.  If you do not agree to all of the terms of the
 *  Agreement do not use this file and delete all copies in your possession or control;
 *  if you do not have a copy of the Agreement, you must contact ARM, Ltd. prior
 *  to any use, copying or further distribution of this software.
 */
/*************************************************************************************************/

/***************************************************************************************************
** INCLUDES
***************************************************************************************************/

#include <string.h>

#include "utils.h"

/***************************************************************************************************
** FUNCTION PROTOTYPES
***************************************************************************************************/

static int Util_PrintInt(char *buf, int buf_len, int i, int base, int sign, int width);

/***************************************************************************************************
** FUNCTIONS
***************************************************************************************************/

/*------------------------------------------------------------------------------------------------*/

int32_t Util_ParseMacAddr(const char *s, uint8_t (*addr)[UTIL_MAC_ADDR_LEN])
{
    int32_t r = 0;
    uint8_t i;

    for (i = 0u; i < UTIL_MAC_ADDR_LEN; i++) {
        char    c;
        uint8_t u;

        c  = *s++;
        r++;
        if ((c == '\0') || !UTIL_IS_XDIGIT(c)) goto label_error;
        u  = UTIL_XDIGIT_TO_INT(c) * 16u;
        c  = *s++;
        r++;
        if ((c == '\0') || !UTIL_IS_XDIGIT(c)) goto label_error;
        u += UTIL_XDIGIT_TO_INT(c);
        (*addr)[5u - i] = u;
        if (i != 5u) {
            c = *s++;
            r++;
            if (c != ':') goto label_error;
        }
    }

    return r;

label_error:
    return 0;
}

/*------------------------------------------------------------------------------------------------*/

int32_t Util_ParseUInt(const char *s, uint32_t *u, uint32_t base)
{
    int32_t r = 0;

    if (base == 0u) {
        if (*s == '0') {
            if (((*(s + 1) == 'x') || (*(s + 1) == 'X')) && UTIL_IS_XDIGIT(*(s + 2))) {
                r += 2;
                s += 2;
                base = 16u;
            } else {
                base = 8u;
            }
        } else {
            base = 10u;
        }
    } else if (base == 16u) {
        if (*s == '0') {
            if (((*(s + 1) == 'x') || (*(s + 1) == 'X')) && UTIL_IS_XDIGIT(*(s + 2))) {
                r += 2;
                s += 2;
            }
        }
    }
    /* One character is required; all characters must be consumed. */
    *u = 0u;
    do {
        char    c;
        uint8_t t;
        c = *s++;
        if (!UTIL_IS_XDIGIT(c)) {
            break;
        }
        t = UTIL_XDIGIT_TO_INT(c);
        if (t >= base) {
            break;
        }
        *u *= base;
        *u += t;
        r++;
    } while (*s != '\0');

    return r;
}

/*------------------------------------------------------------------------------------------------*/

int Util_VSNPrintf(char *str, uint32_t size, const char *format, va_list ap)
{
    size_t len = 0;

    *str = 0;
    size--; /* Ensure we null-terminate within our buffer */

    while (*format && len < size) {
        uint32_t width = 0;
        if (*format != '%') {  /* Non-special */
            *str++ = *format++;
            len++;
            continue;
        }
        format++;
        if (*format == '%') { /* Escaped % */
            *str++ = '%';
            len ++;
            format++;
            continue;
        }
        if (*format == '0') {  /* Width */
            format += Util_ParseUInt(format, &width, 10u);
        }
        if (*format == 'l') {
            format++;
            // XXX handle LONG?
        }
        switch(*format) {
            case 'c': { /* Character */
                char tmp = va_arg(ap, int);
                *str++   = tmp;
                len++;
                break;
            }
            case 'u': { /* Unsigned integer */
                unsigned int temp     = va_arg(ap, unsigned int);
                uint8_t      len_temp = Util_PrintInt(str, size - len, temp, 10, 0, width);
                str                  += len_temp;
                len                  += len_temp;
                break;
            }
            case 'd': { /* Signed integer */
                int     temp     = va_arg(ap, int);
                uint8_t len_temp = Util_PrintInt(str, size - len, temp, 10, 1, width);
                str             += len_temp;
                len             += len_temp;
                break;
            }
            case 'p': { /* Pointer */
                unsigned long temp     = va_arg(ap, unsigned long);
                uint8_t       len_temp = Util_PrintInt(str, size - len, temp, 16, 1, 8u);
                str                   += len_temp;
                len                   += len_temp;
                break;
            }
            case 'x':
            case 'X': { /* Unsigned hex number */
                unsigned int temp     = va_arg(ap, unsigned int);
                uint8_t      len_temp = Util_PrintInt(str, size - len, temp, 16, 0, width);
                str                  += len_temp;
                len                  += len_temp;
                break;
            }
            case 's': { /* String */
                char *temp = va_arg(ap, char *);
                while (temp && *temp && len < size) {
                    *str++ = *temp++;
                    len++;
                }
                break;
            }
            default: {
                // char *ptr = va_arg(ap, char *);
                // XXX insert error into output?
                break;
            }
        }
        format++;
    }

    /* Null-terminate */
    *str = 0;

    if (len > size)
        return size + 2; /* Compensate for -1 earlier */

    return len;
}

/*------------------------------------------------------------------------------------------------*/

int Util_SNPrintf(char *str, uint32_t size, const char *format, ...)
{
    va_list ap;
    int len;

    va_start(ap, format);
    len = Util_VSNPrintf(str, size, format, ap);
    va_end(ap);

    return len;
}

/***************************************************************************************************
** LOCAL FUNCTIONS
***************************************************************************************************/

/*------------------------------------------------------------------------------------------------*/

static int Util_PrintInt(char *buf, int buf_len, int i, int base, int sign, int width)
{
    char *s, *p = buf;
    unsigned int u = (unsigned int) i;
    uint8_t use_width;
    int t;

    use_width = width;

    if (i == 0) {
        s = "0\0";
        width--;
        goto almost;
    }

    if (sign && base == 10 && i < 0) {
        *buf++ = '-';
        u = (unsigned int)-i;
        width--;
    }
    s = buf + buf_len - 1;
    *s = '\0';

    while (u && (!use_width || (width > 0))) {
        t = (unsigned int) u % base;
        if (t >= 10)
            t += 'A' - '0' - 10;
        *--s = (char)(t + '0');
        u /= base;
        width--;
    }

almost:
    while (width > 0) {
        *buf++ = '0';
        width--;
    }
    strcpy(buf, s);

    return strlen(p);
}
