/*****************************************************************************
* Model: comp.qm
* File:  ./src/main.c
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/* @(/3/4) .................................................................*/
#include "qep_port.h"
#include "qmbsp.h"
#include "clock.h"
#include "alarm.h"

#include <stdio.h>


/* Local-scope objects -----------------------------------------------------*/
static QEvent const *l_alarmClockQSto[10];  /* queue storage for AlarmClock */
static TimeEvt l_smlPoolSto[10];       /* storage for small-size event pool */


/*..........................................................................*/
int main(int argc, char *argv[]) {

    BSP_Init(); /* initialize the BSP */

    printf("Orthogonal Component pattern\nQP version: %s\n"
           "Press 'o' to turn the Alarm ON\n"
           "Press 'f' to turn the Alarm OFF\n"
           "Press '0'..'9' to set the Alarm time\n"
           "Press 'a' to set the Clock in 12-hour mode\n"
           "Press 'b' to set the Clock in 24-hour mode\n"
           "Press ESC to quit...\n",
           QEP_getVersion());


    QF_init(); /* initialize the framework and the underlying RT kernel */

    /* publish-subscribe not used, no call to QF_psInit() */

    /* initialize event pools... */
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

    /* instantiate and start the active objects... */
    AlarmClock_ctor();
    QACTIVE_START(APP_alarmClock, 1U,
                  l_alarmClockQSto, Q_DIM(l_alarmClockQSto),
                  (void *)0, 0U, (QEvt *)0);

    return QF_run(); /* run the QF application */
}
