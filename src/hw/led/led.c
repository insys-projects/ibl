
#include "types.h"
#include "gpio.h"
#include "led.h"
#include "target.h"

void LED_init(void)
{
    hwGpioSetDirection(PIN_A, GPIO_OUT);
    hwGpioSetDirection(PIN_B, GPIO_OUT);
    hwGpioSetDirection(PIN_C, GPIO_OUT);
    hwGpioSetDirection(PIN_D, GPIO_OUT);
    hwGpioSetDirection(PIN_E, GPIO_OUT);
    hwGpioSetDirection(PIN_F, GPIO_OUT);
    hwGpioSetDirection(PIN_G, GPIO_OUT);
    hwGpioSetDirection(PIN_DOT, GPIO_OUT);
}

void LED_off(void)
{
    hwGpioSetOutput(PIN_A);
    hwGpioSetOutput(PIN_B);
    hwGpioSetOutput(PIN_C);
    hwGpioSetOutput(PIN_D);
    hwGpioSetOutput(PIN_E);
    hwGpioSetOutput(PIN_F);
    hwGpioSetOutput(PIN_G);
    hwGpioSetOutput(PIN_DOT);
}

void LED_on(void)
{
    hwGpioClearOutput(PIN_A);
    hwGpioClearOutput(PIN_B);
    hwGpioClearOutput(PIN_C);
    hwGpioClearOutput(PIN_D);
    hwGpioClearOutput(PIN_E);
    hwGpioClearOutput(PIN_F);
    hwGpioClearOutput(PIN_G);
    hwGpioClearOutput(PIN_DOT);
}
