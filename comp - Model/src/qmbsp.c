#include "qp_port.h"
#include "qmbsp.h"
#include "clock.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../bsp/led.h"
#include <button.h>
#include <LPC23xx.H>   
#include <rtc.h>
#include <adc.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
void BSP_Init( ) {
 Init_Timer1( );
	//Timer0_Init();
                               /* Init UART                   */
	LED_Init();
  uart_init_0 ();
	ADC_Init();
	RtcInit();
	
	button_init();
  lcd_init();
  lcd_clear();
}
/*..........................................................................*/
void BSP_showMsg(char const *str) {
	lcd_clear();
  set_cursor (0, 0);
	lcd_print (str);
}
/*..........................................................................*/
void BSP_showTime24H(RTC_T rtc) {
	
	char buffer [10];
  lcd_clear();
  set_cursor (0, 0);
	//sprintf(buffer,"%02d:%02d:%02d\n", time.hour, time.minute, time.second)
  snprintf(buffer, sizeof buffer,"%02d:%02d:%02d", rtc.hours, rtc.minutes, rtc.seconds);
	lcd_print(buffer);
}
/*..........................................................................*/
void QF_onStartup(void) {
   // QF_setTickRate(BSP_TICKS_PER_SEC); /* set the desired tick rate */
}

void QF_onIdle(void)	  {
	 ;
}

/*..........................................................................*/
void QF_onCleanup(void) {
    printf("\nBye! Bye!\n");
}
/*..........................................................................*/
void QF_onClockTick(void) {
    QF_TICK(&l_clock_tick); /* perform the QF clock tick processing */
}
/*..........................................................................*/
void Q_onAssert(char const * const file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    QF_stop();
}


int _kbhit(void ) {

	int i;
	 i = 0xFF;
	 return(i);
}


int _getch()

{
	 FILE *f;
 return	 ( fgetc(  f) );
}
	
