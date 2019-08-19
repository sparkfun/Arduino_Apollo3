//*****************************************************************************
//
//! @file main.c
//!
//! @brief A variable-baud rate bootloader for Apollo3 / Artemis module
//!
//! Purpose:
//
//*****************************************************************************

/*
Copyright (c) 2019 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*

Authors:
Owen Lyke, Nathan Seidle

Modified: Juy 22 2019

*/

//*****************************************************************************
//
// Includes
//
//*****************************************************************************
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "stdint.h"

#include "svl_ringbuf.h"
#include "svl_packet.h"
#include "svl_uart.h"
#include "svl_utils.h"


//*****************************************************************************
//
// Defines
//
//*****************************************************************************
#define SVL_VERSION_NUMBER 0x03


// ****************************************
// 
// Bootloader Options
// 
// ****************************************
#define BL_UART_BUF_LEN     (2048 + 512)        // must be larger than maximum frame transmission length for guaranteed performance
#define BL_UART_INST        0                   // which UART peripheral to use for BL data
#define BL_RX_PAD           49                  // RX pad for BL_UART_INST
#define BL_TX_PAD           48                  // TX pad for BL_UART_INST
#define USERCODE_OFFSET     (0xC000 + 0x4000)   // location in flash to begin storing user's code (Linker script needs to be adjusted to offset user's flash to this address)
#define FRAME_BUFFER_SIZE   512                 // maximum number of 4-byte words that can be transmitted in a single frame packet

// ****************************************
// 
// Debug Options
// 
// ****************************************

// #define DEBUG 1                                 // uncomment to enable debug output

#ifdef  DEBUG   
#define DEBUG_BAUD_RATE     921600              // debug output baud rate
#define DEBUG_UART_INST     1                   // debug UART peripheral instance (should not be the same as BL_UART_INST)
#define DEBUG_RX_PAD        25                  // RX pad for 
#define DEBUG_TX_PAD        24
#define DEBUG_UART_BUF_LEN  256
#define DEBUG_PRINT_APP     1                   // undefine to not print app pages
#define APP_PRINT_NUM_PAGE  1
#undef  APP_PRINT_PRETTY                        // define APP_PRINT_PRETTY for the alternate app data print format
uint8_t debug_buffer[DEBUG_UART_BUF_LEN] = {0};
#endif // DEBUG



// ****************************************
// 
// Bootloader Commands
// 
// ****************************************
#define CMD_VERSION (0x01)
#define CMD_BLMODE  (0x02)
#define CMD_NEXT    (0x03)
#define CMD_FRAME   (0x04)
#define CMD_RETRY   (0x05)
#define CMD_DONE    (0x06)




//*****************************************************************************
//
// Macros
//
//*****************************************************************************
#define UART_GPIO_PINCONFIG_INNER( INST, TXRX, PAD ) { .uFuncSel = AM_HAL_PIN_ ## PAD ## _UART ## INST ## TXRX, .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA }
#define UART_GPIO_PINCONFIG( INST, TXRX, PAD ) UART_GPIO_PINCONFIG_INNER( INST, TXRX, PAD )



//*****************************************************************************
//
// Forward Declarations
//
//*****************************************************************************
void        setup                   ( void );
bool        detect_baud_rate        ( uint32_t* baud );
void        start_uart_bl           ( uint32_t baud );
void        enter_bootload          ( void );
uint8_t     handle_frame_packet     ( svl_packet_t* packet, uint32_t* p_frame_address, uint16_t* p_last_page_erased );
void        app_start               ( void );
void        debug_printf            ( char* fmt, ... );




//*****************************************************************************
//
// Globals
//
//*****************************************************************************
art_svl_ringbuf_t   bl_rx_ringbuf = {
    .buf = NULL,
    .len = 0,
    .r_offset = 0,
    .w_offset = 0,
};
void*               hUART_bl    = NULL; // pointer to handle for bootloader UART
void*               hUART_debug = NULL; // pointer to handle for debug UART

#define BL_BAUD_SAMPLES     (5)                 
volatile uint8_t   bl_baud_ticks_index = 0x00;
volatile uint32_t  bl_baud_ticks[BL_BAUD_SAMPLES] = {0};





