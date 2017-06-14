#ifndef bsp_h
#define bsp_h

#include <stdint.h>

int  _kbhit(void);
int   kbhit(void);
int  _getch(void);
int   getch(void);

void BSP_Init();
void BSP_onKeyboardInput(uint8_t key); /* process the keyboard scan code */

#define BSP_TICKS_PER_SEC    100U

void BSP_showMsg(char const *str);
void BSP_showTime12H(char const *str, uint32_t time, uint32_t base);
void BSP_showTime24H(char const *str, uint32_t time, uint32_t base);

#endif /* bsp_h */
