
#include "button.h"
#include <LPC23xx.H>
#include <stdio.h>
#include "qp_port.h"                /* the port of the QEP event processor */
#include "ui.h"
#include "clock.h"

__irq void GPIO_IRQHandler (void);
void button_up(void);
void button_down(void);
QActive*  ao = 0;

void button_init()
{
	PINSEL4 &= ~(3 << 20);                       /* Set P2.10 to GPIO           */
	FIO2DIR &= ~(1 << 10);                       /* Set P2.10 to input          */
	IO2_INT_EN_R |= (1 << 10);                      /* Enable rising egde interrupt for P2.10. */
	IO2_INT_EN_F |= (1 << 10);                      /* Enable falling egde interrupt for P2.10. */
	VICVectAddr17 = (unsigned long)GPIO_IRQHandler;/* Set Interrupt Vector       */
	VICVectCntl17 = 13;                          /* use it for ADC Interrupt    */
	VICIntEnable  = (1  << 17);                  /* Enable ADC Interrupt        */
}

__irq void GPIO_IRQHandler(void)
{
	static int rising;
	static int i;

	
	for(i = 0; i < 2000; i++);

	rising = FIO2PIN & (1 << 10);        /* Value of P2.10 */
	
	if(rising) {
		//button_up();
	}else{
		button_down();
	}
	IO2_INT_CLR = 1 << 10;					  /* Clear Interrupt                    */
	VICVectAddr = 0;                      /* Acknowledge Interrupt              */
	
	
}

void button_up()
{
	static BaseEvt event;
	event.super.sig = BUTTON_DOWN_SIG;
	if(ao != 0) {

		QF_INT_UNLOCK();
		//QActive_postFIFO(ao, (QEvent *)&event);
	 QACTIVE_POST(ao,
									 (QEvent *)&event, (void *)0);
		QF_INT_LOCK();
	}
}

void button_down()
{
	static BaseEvt event;
	event.super.sig = BUTTON_DOWN_SIG;
	if(ao != 0) {
		QF_INT_UNLOCK();
		//QActive_postFIFO(ao, (QEvent *)&event);
				 QACTIVE_POST(ao,
                         (QEvent *)&event, (void *)0);
		QF_INT_LOCK();
	}
}
void button_setAO(QActive* ao_)
{
	ao = ao_;
}
