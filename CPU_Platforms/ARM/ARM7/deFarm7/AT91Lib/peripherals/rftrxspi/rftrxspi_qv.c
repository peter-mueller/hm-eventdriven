/*  AT86RF231 Transciever Interface
* Date of the Last Update:  Dec 15, 2013
* Ported to dresden elektronic deRFarm7 Gateway - Atmel AT91SAM7X-512 MCPU
* Hochschule Mnchen - Seminar Advanced Embedded Systems - Ron Barker
* Support for QM QV Scheduler
* Keil ARM - Link Environment 
*/

/* TODO:
 */

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include <board.h>
#include <bsp_at91.h>
#include <pio/pio.h>
#include <pmc/pmc.h>
#include <spi/spi.h>
#include <aic/aic.h>
#include <rftrxspi/rftrxspi.h>
#include <radio/radio.h>

//------------------------------------------------------------------------------
//         Defines
//------------------------------------------------------------------------------

/// PINs
// List of all pin definitions
#define deRF_PINS SPI1_MOSI,SPI1_MISO,SPI1_SPCK,SPI1_NPCS0,deRF_IRQ,deRF_RST,de_RF_SLP_TR
// Define pin positions of RST and SLP_TR inside list 'deRF_PINS'
#define deRF_PIN_RST		5
#define deRF_PIN_SLP_TR	6
#define deRF_PIN_SPI_SEL 3


#define REG_ADDRESS_MASK	(0x3F)		// Register Address Mask


/// Several Timings (from the swedish guys)	
#define TIME_RESET						6			// Time to hold the RST pin low during reset
#define TIME_TO_ENTER_P_ON		510		// Transition time from VCC is applied to P_ON.
#define TIME_P_ON_TO_TRX_OFF  510		// Transition time from P_ON to TRX_OFF.
#define TIME_CMD_FORCE_TRX_OFF 1
#define TIME_NOCLK_TO_WAKE    6  
#define TIME_STATE_TRANSITION_PLL_ACTIVE  1
#define TIME_TRX_OFF_TO_PLL_ACTIVE  180
#define TIME_PLL_LOCK				150	

// irq reg bits  MASK_BAT_LOW MASK_TRX_UR MASK_AMI MASK_CCA_ED_DONE
#define IRQ_MASK_PLL_LOCK  			0           ///< IRQ reg bit - PLL Lock
#define IRQ_MASK_PLL_UNLOCK 		1           ///< IRQ reg bit - PLL Unlock
#define IRQ_MASK_RX_START  			2       ///< IRQ reg bit - RX Start
#define IRQ_MASK_TRX_END  		  3       ///< IRQ reg bit - TRX End
#define IRQ_MASK_CCA_ED_DONE    4       ///< IRQ reg bit - TRX End
#define IRQ_MASK_AMI  				  5       ///< IRQ reg bit - TRX End
#define IRQ_MASK_TRX_UR         6       ///< IRQ reg bit - TRX End
#define IRQ_MASK_BAT_LOW        7       ///< IRQ reg bit - TRX End

typedef enum{
	deRF_SUCCESS = 0,
	deRF_TIMEOUT = 1,
	deRF_ERROR = 2
} deRF_status_t;

/// Calculates the value of the DLYBCS field given the desired delay (in ns)
#define SPI_DLYBCS(delay, masterClock) \
            ((unsigned int) (((masterClock / 1000000) * delay) / 1000) << 24)

//------------------------------------------------------------------------------
//         Local Variables
//------------------------------------------------------------------------------

#ifdef deRF_PINS
static const Pin pins_deRF[] = {deRF_PINS};
#endif



// SPI Macros
#define SPI_READ(value)    do {                        \
    while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_RDRF) == 0);    \
    value = (AT91C_BASE_SPI1->SPI_RDR & 0xFFFF);       \
} while (0);

#define SPI_WRITE(data)     do {                                      \
             while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TDRE) == 0);        \
             AT91C_BASE_SPI1->SPI_TDR = data & 0xFFFF;                       \
             while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TXEMPTY) == 0);     \
} while (0);


static unsigned char txbuf[130];


//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

// Only internal used functions
void deRF_SetSLPTR(void);
void deRF_ClearSLPTR(void);
void deRF_reset(void);


//------------------------------------------------------------------------------
/// SPI ISR
//------------------------------------------------------------------------------ 
static void spiISRhandler(void)
{
	static unsigned char spi_isr_flag;

	spi_isr_flag=AT91C_BASE_SPI1->SPI_SR;

	spiInterruptDisable(SPI_IRQ_TXEMPTY);
	
	spi_isr_flag=AT91C_BASE_SPI1->SPI_SR;
	
}




