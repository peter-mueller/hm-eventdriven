#include <aic/aic.h>
#include <spi/spi.h>
#include <rftrxspi/rftrxspi.h>
#include "radio.h"
#include <board.h>
#include <pio/pio_it.h>


static const Pin deRF_irq_pin = deRF_IRQ;

static int getISRArrayPosition(enum radioISR_func isr_name);
static void radioIsrHandler(const Pin *pin);

extern char trx_status;



// Array that stores the interrupt callback functions
void (*radioCallBack[8])(char);

//------------------------------------------------------------------------------
/// Initialize the radio interrupt
//------------------------------------------------------------------------------
void radioISRInit(void) {
	int i;

//	buffer[0] = 2;
//	buffer[1] = 0xBB;
    
    for(i = 0; i < 8; i++) {
        radioCallBack[i] = radioDefaultCallback;
    }
		
		// Interruptfunktionalität des IRQ-Pin aktivieren
		PIO_InitializeInterrupts(AT91C_AIC_PRIOR_HIGHEST);		// höchste Prio.
		PIO_ConfigureIt(&deRF_irq_pin, &radioIsrHandler);
		AIC_EnableIT(AT91C_ID_PIOA);
		PIO_EnableIt(&deRF_irq_pin);		
}


//------------------------------------------------------------------------------
/// Add callback to the interrupt routine
/// \param isr_name	Name of the interrupt routine that should be added
/// \param handler	pointer to the callback function
//------------------------------------------------------------------------------
void radioISRSetCallback(enum radioISR_func isr_name, void (*handler)(char)){
		int pos = getISRArrayPosition(isr_name);
    radioCallBack[pos] = handler;
}

//------------------------------------------------------------------------------
/// Remove a callback from the interrupt routine
/// \param isr_name	name of the interrupt routine that should be removed
//------------------------------------------------------------------------------
void radioISRRemoveCallback(enum radioISR_func isr_name) {
		 int pos = getISRArrayPosition(isr_name);
     radioCallBack[pos] = radioDefaultCallback;
}

//------------------------------------------------------------------------------
/// Default callback
/// \param status	Interrupt status
//------------------------------------------------------------------------------
void radioDefaultCallback(char irq_status) {
    return;    
}


//------------------------------------------------------------------------------
void radioTRXEndCallback(char status) {
	
	char radioState = deRF_getTRXstate();
	
	if((radioState == RX_ON) || (radioState == BUSY_RX)) {
		//radioReadPacket();
		
	} else if((radioState == PLL_ON) || (radioState == BUSY_TX)) {
		//deRF_StateTransition(RX_ON);
		
	}
}

/*
 *      Radio Interrupt Service Routine
 */
static void radioIsrHandler(const Pin *pin)
{
    static unsigned char irq_status_register;

		
    irq_status_register = deRF_RR(RADIO_IRQ_STATUS);
	
	
    if (irq_status_register == TRX_END) {
        (*radioCallBack[3])(irq_status_register);
    }

    // Energy detect event
    if (irq_status_register == ED_READY) {
        (*radioCallBack[4])(irq_status_register);
    }
    if (irq_status_register == RX_START) {
        (*radioCallBack[2])(irq_status_register);
    }
    else if (irq_status_register == AMI) {
        (*radioCallBack[5])(irq_status_register);
    }
    else if (irq_status_register == TRX_UR) {
        (*radioCallBack[6])(irq_status_register);
    }
    else if (irq_status_register == PLL_UNLOCK) {
        (*radioCallBack[1])(irq_status_register);
    }
    else if (irq_status_register == PLL_LOCK) {
        (*radioCallBack[0])(irq_status_register);
    }
    else if (irq_status_register == BAT_LOW) {
        (*radioCallBack[7])(irq_status_register);
    }
    else {
    }
}

//------------------------------------------------------------------------------
/// Calculates the array position from the radioISR_func number
/// \param isr_name	name of the interrupt routine
//------------------------------------------------------------------------------
static int getISRArrayPosition(enum radioISR_func isr_name) {
		unsigned char tmp = isr_name;
		int pos = 0;
	
	if(isr_name != 0) {
		
		while( tmp >>= 1) pos++;
		
		return pos;
		
	}
	
	return 0;
	
}

//------------------------------------------------------------------------------
/// Gets a random number from the radio chip
//------------------------------------------------------------------------------
char getRand(void) {
	char rand = 0, tmp;
	unsigned char currentState, i;
	
	currentState = deRF_getTRXstate();
	
	// Transceiver has to be in rx-mode to receive a random number
	// It works only in basic operation mode
	if(currentState == RX_ON) {
		for(i = 0; i < 8; i +=2) {
			// Read PHY_RSSI Register and set all bits exept bit 5 and 6 to zero
			// bit 5 and 6 are from the RND_VALUE register
			tmp = deRF_RR(AT86_PHY_RSSI) & (~0x9F);
			rand = (rand << 2) | (tmp >> 5);
			delay_us(1);
		}
		return rand;	
	}
	return 0;
}
