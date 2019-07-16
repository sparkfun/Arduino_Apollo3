/*
  Artemis Bootloader
  By: Nathan Seidle, Owen Lyke
  SparkFun Electronics
  Date: July 15, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This is a serial bootloader that receives commands from a python script and loads
  user's code as quickly as possible.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Based on SparkFun XBee Bootloader from 2009: https://www.sparkfun.com/tutorials/122

  The Apollo3 memory looks like this:

  0xFF000 -> -------
             |     |
             |     |
               ...
             |     |
             |     |
             |     |
  0x10000 -> | APP |
             | SFE |
  0xC000 ->  | SBL |
             -------

  The Ambiq secure bootloader (SBL) lives at 0x00 to 0xC000.
  The SparkFun Artemis bootloader begins at 0xC000 and takes ~7300 bytes (with debug statements it's ~10k)
  We load user code starting at 0xC000 + 16384 = 0x10000
  If our bootloader completes or times out, we jump to 0x10000+4 to begin running user code
*/

#define BOOTLOADER_VERSION 0x02

// ****************************************
// 
// Bootloader options
// 
// ****************************************
#define SERIAL_PORT Serial                // Which UART port to bootload on
#define USERCODE_OFFSET (0xC000 + 0x4000) // Location in flash to begin storing user's code (Linker script needs to be adjusted to offset user's flash to this address)
#define NUM_BAUDS 5                       // Define the list of baud rates to check
uint32_t bauds[NUM_BAUDS] = {             // Baud rates are checked in order, so 921600 is listed first (for fastest experience)
  921600,
  460800,
  230400,
  115200,
  57600,
};


// ****************************************
// 
// Debug Options
// 
// ****************************************

//#define DEBUG 1 // Uncomment to see debug print statements

#ifdef DEBUG
#define DEBUG_SERIAL_PORT   Serial1
#define DEBUG_BAUD_RATE     921600
#define DEBUG_BUFFER_LEN    256
char debug_buffer[DEBUG_BUFFER_LEN];
#endif DEBUG



// ****************************************
// 
// Globals
// 
// ****************************************
static uint32_t ui32Source[512];

#define MAX_WAIT_IN_MS 50 //Millisecond wait before bootload timeout and begin user code

enum BL_COMMANDS {
  BL_COMMAND_ANNOUNCE = 127, //Delete
  BL_COMMAND_AOK = 0x06,
  BL_COMMAND_BAD_CRC,
  BL_COMMAND_NEXT_FRAME,
  BL_COMMAND_ALL_DONE,
  BL_COMMAND_COMPUTER_READY,
};

bool retransmit_flag = false;
uint16_t frame_size = 0;
uint32_t incoming_crc32 = 0;
uint32_t crc32 = 0;
uint32_t frame_address = 0;
uint16_t last_page_erased = 0;


#define MAX_WAIT_IN_US (MAX_WAIT_IN_MS*1000)


void setup() {
#ifdef DEBUG
  DEBUG_SERIAL_PORT.begin(DEBUG_BAUD_RATE);
  debug_printf("\n\nDebug\n");
#endif

  phase_set_baud_rate();
  phase_send_bl_info();
}