//------------------------------------------------------------------------------
/// Initialize deRF
//------------------------------------------------------------------------------
void deRF_Init(void)
{	
	static unsigned char status;
	/* Configure GPIOs and SPI1 */
	PMC_EnablePeripheral(AT91C_ID_PIOA); 								// turn on PIOA peripheral
	PIO_Configure(pins_deRF, PIO_LISTSIZE(pins_deRF));	// configure GPIOs
	PMC_EnablePeripheral(AT91C_ID_SPI1); 								// turn on SPI1
	
	/* Init ISR for deRF IRQ-pin (must be called after PIO_Configure() s.o. */
	radioISRInit();
	status = AT91C_BASE_SPI1->SPI_SR;
	/* Configure ISR for SPI IRQ */
	AIC_ConfigureIT(AT91C_ID_SPI1, (AT91C_AIC_PRIOR_HIGHEST | AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE), &spiISRhandler);
	AIC_EnableIT(AT91C_ID_SPI1);
	
    /* Set SEL as GPIO */
    AT91C_BASE_PIOA->PIO_OER = (1 << 21);
    AT91C_BASE_PIOA->PIO_PER = (1 << 21);
	
	/* Configure SPI1: 
	 * MSTR=1, PS=0, PCSDEC=0, FDIV=0, MODFDIS=0, LLB=0, PCS=0000,
	 */
	 SPI_Configure(AT91C_BASE_SPI1,AT91C_ID_SPI1,				// SPI1
		AT91C_SPI_MSTR | 																	// set master mode	
		AT91C_SPI_PS_FIXED| 															// fixed peripheral select
		AT91C_SPI_MODFDIS 																// disable mode fault detect																		
	);																							
	status = AT91C_BASE_SPI1->SPI_SR;
	/* Configure Chip Select Line 0 (SPI Mode 0)
	 * CPOL=0, NCPHA=1, CSAAT=0, BITS=0000 (8 data bits), DLYBS=0
	 	SPI_SCBR(SPI_FCLK, BOARD_MCK) |										// use macro SPI_SCBR() to calculate spi baudrate
	 */

/* Configure Chip Select Line 0 (SPI Mode 0)
	 * CPOL=0, NCPHA=1, CSAAT=0, BITS=0000 (8 data bits), DLYBS=0
	 */
	SPI_ConfigureNPCS(AT91C_BASE_SPI1,									// SPI1
		0,																								// NPCS0
		AT91C_SPI_NCPHA |																	// CPOL = 0 & NCPHA = 1 : SPI Mode 0
		SPI_SCBR(SPI_FCLK, BOARD_MCK) |										// use macro SPI_SCBR() to calculate spi baudrate
		SPI_DLYBS(400, BOARD_MCK) |
		SPI_DLYBCT(700, BOARD_MCK)   											// idle time between consecutive SPI accesses (see p. 157)
	);
		
	
	status = AT91C_BASE_SPI1->SPI_SR;	
	/* Disable all SPI Interrupts*/
	AT91C_BASE_SPI1->SPI_IDR = 0x3FF;
	status = AT91C_BASE_SPI1->SPI_SR;
	/* Enable SPI1 */
	SPI_Enable(AT91C_BASE_SPI1);
	status = AT91C_BASE_SPI1->SPI_SR;
	/* Wait for radio */
	delay_us(TIME_TO_ENTER_P_ON);
	
	/* Reset AT86RF231 for hardware / software synchronization reasons */
	deRF_reset();
	
	/* Disable radio interrupts until we do some configuration */ 
	deRF_WR(AT86_IRQ_MASK, 0);  

	/* force transceiver off while we configure the intps */
  deRF_WR(TRX_STATE, FORCE_TRX_OFF);  
  delay_us(TIME_P_ON_TO_TRX_OFF);
	
	/* wait for transceiver to transition to the off state */
  while (deRF_getTRXstate() != TRX_OFF) ;
			
	/* Enable TX/RX interrupt source only  */ 
	deRF_WR(AT86_IRQ_MASK, (1<<IRQ_MASK_TRX_END) | (1<<IRQ_MASK_RX_START));
	deRF_RR(AT86_IRQ_STATUS);
	//deRF_WR(AT86_IRQ_MASK, ((1<<IRQ_MASK_TRX_END) | (1<<IRQ_MASK_RX_START) | (1<<IRQ_MASK_TRX_UR) ));
	
	/* Activate Dynamic Frame Buffer Protection */
	/* 0 == 802.15.4  250  0x 
	deRF_WR(AT86_TRX_CTRL2, 0x80); */
	
/* NEW: Extended Operating Mode */

	/* Set Short Address, PAN ID & IEEE Address */
	deRF_SetShortAddress(SHORT_ADDR_DEFAULT);
	deRF_SetPANID(PAN_ID_DEFAULT);
	deRF_SetIEEEAddress(IEEE_ADDR_DEFAULT);
	
	deRF_SetPromiscuousMode(1);
	
	/* Set Maximum Frame & CSMA Retries */
	deRF_SetMaxFrameRetries(MAX_FRAME_RETRIES);
	deRF_SetMaxCSMARetries(MAX_CSMA_RETRIES);

	/* Set CSMA SEED */
	//deRF_SetCSMASeed(RandNumber());
	deRF_SetCSMASeed(0xF123);
	
	/* Enable automatic FCS generation */
	deRF_SetAutoCRC(1);
	
	/* Set max. and min. back-off exponent */
	deRF_SetCSMAbackOff(MAX_BE, MIN_BE);
	
	/* Set channel */
	(void)deRF_SetChannel(CHANNEL_DEFAULT);
		
	/* Switch to RX_AACK_ON state */
	deRF_StateTransition(RX_AACK_ON);  
  while (deRF_getTRXstate() != RX_AACK_ON);
	
		status = AT91C_BASE_SPI1->SPI_SR;
}



