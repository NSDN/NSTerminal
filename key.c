#include "key.h"
#include "sys.h"
#include "pin.h"

const static uint8_t keyMap[2][4][5][2] = {
    {
        { { 'q', 'w' }, { 'e', 'r' }, { 't', 'y' }, { 'u', 'i' }, { 'o', 'p' } },
        { { 'a', 's' }, { 'd', 'f' }, { 'g', 'h' }, { 'j', 'k' }, { 'l', '"' } },
        { { 'z', 'x' }, { 'c', 'v' }, { 'b', 'n' }, { 'm', ',' }, { '\b', '\0' } },
        { { KEY_SHIFT, '\0' }, { ' ', '\0' }, { ' ', '\0' }, { KEY_SWITCH, '\0' }, { '\0', '\0' } }
    },
    {
        { { '1', '\0' }, { '2', '\0' }, { '3', '\0' }, { '+', '\0' }, { '*', '\0' } },
        { { '4', '\0' }, { '5', '\0' }, { '6', '\0' }, { '-', '\0' }, { '/', '\0' } },
        { { '7', '\0' }, { '8', '\0' }, { '9', '\0' }, { '=', '\0' }, { '\b', '\0' } },
        { { '.', '\0' }, { '0', '\0' }, { '(', ')' }, { KEY_SWITCH, '\0' }, { '\0', '\0' } },
    }
};

__bit mode = 0, single = 0, shift = 0;
uint8_t curr = 0;
uint16_t counter = 0;

void scanRow(uint8_t row) {
    uint8_t val;
    BR0 = row != 0;
    BR1 = row != 1;
    BR2 = row != 2;
    BR3 = row != 3;
    delay_us(5);
    val = BCx;
    for (uint8_t i = 0; i < 5; i++) {
        if ((val & (1 << i)) == 0) {
            while ((val & (1 << i)) == 0) {
                val = BCx;
                delay_us(5);
            }
            
            const uint8_t* btn = keyMap[mode][3 - row][i];
            if (btn[0] == '\0')
                continue;
            if (btn[1] == '\0') {
                single = 1;
                curr = btn[0];
            } else {
                single = 0;
                counter = 0;
                curr = (curr == btn[0] ? btn[1] : btn[0]);
            }
        }
    }

    if (BDU == 0) {
        while (BDU == 0)
            delay_us(5);
        single = 1;
        curr = '\n';
    } 
    if (BDD == 0) {
        while (BDD == 0)
            delay_us(5);
        single = 1;
        curr = KEY_ESCAPE;
    }
}

uint8_t getKey() {
    uint8_t v = 0;
    for (v = 0; v < 4; v++)
        scanRow(v);

    delay(1);

    if (single) {
        switch (curr) {
            case KEY_SHIFT:
                shift = !shift;
                return KEY_NONE;
                break;
            case KEY_SWITCH:
                mode = !mode;
                single = 0;
                counter = 0;
                curr = 0;
                return KEY_NONE;
                break;
        }
        single = 0;
        counter = 0;
        v = curr;
        curr = 0;
        return v;
    } else {
        counter += 1;
        if (counter > 10) {
            counter = 0;
            v = curr;
            curr = 0;
            if (shift && (v >= 'a' && v <= 'z')) {
                v -= 0x20;
                shift = 0;
            }
            return v;
        }
    }

    return KEY_NONE;
}
