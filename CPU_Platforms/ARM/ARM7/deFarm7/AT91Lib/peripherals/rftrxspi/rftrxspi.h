/*  AT86RF231 Transciever Interface
* Date of the Last Update:  Oct 01, 2012
* Ported to dresden elektronic deRFarm7 Gateway - Atmel AT91SAM7X-512 MCPU
* Hochschule München - Seminar Advances Embedded Systems - Ron Barker
* Keil  ARM - µLink Environment */


#ifndef RFTRXSPI_H
#define RFTRXSPI_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include <radio/radio.h>
//------------------------------------------------------------------------------
//         #defines
//------------------------------------------------------------------------------
/// Define SPI1 Master Clockrate (max. 3.5 MHz)
#define SPI_FCLK		8000000				// [Hz]




#define HAL_MIN_FRAME_LENGTH   ( 0x03 ) //!< A frame should be at least 3 bytes.
#define HAL_MAX_FRAME_LENGTH   ( 0x7F ) //!< A frame should no more than 127 bytes.
#define HAL_DUMMY_READ         (0x00) //!< Dummy value for the SPI.

/// Global RF Values
#define PAN_ID_DEFAULT			0xFFFF


#define SHORT_ADDR_DEFAULT	0x1234
#define IEEE_ADDR_DEFAULT		0x0004860000000001 
/* The ieee mac default address is based on the iee assignment for the ITTC University of Kansas (USA) and is used here only
to provide interpretable address for the wireshark sniffer.  During runtime an unique id will be generated.
*/



#define CHANNEL_DEFAULT			11

#define MAX_FRAME_RETRIES		3			// 4-bit value (p. 73)
#define MAX_CSMA_RETRIES		4			// 3-bit value (p. 73)
#define MAX_BE							5			// maximum back-off exponent
#define MIN_BE							3			// minimum back-off exponent

/// AT86RF231 TRX_STATUS states (see p. 45)
typedef enum {P_ON 		= 0x00,
							BUSY_RX = 0x01,
							BUSY_TX = 0x02,
							FORCE_TRX_OFF = 0x03,
							FORCE_PLL_ON = 0x04,
							RX_ON 	= 0x06,
							TRX_OFF = 0x08,
							PLL_ON  = 0x09,
							SLEEP		= 0x0F,
							BUSY_RX_AACK = 0x11,
							BUSY_TX_ARET = 0x12,
							RX_AACK_ON = 0x16,
							TX_ARET_ON = 0x19,
							RX_ON_NOCLK = 0x1C,
							RX_AACK_ON_NOCLK = 0x1D,
							BUSY_RX_AACK_NOCLK = 0x1E,
							STATE_TRANSITION_IN_PROGRESS = 0x1F
						 } trx_states_t;

/// AT86RF231 TRX_CMD list for TRX_STATE register (see p. 46)
/*typedef enum {NOP_CMD 		= 0x00,
							TX_START_CMD = 0x02,
							FORCE_TRX_OFF_CMD = 0x03,
							FORCE_PLL_ON_CMD = 0x04,
							RX_ON_CMD = 0x06,
							TRX_OFF_CMD = 0x08,
							PLL_ON_CMD = 0x09,
							RX_AACK_ON_CMD = 0x16,
							TX_ARET_ON_CMD = 0x19
						 } trx_cmd_t; */
						 
/// Enum for TRAC_STATUS in register TRX_STATE
typedef enum { TRAC_SUCCESS = 0,
							 TRAC_SUCCESS_DATA_PENDING = 1,
							 TRAC_SUCCESS_WAIT_FOR_ACK = 2,
							 TRAC_CHANNEL_ACCESS_FAILURE = 3,
							 TRAC_NO_ACK = 5,
							 TRAC_INVALID = 7 
						 } trac_status_t;


/// SPI stuff
typedef enum{
	SPI_CMD_RR = 0x80, 		// register read
	SPI_CMD_RW = 0xC0,		// register write
	SPI_CMD_FR = 0x20, 		// frame read
	SPI_CMD_FW = 0x60, 		// frame write
	SPI_CMD_SR = 0x00, 		// sram read
	SPI_CMD_SW = 0x40 		// sram write
} at86_spi_cmd_t;

/// AT86RF231 Registers
#define TRX_STATUS				(0x01)
#define TRX_STATE					(0x02)
#define TRX_CTRL_1				(0x04)
#define PHY_CC_CA					(0x08)
#define XAH_CTRL_1				(0x17)
#define PART_NUM					(0x1C)
#define SHORT_ADDR_0			(0x20)
#define SHORT_ADDR_1			(0x21)
#define PAN_ID_0					(0x22)
#define PAN_ID_1					(0x23)
#define IEEE_ADDR_0				(0x24)
#define IEEE_ADDR_1				(0x25)
#define IEEE_ADDR_2				(0x26)
#define IEEE_ADDR_3				(0x27)
#define IEEE_ADDR_4				(0x28)
#define IEEE_ADDR_5				(0x29)
#define IEEE_ADDR_6				(0x2A)
#define IEEE_ADDR_7				(0x2B)
#define XAH_CTRL_0				(0x2C)
#define CSMA_SEED_0				(0x2D)
#define CSMA_SEED_1				(0x2E)
#define CSMA_BE						(0x2F)

