// Author: Owen Lyke
// Created: April 2 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates that the IDE is able to compile the Ambiq Apollo3 HAL
// Turn on verbose output and find the output path (i.e. ~/arduino_build_######/example0_compilation.ino.bin)
// Use the Ambiq SDK + tools to upload the binary to your microcontroller

static int boardSetup(void);

void setup() {
  // put your setup code here, to run once:
  boardSetup();

  am_util_stdio_terminal_clear();

  am_util_stdio_printf("SparkFun Arduino Apollo3 Compilation Test\n");
  am_util_stdio_printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);
  am_bsp_uart_string_print("Hello, World!\r\n");  // Sting_print has less overhead than printf (and less risky behavior since no varargs)
}

void loop() {
  // put your main code here, to run repeatedly:

}

static int boardSetup(void)
{
    // Set the clock frequency.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.
    am_bsp_low_power_init();

    // Initialize the printf interface for ITM/SWO output.
    am_bsp_uart_printf_enable(); // Enable UART - will set debug output to UART
    //am_bsp_itm_printf_enable(); // Redirect debug output to SWO

    // Setup LED's as outputs
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_RED, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_GREEN, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_YELLOW, g_AM_HAL_GPIO_OUTPUT_12);

    // Turn on the LEDs
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_RED);
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_GREEN);
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_YELLOW);

    return 0;
}