//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void){

    bool        baud_valid = false;
    uint32_t    bl_baud = 0x00;
    uint8_t     bl_buffer[BL_UART_BUF_LEN] = {0};

    #define         PLLEN_VER 1
    uint8_t         packet_ver_buf[PLLEN_VER] = {SVL_VERSION_NUMBER};
    svl_packet_t    svl_packet_version = { CMD_VERSION, packet_ver_buf, PLLEN_VER, PLLEN_VER };
    svl_packet_t    svl_packet_blmode = { CMD_BLMODE, NULL, 0, 0 };

    art_svl_ringbuf_init( &bl_rx_ringbuf, bl_buffer, BL_UART_BUF_LEN );
    setup();

    debug_printf("\n\nArtemis SVL Bootloader - DEBUG\n\n");

    baud_valid = detect_baud_rate( &bl_baud );                                      // Detects the baud rate. Returns true if a valid baud rate was found
    if( baud_valid == false ){        
        app_start();                                                                // w/o valid baud rate jump t the app
    }

    start_uart_bl( bl_baud );                                                       // This will create a 23 us wide low 'blip' on the TX line (until possibly fixed)
    am_util_delay_us(200);                                                          // At the minimum baud rate of 115200 one byte (10 bits with start/stop) takes 10/115200 or 87 us. 87+23 = 100, double to be safe

    debug_printf("phase:\tconfirm bootloading entry\n");
    debug_printf("\tsending Artemis SVL version packet\n");
    svl_packet_send( &svl_packet_version );                                         // when baud rate is determined send the version packet

    debug_printf("\twaiting for bootloader confirmation\n");
    if(svl_packet_wait( &svl_packet_blmode ) != 0){                                 // wait for the bootloader to confirm bootloader mode entry
        debug_printf("\tno confirmation received\n");
        app_start();  // break to app
    }
    debug_printf("\tentering bootloader\n\n");

    enter_bootload();       // Now we are locked in
    am_util_delay_ms(10);

    am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOI, 0); //Cause a system Power On Init to release as much of the stack as possible

    debug_printf("ERROR - runoff");
    while (1){                                              // Loop forever while sleeping.
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);    // Go to Deep Sleep.
    }
}





//*****************************************************************************
//
// Function definitions below 
//
//*****************************************************************************





#ifdef DEBUG
void start_uart_debug( void ){
    const am_hal_gpio_pincfg_t debug_uart_tx_pinconfig = UART_GPIO_PINCONFIG( DEBUG_UART_INST, TX, DEBUG_TX_PAD );
    const am_hal_gpio_pincfg_t debug_uart_rx_pinconfig = UART_GPIO_PINCONFIG( DEBUG_UART_INST, RX, DEBUG_RX_PAD );
    const am_hal_uart_config_t debug_uart_config = {
        // Standard UART settings: 115200-8-N-1
        .ui32BaudRate = DEBUG_BAUD_RATE,
        .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

        // Set TX and RX FIFOs to interrupt at half-full.
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                        AM_HAL_UART_RX_FIFO_1_2),

        // Buffers
        .pui8TxBuffer = NULL,
        .ui32TxBufferSize = 0,
        .pui8RxBuffer = NULL,
        .ui32RxBufferSize = 0,
    };

    // Initialize the printf interface for UART output.
    am_hal_uart_initialize(DEBUG_UART_INST, &hUART_debug);
    am_hal_uart_power_control(hUART_debug, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(hUART_debug, &debug_uart_config);

    // Disable that pesky FIFO
    UARTn(DEBUG_UART_INST)->LCRH_b.FEN = 0; 

    // Enable the UART pins.
    am_hal_gpio_pinconfig(DEBUG_TX_PAD, debug_uart_tx_pinconfig);
    am_hal_gpio_pinconfig(DEBUG_RX_PAD, debug_uart_rx_pinconfig);

    // Enable interrupts.
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + DEBUG_UART_INST));
    am_hal_uart_interrupt_enable(hUART_debug, (AM_HAL_UART_INT_RX));
}
#endif // DEBUG