//This is the main bootloader
//We'll loop until the whole file is received
void loop() {
  //Determine if the last received data was good or bad
  if (crc32 != incoming_crc32)
  {
RESTART:
    SERIAL_PORT.write(BL_COMMAND_BAD_CRC); //Tell client to resend last frame
    debug_printf("RESTART!\n");
  }
  else
  {
    SERIAL_PORT.write(BL_COMMAND_NEXT_FRAME); //Get next frame
    debug_printf("\nGetting next frame\n");
  }

  while (1) //Wait for the computer to tell us its ready to chat
  {
    if (getch() == BL_COMMAND_COMPUTER_READY) break; //This is the "gimme the next chunk" command
    if (retransmit_flag == true) goto RESTART;
  }

  frame_size = ((uint16_t)getch() << 8) | getch(); //Get the size of this frame
  if (retransmit_flag == true)goto RESTART;

  if (frame_size == BL_COMMAND_ALL_DONE) //Check to see if we are done
  {
    debug_printf("Done!\n");
    am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOI, 0); //Cause a system Power On Init to release as much of the stack as possible
  }

  debug_printf("frame_size: %d\n", frame_size);

  //Get the memory address at which to store this block of data
  frame_address = get4Bytes();
  if (retransmit_flag == true) goto RESTART;

  //debug_printf("frame_address: %d\n", frame_address);

  //Get CRC from client
  incoming_crc32 = get4Bytes();
  if (retransmit_flag == true) goto RESTART;

  //debug_printf("incoming_crc32: %d\n", incoming_crc32);

  //Receive this frame
  crc32 = 0;
  for (uint16_t i = 0 ; i < frame_size / 4 ; i++)
  {
    ui32Source[i] = get4BytesReversed();
    crc32 += ui32Source[i]; //Calculate CRC as we go
  }

  if (incoming_crc32 == crc32)
  {
    debug_printf("CRC good.\n");

    int32_t i32ReturnCode = 0;

    //Frames coming from the computer are 2k bytes, but we erase 8k bytes in a page
    //Only erase a page if we haven't erased it before
    if (last_page_erased < AM_HAL_FLASH_ADDR2PAGE(frame_address + USERCODE_OFFSET))
    {
      debug_printf("Erasing instance %d, page %d\n\r", AM_HAL_FLASH_ADDR2INST(frame_address + USERCODE_OFFSET), AM_HAL_FLASH_ADDR2PAGE(frame_address + USERCODE_OFFSET));

      //Erase the 8k page for this address
      i32ReturnCode = am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                                              AM_HAL_FLASH_ADDR2INST(frame_address + USERCODE_OFFSET),
                                              AM_HAL_FLASH_ADDR2PAGE(frame_address + USERCODE_OFFSET) );
      last_page_erased = AM_HAL_FLASH_ADDR2PAGE(frame_address + USERCODE_OFFSET);
    }

    if (i32ReturnCode)
    {
      debug_printf("FLASH_MASS_ERASE i32ReturnCode = 0x%x.\n\r", i32ReturnCode);
    }

    //Record the array
    i32ReturnCode = program_array(frame_address + USERCODE_OFFSET);

    if (i32ReturnCode){
      debug_printf("FLASH_WRITE error = 0x%x.\n\r", i32ReturnCode);
    }else{
      debug_printf("Array recorded to flash\n");
    }
  }
  else
  {
    debug_printf("Bad CRC:\n");
    debug_printf("incoming_crc32: 0x%04X\n\r", incoming_crc32);
    debug_printf("CRC: 0x%04X\n\r", crc32);
  }
}








// ****************************************
// 
// Jump to the application
// 
// ****************************************
void app_start()
{
  // Print a section of flash
  uint32_t start_address = USERCODE_OFFSET;
  debug_printf("Printing page starting at offset 0x%04X", start_address);
  for (uint16_t x = 0; x < 512; x++)
  {
    if (x % 8 == 0)
    {
      debug_printf("\nAdr: 0x%04X", start_address + (x * 4));
    }
    debug_printf(" 0x%08X", *(uint32_t *)(start_address + (x * 4)));
  }
  debug_printf("\n");

  uint32_t entryPoint = *(uint32_t *)(USERCODE_OFFSET + 4);

  debug_printf("Jump to 0x%08X", entryPoint);
  delay(10); //Wait for prints to complete

  goto *entryPoint; //Jump to start of user code

  debug_printf("Hould not be here\n");
  while (1){};
}



// ****************************************
// 
// Commit whatever is in the ui32Source to the given location
// 
// ****************************************
int32_t program_array(uint32_t ui32PrgmAddr)
{
  uint32_t *pui32Dst;
  int32_t i32ReturnCode;
  //
  // Program a few words in a page in the main block of instance 1.
  //
  pui32Dst = (uint32_t *) ui32PrgmAddr;
  i32ReturnCode = am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                  ui32Source,
                  pui32Dst,
                  512);

  return (i32ReturnCode);
}



// ****************************************
// 
// Baud determination phase
// 
// ****************************************
int8_t phase_set_baud_rate( void ){
  
  bool set_baud_success = false;
  uint8_t baud_index = 0;
  const uint8_t BAUD_RATE_CMD_LEN = 7;
  char recvd[BAUD_RATE_CMD_LEN] = {0};
  const uint32_t send_baud_timeout_ms = 40;
  int8_t timed_out = 0x00;

  // Loop through all baud rates to check
  while(baud_index < NUM_BAUDS){
    
    SERIAL_PORT.begin(bauds[baud_index]);

    // Wait for at least one byte to be available:
    uint32_t wait_bytes = 1;
    wait_until( &wait_bytes, send_baud_timeout_ms );

    // When at least one byte is available either read a full command or time out
    uint32_t num_2_recv = BAUD_RATE_CMD_LEN;
    memset( (void*)recvd, 0x00, sizeof(recvd)/sizeof(char));
    timed_out = read_until( recvd, &num_2_recv, send_baud_timeout_ms );
    debug_printf("Received: {%d, %d, %d, %d, %d, %d, %d} (timed out? %d) (recieved: %d)\n", recvd[0], recvd[1], recvd[2], recvd[3], recvd[4], recvd[5], recvd[6], timed_out, num_2_recv);

    if((timed_out == 0x00) && (num_2_recv == BAUD_RATE_CMD_LEN)){ // Looks like we got a message, now check baud match
      uint32_t got_baud = ( (((uint32_t)recvd[3]) << 24) | (((uint32_t)recvd[4]) << 16) | (((uint32_t)recvd[5]) << 8) | (((uint32_t)recvd[6]) << 0) );
      debug_printf("Got baud: %d, at baud: %d\n", got_baud, bauds[baud_index]);
      if( got_baud == bauds[baud_index] ){
        set_baud_success = true;
        break;
      }
    }

    SERIAL_PORT.end();
    baud_index++;
  } 

  // report success or failure
  if(set_baud_success){
    // Indicate to the host that the baud rate is set correctly
    send_bytes(recvd, BAUD_RATE_CMD_LEN);
    return 0;
  }else{
    return -1;
  }
}