//------------------------------------------------------------------------------
/// Read content of a AT86RT321RF register
/// \param address  Address of register
//------------------------------------------------------------------------------
unsigned char deRF_RR(unsigned char address)
{
	unsigned short ret;
	disable_irq	();
	deRF_ClearSEL();
	/* check for active spi communication */
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
  txbuf[0] = (SPI_CMD_RR|(REG_ADDRESS_MASK & address));	// spi_cmd_rr + address
	txbuf[1] = 0xFF;																			// dummy data
	
	// Interruptsperre bzgl. QF aktivieren
	SPI_WriteBuffer(AT91C_BASE_SPI1,txbuf,2);
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	ret = SPI_Read(AT91C_BASE_SPI1);
	// Interruptsperre deaktivieren
	deRF_SetSEL();
	enable_irq();
	return (unsigned char)(ret & 0x00ff);
}

//------------------------------------------------------------------------------
/// Writes a new value to one of the AT86RT321RF registers
/// \param reg  Address of register
/// \param value  Value to write 
//------------------------------------------------------------------------------
void deRF_WR(unsigned char address, unsigned char value)
{
	deRF_ClearSEL();
	/* check for active spi communication */
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
  txbuf[0] = (SPI_CMD_RW|(REG_ADDRESS_MASK & address));	// spi_cmd_wr + address
	txbuf[1] = value;									// value to write
	
	// Interruptsperre bzgl. QF aktivieren
	
	SPI_WriteBuffer(AT91C_BASE_SPI1,txbuf,2);
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	// Interruptsperre deaktivieren
	deRF_SetSEL();
}
//------------------------------------------------------------------------------
/// Reads Data from SRAM
/// \param data  Points to PSDU source data
/// \param length number of PSDU data (bytes)
//------------------------------------------------------------------------------
void deRF_READ_SRAM(unsigned char *data, unsigned char length)
{

	deRF_ClearSEL();	
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
 
	
	// Interruptsperre bzgl. QF aktivieren
	SPI_ReadBuffer(AT91C_BASE_SPI1,data, length);
                                  
	
	 while(!SPI_IsFinished(AT91C_BASE_SPI1));
	// Interruptsperre deaktivieren
	deRF_SetSEL();	
}


//------------------------------------------------------------------------------
/// Writes data into AT86RT321RF frame buffer
/// \param data  Points to PSDU source data
/// \param length number of PSDU data (bytes)
//------------------------------------------------------------------------------
void deRF_WF(unsigned char *data, unsigned char length)
{
	unsigned char i;
	deRF_ClearSEL();	
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
  txbuf[0] = SPI_CMD_FW;
	txbuf[1] = length + 2;
	
	for(i=2; i<length+2; i++)
		txbuf[i] = data[i-2];
	
	// Interruptsperre bzgl. QF aktivieren
	SPI_WriteBuffer(AT91C_BASE_SPI1,txbuf,length+2);
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	// Interruptsperre deaktivieren
	deRF_SetSEL();	
}

