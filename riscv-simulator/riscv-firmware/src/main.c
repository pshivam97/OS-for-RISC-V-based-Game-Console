#include <stdint.h>

extern volatile uint32_t TimerTicks;
#define CARTRIDGE_STATUS_REG (*(volatile uint32_t *)0x4000001C)
typedef void (*TEntryFunction)(void);

uint32_t foo(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);

int main() {
    TEntryFunction EntryFunction;
    uint32_t V = foo(1, 2, 3, 4, 5);
    while(1){
        if(CARTRIDGE_STATUS_REG & 0x1){
            EntryFunction = (TEntryFunction)(CARTRIDGE_STATUS_REG & 0xFFFFFFFC);
            EntryFunction();
        }
    }


    return 0;
}

uint32_t foo(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e){
    return a + b * 2 + c * 3 + d * 4 + e * 5;
}
