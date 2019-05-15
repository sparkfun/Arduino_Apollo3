#include "ap3_gpio.h"
#include "variant.h"

ap3_gpio_pad_t ap3_gpio_pin2pad(ap3_gpio_pin_t pin){
    return ap3_variant_pinmap[pin];  
}










typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArg)(void*);
typedef struct {
    voidFuncPtr fn;
    void* arg;
    bool functional;
} InterruptHandle_t;
// static InterruptHandle_t __pinInterruptHandlers[AP3_GPIO_MAX_EXT_INT_HANDLERS] = {0,};

void padMode(uint8_t pad, am_hal_gpio_pincfg_t mode, ap3_err_t* retval){
    if(!ap3_gpio_is_valid(pad)) {
        if( retval != NULL){
            *retval = AP3_OUT_OF_RANGE; 
        }
        return;
    }
    am_hal_gpio_pinconfig(pad, mode);
}

void padMode(uint8_t pad, am_hal_gpio_pincfg_t mode){
    padMode(pad, mode, NULL);
}

void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode, ap3_err_t* retval){
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    padMode(pad, mode, retval);
}

void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode){
    pinMode(pin, mode, NULL);
}

extern void digitalWrite(uint8_t pin, uint8_t val)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if(!ap3_gpio_is_valid(pad)) {
        return;
    }
    if(val){
        am_hal_gpio_output_set(ap3_gpio_pin2pad(pin));
    }else{
        am_hal_gpio_output_clear(ap3_gpio_pin2pad(pin));
    }
}

extern int digitalRead(uint8_t pin)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if(!ap3_gpio_is_valid(pad)) {
        return 0;
    }
    // determine if output or input
    uint32_t ui32BaseAddr = pad / 8;
    uint32_t ui32BaseShift = ((pad % 8) * 4) + 1;
    uint8_t output = ((AM_REGVAL(&GPIO->CFGA + ui32BaseAddr) >> ui32BaseShift) & 0x03);

    if(output){
        return (int)am_hal_gpio_output_read(pad);
    }else{
        return (int)am_hal_gpio_input_read(pad);
    }
    return 0;
}

// static intr_handle_t gpio_intr_handle = NULL;

static void onPinInterrupt()
{
    // uint32_t gpio_intr_status_l=0;
    // uint32_t gpio_intr_status_h=0;

    // gpio_intr_status_l = GPIO.status;
    // gpio_intr_status_h = GPIO.status1.val;
    // GPIO.status_w1tc = gpio_intr_status_l;//Clear intr for gpio0-gpio31
    // GPIO.status1_w1tc.val = gpio_intr_status_h;//Clear intr for gpio32-39

    // uint8_t pin=0;
    // if(gpio_intr_status_l) {
    //     do {
    //         if(gpio_intr_status_l & ((uint32_t)1 << pin)) {
    //             if(__pinInterruptHandlers[pin].fn) {
    //                 if(__pinInterruptHandlers[pin].arg){
    //                     ((voidFuncPtrArg)__pinInterruptHandlers[pin].fn)(__pinInterruptHandlers[pin].arg);
    //                 } else {
    //                     __pinInterruptHandlers[pin].fn();
    //                 }
    //             }
    //         }
    //     } while(++pin<32);
    // }
    // if(gpio_intr_status_h) {
    //     pin=32;
    //     do {
    //         if(gpio_intr_status_h & ((uint32_t)1 << (pin - 32))) {
    //             if(__pinInterruptHandlers[pin].fn) {
    //                 if(__pinInterruptHandlers[pin].arg){
    //                     ((voidFuncPtrArg)__pinInterruptHandlers[pin].fn)(__pinInterruptHandlers[pin].arg);
    //                 } else {
    //                     __pinInterruptHandlers[pin].fn();
    //                 }
    //             }
    //         }
    //     } while(++pin<GPIO_PIN_COUNT);
    // }
}

extern void cleanupFunctional(void* arg);

extern void attachInterruptFunctionalArg(uint8_t pin, voidFuncPtrArg userFunc, void * arg, int intr_type, bool functional)
{
    // static bool interrupt_initialized = false;

    // if(!interrupt_initialized) {
    //     interrupt_initialized = true;
    //     esp_intr_alloc(ETS_GPIO_INTR_SOURCE, (int)ESP_INTR_FLAG_IRAM, __onPinInterrupt, NULL, &gpio_intr_handle);
    // }

    // // if new attach without detach remove old info
    // if (__pinInterruptHandlers[pin].functional && __pinInterruptHandlers[pin].arg)
    // {
    // 	cleanupFunctional(__pinInterruptHandlers[pin].arg);
    // }
    // __pinInterruptHandlers[pin].fn = (voidFuncPtr)userFunc;
    // __pinInterruptHandlers[pin].arg = arg;
    // __pinInterruptHandlers[pin].functional = functional;

    // esp_intr_disable(gpio_intr_handle);
    // if(esp_intr_get_cpu(gpio_intr_handle)) { //APP_CPU
    //     GPIO.pin[pin].int_ena = 1;
    // } else { //PRO_CPU
    //     GPIO.pin[pin].int_ena = 4;
    // }
    // GPIO.pin[pin].int_type = intr_type;
    // esp_intr_enable(gpio_intr_handle);
}

extern void attachInterruptArg(uint8_t pin, voidFuncPtrArg userFunc, void * arg, int intr_type)
{
	// __attachInterruptFunctionalArg(pin, userFunc, arg, intr_type, false);
}

extern void attachInterrupt(uint8_t pin, voidFuncPtr userFunc, int intr_type) {
    // __attachInterruptFunctionalArg(pin, (voidFuncPtrArg)userFunc, NULL, intr_type, false);
}

extern void detachInterrupt(uint8_t pin)
{
    // esp_intr_disable(gpio_intr_handle);
    // if (__pinInterruptHandlers[pin].functional && __pinInterruptHandlers[pin].arg)
    // {
    // 	cleanupFunctional(__pinInterruptHandlers[pin].arg);
    // }
    // __pinInterruptHandlers[pin].fn = NULL;
    // __pinInterruptHandlers[pin].arg = NULL;
    // __pinInterruptHandlers[pin].functional = false;

    // GPIO.pin[pin].int_ena = 0;
    // GPIO.pin[pin].int_type = 0;
    // esp_intr_enable(gpio_intr_handle);
}


// // Look, here's something that the ESP32 core did. Might be worth trying out.
// extern void __pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode)
// {
//     // code
// }
// extern void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode) __attribute__ ((weak, alias("__pinMode")));
// extern void digitalWrite(uint8_t pin, uint8_t val) __attribute__ ((weak, alias("__digitalWrite")));
// extern int digitalRead(uint8_t pin) __attribute__ ((weak, alias("__digitalRead")));
// extern void attachInterrupt(uint8_t pin, voidFuncPtr handler, int mode) __attribute__ ((weak, alias("__attachInterrupt")));
// extern void attachInterruptArg(uint8_t pin, voidFuncPtrArg handler, void * arg, int mode) __attribute__ ((weak, alias("__attachInterruptArg")));
// extern void detachInterrupt(uint8_t pin) __attribute__ ((weak, alias("__detachInterrupt")));