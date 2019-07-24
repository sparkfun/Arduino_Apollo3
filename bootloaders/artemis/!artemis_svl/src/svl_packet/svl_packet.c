#include "svl_packet.h"

void*	read_param = NULL;
void*	write_param = NULL;
void*	avail_param = NULL;

svl_packet_read_byte_fn_t 		read_fn = NULL;
svl_packet_write_byte_fn_t 		write_fn = NULL;
svl_packet_avail_bytes_fn_t 	avail_fn = NULL;
svl_packet_millis_fn_t			millis_fn = NULL;

inline __attribute__((always_inline)) size_t svl_packet_read_byte( uint8_t* c ){
	size_t retval = 0x00;
	if(read_fn != NULL){ retval = read_fn( read_param, c ); }
	return retval;
}

inline __attribute__((always_inline)) size_t svl_packet_write_byte( uint8_t c ){
	size_t retval = 0x00;
	if(write_fn != NULL){ retval = write_fn( write_param, c ); }
	return retval;
}

inline __attribute__((always_inline)) size_t svl_packet_avail_bytes( void ){
	size_t retval = 0x00;
	if(avail_fn != NULL){ retval = avail_fn( avail_param ); }
	return retval;
}

inline __attribute__((always_inline)) size_t svl_packet_millis( void ){
	size_t retval = 0x00;
	if(millis_fn != NULL){ retval = millis_fn(); }
	return retval;
}

void   	svl_packet_link_read_fn		(svl_packet_read_byte_fn_t fn, void* param){
	read_param = param;
	read_fn = fn;
}

void  	svl_packet_link_write_fn	(svl_packet_write_byte_fn_t fn, void* param){
	write_param = param;
	write_fn = fn;
}

void   	svl_packet_link_avail_fn	(svl_packet_avail_bytes_fn_t fn, void* param){
	avail_param = param;
	avail_fn = fn;
}

void	svl_packet_link_millis_fn	( svl_packet_millis_fn_t fn ){
	millis_fn = fn;
}




void svl_packet_send(svl_packet_t* packet ){
	uint16_t crc = svl_packet_get_crc16(packet);

	svl_packet_write_byte( ((packet->pl_len + 3) >>   8) ); 	// len high byte (including command and CRC bytes)
	svl_packet_write_byte( ((packet->pl_len + 3) & 0xFF) ); 	// len low byte  (including command and CRC bytes)

	svl_packet_write_byte( (packet->cmd) );            			// command byte

	if((packet->pl != NULL) && (packet->pl_len != 0)){
		for(uint16_t indi = 0; indi < packet->pl_len; indi++){  // payload
			svl_packet_write_byte( *(packet->pl + indi) );
		}
	}

	svl_packet_write_byte( (uint8_t)(crc >>   8) );     		// CRC H
	svl_packet_write_byte( (uint8_t)(crc & 0xFF) );     		// CRC L
}



uint8_t svl_packet_wait(svl_packet_t* packet ){

    // wait for 2 bytes (the length bytes)
    // wait for length bytes to come in
    // make sure that 'length' bytes are enough to satisfy the desired payload length

    if(packet == NULL)							{ return (SVL_PACKET_ERR); }

    const uint8_t num_bytes_length = 2;
    if(svl_packet_wait_bytes(num_bytes_length))	{ return (SVL_PACKET_ERR_TIMEOUT | SVL_PACKET_LEN); }
    uint16_t len = svl_packet_get_uint16_t();

    if( len == 0 )								{ return ( SVL_PACKET_ERR_ZLP ); }
    if( (len-3) > packet->max_pl_len )			{ return ( SVL_PACKET_ERR_MEM | SVL_PACKET_PL ); }
    if(svl_packet_wait_bytes(len))				{ return ( SVL_PACKET_ERR_TIMEOUT | SVL_PACKET_PL ); }

    svl_packet_read_byte( &packet->cmd );
    packet->pl_len = (len-3);
    if((packet->pl != NULL) && (packet->max_pl_len != 0)){
        uint32_t indi = 0x00;
        for( indi = 0; indi < packet->pl_len; indi++ ){   // Fill payload with data
            svl_packet_read_byte( (packet->pl + indi) );
        }
        for( ; indi < packet->max_pl_len; indi++ ){  // Zero out remaining bytes
            *(packet->pl + indi) = 0x00;
        }
    }

    uint16_t crc 	= svl_packet_get_uint16_t();
    uint16_t check 	= svl_packet_get_crc16( packet );

    if( crc != check ){ return (SVL_PACKET_ERR_CRC); }

    return (SVL_PACKET_OK);
}



uint16_t svl_packet_get_uint16_t( void ){
    uint8_t h = 0x00;
    uint8_t l = 0x00;
    svl_packet_read_byte( &h );
    svl_packet_read_byte( &l );
    return (((uint16_t)h << 8) | (l & 0xFF));
}



uint8_t svl_packet_wait_bytes(uint32_t num){
	uint32_t timeout_ms = 500;

	uint32_t start = svl_packet_millis();
	uint32_t avail = 0;
	while((svl_packet_millis() - start) < timeout_ms){
		avail = svl_packet_avail_bytes();
		if(avail >= num){
			return 0;
		}
	}

	// debug_printf("only got %d bytes...\n",avail);
	return 1;
}



uint16_t svl_packet_get_crc16(svl_packet_t* packet){
//    # Load the register with zero bits.
//    # Augment the message by appending W zero bits to the end of it.
//    # While (more message bits)
//    #     Begin
//    #     Shift the register left by one bit, reading the next bit of the
//    #         augmented message into register bit position 0.
//    #     If (a 1 bit popped out of the register during step 3)
//    #         Register = Register XOR Poly.
//    #     End
//    # The register now contains the remainder.
  uint16_t reg = 0x0000;
  bool c = false;
  uint16_t poly = 0x8005;
  for( uint16_t indi = 0; indi < packet->pl_len + 2 + 1; indi++ ){ // addtl 2 bytes for padding, 1 byte to process cmd
    for( uint8_t b = 0; b < 8; b++ ){
      c = false;
      if(reg & 0x8000){
        c = true;
      }
      reg <<= 1;
      reg &= 0xFFFF;
      if(indi < (packet->pl_len + 1)){
        if(indi == 0){
          if( packet->cmd & (0x80 >> b)){
            reg |= 0x0001;
          }
        }else{
          if((*(packet->pl + indi - 1)) & (0x80 >> b)){
            reg |= 0x0001;
          }
        }
      }
      if(c){
        reg = (reg ^ poly);
      }
    }
  }
  return reg;
}