/*****************************************************************************
* Model: comp.qm
* File:  ./src/clock.h
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
/* @(/3/2) .................................................................*/
#ifndef clock_h
#define clock_h

#include "qp_port.h"
#include "alarm.h"


enum AlarmClockSignals {
    TICK_SIG = Q_USER_SIG, /* time tick event */
    BUTTON_SIG,  /* advalue */
    ADVal_SIG,
    ALARM_ON_SIG,   /* turn the alarm on */
    ALARM_OFF_SIG,  /* turn the alarm off */
    ALARM_SIG,  /* alarm event from Alarm component to AlarmClock container */
    TIME_SIG,       /* time event sent to Alarm (contains current time) */
    TERMINATE_SIG   /* terminate the application */
};

/* @(/1/0) .................................................................*/
typedef struct TimeEvtTag {
/* protected: */
    QEvt super;

/* public: */
    Time current;
} TimeEvt;


extern QActive * const APP_alarmClock;

/* @(/2/3) .................................................................*/
void AlarmClock_ctor(void);
;

/* @(/2/1) .................................................................*/
typedef struct AlarmClockTag {
/* protected: */
    QActive super;

/* private: */
    Alarm alarm;

/* public: */
    QTimeEvt timeEvt;

/* private: */
    Time current;

/* public: */
    uint32_t adval;
    uint8_t strength;
} AlarmClock;

/* protected: */
QState AlarmClock_initial(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_timekeeping(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_setBrewStrength(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_showCurrentTime(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_set_hour(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_set_minute(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_enableAlarm(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_final(AlarmClock * const me, QEvt const * const e);
QState AlarmClock_brewing(AlarmClock * const me, QEvt const * const e);


#endif /* clock_h */
