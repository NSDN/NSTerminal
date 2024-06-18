#ifndef __SYS_H
#define __SYS_H

#include "ch558.h"
#include <stdint.h>

//#define USE_EXT_OSC
#define FREQ_SYS 56000000
#define UART_BUF_SIZE 32

void sysPinConfig();
void sysClockConfig();
void delay(uint16_t t);
void delay_us(uint16_t t);
void sysTickConfig();
uint32_t sysGetTickCount();

void uartConfig(uint32_t baud);
void uartSend(uint8_t data);
void uartWrite(char* s);
uint8_t uartGet(uint8_t* dat);
uint8_t uartGets(uint8_t* dat, uint8_t len);

#endif