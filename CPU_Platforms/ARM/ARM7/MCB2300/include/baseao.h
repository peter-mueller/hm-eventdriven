
#ifndef baseao_h
#define baseao_h

#include "qp_port.h"                /* the port of the QEP event processor */

#ifdef DEBUG
	#define debug(s) 		printf(s)
	#define debugf(s, p)	printf(s,p)
#else
	#define debug(s)
	#define debugf(s, p)
#endif

typedef struct BaseTimEvtTag {
    QTimeEvt  super;
	uint8_t   clk_ctr;
} BaseTimEvt;

typedef struct BaseEvtTag {
    QEvent  super;
} BaseEvt;

															   
#endif
