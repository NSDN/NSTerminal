#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>

void lcdInit();
void lcdDraw(uint8_t x, uint8_t y, char c);
void lcdPrint(uint8_t x, uint8_t y, char* str);
void lcdClear();

#endif
