#include <stdint.h>

#define TEXTMODE 0x0
#define GRAPHICSMODE 0x1
#define BACKGROUND_CONTROL_0 (*(volatile uint32_t *) (0x500FF100))
#define BACKGROUND_CONTROL_1 (*(volatile uint32_t *) (0x500FF104))
#define BACKGROUND_CONTROL_2 (*(volatile uint32_t *) (0x500FF108))
#define BACKGROUND_CONTROL_3 (*(volatile uint32_t *) (0x500FF10C))
#define BACKGROUND_CONTROL_4 (*(volatile uint32_t *) (0x500FF110))
#define BACKGROUND_PALETTE_0 (*(volatile uint32_t *) (0x500FC000))
#define BACKGROUND_PALETTE_1 (*(volatile uint32_t *) (0x500FC400))
#define BACKGROUND_PALETTE_2 (*(volatile uint32_t *) (0x500FC800))
#define BACKGROUND_PALETTE_3 (*(volatile uint32_t *) (0x500FCC00))
#define SPRITE_PALETTE_0 (*(volatile uint32_t *) (0x500FD000))
#define SPRITE_PALETTE_1 (*(volatile uint32_t *) (0x500FD400))
#define SPRITE_PALETTE_2 (*(volatile uint32_t *) (0x500FD800))
#define SPRITE_PALETTE_3 (*(volatile uint32_t *) (0x500FDC00))
#define INTERRUPT_ENABLE   (*((volatile uint32_t *)0x40000000))
#define INTERRUPT_PENDING   (*((volatile uint32_t *)0x40000004))
#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW    (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH   (*((volatile uint32_t *)0x40000014))
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))
#define MODE (*((volatile uint32_t *) 0x500FF414))

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code

__attribute__((always_inline)) inline uint32_t csr_mcause_read(void){
    uint32_t mcause_result;
    asm volatile ("csrr %0, mcause" : "=r"(mcause_result));
    return mcause_result;
}

