/*
  Artemis Low Power: How low can we go?
  By: Nathan Seidle
  SparkFun Electronics
  Date: October 17th, 2019
  License: This code is public domain. Based on deepsleep_wake.c from Ambiq SDK v2.2.0.
  A big thanks to robin_hodgson for pointing out the HFRC shutdown requirement and
  turning off the SWD pins.

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15376

  How close can we get to 2.7uA in deep sleep?
  This example shows how decrease the Artemis current consumption to ~2.4uA in deep sleep
  with a wake up every 5 seconds to blink the LED. The RTC is used to trigger an interrupt
  every second.

  Note that Artemis modules with revision A1 silicon will use ~30uA. Please see the
  Ambiq errata for more information: https://www.ambiqmicro.com/static/mcu/files/Apollo3_Blue_Errata_List_v1_0_external_release.pdf

  To monitor the current to the Edge cut the MEAS jumper, solder in headers, and attach
  a DMM via IC hooks (https://www.sparkfun.com/products/506).

  The USB to serial bridge draws some current:
    Serial Basic C - ~1.2uA (https://www.sparkfun.com/products/15096)
    FTDI Basic - ~5.5uA (https://www.sparkfun.com/products/9873)

*/

static uint32_t g_RTCseconds = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Low power sleep example");

    pinMode(LED_BUILTIN, OUTPUT);

    //Turn off ADC
    power_adc_disable();

    // Set the clock frequency.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Initialize for low power in the power control block
    am_hal_pwrctrl_low_power_init();

    // Disabling the debugger GPIOs saves about 1.2 uA total:
    am_hal_gpio_pinconfig(20 /* SWDCLK */, g_AM_HAL_GPIO_DISABLE);
    am_hal_gpio_pinconfig(21 /* SWDIO */, g_AM_HAL_GPIO_DISABLE);

    // These two GPIOs are critical: the TX/RX connections between the Artemis module and the CH340S on the Blackboard
    // are prone to backfeeding each other. To stop this from happening, we must reconfigure those pins as GPIOs
    // and then disable them completely:
    Serial.println("The TX and RX pins need to be disabled to minimize the current draw.");
    Serial.println("You should not see any more Serial messages after this...");
    delay(100);
    am_hal_gpio_pinconfig(48 /* TXO-0 */, g_AM_HAL_GPIO_DISABLE);
    am_hal_gpio_pinconfig(49 /* RXI-0 */, g_AM_HAL_GPIO_DISABLE);

    // The default Arduino environment runs the System Timer (STIMER) off the 48 MHZ HFRC clock source.
    // The HFRC appears to take over 60 uA when it is running, so this is a big source of extra
    // current consumption in deep sleep.
    // For systems that might want to use the STIMER to generate a periodic wakeup, it needs to be left running.
    // However, it does not have to run at 48 MHz. If we reconfigure STIMER (system timer) to use the 32768 Hz
    // XTAL clock source instead the measured deepsleep power drops by about 64 uA.
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);

    // This option selects 32768 Hz via crystal osc. This appears to cost about 0.1 uA versus selecting "no clock"
    am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);

    // This option would be available to systems that don't care about passing time, but might be set
    // to wake up on a GPIO transition interrupt.
    //am_hal_stimer_config(AM_HAL_STIMER_NO_CLK);

    //Turn OFF Flash1
    if (am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_512K))
    {
        while (1)
            ;
    }

    // Power down SRAM
    PWRCTRL->MEMPWDINSLEEP_b.SRAMPWDSLP = PWRCTRL_MEMPWDINSLEEP_SRAMPWDSLP_ALLBUTLOWER32K;

    setupRTC();

    Serial.println("Going to sleep...");
    delay(100);   //Wait for print to complete
    Serial.end(); //Disable Serial

    // Enable interrupts to the core.
    am_hal_interrupt_master_enable();
}

void loop()
{
    // Go to Deep Sleep.
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
}

void setupRTC()
{
    // Enable the XT for the RTC.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_START, 0);

    // Select XT for RTC clock source
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);

    // Enable the RTC.
    am_hal_rtc_osc_enable();

    // Set the alarm repeat interval to be every second.
    am_hal_rtc_alarm_interval_set(AM_HAL_RTC_ALM_RPT_SEC);

    // Clear the RTC alarm interrupt.
    am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

    // Enable the RTC alarm interrupt.
    am_hal_rtc_int_enable(AM_HAL_RTC_INT_ALM);

    // Enable RTC interrupts to the NVIC.
    NVIC_EnableIRQ(RTC_IRQn);
}

extern "C" void am_rtc_isr(void)
{
    // Clear the RTC alarm interrupt.
    am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

    // Check the desired number of seconds until LED is toggled.
    if (++g_RTCseconds >= 5)
    {
        // Reset the seconds counter.
        g_RTCseconds = 0;

        // Toggle LED
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
    }
}
