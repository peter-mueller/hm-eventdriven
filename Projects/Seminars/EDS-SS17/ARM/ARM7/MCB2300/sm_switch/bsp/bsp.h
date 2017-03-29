
#ifndef bsp_h
#define bsp_h

#include <stdint.h>
#define BSP_TICKS_PER_SEC 12000000/10

void BSP_Init(void);

void init_serial(void);
void uart_init_0(void);

int kbhit(void);
int getch(void);
int _getch(void);

void BSP_Display(uint8_t timeout);

#endif