//------------------------------------------------------------------------------
/// Reads data from AT86RT321RF frame buffer
/// \param data  Points to PSDU destination data
/// \return length of received PSDU data (bytes)
//------------------------------------------------------------------------------
void deRF_RF(void *rx_buffer, void *tx_buffer)
{
		AT91S_SPI *spi = AT91C_BASE_SPI1;
	
	
		deRF_ClearSEL();	
		// SPI transmission in progress?
		while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
    // Check if first bank is free
    if (spi->SPI_TCR == 0 & spi->SPI_RCR == 0) {
        spi->SPI_TPR = (unsigned int) tx_buffer;
        spi->SPI_TCR = 130;
        spi->SPI_PTCR = AT91C_PDC_TXTEN;
				spi->SPI_RPR = (unsigned int) rx_buffer;
        spi->SPI_RCR = 130;
        spi->SPI_PTCR = AT91C_PDC_RXTEN;
    }
    // Check if second bank is free
    else if (spi->SPI_TNCR == 0 & spi->SPI_RNCR == 0) {
        spi->SPI_TNPR = (unsigned int) tx_buffer;
        spi->SPI_TNCR = 130;
				spi->SPI_RNPR = (unsigned int) rx_buffer;
        spi->SPI_RNCR = 130;
    }
		deRF_SetSEL();	
		
	 while(!(AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_RXBUFF));
  while(!(AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TXEMPTY));

  // Disable DMA
 // AT91C_BASE_SPI1->SPI_PTCR = AT91C_PDC_TXTDIS;

    (unsigned char)( spi->SPI_RDR ); // it's important to read RDR here!

    //(void)( spi->SPI_RDR );		
}

//------------------------------------------------------------------------------
/// Reseting AT86RT321RF (/RST is low active)
//------------------------------------------------------------------------------
void deRF_reset(void)
{
	PIO_Clear(&pins_deRF[deRF_PIN_RST]);		// /RST = HIGH
	deRF_ClearSLPTR();
	delay_us(10);
	PIO_Set(&pins_deRF[deRF_PIN_RST]);			// /RST = LOW
	delay_us(1000);
}

//------------------------------------------------------------------------------
/// Set SLP_TR to HIGH
//------------------------------------------------------------------------------
void deRF_SetSLPTR(void)
{
	PIO_Set(&pins_deRF[deRF_PIN_SLP_TR]);		// SLP_TR = HIGH
}

//------------------------------------------------------------------------------
/// Set SLP_TR to LOW
//------------------------------------------------------------------------------
void deRF_ClearSLPTR(void)
{
	PIO_Clear(&pins_deRF[deRF_PIN_SLP_TR]);	// SLP_TR = LOW
}

//------------------------------------------------------------------------------
/// Set SEL to HIGH
//------------------------------------------------------------------------------
void deRF_SetSEL(void)
{
	PIO_Set(&pins_deRF[deRF_PIN_SPI_SEL]);		// SLP_TR = HIGH
}

//------------------------------------------------------------------------------
/// Set SEL to LOW
//------------------------------------------------------------------------------
void deRF_ClearSEL(void)
{
	PIO_Clear(&pins_deRF[deRF_PIN_SPI_SEL]);	// SLP_TR = LOW
}


//------------------------------------------------------------------------------
/// Primitive delay function for MCK=48MHz
/// \param us delay in us to wait
//------------------------------------------------------------------------------
void delay_us(int us)
{
	unsigned char i;
	int j;
	for(j=0; j<us; j++)
		for(i=0; i<7; i++);
}

//------------------------------------------------------------------------------
/// Enables SPI interrupt source type
/// \param irq_type spi interrupt source type
//------------------------------------------------------------------------------
void spiInterruptEnable(at91sam_spi_irq_sources_t irq_type)
{
	static unsigned char spi_isr_flag;
	spi_isr_flag=AT91C_BASE_SPI1->SPI_SR;
	AT91C_BASE_SPI1->SPI_IER = irq_type;
}

//------------------------------------------------------------------------------
/// Disables SPI interrupt source type
/// \param irq_type spi interrupt source type
//------------------------------------------------------------------------------
void spiInterruptDisable(at91sam_spi_irq_sources_t irq_type)
{
	AT91C_BASE_SPI1->SPI_IDR = irq_type;
}


