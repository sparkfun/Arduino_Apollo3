#ifndef _SVL_PACKET_H_
#define _SVL_PACKET_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

typedef struct _svl_packet_t{  // An SVL3 packet consists of 5+N bytes. N is the length of the data payload, and there are 5 bytes that are always transmitted
  //        len                 // 2 - length of the remainder of the packet (pllen + 3)  (note, this is automatically calculated)
  uint8_t   cmd;                // 1 - The command                                       
  uint8_t*  pl;                 // N - The payload (pointer)                             
  uint16_t  pl_len;             //   - Length of the payload in bytes                         (note, this is not transmitted across the line, just used internally)
  //        crc                 // 2 - CRC16 on the command and the payload. poly = 0x8005, nothing extra or fancy. Byte order MSB first, bit order MSB first
  uint16_t  max_pl_len;         //   - This is the number of bytes pointed to by 'pl'
}svl_packet_t;

enum{
	SVL_PACKET_OK 			= 0x00,
	SVL_PACKET_ERR 			= 0x01,	// general error
	SVL_PACKET_ERR_TIMEOUT	= 0x02,	// timeout
	SVL_PACKET_ERR_ZLP		= 0x04,	// zero length packet
	SVL_PACKET_ERR_MEM		= 0x08,	// not enough space to receive packet 
	SVL_PACKET_ERR_CRC		= 0x10,	// crc mismatch

	SVL_PACKET_LEN			= 0x80,	// flag indicating 'len' header
	SVL_PACKET_PL			= 0x40, // flag indicating payload
};

typedef size_t (*svl_packet_read_byte_fn_t)		( void*, uint8_t* );
typedef size_t (*svl_packet_write_byte_fn_t)	( void*, uint8_t );
typedef size_t (*svl_packet_avail_bytes_fn_t)	( void* );
typedef size_t (*svl_packet_millis_fn_t)		( void );

void      		svl_packet_link_read_fn			( svl_packet_read_byte_fn_t fn,		void* param );
void      		svl_packet_link_write_fn		( svl_packet_write_byte_fn_t fn, 	void* param );
void      		svl_packet_link_avail_fn		( svl_packet_avail_bytes_fn_t fn,	void* param );
void      		svl_packet_link_millis_fn		( svl_packet_millis_fn_t fn );	

void      		svl_packet_send         		(svl_packet_t* packet );
uint8_t   		svl_packet_wait         		(svl_packet_t* packet );

uint16_t  		svl_packet_get_uint16_t 		( void );
uint8_t   		svl_packet_wait_bytes   		( uint32_t num );
uint16_t  		svl_packet_get_crc16    		(svl_packet_t* packet);

#endif // _SVL_PACKET_H_