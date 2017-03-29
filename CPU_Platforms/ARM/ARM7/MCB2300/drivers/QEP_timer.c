




/* Timer 1 for the QF Port to the LPC2300 

		MR0I 1 Interrupt on MR0: an interrupt is generated when MR0 matches  the value in the TC.
		
		MR0R 1 Reset on MR0: the TC will be reset if MR0 matches it. 

*/

#include "qp_port.h" 
#include "lpc23xx.h"

int ad_start;

 __irq void T1_IRQHandler(void ) {



#ifdef QM_TICK
#ifdef QK
 	__disable_irq(); 
  	  ++QK_intNest_; 
#endif  

QF_INT_UNLOCK();
										/* ACK Timer1 int */
if ( ad_start) 
	 AD0CR |= 0x01000000; 
	QF_tick();

	QF_INT_LOCK(); 
#ifdef QK 
	   --QK_intNest_;
	   if (   QK_intNest_ ==0) 
	   		QK_schedule_();
#endif
		 
#endif 
 	
	
										/* ACK Timer1 int */


	

	  T1IR        = 1;                      /* Clear interrupt flag               */

    VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}

  	   

void Init_Timer1(void )	  {


 /* Enable and setup timer interrupt, start timer  
                           */
  T1MR0         = 11999;                       /* 1msec = 12000-1 at 12.0 MHz */
  T1MCR         = 3;                           /* Interrupt and Reset on MR0  */
  T1TCR         = 1;                           /* Timer0 Enable               */
  VICVectAddr5  = (unsigned long)T1_IRQHandler;/* Set Interrupt Vector        */
  VICVectCntl5  = 15;                           /* use it for Timer1 Priority  */
  VICIntEnable  = (1  << 5);                   /* Enable Timer0 Interrupt     */

  ad_start=0;

}



/* Timer0 IRQ: Executed periodically                                          */
__irq void T0_IRQHandler (void) {
 

  T0IR        = 1;                      /* Clear interrupt flag               */
  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}


void Init_timer0(void) {
  /* Enable and setup timer interrupt, start timer                            */
  T0MR0         = 11999;                       /* 1msec = 12000-1 at 12.0 MHz */
  T0MCR         = 3;                           /* Interrupt and Reset on MR0  */
  T0TCR         = 1;                           /* Timer0 Enable               */
  VICVectAddr4  = (unsigned long)T0_IRQHandler;/* Set Interrupt Vector        */
  VICVectCntl4  = 15;                          /* use it for Timer0 Interrupt */
  VICIntEnable  = (1  << 4);                   /* Enable Timer0 Interrupt     */

	
}
start_ad() {

ad_start=1;

}
