
#include "types.h"
#include "gpio.h"
#include "led.h"
#include "target.h"

void LED_init(void)
{
#if !defined(INSYS_FM408C_1G) && !defined(INSYS_FM408C_2G)
    hwGpioSetDirection(PIN_A, GPIO_OUT);
    hwGpioSetDirection(PIN_B, GPIO_OUT);
    hwGpioSetDirection(PIN_C, GPIO_OUT);
    hwGpioSetDirection(PIN_D, GPIO_OUT);
    hwGpioSetDirection(PIN_E, GPIO_OUT);
    hwGpioSetDirection(PIN_F, GPIO_OUT);
    hwGpioSetDirection(PIN_G, GPIO_OUT);
    hwGpioSetDirection(PIN_DOT, GPIO_OUT);
    hwGpioSetDirection(PIN_GPIO13, GPIO_OUT);
#else
    hwGpioSetDirection(0, GPIO_OUT);
    hwGpioSetDirection(12, GPIO_OUT);
#endif
}

void LED_off(void)
{
#if !defined(INSYS_FM408C_1G) && !defined(INSYS_FM408C_2G)
    hwGpioSetOutput(PIN_A);
    hwGpioSetOutput(PIN_B);
    hwGpioSetOutput(PIN_C);
    hwGpioSetOutput(PIN_D);
    hwGpioSetOutput(PIN_E);
    hwGpioSetOutput(PIN_F);
    hwGpioSetOutput(PIN_G);
    hwGpioSetOutput(PIN_DOT);
#else
    hwGpioSetOutput(0);
    hwGpioSetOutput(12);
#endif
}

void LED_on(void)
{
#if !defined(INSYS_FM408C_1G) && !defined(INSYS_FM408C_2G)
    hwGpioClearOutput(PIN_A);
    hwGpioClearOutput(PIN_B);
    hwGpioClearOutput(PIN_C);
    hwGpioClearOutput(PIN_D);
    hwGpioClearOutput(PIN_E);
    hwGpioClearOutput(PIN_F);
    hwGpioClearOutput(PIN_G);
    hwGpioClearOutput(PIN_DOT);
#else
    hwGpioClearOutput(0);
    hwGpioClearOutput(12);
#endif
}

void LED_smart(int symbol)
{
    unsigned char   code = 0;

    LED_off();

#if !defined(INSYS_FM408C_1G) && !defined(INSYS_FM408C_2G)
    switch(symbol) {
    case '0':	code = 0x3F; break;
    case '1':	code = 0x06; break;
    case '2':	code = 0x5B; break;
    case '3':	code = 0x4F; break;
    case '4':	code = 0x66; break;
    case '5':	code = 0x6D; break;
    case '6':	code = 0x7D; break;
    case '7':	code = 0x07; break;
    case '8':	code = 0x7F; break;
    case '9':	code = 0x6F; break;
    case 'A':	code = 0x77; break;
    case 'b':	code = 0x7C; break;
    case 'c':	code = 0x58; break;
    case 'C':	code = 0x39; break;
    case 'd':	code = 0x5E; break;
    case 'E':	code = 0x79; break;
    case 'F':	code = 0x71; break;
    case 'h':	code = 0x74; break;
    case 'H':	code = 0x76; break;
    case 'L':	code = 0x38; break;
    case 'o':	code = 0x5C; break;
    case 'P':	code = 0x73; break;
    case 'u':	code = 0x1C; break;
    case 'U':	code = 0x3E; break;
    case 'Y':	code = 0x6E; break;
    case 'S':	code = 0xED; break;
    }

    if((code>>0)&1) hwGpioClearOutput(PIN_A);   // LED ON
    if((code>>1)&1) hwGpioClearOutput(PIN_B);   // LED ON
    if((code>>2)&1) hwGpioClearOutput(PIN_C);   // LED ON
    if((code>>3)&1) hwGpioClearOutput(PIN_D);   // LED ON
    if((code>>4)&1) hwGpioClearOutput(PIN_E);   // LED ON
    if((code>>5)&1) hwGpioClearOutput(PIN_F);   // LED ON
    if((code>>6)&1) hwGpioClearOutput(PIN_G);   // LED ON
    if((code>>7)&1) hwGpioClearOutput(PIN_DOT); // LED ON
#else
    switch(symbol) {
    case '0': { hwGpioClearOutput(0); } break;
    case '1': { hwGpioClearOutput(12); } break;
    case '2': { hwGpioClearOutput(0); hwGpioClearOutput(12); } break;
    }
#endif
}

void GPIO_set(void)
{
    hwGpioSetOutput(PIN_GPIO13);
}

void GPIO_clear(void)
{
    hwGpioClearOutput(PIN_GPIO13);

}

void DDR_ok(void)
{
    LED_off();
    hwGpioClearOutput(PIN_A);
    hwGpioClearOutput(PIN_D);
}

void DDR_error(void)
{
    LED_off();
    hwGpioClearOutput(PIN_F);
    hwGpioClearOutput(PIN_C);
}
