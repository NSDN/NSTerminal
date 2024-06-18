#include "sys.h"
#include "lcd.h"
#include "key.h"
#include "pin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __tim2Interrupt() __interrupt (INT_NO_TMR2) __using (2);
void __uart_interrupt() __interrupt (INT_NO_UART0) __using (1);

#define __flip_led() do { LED0 = !LED0; LED1 = !LED1; } while (0)

char outBuf[4][20] = { { ' ' }, { ' ' }, { ' ' }, { ' ' } };
int8_t xptr = 0, yptr = 0;

void main() {
    sysClockConfig();
    delay(10);

    sysPinConfig();
    LED0 = 0; LED1 = 1; LBLK = 0;

    sysTickConfig();
    uartConfig(115200);
    ES = 1;
    EA = 1;
    delay(1000);
    __flip_led();
    LBLK = 1;

    lcdInit();
    delay(20);
    lcdClear();
    delay(20);

    while (1) {
        uint8_t key = getKey();
        if (key != KEY_NONE) {
            switch (key) {
            case KEY_ESCAPE:
                memset(outBuf, ' ', 20 * 4);
                xptr = 0; yptr = 0;
                break;
            case '\b':
                xptr -= 1;
                if (xptr < 0) {
                    yptr -= 1;
                    if (yptr < 0)
                        yptr = 0;
                    xptr = 19;
                }
                outBuf[yptr][xptr] = ' ';
                break;
            case '\n':
                xptr = 0;
                yptr += 1;
                if (yptr >= 4) {
                    yptr = 3;
                    memcpy(outBuf[0], outBuf[1], 20 * 3);
                    memset(outBuf[3], ' ', 20);
                }
                break;
            default:
                outBuf[yptr][xptr] = key;
                xptr += 1;
                if (xptr >= 20) {
                    xptr = 0;
                    yptr += 1;
                    if (yptr >= 4)
                        yptr = 0;
                }
                break;
            }
        }

        for (uint8_t i = 0; i < 4; i++)
            lcdPrint(0, i, outBuf[i]);
    }
}