//*****************************************************************************
//
// Setup
//
//*****************************************************************************
void setup( void ){
    // Set the clock frequency.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the stimer
    am_hal_stimer_int_enable(AM_HAL_STIMER_INT_OVERFLOW);
    NVIC_EnableIRQ(STIMER_IRQn);
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

#ifdef DEBUG
    start_uart_debug();
#endif

    // Enable interrupts.
    am_hal_interrupt_master_enable();
}


// //*****************************************************************************
// //
// // Un-set-up
// //
// //*****************************************************************************
// void unsetup( void ){
//     // Set the clock frequency.
//     am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

//     // Set the default cache configuration
//     am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
//     am_hal_cachectrl_enable();

//     // Configure the stimer
//     am_hal_stimer_int_enable(AM_HAL_STIMER_INT_OVERFLOW);
//     NVIC_EnableIRQ(STIMER_IRQn);
//     am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
//     am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

// #ifdef DEBUG
//     start_uart_debug();
// #endif

//     // Enable interrupts.
//     am_hal_interrupt_master_enable();
// }


// ****************************************
// 
// Baud Rate Detect Phase
// 
// ****************************************
bool detect_baud_rate( uint32_t* baud ){
    uint32_t  bl_entry_timeout_ms = 200;
    uint32_t  bl_entry_timeout_start = millis();
    bool baud_is_valid = false;
    bool timed_out = true;

    debug_printf("phase:\tdetect baud rate\n");

    enable_burst_mode();

    am_hal_gpio_pinconfig(BL_RX_PAD, g_AM_HAL_GPIO_INPUT_PULLUP);

    ap3_gpio_enable_interrupts(BL_RX_PAD, AM_HAL_GPIO_PIN_INTDIR_LO2HI);
    am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(BL_RX_PAD));
    am_hal_gpio_interrupt_enable(AM_HAL_GPIO_BIT(BL_RX_PAD));
    NVIC_EnableIRQ(GPIO_IRQn);

    while( (millis() - bl_entry_timeout_start) < bl_entry_timeout_ms ){
        // try to detect baud rate

        // debug_printf("\ttime (ms):\t%d\n", millis());

        if( bl_baud_ticks_index == BL_BAUD_SAMPLES ){

            // compute differences between samples
            for(uint8_t indi = 0; indi < (BL_BAUD_SAMPLES-1); indi++){
                bl_baud_ticks[indi] = bl_baud_ticks[indi+1]-bl_baud_ticks[indi];
            }

            float mean = 0.0;
            for(uint8_t indi = 0; indi < (BL_BAUD_SAMPLES-1); indi++){
                mean += bl_baud_ticks[indi];
            }
            mean /= (BL_BAUD_SAMPLES-1);


            if( mean < 3 ){
            // invalid
            }else if( ( mean >= 4) && ( mean <= 8) ){
                *baud = 921600;
                baud_is_valid = true;
            }else if( ( mean >= 10) && ( mean <= 14) ){
                *baud = 460800;
                baud_is_valid = true;
            }else if( ( mean >= 25) && ( mean <= 30) ){
                *baud = 230400;
                baud_is_valid = true;
            }else if( ( mean >= 45) && ( mean <= 55) ){
                *baud = 115200;
                baud_is_valid = true;
            }else if( ( mean >= 91) && ( mean <= 111) ){
                *baud = 57600;
                baud_is_valid = true;
            }else{
                // invalid
            }

            if(baud_is_valid){
                timed_out = false;
            }
            
            break;            // exit the timeout loop
        }
    }

    am_hal_gpio_interrupt_disable(AM_HAL_GPIO_BIT(BL_RX_PAD));
    am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(BL_RX_PAD));
    NVIC_DisableIRQ(GPIO_IRQn);

    disable_burst_mode();

    
#ifdef DEBUG
    // show differences for debugging purposes
    debug_printf("\ttiming differences: { ");
    for(uint8_t indi = 0; indi < (BL_BAUD_SAMPLES-1); indi++){
        debug_printf("%d", bl_baud_ticks[indi] );
        if( indi < (BL_BAUD_SAMPLES - 2) ){
            debug_printf(", ");
        }
    }
    debug_printf("}\n");