//------------------------------------------------------------------------------
/// Returns current state
/// \return current trx state
//------------------------------------------------------------------------------
unsigned char deRF_getTRXstate(void){
	return (deRF_RR(TRX_STATUS) & 0x1F);
}

//------------------------------------------------------------------------------
/// Performs state transition
/// \param state desired state
/// \return deRF_SUCCESS or deRF_TIMEOUT
//------------------------------------------------------------------------------
unsigned char deRF_StateTransition(trx_states_t state)
{
	unsigned char current_state;
	int delay;
	
	/* Aktuellen State ermitteln */
	current_state = deRF_getTRXstate();
	
	/* Bereits im gewuenschten Zustand? */
	if(current_state == state)
		return deRF_SUCCESS;
	
	/* Falls BUSY -> warten bis neutraler Zustand erreicht ist */
	if ((current_state == BUSY_TX_ARET) || (current_state == BUSY_RX_AACK) || (current_state == BUSY_RX) || (current_state == BUSY_TX))  
    while (deRF_getTRXstate() == current_state);  
  	

	/* Evtl. Vorbedingungen erfllen */
	switch(state)
	{
		case TRX_OFF:
			deRF_ClearSLPTR();  
			delay_us(TIME_NOCLK_TO_WAKE);  
			deRF_WR(TRX_STATE, FORCE_TRX_OFF);  
			delay_us(TIME_CMD_FORCE_TRX_OFF);
			break;
		case TX_ARET_ON:
			if(current_state == RX_AACK_ON){ 
				deRF_WR(TRX_STATE, PLL_ON);
				delay_us(TIME_STATE_TRANSITION_PLL_ACTIVE); 
			}
			break;
		case RX_AACK_ON:
			if(current_state == TX_ARET_ON){
				deRF_WR(TRX_STATE, PLL_ON);
				delay_us(TIME_STATE_TRANSITION_PLL_ACTIVE);				
			}
			break;
		default:
			break;
	}
	
	/* Eigentliche Transition ausfhren */
	deRF_WR(TRX_STATE, state);
	

	/* Zustandswechsel von TRX_OFF aus bentigt lngere Wartezeit, alle anderen Zustnde
	   sind innerhalb einer 1us erreichbar */
	delay = (current_state == TRX_OFF) ? TIME_TRX_OFF_TO_PLL_ACTIVE : TIME_STATE_TRANSITION_PLL_ACTIVE;  
	delay_us(delay);  

	/* Abschliessende Kontrolle */
	if (deRF_getTRXstate() == state)  
	{  
			return deRF_SUCCESS;
	}  
	return deRF_TIMEOUT;
}


//------------------------------------------------------------------------------
/// Sets MAC short address for Frame Filter address recognition
/// \param mac  MAC short address
//------------------------------------------------------------------------------
void deRF_SetShortAddress(unsigned short mac)
{
	deRF_WR(SHORT_ADDR_0, (mac & 0x00FF));		// lower byte
	deRF_WR(SHORT_ADDR_1, (mac & 0xFF00) >> 8);		// higher byte
}


//------------------------------------------------------------------------------
/// Returns MAC short address
/// \return MAC short address
//------------------------------------------------------------------------------
unsigned short deRF_GetShortAddress(void)
{
	unsigned short mac;
	
	mac = deRF_RR(SHORT_ADDR_0);					// lower byte
	mac |= deRF_RR(SHORT_ADDR_1) << 8;		// higher byte
	
	return mac;
}


//------------------------------------------------------------------------------
/// Sets MAC PAN ID for Frame Filter address recognition
/// \param pan  MAC PAN ID
//------------------------------------------------------------------------------
void deRF_SetPANID(unsigned short pan)
{
	deRF_WR(PAN_ID_0, (pan & 0x00FF));				// lower byte
	deRF_WR(PAN_ID_1, (pan & 0xFF00) >> 8);		// higher byte
}

//------------------------------------------------------------------------------
/// Returns MAC PAN ID
/// \return MAC PAN ID
//------------------------------------------------------------------------------
unsigned short deRF_GetPANID(void)
{
	unsigned short pan;
	pan = deRF_RR(PAN_ID_0);					// lower byte
	pan |= deRF_RR(PAN_ID_1) << 8;		// higher byte
	
	return pan;
}

//------------------------------------------------------------------------------
/// Sets MAC IEEE address for Frame Filter address recognition
/// \param ieee   64 Bit MAC IEEE address
//------------------------------------------------------------------------------
void deRF_SetIEEEAddress(unsigned long long ieee)
{
	unsigned char i, part;
	 
	for(i=0; i<8; i++){
		part = (unsigned char)(ieee >> (8*i));
		deRF_WR(IEEE_ADDR_0 + i, part);
	}
}

