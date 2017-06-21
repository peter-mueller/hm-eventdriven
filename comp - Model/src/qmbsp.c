#include "qp_port.h"
#include "qmbsp.h"
#include "clock.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Q_DEFINE_THIS_FILE

int timeEquals(Time a, Time b) {
  return a.hour == b.hour && a.minute == b.minute && a.second == b.second;
}

Time timeAdd(Time time, uint8_t seconds) {
    time.second += seconds;
    time.minute += time.second / 60;
    time.second = time.second % 60;

    time.hour += time.minute / 60;
    time.hour = time.minute % 60;

    time.hour = time.hour % 24;
    return time;
}

/*..........................................................................*/
void BSP_Init( ) {
 Init_Timer1( );
//	Init_timer0();
                               /* Init UART                   */
  uart_init_0 ( );
  lcd_init();
  lcd_clear();
  set_cursor (0, 1);
}
/*..........................................................................*/
void BSP_showMsg(char const *str) {
    printf(str);
    printf("\n");
    fflush(stdout);
}
/*..........................................................................*/
void BSP_showTime24H(char const *str, Time  time) {
    printf(str);
	printf("%02d:%02d:%02d\n", time.hour, time.minute, time.second);
    fflush(stdout);
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
	