#endif // DEBUG

    if(!baud_is_valid){
        debug_printf("\tbaud rate not detected.\n\t\trising edges:\t%d\n\t\ttimed out:\t%d\n\n", bl_baud_ticks_index, timed_out);
    }else{
        debug_printf("\tdetected valid baud rate:\t%d\n\n", *baud);
    }

    return baud_is_valid;
}



//*****************************************************************************
//
// Start BL UART at desired baud
//
//*****************************************************************************
void start_uart_bl( uint32_t baud ){
    const am_hal_gpio_pincfg_t bl_uart_tx_pinconfig = UART_GPIO_PINCONFIG( BL_UART_INST, TX, BL_TX_PAD );
    const am_hal_gpio_pincfg_t bl_uart_rx_pinconfig = UART_GPIO_PINCONFIG( BL_UART_INST, RX, BL_RX_PAD );
    am_hal_uart_config_t bl_uart_config =
    {
        // Standard UART settings: 115200-8-N-1
        .ui32BaudRate = baud,
        .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

        // Set TX and RX FIFOs to interrupt at half-full.
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                        AM_HAL_UART_RX_FIFO_1_2),

        // Buffers
        .pui8TxBuffer = NULL,
        .ui32TxBufferSize = 0,
        .pui8RxBuffer = NULL,
        .ui32RxBufferSize = 0,
    };

    // Initialize the printf interface for UART output.
    am_hal_uart_initialize(BL_UART_INST, &hUART_bl);
    am_hal_uart_power_control(hUART_bl, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(hUART_bl, &bl_uart_config);

    // Disable that pesky FIFO
    UARTn(BL_UART_INST)->LCRH_b.FEN = 0; 

    // Enable the UART pins.
    am_hal_gpio_pinconfig(BL_TX_PAD, bl_uart_tx_pinconfig);
    am_hal_gpio_pinconfig(BL_RX_PAD, bl_uart_rx_pinconfig);

    // Enable interrupts.
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + BL_UART_INST));
    am_hal_uart_interrupt_enable(hUART_bl, (AM_HAL_UART_INT_RX));

    // Provide SVL Packet interfaces
    svl_packet_link_read_fn			( art_svl_ringbuf_read,	        &bl_rx_ringbuf );
    svl_packet_link_avail_fn		( art_svl_ringbuf_available,	&bl_rx_ringbuf );	
    svl_packet_link_millis_fn		( millis );
    svl_packet_link_write_fn		( svl_uart_write_byte, 	        hUART_bl );
}



// ****************************************
// 
// Bootload phase
// 
// ****************************************
void enter_bootload( void ){
  
    bool              done = false;
    uint32_t          frame_address = 0;
    uint16_t          last_page_erased = 0;
    uint8_t           retransmit = 0;
    static uint32_t   frame_buffer[FRAME_BUFFER_SIZE];

    svl_packet_t      svl_packet_incoming_frame = { CMD_FRAME, (uint8_t*)frame_buffer, sizeof(frame_buffer)/sizeof(uint8_t), sizeof(frame_buffer)/sizeof(uint8_t) };
    svl_packet_t      svl_packet_retry = { CMD_RETRY, NULL, 0, 0 };
    svl_packet_t      svl_packet_next = { CMD_NEXT, NULL, 0, 0 };

    debug_printf("phase:\tbootload\n");

    while(!done){

        if(retransmit != 0){
            debug_printf("\trequesting retransmission\n");
            svl_packet_send( (svl_packet_t*)&svl_packet_retry );                        // Ask to retransmit
        }else{
            debug_printf("\trequesting next app frame\n");
            svl_packet_send( (svl_packet_t*)&svl_packet_next );                         // Ask for the next frame packet
        }
        retransmit = 0;

        uint8_t stat = svl_packet_wait( &svl_packet_incoming_frame );
        if( stat != 0 ){                                                                // wait for either a frame or the done command
            debug_printf("\t\terror receiving packet (%d)\n", stat);
            retransmit = 1;
            continue;
        }

        // debug_printf("Successfully received incoming frame packet (todo: add extra details in debug)\n", stat);

        if( svl_packet_incoming_frame.cmd == CMD_FRAME ){
            debug_printf("\t\treceived an app frame\n");
            if( handle_frame_packet( &svl_packet_incoming_frame, &frame_address, &last_page_erased ) != 0 ){
                // debug_printf("\t\t\tbootload error - packet could not be handled\n");
                retransmit = 1;
                continue;
            }
        }else if( svl_packet_incoming_frame.cmd == CMD_DONE ){
            debug_printf("\t\treceived done signal!\n\n");
            done = true;
        }else{
            debug_printf("bootload error - unknown command\n");
            retransmit = 1;
            continue;
        }
    }

    // finish bootloading
}