/// AT86RD registers
typedef enum
{
    AT86_TRX_STATUS     = 0x01,
    AT86_TRX_STATE      = 0x02,
    AT86_TRX_CTRL_0     = 0x03,
    AT86_TRX_CTRL_1     = 0x04,
    AT86_PHY_TX_PWR     = 0x05,
    AT86_PHY_RSSI       = 0x06,
    AT86_PHY_ED_LEVEL   = 0x07,
    AT86_PHY_CC_CCA     = 0x08,
    AT86_CCA_THRES      = 0x09,
    AT86_RX_CTRL        = 0x0a,
    AT86_SFD_VAL        = 0x0b,
    AT86_TRX_CTRL2      = 0x0c,
    AT86_ANT_DIV        = 0x0d,
    AT86_IRQ_MASK       = 0x0e,
    AT86_IRQ_STATUS     = 0x0f,
    AT86_VREG_CTRL      = 0x10,
    AT86_BATMON         = 0x11,
    AT86_XOSC_CTRL      = 0x12,
    AT86_RX_SYN         = 0x15,
    AT86_XAH_CTRL_1     = 0x17,
    AT86_FTN_CTRL       = 0x18,
    AT86_PLL_CF         = 0x1a,
    AT86_PLL_DCU        = 0x1b,
    AT86_PART_NUM       = 0x1c,
    AT86_VERSION_NUM    = 0x1d,
    AT86_MAN_ID_0       = 0x1e,
    AT86_MAN_ID_1       = 0x1f,
    AT86_SHORT_ADDR_0   = 0x20,
    AT86_SHORT_ADDR_1   = 0x21,
    AT86_PAN_ID_0       = 0x22,
    AT86_PAN_ID_1       = 0x23,
    AT86_IEEE_ADDR_0    = 0x24,
    AT86_IEEE_ADDR_1    = 0x25,
    AT86_IEEE_ADDR_2    = 0x26,
    AT86_IEEE_ADDR_3    = 0x27,
    AT86_IEEE_ADDR_4    = 0x28,
    AT86_IEEE_ADDR_5    = 0x29,
    AT86_IEEE_ADDR_6    = 0x2a,
    AT86_IEEE_ADDR_7    = 0x2b,
    AT86_XAH_CTRL_0     = 0x2c,
    AT86_CSMA_SEED_0    = 0x2d,
    AT86_CSMA_SEED_1    = 0x2e,
    AT86_CSMA_BE        = 0x2f
} at86_reg_t;

	
typedef enum {
	SPI_IRQ_RDRF 		= 0x001,
	SPI_IRQ_TDRE 		= 0x002,
	SPI_IRQ_MODF 		= 0x004,
	SPI_IRQ_OVRES 	= 0x008,
	SPI_IRQ_ENDRX 	= 0x010,
	SPI_IRQ_ENDTX 	= 0x020,
	SPI_IRQ_RXBUFF 	= 0x040,
	SPI_IRQ_TXBUFE 	= 0x080,
	SPI_IRQ_NSSR 		= 0x100,
	SPI_IRQ_TXEMPTY = 0x200
} at91sam_spi_irq_sources_t;





typedef struct{
    unsigned char  length;                       ///< Length of frame.
    unsigned char data[ HAL_MAX_FRAME_LENGTH ]; ///< Actual frame data.
    unsigned char lqi;                          ///< LQI value for received frame.
    bool crc;                        ///< Flag - did CRC pass for received frame?
} __attribute__((packed)) rx_frame_t;



//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------
void deRF_Init(void);
unsigned char deRF_RR(unsigned char address);
void deRF_WR(unsigned char address, unsigned char value);
void deRF_WF(unsigned char *data, unsigned char length);
void deRF_RF(void *rx_buffer, void *tx_buffer);
unsigned char deRF_StateTransition(trx_states_t state);
void deRF_SetShortAddress(unsigned short mac);
unsigned short deRF_GetShortAddress(void);
void deRF_SetPANID(unsigned short pan);
unsigned short deRF_GetPANID(void);
void deRF_SetIEEEAddress(unsigned long long ieee);
void deRF_GetIEEEAddress(unsigned char *ieee);
unsigned char deRF_GetPartNumber(void);
void deRF_ClearSLPTR(void);
void deRF_SetSLPTR(void);
void deRF_reset(void);
unsigned char deRF_getTRXstate(void);

void deRF_TransmitFrame(unsigned char *data, unsigned char length);
unsigned char deRF_ReceiveFrame(unsigned char *data);
void deRF_SetMaxFrameRetries(unsigned char n);
void deRF_SetMaxCSMARetries(unsigned char n);
void deRF_SetCoordinatorMode(unsigned char n);
void deRF_SetCSMASeed(unsigned short seed);
void deRF_SetPromiscuousMode(unsigned char n);
void deRF_SetAutoCRC(unsigned char n);
void deRF_SetCSMAbackOff(unsigned char maxBE, unsigned char minBE);
unsigned char deRF_SetChannel(unsigned char channel);
unsigned char deRF_GetChannel(void);

void delay_us(int);

void deRF_SetSEL(void);
void deRF_ClearSEL(void);

void spiInterruptEnable(at91sam_spi_irq_sources_t irq_type);
void spiInterruptDisable(at91sam_spi_irq_sources_t irq_type);

#endif //#ifndef RFTRXSPI_H

