#ifndef bsp_h
#define bsp_h

#include <stdint.h>
#include <rtc.h>

int  _kbhit(void);
int   kbhit(void);
int  _getch(void);
int   getch(void);

void BSP_Init(void);
void BSP_onKeyboardInput(uint8_t key); /* process the keyboard scan code */

#define BSP_TICKS_PER_SEC    100U

void BSP_showMsg(char const *str);
void BSP_showTime24H(RTC_T time);


#endif /* bsp_h */
