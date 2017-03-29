/* Product: Product: QP/Cs
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Nov 17, 2013
* Ported to dresden elektronic deRFarm7 Gateway - Atmel AT91SAM7X-512 MCPU
* Keil  ARM - µLink Environment */

#ifndef qf_port_h
#define qf_port_h

                 /* The maximum number of active objects in the application */
#define QF_MAX_ACTIVE               8

#define QF_MAX_TICK_RATE            2




void __swi(0xFE)  disable_irq (void);
void __swi(0xFF)  enable_irq (void);

#ifdef QK
#define QF_INT_LOCK(dummy)        disable_irq();    	
#define QF_INT_UNLOCK(dummy)      enable_irq();  


#else
#define QF_INT_LOCK(dummy)         	__disable_irq();
#define QF_INT_UNLOCK(dummy)        __enable_irq();

#endif                                            /* QF interrupt disable/enable */
#define QF_INT_DISABLE()            (__disable_irq())
#define QF_INT_ENABLE()             (__enable_irq())



	


                                          /* QF critical section entry/exit */
/* QF_CRIT_STAT_TYPE not defined: unconditional interrupt unlocking" policy */
#define QF_CRIT_ENTRY(dummy)       disable_irq();
#define QF_CRIT_EXIT(dummy)        enable_irq(); 




#include "qep_port.h"                                           /* QEP port */
#ifdef QK
#include "qk.h"
#else
#include "qvanilla.h"                       /* "Vanilla" cooperative kernel */
#endif

#include "qf.h"                 /* QF platform-independent public interface */

#endif                                                         /* qf_port_h */
