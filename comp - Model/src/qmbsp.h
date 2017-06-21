#ifndef bsp_h
#define bsp_h

#include <stdint.h>

typedef struct TimeTag {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time;

int timeEquals(Time a, Time b);
Time timeAdd(Time time, uint8_t seconds);


int  _kbhit(void);
int   kbhit(void);
int  _getch(void);
int   getch(void);

void BSP_Init();
void BSP_onKeyboardInput(uint8_t key); /* process the keyboard scan code */

#define BSP_TICKS_PER_SEC    100U

void BSP_showMsg(char const *str);
void BSP_showTime24H(char const *str, Time time);


#endif /* bsp_h */