__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void){
    uint32_t result;
    asm volatile ("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val){
    asm volatile ("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val){
    asm volatile ("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
    asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
    asm volatile ("csrci mstatus, 0x8");
}

volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF114);
volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF214);
volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

volatile uint32_t interrupt_pending_reg;
volatile uint32_t TimerTicks;

void init(void){
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    INTERRUPT_ENABLE = INTERRUPT_ENABLE | 0x4; // Enabling the Command Interrupt Bit

    while(Base < End){
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while(Base < End){
        *Base++ = 0;
    }

    csr_write_mie(0x888);       // Enable all interrupt soruces
    csr_enable_interrupts();    // Global interrupt enable
    MTIMECMP_LOW = 100;
    MTIMECMP_HIGH = 0;
}

void display_text_on_console(char* message_to_display, uint32_t start_index)  {
    uint32_t offset = 0;
    if(start_index < 0 || start_index > 2304) return;

    while(message_to_display[offset] != '\0')   {
        VIDEO_MEMORY[start_index + offset] = message_to_display[offset];
        offset++;
    }
    return;
}

void switch_mode(uint32_t mode)   {
    if(mode == 0) MODE = TEXTMODE;
    else MODE = GRAPHICSMODE;
    return;
}

void setBackgroundControl(int control_index, uint16_t x, uint16_t y, uint8_t z, uint8_t palette_index)  {
      if(control_index == 0) BACKGROUND_CONTROL_0 = ((z << 22) | (y << 12) | (x << 2) | palette_index);
      else if(control_index == 1) BACKGROUND_CONTROL_1 = ((z << 22) | (y << 12) | (x << 2) | palette_index);
      else if(control_index == 2) BACKGROUND_CONTROL_2 = ((z << 22) | (y << 12) | (x << 2) | palette_index);
      else if(control_index == 3) BACKGROUND_CONTROL_3 = ((z << 22) | (y << 12) | (x << 2) | palette_index);
      else if(control_index == 4) BACKGROUND_CONTROL_4 = ((z << 22) | (y << 12) | (x << 2) | palette_index);
}

void setBackgroundPalette(uint8_t palette_index, int R, int G, int B, int ALPHA)  {
      if(palette_index == 0) BACKGROUND_PALETTE_0 = (ALPHA << 24 | R << 16 | G << 8 | B);
      else if(palette_index == 1) BACKGROUND_PALETTE_1 = (ALPHA << 24 | R << 16 | G << 8 | B);
      else if(palette_index == 2) BACKGROUND_PALETTE_2 = (ALPHA << 24 | R << 16 | G << 8 | B);
      else if(palette_index == 3) BACKGROUND_PALETTE_3 = (ALPHA << 24 | R << 16 | G << 8 | B);
}

void setLargeSpriteControl(int control_index, uint16_t x, uint16_t y, uint8_t w, uint8_t h, uint8_t palette_index)  {
      LARGE_SPRITE_CONTROL[control_index] = ((h << 26) | (w << 21) | (y << 12) | (x << 2) | palette_index);
}

void setSmallSpriteControl(int control_index, uint16_t x, uint16_t y, uint8_t w, uint8_t h, uint8_t z, uint8_t palette_index)  {
      SMALL_SPRITE_CONTROL[control_index] = ((z << 29) | (h << 25) | (w << 21) | (y << 12) | (x << 2) | palette_index);
}

void setSpritePalette(uint8_t palette_index, int R, int G, int B, int ALPHA)  {
      if(palette_index == 0) SPRITE_PALETTE_0 = (ALPHA << 24 | R << 16 | G << 8 | B);
      else if(palette_index == 1) SPRITE_PALETTE_1 = (ALPHA << 24 | R << 16 | G << 8 | B);
      else if(palette_index == 2) SPRITE_PALETTE_2 = (ALPHA << 24 | R << 16 | G << 8 | B);
      else if(palette_index == 3) SPRITE_PALETTE_3 = (ALPHA << 24 | R << 16 | G << 8 | B);
}

void setBackground(int control_index, uint16_t x, uint16_t y, uint8_t z, uint8_t palette_index, int R, int G, int B, int ALPHA)   {
    setBackgroundControl(control_index,x,y,z,palette_index);
    setBackgroundPalette(palette_index,R,G,B,ALPHA);
}

void setLargeSprite(int control_index, uint16_t x, uint16_t y, uint8_t w, uint8_t h, uint8_t palette_index, int R, int G, int B, int ALPHA)   {
    setLargeSpriteControl(control_index,x,y,w,h,palette_index);
    setSpritePalette(palette_index,R,G,B,ALPHA);
}

void setSmallSprite(int control_index, uint16_t x, uint16_t y, uint8_t w, uint8_t h, uint8_t z, uint8_t palette_index, int R, int G, int B, int ALPHA)   {
    setSmallSpriteControl(control_index,x,y,w,h,z,palette_index);
    setSpritePalette(palette_index,R,G,B,ALPHA);
}

void c_interrupt_handler(void){
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare>>32;
    MTIMECMP_LOW = NewCompare;
    TimerTicks = TimerTicks+1;
    interrupt_pending_reg = INTERRUPT_PENDING;
    INTERRUPT_PENDING = INTERRUPT_PENDING & 0x4; // Resolves the command interrupt raised
}

uint32_t c_syscall(uint32_t* param, char* params){
    if(param[0] == 1){ // returns the "Current TimerTicks"
        return TimerTicks;
    }
    if(param[0] == 2){ // returns the status of the "Mode Control Register"
        return MODE;
    }
    if(param[0] == 3){ // returns the status of the "Controller Register"
        return CONTROLLER;
    }
    if(param[0] == 4){  // returns the status of the "Interrupt Pending Register"
        return interrupt_pending_reg;
    }
    if(param[0] == 5){ // switches between "Text Mode" and "Graphics Mode"
        switch_mode(param[1]);
        return 0;
    }
    if(param[0] == 6){ // Sets the Background
        setBackground(param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8],param[9]);
        return 0;
    }
    if(param[0] == 7){ // Displays a message/text on the Gaming Console
        display_text_on_console(params,param[1]);
        return 0;
    }
    if(param[0] == 8){ // Sets a Small Sprite
        setSmallSprite(param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8],param[9],param[10],param[11]);
        return 0;
    }
    if(param[0] == 9){ // Sets of Large Sprite
        setLargeSprite(param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8],param[9],param[10]);
    }
    return 0;
}