//------------------------------------------------------------------------------
/// Returns MAC IEEE address
/// \param Pointer to 8 byte array
//------------------------------------------------------------------------------
void deRF_GetIEEEAddress(unsigned char *ieee)
{
	unsigned char i;
	
	for(i=0; i<8; i++)
		ieee[i] = deRF_RR(IEEE_ADDR_0+i);
}

//------------------------------------------------------------------------------
/// Returns MAC IEEE address
/// \param Pointer to 8 byte array
//------------------------------------------------------------------------------
unsigned char deRF_GetPartNumber(void)
{
	unsigned char i;
	

		i= deRF_RR(PART_NUM);
	  return(i);
}


//------------------------------------------------------------------------------
/// Sets number of attempts to retransmit a frame, when it was not acknowledged 
/// by the recipient, before the transaction gets cancelled.
/// \param n   Number of retries (0-15)
//------------------------------------------------------------------------------
void deRF_SetMaxFrameRetries(unsigned char n){
	unsigned char reg_value;
	
	if (n > 0xf)
		return;
	
	reg_value = 0x0F & deRF_RR(XAH_CTRL_0);
	reg_value |= n << 4;
	deRF_WR(XAH_CTRL_0,reg_value);
}

//------------------------------------------------------------------------------
/// Sets number of in TX_ARET mode to repeat the CSMACA procedure before the 
/// transaction gets cancelled.
/// \param n   Number of retries (0,1,...,5)
//------------------------------------------------------------------------------
void deRF_SetMaxCSMARetries(unsigned char n){
	unsigned char reg_value;
	
	if (n > 5)
		return;
	
	reg_value = 0xF1 & deRF_RR(XAH_CTRL_0);
	reg_value |= n << 1;
	deRF_WR(XAH_CTRL_0,reg_value);
}

//------------------------------------------------------------------------------
/// Enable/Disable PAN Coordinator Mode -
/// \param n   1: coord. mode on, 0: coord. mode off
//------------------------------------------------------------------------------
void deRF_SetCoordinatorMode(unsigned char n){
	unsigned char reg_value;
	
	reg_value = deRF_RR(CSMA_SEED_1);
	
	if(n == 1)
		reg_value |= (1 << 3);
	else if(n == 0)
		reg_value &= ~(1 << 3);
	else
		return;
	
	deRF_WR(CSMA_SEED_1,reg_value);
}

//------------------------------------------------------------------------------
/// Sets CSMA SEED
/// CSMA_SEED is the seed for the random number generation that determines the
/// length of the back-off period in the CSMA-CA algorithm.
/// It is recommended to initialize registers CSMA_SEED by random values.
/// \param seed  11 bit value
//------------------------------------------------------------------------------
void deRF_SetCSMASeed(unsigned short seed){
	unsigned char reg_value;
	
	deRF_WR(CSMA_SEED_0,(seed & 0xFF));
	
	reg_value = 0xF8 & deRF_RR(CSMA_SEED_1);
	reg_value |= (seed >> 8) & 0x07;
	deRF_WR(CSMA_SEED_1,reg_value);
}

//------------------------------------------------------------------------------
/// Enable/disbale promiscuous mode
/// If this promiscuous mode is enabled, every incoming frame with a valid PHR 
/// finishes with IRQ_3 (TRX_END) interrupt even if the third level filter rules 
/// do not match or the FCS is not valid.
/// \param n  1: promis. mode on, 0: promis. mode off
//------------------------------------------------------------------------------
void deRF_SetPromiscuousMode(unsigned char n){
	unsigned char reg_value;
	
	reg_value = deRF_RR(XAH_CTRL_1);
	
	if(n == 1)
		reg_value |= (1 << 1);
	else if(n == 0)
		reg_value &= ~(1 << 1);
	else
		return;
	
	deRF_WR(XAH_CTRL_1,reg_value);
}

//------------------------------------------------------------------------------
/// Enable/disbale automatic FCS (Frame Check Sequence) generation
/// If enabled, the Frame Buffer write access can be stopped right after
/// MAC payload. There is no need to write FCS dummy bytes.
/// For further information see p. 86
/// \param n  1: auto fcs mode on, 0: auto fcs mode off
//------------------------------------------------------------------------------
void deRF_SetAutoCRC(unsigned char n){
	unsigned char reg_value;
	
	reg_value = deRF_RR(TRX_CTRL_1);
	
	if(n == 1)
		reg_value |= (1 << 5);
	else if(n == 0)
		reg_value &= ~(1 << 5);
	else
		return;
	
	deRF_WR(TRX_CTRL_1,reg_value);
}

