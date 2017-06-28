#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include <clock.h>
#include "adc.h"

static short AD_last;                          /* Last converted value               */
QActive*  ad_ao = 0;

/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq void ADC_IRQHandler(void) {

  AD_last = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result             */
	
	adc_change();

  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}


void adc_change()
{
	static ADEvt event;
	event.super.sig = AD_CHANGED_SIG;
	event.value = AD_last;
	if(ad_ao != 0) {
		QF_INT_UNLOCK();
		QActive_postFIFO(ad_ao, (QEvent *)&event);
		QF_INT_LOCK();
	}
}
void ad_setAO(QActive* ao_)
{
	ad_ao = ao_;
}




void ADC_Init(void) {
	
	/* Power enable, Setup pin, enable and setup AD converter interrupt         */
  PCONP        |= (1 << 12);                   /* Enable power to AD block    */
  PINSEL1       = 0x4000;                      /* AD0.0 pin function select   */
  AD0INTEN      = (1 <<  0);                   /* CH0 enable interrupt        */
  AD0CR         = 0x00200301;                  /* Power up, PCLK/4, sel AD0.0 */
  VICVectAddr18 = (unsigned long)ADC_IRQHandler;/* Set Interrupt Vector       */
  VICVectCntl18 = 14;                          /* use it for ADC Interrupt    */
  VICIntEnable  = (1  << 18);                  /* Enable ADC Interrupt        */
	
}

 short AD_Get_Val(void ) {
	
	return ( AD_last);
}

