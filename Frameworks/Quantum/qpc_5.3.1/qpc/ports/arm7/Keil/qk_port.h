/* Product: Product: QP/Cs
* Last Updated for Version: 5.1.0
* Date of the Last Update:  Nov 18, 2013
* Ported to dresden elektronic deRFarm7 Gateway - Atmel AT91SAM7X-512 MCPU
* Keil  ARM - µLink Environment 
* University of Appplied Sciences Munich
*/
#ifndef qk_port_h
#define qk_port_h

                                             /* QK interrupt entry and exit */
#define QK_ISR_ENTRY() do { \
    __disable_irq(); \
    ++QK_intNest_; \
    QF_QS_ISR_ENTRY(QK_intNest_, QK_currPrio_); \
    __enable_irq(); \
} while (0)

#define QK_ISR_EXIT()  do { \
    __disable_irq(); \
    QF_QS_ISR_EXIT(QK_intNest_, QK_currPrio_); \
    --QK_intNest_; \
       __enable_irq(); \
} while (0)

#include "qk.h"                 /* QK platform-independent public interface */

#endif                                                         /* qk_port_h */