//------------------------------------------------------------------------------
/// Sets the maximum and minimum back-off exponent used in CSMA-CA algorithm.
/// For further information see p. 75
/// \param maxBE  4 bit value
/// \param minBE  4 bit value
//------------------------------------------------------------------------------
void deRF_SetCSMAbackOff(unsigned char maxBE, unsigned char minBE){
	deRF_WR(CSMA_BE,(((maxBE & 0x0f) << 4) | (minBE & 0x0F)));
}


//------------------------------------------------------------------------------
/// Sets the channel.
/// For further information see p. 123
/// \param channel  channel number (valid values: 11 - 26)
/// \return deRF_SUCCESS: channel change successful
///         deRF_TIMEOUT: channel change timed out
//------------------------------------------------------------------------------
unsigned char deRF_SetChannel(unsigned char channel)
{
	unsigned char state;  
	deRF_WR(PHY_CC_CA, (0x1F & channel));  

	// add a delay to allow the PLL to lock if in active mode.  
	state = deRF_getTRXstate();  
	if ((state == RX_ON) || (state == PLL_ON))  
	{  
			delay_us(TIME_PLL_LOCK);  
	}  

	// channel changed ?
	if ((deRF_RR(PHY_CC_CA) & 0x1F) == channel)  
	{  
			return deRF_SUCCESS;  
	}  

	return deRF_TIMEOUT; 
}

//------------------------------------------------------------------------------
/// Returns the current RF channel number.
/// \return channel number (0x0B - 0x1A)
//------------------------------------------------------------------------------
unsigned char deRF_GetChannel(void)
{
	return ((deRF_RR(PHY_CC_CA) & 0x1F));	
}


void deRF_TransmitFrame_F(unsigned char *data, unsigned char length){
	
	/* Write data to AT86RF Flash*/
	deRF_WF(data, length);
		
	/* Send data */
	deRF_SetSLPTR();
	delay_us(1);
	deRF_ClearSLPTR();
}
//------------------------------------------------------------------------------
/// Transmit MAC Frame
/// \param *data  points to data array
/// \param length  count of data bytes to transmit
//------------------------------------------------------------------------------
void deRF_TransmitFrame(unsigned char *data, unsigned char length){	
	
	

	/* check if we are in transmit state, if not perform transition */
	if(deRF_getTRXstate() != TX_ARET_ON)
		if(deRF_StateTransition(TX_ARET_ON) == deRF_TIMEOUT)
			return;
	
	/* HINT: Folgende Anweisung muss drin bleiben,
   *       ansonsten kommt es bei schneller Empfangsfolge zum Reset
   */
	while(deRF_getTRXstate() != TX_ARET_ON) ;
	spiInterruptEnable(SPI_IRQ_TDRE);		
	/* Write data to AT86RF Flash*/
	deRF_WF(data, length);

	/* Send data */
	deRF_SetSLPTR();
	delay_us(1);
	deRF_ClearSLPTR();
			
}

//------------------------------------------------------------------------------
/// Receive MAC Frame
/// \param *data  points to data array where we store the received frame bytes
/// \return  count of received data bytes (length of mac frame)
//------------------------------------------------------------------------------
unsigned char deRF_ReceiveFrame(unsigned char *data){
	unsigned char length;
	
	/* prepare txbuf and init spi transmission via PDC */
	/* HINT: the use of the PDC for spi transmission is the reason
	         that we need two buffers with the same size (rxbuf & txbuf)
	 */
	spiInterruptEnable(SPI_IRQ_TDRE);
	
	txbuf[0] = SPI_CMD_FR;
	deRF_RF(data, txbuf);
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
#ifdef XXX	
	/* determine count of received frame bytes */
	length = rxbuf[1];
	
	/* copy mac frame only */
	for(i=0; i<(length+2); i++)
		data[i] = rxbuf[i+2];
#endif
	length = data[1];
	return length;
}






unsigned char  spiReadWrite(unsigned char  val)
{
AT91S_SPI *spi = AT91C_BASE_SPI1;


    /* Transmission is started by writing the transmit data. */

 spi->SPI_TDR = (unsigned short )val;
 
	/* Wait for receiver data register full. */
    while((spi->SPI_SR & AT91C_SPI_RDRF) == 0);

    /* Read data. */
    val = spi->SPI_RDR;

    return val;
}




