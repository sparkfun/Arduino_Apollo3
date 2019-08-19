#include "svl_ringbuf.h"

size_t art_svl_ringbuf_init( void* vrb, uint8_t* buf, size_t len ){
    if( vrb == NULL ){ return 0; }
    art_svl_ringbuf_t* rb = (art_svl_ringbuf_t*)vrb;
    
    rb->buf = buf;
    rb->len = len;
    rb->r_offset = 0;
    rb->w_offset = 0;

    return rb->len;
}

size_t art_svl_ringbuf_available( void* vrb ){
    if( vrb == NULL ){ return 0; }
    art_svl_ringbuf_t* rb = (art_svl_ringbuf_t*)vrb;

    size_t avail = 0x00;
    if((rb->w_offset) >= (rb->r_offset)){
        avail = rb->w_offset - rb->r_offset;
    }else{
        avail = rb->len - (rb->r_offset - rb->w_offset);
    }
    return avail;
}

size_t art_svl_ringbuf_bytes_free( void* vrb ){
    if( vrb == NULL ){ return 0; }
    art_svl_ringbuf_t* rb = (art_svl_ringbuf_t*)vrb;

    size_t friegh = 0x00;
    if((rb->w_offset) >= (rb->r_offset)){
        friegh = rb->len - rb->w_offset + rb->r_offset -1;
    }else{
        friegh = rb->r_offset - rb->w_offset - 1;
    }
    return friegh;
}

size_t art_svl_ringbuf_write( void* vrb, uint8_t c ){
    if( vrb == NULL ){ return 0; }
    art_svl_ringbuf_t* rb = (art_svl_ringbuf_t*)vrb;

    if(art_svl_ringbuf_bytes_free(rb) > 0){
        *(rb->buf + rb->w_offset) = c;
        rb->w_offset++;
        if(rb->w_offset >= rb->len){
            rb->w_offset = 0;
        }
        return 1;
    }
    return 0;
}

size_t art_svl_ringbuf_read( void* vrb, uint8_t* c ){
    if( vrb == NULL ){ return 0; }
    art_svl_ringbuf_t* rb = (art_svl_ringbuf_t*)vrb;
    
    if(art_svl_ringbuf_available(rb) > 0){
        *c = *(rb->buf + rb->r_offset);
        rb->r_offset++;
        if(rb->r_offset >= rb->len){
            rb->r_offset = 0;
        }
        return 1;
    }
    return 0;
}