

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Source file<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//#include <RTL.h>
#include <stdio.h>
#include <LPC23xx.H> /* LPC23xx definitions */
#include  "rtc.h"

volatile unsigned char rtc_update;

/*****************************************************************************************/

void Set_RTC(	 RTC_T* new_rtc) {

 	 new_rtc->seconds = 0x00;
   	 new_rtc->minutes= 0x30;
	 new_rtc->hours= 0xE;
  	 new_rtc->day_of_week= 0x03;
	 new_rtc->day_of_month= 0x07;
	 new_rtc->month= 0x0C;
     new_rtc->year = 2010;


}

void Write_RTC(RTC_T* new_rtc){


	

	RTC_CCR &=  0x01; /* disable RTC */
	RTC_SEC = new_rtc->seconds;
	RTC_MIN = new_rtc->minutes;
	RTC_HOUR = new_rtc->hours;
	RTC_DOW = new_rtc->day_of_week;
	RTC_DOM = new_rtc->day_of_month;
	RTC_MONTH = new_rtc->month;
	RTC_YEAR = new_rtc->year;
	RTC_DOY = new_rtc->day_of_year;
	RTC_CCR |= 0x01; /* enable RTC */




}


/*****************************************************************************************/
void Read_RTC(RTC_T* rtc){


rtc->seconds = ((RTC_CTIME0 & 0x0000003F)>> 0);
rtc->minutes = ((RTC_CTIME0 & 0x00003F00)>> 8);
rtc->hours = ((RTC_CTIME0 & 0x001F0000)>> 16);
rtc->day_of_week = ((RTC_CTIME0 & 0x07000000)>> 24);
rtc->day_of_month = ((RTC_CTIME1 & 0x0000001F)>> 0);
rtc->month = ((RTC_CTIME1 & 0x0000FF00)>> 8);
rtc->year = ((RTC_CTIME1 & 0x0FFF0000)>> 16);
rtc->day_of_year = ((RTC_CTIME2 & 0x00000FFF)>> 0);

RTC_CCR |= 0x01;

return;

}
/*****************************************************************************************/
void isr_rtc_event(void) __irq {

/* one secound RTC interrupt heandler*/
if (RTC_ILR &=0x00000001){
RTC_ILR = 0x00000001;
rtc_update = 1;
}
VICVectAddr = 0; /* Acknowledge Interrupt */
}
/*****************************************************************************************/
void RtcInit(){

RTC_PREINT = 0x016d;
RTC_PREFRAC = 0x1680;
PCONP |= 0x200; /* Enable power for RTC */
RTC_CCR = 0x012; /* CTC reset and select RTC clock from 32 Khz.oscillator 0x 0x12 */
RTC_ILR = 0xFF; /* Clear RTC interrupt register */
RTC_AMR = 0xFF; /* Dasable all the alrams. */
RTC_CISS = 0; /* Disable all subsecond interrupts. */
RTC_CCR = 0x01; /* Enable RTC */
RTC_CIIR = 0x01; /* Enable only second count interrupt. minute */

VICVectAddr13 = (unsigned int)isr_rtc_event; /* Set Interrupt Vector */
VICVectCntl13 = 13; /* use it for RTC Interrupt */
VICIntEnable = (1 << 13); /* Enable Interrupt */
}


/*****************************************************************************************/