/** @brief  This function will upload a frame from the radio transceiver's frame
 *          buffer. This function is relocated to SRAM.
 *
 *          If the frame currently available in the radio transceiver's frame buffer
 *          is out of the defined bounds. Then the frame length, lqi value and crc
 *          be set to zero. This is done to indicate an error.
 *
 */


unsigned char  mac_buffer_tx[sizeof(rx_frame_t)];  ///< Global MAC buffer (transmit)
unsigned char  mac_buffer_rx[sizeof(rx_frame_t)];  ///< Global MAC buffer (receive)
int  hal_frame_read(unsigned char *frame_buf)
{
		int ret_len;   
	
    unsigned char frame_length;
    unsigned char tempData;
    unsigned char *rx_data=mac_buffer_rx;
		AT91S_SPI *spi = AT91C_BASE_SPI1;	
    rx_frame_t *rx_frame=(rx_frame_t*)mac_buffer_rx;

   //  framework - RTOS Signal fnInUserInterrupt(1); // entering;
    disable_irq();
    
    // Pull SEL line.
   	deRF_ClearSEL();	
		while(!SPI_IsFinished(AT91C_BASE_SPI1));
    // Send frame read command.
    tempData = spiReadWrite(SPI_CMD_FR);

    // Read frame length.
    frame_length = spiReadWrite(HAL_DUMMY_READ);

    ret_len= frame_length;

    // Check for correct frame length.
    if ((frame_length >= HAL_MIN_FRAME_LENGTH) && (frame_length <= HAL_MAX_FRAME_LENGTH))
    {
        *rx_data++ = frame_length; //Store frame length.
        do
        {
            tempData = spiReadWrite(HAL_DUMMY_READ); // dummy write
            *rx_data++ =  *frame_buf++ =tempData;

        } while (--frame_length > 0);

        // Read LQI value for this frame.
        rx_frame->lqi = spiReadWrite(HAL_DUMMY_READ);

        // Release SEL line.
      deRF_SetSEL(); //  PIO_SODR_A = SPI_CS0_1B;
    }
    else
    {
        // Release SEL line.
        deRF_SetSEL(); //PIO_SODR_A = SPI_CS0_1B;
			  ret_len= 0;
        if (rx_frame)
        {
            rx_frame->length = 0;
            rx_frame->lqi    = 0;
            rx_frame->crc    = 0;
        }
    }

    // A dummy read to ensure SPI1 RDRF flag is reset.
    spi->SPI_RDR; //SPI_RDR_1;
    
    //uEnable_Interrupt();
	enable_irq();
   //  framewor RTO fnInUserInterrupt(0); // leaving
return (ret_len);

}



 #define HAL_TRX_CMD_RADDRM     (0x7F) //!< Register Address Mask.
void hal_frame_write(unsigned char  *write_buffer, unsigned char  length)
{
			AT91S_SPI *spi = AT91C_BASE_SPI1;


    length &= HAL_TRX_CMD_RADDRM; //Truncate length to maximum frame length.

   // fnInUserInterrupt(1); // entering;
    disable_irq(); //  uDisable_Interrupt() ;
	
	if(deRF_getTRXstate() != TX_ARET_ON)
		if(deRF_StateTransition(TX_ARET_ON) == deRF_TIMEOUT)

    //Toggle the SLP_TR pin to initiate the frame transmission.
//    hal_set_slptr_high();
	
//    hal_set_slptr_low();

    // Pull SEL line.
   	deRF_ClearSEL(); // PIO_CODR_A = SPI_CS0_1B;
	while(!SPI_IsFinished(AT91C_BASE_SPI1));

    // SEND FRAME WRITE COMMAND AND FRAME LENGTH.
    spiReadWrite(SPI_CMD_FW );
    spiReadWrite(length);

    //Download to the Frame Buffer.
    do
    {
        spiReadWrite(*write_buffer++);
        --length;

    } while (length > 0);

     // Release SEL line.
     deRF_SetSEL(); //PIO_SODR_A = SPI_CS0_1B;
	deRF_SetSLPTR();
		delay_us(1);
		deRF_ClearSLPTR();
    // Dummy read.
    spi->SPI_RDR;

     enable_irq();// uEnable_Interrupt();
   //  RTOS  fnInUserInterrupt(0); // leaving
}
