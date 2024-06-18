#include "sys.h"
#include "pin.h"

#include <string.h>

void sysPinConfig() {
    PORT_CFG &= ~(bP0_OC | bP2_OC);
    P0_DIR |= 0xE0; // P05 P06 P07
    P2_DIR |= (1 << 7);
    P4_DIR |= (1 << 1);
}

void sysClockConfig() {
#ifdef USE_EXT_OSC
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_XT;
    delay(10);
#endif
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG &= ~MASK_SYS_CK_DIV;
    CLOCK_CFG |= 6; // fSys = 56MHz;
    PLL_CFG = 0xFC; // fPll = 336MHz;
    SAFE_MOD = 0xFF;
#ifdef USE_EXT_OSC
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG &= ~bOSC_EN_INT;
    SAFE_MOD = 0xFF;
#endif
}

void delay(uint16_t t) {
    while (t) {
        delay_us(1000);
        -- t;
    }
}

void delay_us(uint16_t t) {
    while (t) {
		++ SAFE_MOD;
#ifdef	FREQ_SYS
#if		FREQ_SYS >= 14000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 16000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 18000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 20000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 22000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 24000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 26000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 28000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 30000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 32000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 34000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 36000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 38000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 40000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 42000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 44000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 46000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 48000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 50000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 52000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 54000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 56000000
		++ SAFE_MOD;
#endif
#endif
		-- t;
	}
}

void sysTickConfig() {
    T2MOD |= (bTMR_CLK | bT2_CLK); C_T2 = 0;
    RCAP2 = T2COUNT = 0xFFFF - (uint16_t) (FREQ_SYS / 1000L);   // 1000Hz
    TR2 = 1;
    ET2 = 1;
}

volatile uint32_t sysTickCount = 0;

void __tim2Interrupt() __interrupt (INT_NO_TMR2) __using (2) {
    if (TF2) {
        TF2 = 0;
        sysTickCount += 1;
    }
}

uint32_t sysGetTickCount() {
    return sysTickCount;
}

void uartConfig(uint32_t baud) {
    uint32_t x; uint8_t x2;
    
    SM0 = 0; SM1 = 1; SM2 = 0;
    TCLK = 0; RCLK = 0;
    PCON |= SMOD;
    x = 10 * FREQ_SYS / baud / 16;

    x2 = x % 10;
    x /= 10;
    if (x2 >= 5) x++;

    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;
    TH1 = 0 - x;
    TR1 = 1;
    TI = 1;
    REN = 1;
}

volatile __bit busy = 0;
static uint8_t recBuf[UART_BUF_SIZE] = { 0 };
volatile uint8_t recPtr = 0;

void __uart_interrupt() __interrupt (INT_NO_UART0) __using (1) {
    if (TI) {
        TI = 0;
        busy = 0;
    }
    if (RI) {
        RI = 0;
        if (recPtr < UART_BUF_SIZE) {
            recBuf[recPtr] = SBUF;
            recPtr += 1;
        }
    }
}

void uartSend(uint8_t data) {
    while (busy);
    busy = 1;
    SBUF = data;
}

void uartWrite(char* s) {
    while (*s) {
        uartSend(*s++);
    }
}

uint8_t uartGet(uint8_t* dat) {
    if (recPtr > 0) {
        *dat = recBuf[recPtr - 1];
        memset(recBuf, 0, UART_BUF_SIZE);
        recPtr = 0;
        return 1;
    } else
        return 0;
}

uint8_t uartGets(uint8_t* dat, uint8_t len) {
    if (recPtr >= len) {
        memcpy(dat, recBuf, recPtr);
        memset(recBuf, 0, UART_BUF_SIZE);
        recPtr = 0;
        return 1;
    } else
        return 0;
}
