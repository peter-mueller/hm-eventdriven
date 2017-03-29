/*****************************************************************************
* Product:  QF/C, Cortex-M3, Vanilla port, IAR compiler
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 07, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qf_port_h
#define qf_port_h

                 /* The maximum number of active objects in the application */
#define QF_MAX_ACTIVE               10
                                          /* QF critical section entry/exit */
/* QF_INT_KEY_TYPE not defined: "unconditional interrupt unlocking" policy  */

//#define QF_INT_KEY_TYPE

//extern int GET_CPSR(void);
//extern void SET_CPSR(int);

#ifdef QK

#ifdef ARM7
void __swi(0xFE)  disable_irq (void);
void __swi(0xFF)  enable_irq (void);



#endif
#ifdef CORTEX
#define SVC_00 0x01
#define SVC_01 0x02

//void __svc(SVC_00)  disable_irq (int i);
//void __svc(SVC_01)  enable_irq (int i);
void __SVC_0  (int i);
void __SVC_1  (int i);
#endif

#define QF_INT_LOCK(dummy)        __SVC_0(0);    	
#define QF_INT_UNLOCK(dummy)      __SVC_1(1);  

#else
#define QF_INT_LOCK(dummy)         	__disable_irq();
#define QF_INT_UNLOCK(dummy)       __enable_irq();

#endif
									
#include "qep_port.h"                                           /* QEP port */
#ifdef QK
#include "qk.h"
#else
#include "qvanilla.h"                       /* "Vanilla" cooperative kernel */
#endif
#include "qf.h"                 /* QF platform-independent public interface */

#endif                                                         /* qf_port_h */