// ****************************************
// 
// Handle a frame packet
// 
// ****************************************
uint8_t handle_frame_packet(svl_packet_t* packet, uint32_t* p_frame_address, uint16_t* p_last_page_erased ){

    // debug_printf("\t\thandling frame\n");

    uint32_t num_words = (packet->pl_len / 4);
    int32_t i32ReturnCode = 0;

    debug_printf("\t\tframe_address = 0x%08X, num_words = %d\n", *(p_frame_address), num_words);

    // Check payload length is multiple of words
    if( (packet->pl_len % 4) ){
        debug_printf("Error: frame packet not integer multiple of words (4 bytes per word)\n");
        return 1;
    }

    uint32_t offset_address = (*(p_frame_address) + USERCODE_OFFSET); 
    if ( (*p_last_page_erased) < AM_HAL_FLASH_ADDR2PAGE( offset_address ) ) {     // Prevent erasing partially-filled pages
        // debug_printf("Erasing instance %d, page %d\n\r", AM_HAL_FLASH_ADDR2INST( offset_address ), AM_HAL_FLASH_ADDR2PAGE(offset_address) );

        //Erase the 8k page for this address
        i32ReturnCode = am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY, AM_HAL_FLASH_ADDR2INST( offset_address ), AM_HAL_FLASH_ADDR2PAGE( offset_address ) );
        *(p_last_page_erased) = AM_HAL_FLASH_ADDR2PAGE( offset_address );

        if (i32ReturnCode)
        {
            debug_printf("FLASH_MASS_ERASE i32ReturnCode = 0x%x.\n\r", i32ReturnCode);
        }
    }

    // //Record the array
    // debug_printf("Recording %d words (%d bytes) to memory\n", num_words, 4*num_words);
    i32ReturnCode = am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY, (uint32_t*)packet->pl, (uint32_t*)(*(p_frame_address) + USERCODE_OFFSET), num_words);
    if (i32ReturnCode){
        debug_printf("FLASH_WRITE error = 0x%x.\n\r", i32ReturnCode);
        return 1;
    }

    // debug_printf("Array recorded to flash\n");
    *(p_frame_address) += num_words*4;
    return 0;
}



// ****************************************
// 
// Jump to the application
// 
// ****************************************
void app_start( void ){
//     debug_printf("\n\t-- app start --\n");
// #ifdef DEBUG
// #ifdef DEBUG_PRINT_APP
//     uint32_t start_address = USERCODE_OFFSET;     // Print a section of flash
//     debug_printf("Printing page starting at offset 0x%04X\n", start_address);
// #ifdef APP_PRINT_PRETTY
//     for (uint16_t x = 0; x < 512*APP_PRINT_NUM_PAGE; x++){
//         if (x % 8 == 0){
//             debug_printf("\nAdr: 0x%04X", start_address + (x * 4));
//         }
//         debug_printf(" 0x%08X", *(uint32_t *)(start_address + (x * 4)));
//     }
//     debug_printf("\n");
// #else
//     for (uint16_t x = 0; x < 512*APP_PRINT_NUM_PAGE; x++){
//         if (x % 4 == 0){
//             debug_printf("\n");
//         }
//         uint32_t wor = *(uint32_t *)(start_address + (x * 4));
//         debug_printf("%02x%02x %02x%02x", (wor & 0x000000FF), (wor & 0x0000FF00) >> 8, (wor & 0x00FF0000) >> 16, (wor & 0xFF000000) >> 24 );
//         if( (x%4) != 3 ){
//             debug_printf(" ");
//         }
//     }
//     debug_printf("\n");
// #endif // APP_PRINT_PRETTY
// #endif // DEBUG_PRINT_APP
// #endif // DEBUG

    // unsetup(); // todo:

    void* entryPoint = (void *)(*((uint32_t*)(USERCODE_OFFSET + 4)));
    debug_printf("\nJump to App at 0x%08X\n\n", (uint32_t)entryPoint);
    am_util_delay_ms(10);   // Wait for prints to complete
    goto *entryPoint;       // Jump to start of user code
}






