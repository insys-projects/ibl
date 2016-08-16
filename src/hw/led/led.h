
#ifndef LED_H_
#define LED_H_

#include "types.h"

#define PIN_A       0
#define PIN_B       1
#define PIN_C       2
#define PIN_D       3
#define PIN_E       4
#define PIN_F       7
#define PIN_G       8
#define PIN_DOT     9
#define PIN_GPIO13  13

void LED_init(void);
void LED_off(void);
void LED_on(void);
void LED_smart(int symbol);
void GPIO_set(void);
void GPIO_clear(void);

#endif // LED_H_
