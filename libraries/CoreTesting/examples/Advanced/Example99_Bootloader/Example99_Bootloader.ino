/*
  Artemis Bootloader
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This is a serial bootloader that receives commands from a python script and loads
  user's code as quickly as possible.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Based on XBee Bootloader from 2009: https://www.sparkfun.com/tutorials/122

  The Apollo3 memory looks like this:

  0xFF000 -> -------
             |     |
             |     |
               ...
             |     |
             |     |
             |     |
  0x14000 -> | APP |
             | SFE |
  0xC000 ->  | SBL |
             -------

  The factory secure bootloader (SBL) lives at 0x00 to 0xC000.
  The SparkFun Artemis bootloader begins at 0xC000 and takes ~20k bytes
  We load user code starting at 0xC000 + 0x8000 (32768) = 0x14000
  If our bootloader times out, we jump to 0x14000 to begin running user code

  TODO:
  Protect bootloader section in flash
  Make sure all RAM is released
*/
static uint32_t ui32Source[512];

#define MAX_WAIT_IN_MS 100 //Millisecond wait before bootload timeout and begin user code

enum BL_COMMANDS {
  BL_COMMAND_ANNOUNCE = 0x05,
  BL_COMMAND_AOK,
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

// Location in flash to begin storing user's code
//#define USERCODE_OFFSET 0x14000
#define USERCODE_OFFSET 0x80000 // Should be start of flash instance 1

//Comment out this line to turn off debug statements on Serial1
#define DEBUG 1

void setup() {
  Serial.begin(9600); //Start comm with computer at low baud rate. We'll go faster later.

#ifdef DEBUG
  Serial1.begin(921600);
  Serial1.println();
  Serial1.println();
  Serial1.println("Debug");
#endif

  Serial.write(BL_COMMAND_ANNOUNCE); //Tell the world we can be bootloaded

  //Check to see if the computer responded
  uint16_t count = 0;
  while (Serial.available() == 0)
  {
    if (count++ > MAX_WAIT_IN_MS)
    {
      app_start();
    }
    delay(1);
  }
  if (Serial.read() != BL_COMMAND_AOK) app_start(); //If the computer did not respond correctly with a AOK, we jump to user's program

  //Now get upload baud rate from caller
  uint32_t uploadRate = get4Bytes();

  Serial.flush();
  Serial.begin(uploadRate); //Go to new baud rate

  //We need to give the computer some time to switch baud rates
  //Let's erase the flash while we wait

#ifdef DEBUG
  Serial1.printf("Erasing all of flash instance %d.\n\r", AM_HAL_FLASH_ADDR2INST(USERCODE_OFFSET));
#endif

  int32_t i32ReturnCode = am_hal_flash_mass_erase(AM_HAL_FLASH_PROGRAM_KEY, AM_HAL_FLASH_ADDR2INST(USERCODE_OFFSET));

#ifdef DEBUG
  if (i32ReturnCode)
  {
    Serial1.printf("FLASH_MASS_ERASE i32ReturnCode =  0x%x.\n\r", i32ReturnCode);
  }
#endif

  delay(10); //Give the computer some time to switch baud rates

#ifdef DEBUG
  Serial1.print("Bootloading @ ");
  Serial1.println(uploadRate);
#endif
}

//This is the main bootloader
//We'll loop until the whole file is received
void loop() {
  //Determine if the last received data was good or bad
  if (crc32 != incoming_crc32)
  {
RESTART:
    Serial.write(BL_COMMAND_BAD_CRC); //Tell client to resend last frame
#ifdef DEBUG
    Serial1.println("RESTART!");
#endif
  }
  else
  {
    Serial.write(BL_COMMAND_NEXT_FRAME); //Get next frame
#ifdef DEBUG
    Serial1.println();
    Serial1.println("Getting next frame");
#endif
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
    am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOI, 0); //Cause a system Power On Init to release as much of the stack as possible
  }

#ifdef DEBUG
  Serial1.print("frame_size: ");
  Serial1.println(frame_size);
#endif

  //Get the memory address at which to store this block of data
  frame_address = get4Bytes();
  if (retransmit_flag == true) goto RESTART;

  //Serial1.print("frame_address: ");
  //Serial1.println(frame_address);

  //Get CRC from client
  incoming_crc32 = get4Bytes();
  if (retransmit_flag == true) goto RESTART;

  //Serial1.print("incoming_crc32: ");
  //Serial1.println(incoming_crc32);

  //Receive this frame
  crc32 = 0;
  for (uint16_t i = 0 ; i < frame_size / 4 ; i++)
  {
    ui32Source[i] = get4BytesReversed();
    crc32 += ui32Source[i]; //Calculate CRC as we go
  }

  if (incoming_crc32 == crc32)
  {
#ifdef DEBUG
    Serial1.println("CRC good. Record array");
#endif
    int32_t i32ReturnCode = program_array(frame_address + USERCODE_OFFSET);

#ifdef DEBUG
    if (i32ReturnCode)
      Serial1.printf("FLASH_WRITE error =  0x%x.\n", i32ReturnCode);
#endif
  }
  else
  {
#ifdef DEBUG
    Serial1.println("Bad CRC:");
    Serial1.printf("incoming_crc32: 0x%04X\n\r", incoming_crc32);
    Serial1.printf("CRC: 0x%04X\n\r", crc32);
#endif
  }
}

//Gets a character from Serial
uint8_t getch()
{
  retransmit_flag = false;

  uint16_t counter = 0;
  while (Serial.available() == 0)
  {
    delayMicroseconds(10);
    if (counter++ > 10000)
    {
      retransmit_flag = true;
      return (0); //Timeout
    }
  }
  return (Serial.read());
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

void app_start()
{
  // Print a section of flash
  //  uint32_t start_address = USERCODE_OFFSET;
  //  Serial1.printf("Printing page starting at offset 0x%04X", start_address);
  //  for (uint16_t x = 0; x < 512; x++)
  //  {
  //    if (x % 8 == 0)
  //    {
  //      Serial1.println();
  //      Serial1.printf("Adr: 0x%04X", start_address + (x * 4));
  //    }
  //    Serial1.printf(" 0x%08X", *(uint32_t *)(start_address + (x * 4)));
  //  }
  //  Serial1.println();

  uint32_t entryPoint = *(uint32_t *)(USERCODE_OFFSET + 4);

#ifdef DEBUG
  Serial1.printf("Jump to 0x%08X", entryPoint);
#endif
  delay(10); //Wait for prints to complete

  goto *entryPoint; //Jump to start of user code

#ifdef DEBUG
  Serial1.println("Not here");
#endif
  while (1);
}

//Commits whatever is in the ui32Source to the given location
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
