#include "lcd.h"

#include "sys.h"
#include "pin.h"

#define IIC_ADDR        0x4E

#define LCD_DAT_MASK	0xF0
#define LCD_RS_MASK		0x01
#define LCD_RW_MASK		0x02
#define LCD_E_MASK		0x04

void __iic_start() {
    LSDA = 1;
    LSCL = 1; delay_us(5);
    LSDA = 0; delay_us(5);
}

void __iic_stop() {
    LSDA = 0;
    LSCL = 1; delay_us(5);
    LSDA = 1; delay_us(5);
}

void __iic_ack() {
    LSCL = 0;
    LSDA = 1;
    LSCL = 1; delay_us(5);
    for (uint8_t i = 0; i < 0xFF; i++)
        if (LSDA == 0) break;
    LSCL = 0; delay_us(5);
}

void __iic_nak() {
    LSCL = 0;
    LSDA = 1;
    LSCL = 1; delay_us(5);
    LSCL = 0; delay_us(5);
}

void __iic_wr(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        LSCL = 0; delay_us(5);
        LSDA = (data & 0x80); delay_us(5);
        LSCL = 1; delay_us(5);
        data <<= 1;
    }
    LSCL = 0;
    LSDA = 1;
    delay_us(5);
}

uint8_t __iic_rd() {
    uint8_t data = 0;
    __bit tmp;
    LSCL = 0; delay_us(5);
    for (uint8_t i = 0; i < 8; i++) {
        LSCL = 1; delay_us(5);
        tmp = LSDA;
        data <<= 1;
        data |= tmp;
        delay_us(5);
        LSCL = 0; delay_us(5);
    }
    return data;
}

void __iic_write(uint8_t data) {
    __iic_start();
    __iic_wr(IIC_ADDR & 0xFE);
    __iic_ack();
    __iic_wr(data);
    __iic_ack();
    __iic_stop();
}

uint8_t __iic_read() {
    uint8_t data = 0;
    __iic_start();
    __iic_wr(IIC_ADDR | 0x01);
    __iic_ack();
    data = __iic_rd();
    __iic_nak();
    __iic_stop();
    return data;
}

void waitBusy() {
    uint8_t t = 0;
    t = __iic_read();
    t &= ~(LCD_E_MASK | LCD_RS_MASK);
    t |= (LCD_RW_MASK | LCD_DAT_MASK);
    __iic_write(t);
    delay_us(5);

    t |= LCD_E_MASK;
    __iic_write(t);
    delay_us(5);
    while (__iic_read() & 0x80) {
        t &= ~LCD_E_MASK;
        __iic_write(t);
        delay_us(5);
        t |= LCD_E_MASK;
        __iic_write(t);
        delay_us(5);
    }
    t &= ~LCD_E_MASK;
    __iic_write(t);
}

void switchTo4BitBus() {
    delay(20);

    uint8_t t = 0;
    waitBusy();
    t = __iic_read();
    t &= ~LCD_RS_MASK;
    t &= ~(LCD_RW_MASK | LCD_E_MASK);
    t &= ~LCD_DAT_MASK;
    t |= (0x20 & LCD_DAT_MASK);
    __iic_write(t);
    
    t |= LCD_E_MASK;
    __iic_write(t);
    delay_us(5);
    t &= ~LCD_E_MASK;
    __iic_write(t);

    delay(5);
}

void writeByte(uint8_t value, uint8_t rs) {
    uint8_t t = 0;
    waitBusy();
    t = __iic_read();
    t &= ~LCD_RS_MASK;
    t |= rs ? LCD_RS_MASK : 0;
    t &= ~(LCD_RW_MASK | LCD_E_MASK);
    t &= ~LCD_DAT_MASK;
    t |= (value & LCD_DAT_MASK);
    __iic_write(t);
    t |= LCD_E_MASK;
    __iic_write(t);
    delay_us(5);

    t &= ~LCD_E_MASK;
    t &= ~LCD_DAT_MASK;
    t |= ((value << 4) & LCD_DAT_MASK);
    __iic_write(t);
    t |= LCD_E_MASK;
    __iic_write(t);
    delay_us(5);

    t &= ~LCD_E_MASK;
    __iic_write(t);
}

void writeCmd(uint8_t value) { writeByte(value, 0); }

void writeData(uint8_t value) { writeByte(value, 1); }

void lcdInit() {
    switchTo4BitBus();

    writeCmd(0x28);
    writeCmd(0x08);
    writeCmd(0x06);
    writeCmd(0x01);
    writeCmd(0x0C);
    writeCmd(0x80);
}

void lcdDraw(uint8_t x, uint8_t y, char c) {
    writeCmd(0x80 + ((y & 0x01) ? 0x40 : 0) + ((y > 1) ? 0x14 : 0) + x);
    writeData(c);
}

void lcdPrint(uint8_t x, uint8_t y, char* str) {
    uint8_t i; uint8_t tx = x, ty = y;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            tx = x;
            ty = 1 - ty;
        } else {
            lcdDraw(tx + i, ty, str[i]);
        }
    }
}

void lcdClear() {
    writeCmd(0x01);
    delay(10);
}
