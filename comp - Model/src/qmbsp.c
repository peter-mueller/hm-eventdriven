#include "qp_port.h"
#include "qmbsp.h"
#include "clock.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Q_DEFINE_THIS_FILE

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
void BSP_showTime24H(char const *str, uint32_t time, uint32_t base) {
    printf(str);
    printf("%02d:%02d\n", (int)(time / base), (int)(time % base));
    fflush(stdout);
}
/*..........................................................................*/
void BSP_showTime12H(char const *str, uint32_t time, uint32_t base) {
    uint32_t h = time / base;

    printf(str);
    printf("%02d:%02d %s\n", (h % 12) ? (h % 12) : 12,
           time % base, (h / 12) ? "PM" : "AM");
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
    if (_kbhit()) { /* any key pressed? */
        BSP_onKeyboardInput(_getch());
    }
}
/*..........................................................................*/
void BSP_onKeyboardInput(uint8_t key) {
    switch (key) {
        case 'o': { /* 'o': Alarm on event? */
            QACTIVE_POST(APP_alarmClock,
                         Q_NEW(QEvt, ALARM_ON_SIG), (void *)0);
            break;
        }
        case 'f': { /* 'f': Alarm off event? */
            QACTIVE_POST(APP_alarmClock,
                         Q_NEW(QEvt, ALARM_OFF_SIG), (void *)0);
            break;
        }
        case '1':   /* '1' */
        case '2':   /* '2' */
        case '3':   /* '3' */
        case '4':   /* '4' */
        case '5':   /* '5' */
        case '6':   /* '6' */
        case '7':   /* '7' */
        case '8':   /* '8' */
        case '9': { /* '9' */
            SetEvt *e = Q_NEW(SetEvt, ALARM_SET_SIG);
            e->digit = (uint8_t)(key - '0');
            QACTIVE_POST(APP_alarmClock, (QEvt *)e, (void *)0);
            break;
        }
        case '0': { /* '0' */
            SetEvt *e = Q_NEW(SetEvt, ALARM_SET_SIG);
            e->digit = 0;
            QACTIVE_POST(APP_alarmClock, (QEvt *)e, (void *)0);
            break;
        }
        case 'a': { /* 'a': Clock 12H event? */
            QACTIVE_POST(APP_alarmClock,
                         Q_NEW(QEvt, CLOCK_12H_SIG), (void *)0);
            break;
        }
        case 'b': { /* 'b': Clock 24H event? */
            QACTIVE_POST(APP_alarmClock,
                         Q_NEW(QEvt, CLOCK_24H_SIG), (void *)0);
            break;
        }
        case '\33': { /* ESC pressed? */
            QACTIVE_POST(APP_alarmClock,
                         Q_NEW(QEvt, TERMINATE_SIG), (void *)0);
            break;
        }
    }
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
	