// ****************************************
// 
// Send Bootloader Information Phase
// 
// ****************************************
void phase_send_bl_info( void ){

//  delay(3); //Necessary to separate a blip on TX line when port opens from annouce command

  delayMicroseconds(10);

  SERIAL_PORT.write(BL_COMMAND_ANNOUNCE); //Tell the world we can be bootloaded

  // ToDo: we will need to make sure that no more baud rate negotiation messages are coming in at this point...

  //Check to see if the computer responded
  uint32_t count = 0;
  while (SERIAL_PORT.available() == 0)
  {
    if (count++ > MAX_WAIT_IN_US)
    {
      debug_printf("No response from computer\n");
      app_start();
    }
    delayMicroseconds(1);
  }
  if (SERIAL_PORT.read() != BL_COMMAND_AOK)
  {
    debug_printf("Invalid response from computer\n");
    app_start(); //If the computer did not respond correctly with a AOK, we jump to user's program
  }

  SERIAL_PORT.write(BOOTLOADER_VERSION);

  debug_printf("Bootloading!\n");
}





// ****************************************
// 
// Convenience UART functions
// 
// ****************************************
int8_t read_until( char* buff, uint32_t* num_io, uint32_t timeout_ms ){
  uint32_t start = millis();
  uint32_t desired = *(num_io);
  uint32_t received = 0;
  while( (millis() - start) < timeout_ms ){
    if( received >= desired ){                  // Check if all bytes received
      break;
    }
    if(SERIAL_PORT.available()){                // Store the byte
      *(buff + received) = SERIAL_PORT.read();
      received++;
    }
  }
  *(num_io) = received;                         // Provide the number of bytes received within the timeout
  if(!(received >= desired)){                   // Warn that timeout happened b4 bytes received
    return -1;  
  }
  return 0;
}

int8_t wait_until( uint32_t* num_io, uint32_t timeout_ms ){
  uint32_t start = millis();
  uint32_t desired = *(num_io);
  uint32_t avail = 0;
  while( (millis() - start) < timeout_ms ){
    avail = SERIAL_PORT.available();          // How many bytes available?
    if( avail >= desired ){                   // Check if all bytes available
      break;
    }
  }
  *(num_io) = avail;                          // Provide the number of bytes received within the timeout
  if(!(avail >= desired)){                    // Warn that timeout happened b4 bytes received
    return -1;  
  }
  return 0;
}

void send_bytes( const char* buff, uint32_t num ){
  for(uint32_t indi = 0; indi < num; indi++){
    SERIAL_PORT.write(buff[indi]);
  }
}

uint8_t getch()
{
  retransmit_flag = false;

  uint16_t counter = 0;
  while (SERIAL_PORT.available() == 0)
  {
    delayMicroseconds(10);
    if (counter++ > 10000)
    {
      retransmit_flag = true;
      return (0); //Timeout
    }
  }
  return (SERIAL_PORT.read());
}

uint32_t get4Bytes(void)
{
  uint32_t incoming = 0;
  for (byte x = 0 ; x < 4 ; x++)
  {
    incoming <<= 8;
    incoming |= getch();
  }
  return (incoming);
}

uint32_t get4BytesReversed(void)
{
  uint32_t incoming = 0;
  for (byte x = 0 ; x < 4 ; x++)
  {
    incoming >>= 8;
    incoming |= (getch() << 8 * 3);
  }
  return (incoming);
}



// ****************************************
// 
// A debug print function
// 
// ****************************************
void debug_printf(char* fmt, ...){
#ifdef DEBUG
  va_list args;
  va_start (args, fmt);
  vsnprintf(debug_buffer, DEBUG_BUFFER_LEN, (const char*)fmt, args);
  va_end (args);
  DEBUG_SERIAL_PORT.print(debug_buffer);
#endif //DEBUG  
}
