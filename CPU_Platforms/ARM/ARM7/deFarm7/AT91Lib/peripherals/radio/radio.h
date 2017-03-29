
#ifndef RADIO_H
#define	RADIO_H

#include <stdbool.h>

// Radio  ISR Registers
// See AT86RF231 Documentation Page 29 for mor information

// Indicates PLL lock.
#define PLL_LOCK	0x01
// Indicates PLL unlock. If the radio transceiver is BUSY_TX / BUSY_TX_ARET state, the PA is turned off immediately.
#define PLL_UNLOCK	0x02
// Indicates the start of a PSDU reception. The TRX_STATE changes to BUSY_RX, the PHR is valid to read from Frame Buffer.
#define RX_START	0x04
// RX: Indicates the completion of a frame reception. TX: Indicates the completion of a frame transmission.
#define TRX_END		0x08
// Multi-functional interrupt: 
// 1. AWAKE_END: Indicates radio transceiver reached TRX_OFF state after P_ON, RESET, or SLEEP states. 
// 2. CCA_ED_DONE: Indicates the end of a CCA or ED measurement.
#define ED_READY	0x10
// Indicates address matching.
#define	AMI		0x20
// Indicates a Frame Buffer access violation.
#define TRX_UR		0x40
// Indicates a supply voltage below the programmed threshold.
#define BAT_LOW		0x80


#define RADIO_IRQ_MASK 		0x0E
#define RADIO_IRQ_STATUS	0x0F


enum radioISR_func {
        PLL_LOCK_FUNC = 0x01,
        PLL_UNLOCK_FUNC = 0x02,
        RX_START_FUNC = 0x04,
        TRX_END_FUNC = 0x08,
        ED_READY_FUNC = 0x10,
        AMI_FUNC = 0x20,
        TRX_UR_FUNC = 0x40,
        BAT_LOW_FUNC = 0x80
};

typedef struct transceiver_config {
	unsigned short short_addr;		// 802.15.4 short address
	unsigned long long ieee_addr;	// ieee (mac) address
	unsigned short pan_id;				// 802.15.4 PAN id
	unsigned char frame_retries;	// Number of retransmissions
	unsigned char csma_retries;		// Number of CSMA retries
	char transmit_power;					// Transmit power
	unsigned char channel;				// Channel to use
	bool promiscous_mode;					// Promiscous Mode enable (true) disable (false)
	bool ack;											// Acknowledge enable (true) disable (false)
	bool auto_crc;								// Auto CRC  enable (true) disable (false)
	bool pan_coordinator;					// PAN coordinator mode enable (true) disable (false)	
	
}transceiver_config_t;

void radioISRInit(void);
void radioISRSetCallback(enum radioISR_func, void (*handler)(char));
void radioISRRemoveCallback(enum radioISR_func);
void radioDefaultCallback(char irq_status);
void radioTRXEndCallback(char irq_status);
char getRand(void);

#endif	/* RADIO_H */