// ****************************************
// 
// Debug printf function
// 
// ****************************************
void debug_printf(char* fmt, ...){
#ifdef DEBUG
    char    debug_buffer        [DEBUG_UART_BUF_LEN];
    va_list args;
    va_start (args, fmt);
    vsnprintf(debug_buffer, DEBUG_UART_BUF_LEN, (const char*)fmt, args);
    va_end (args);

    svl_uart_print(hUART_debug, debug_buffer);
#endif //DEBUG  
}



//*****************************************************************************
//
// UART interrupt handlers
//
//*****************************************************************************
void am_uart_isr(void){
    // Service the FIFOs as necessary, and clear the interrupts.
#if BL_UART_INST == 0
    uint32_t ui32Status, ui32Idle;
    am_hal_uart_interrupt_status_get(hUART_bl, &ui32Status, true);
    am_hal_uart_interrupt_clear(hUART_bl, ui32Status);
    am_hal_uart_interrupt_service(hUART_bl, ui32Status, &ui32Idle);
    if (ui32Status & AM_HAL_UART_INT_RX)
    {
        uint8_t c = 0x00;
        if ( svl_uart_read( hUART_bl, (char*)&c, 1) != 0 ){
            art_svl_ringbuf_write( &bl_rx_ringbuf, c );
        }
    }
#else
#ifdef DEBUG
    am_hal_uart_interrupt_status_get(hUART_debug, &ui32Status, true);
    am_hal_uart_interrupt_clear(hUART_debug, ui32Status);
    am_hal_uart_interrupt_service(hUART_debug, ui32Status, &ui32Idle);
#endif // DEBUG
#endif // BL_UART_INST == 0
}

void am_uart1_isr(void){
    // Service the FIFOs as necessary, and clear the interrupts.
#if BL_UART_INST == 1
    uint32_t ui32Status, ui32Idle;
    am_hal_uart_interrupt_status_get(hUART_bl, &ui32Status, true);
    am_hal_uart_interrupt_clear(hUART_bl, ui32Status);
    am_hal_uart_interrupt_service(hUART_bl, ui32Status, &ui32Idle);
    if (ui32Status & AM_HAL_UART_INT_RX)
    {
        uint8_t c = 0x00;
        if ( read( hUART_bl, &c, 1) != 0 ){
            art_svl_ringbuf_write( &bl_rx_ringbuf, c );
        }
    }
#else
#ifdef DEBUG
    uint32_t ui32Status, ui32Idle;
    am_hal_uart_interrupt_status_get(hUART_debug, &ui32Status, true);
    am_hal_uart_interrupt_clear(hUART_debug, ui32Status);
    am_hal_uart_interrupt_service(hUART_debug, ui32Status, &ui32Idle);
#endif // DEBUG
#endif // BL_UART_INST == 0
}



//*****************************************************************************
//
// GPIO interrupt handler
//
//*****************************************************************************
void am_gpio_isr( void ){
    am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(BL_RX_PAD));
    if( bl_baud_ticks_index < BL_BAUD_SAMPLES ){
        bl_baud_ticks[bl_baud_ticks_index++] = CTIMER->STTMR;
    }
}

//*****************************************************************************
//
// STimer interrupt handler
//
//*****************************************************************************
void am_stimer_isr(void)
{
    am_hal_stimer_int_clear(AM_HAL_STIMER_INT_OVERFLOW);
    ap3_stimer_overflows += 1;
    // At the fastest rate (3MHz) the 64 bits of the stimer
    // along with this overflow counter can keep track of
    // the time for ~ 195,000 years without wrapping to 0
}