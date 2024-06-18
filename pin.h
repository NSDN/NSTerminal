#ifndef __PIN_H
#define __PIN_H

// LEDs
#define LED0    P32
#define LED1    P33

// CH9121
#define ERST    P16
#define EFACT   P17
#define ECFG    P15
#define ETCP    P14
#define ERUN    P13

// LCD
#define LSDA    P11
#define LSCL    P12
#define LBLK    P27

// Buttons
#define BCx     (P0 & 0x1F)
#define BC0     P00
#define BC1     P01
#define BC2     P02
#define BC3     P03
#define BC4     P04
#define BR0     P05
#define BR1     P06
#define BR2     P07
#define BR3     P41

#define BDU     P20
#define BDL     P21
#define BDR     P22
#define BDD     P23

#define BEU     P36
#define BED     P37

#endif
