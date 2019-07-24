#ifndef _SVL_RINGBUF_H_
#define _SVL_RINGBUF_H_

#include "stdio.h"

typedef struct _art_svl_ringbuf_t {
    uint8_t*        buf;
    size_t          len;
    volatile size_t r_offset;
    volatile size_t w_offset;
}art_svl_ringbuf_t;

size_t art_svl_ringbuf_init         ( void* rb, uint8_t* buf, size_t len );
size_t art_svl_ringbuf_available    ( void* rb );
size_t art_svl_ringbuf_bytes_free   ( void* rb );
size_t art_svl_ringbuf_write        ( void* rb, uint8_t c );
size_t art_svl_ringbuf_read         ( void* rb, uint8_t* c );


#endif // _SVL_RINGBUF_H_