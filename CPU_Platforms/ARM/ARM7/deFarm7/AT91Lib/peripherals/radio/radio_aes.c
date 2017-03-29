#include <aic/aic.h>
#include <spi/spi.h>
#include <rftrxspi/rftrxspi.h>
#include "radio.h"
#include <board.h>
#include <pio/pio_it.h>

#include "radio_aes.h"

void aes_sram_write(unsigned char addr, unsigned char *data, unsigned char length);
void aes_sram_read(unsigned char *data);
void aes_sram_rw(unsigned char cmd, unsigned char *txdata, unsigned char *rxdata) ;

// Indicates that a key is set
unsigned char keyset = 0;
// Indicates if AES SRAM is in key mode or data mode
unsigned char new_key = 0;

//------------------------------------------------------------------------------
/// AES ECB encryption
/// \param data Pointer the uncrypted data (has to be a multiple of 16)
/// \param length	Length of data
//------------------------------------------------------------------------------
void aes_ecb_encrypt(unsigned char *data, unsigned char length){
	
	unsigned char pos;
	
	
	if(keyset == 0 || data == 0 || length == 0 || length%16 != 0) 
		return;

	// Encrypt first 16 bytes
	aes_sram_rw(AES_DIR_ENC | AES_MODE_ECB, data, data);
	pos = 16;
	// Encrypt rest of the data
	while(pos < length){
		
		aes_sram_rw(AES_DIR_ENC | AES_MODE_ECB, &data[pos], &data[pos-16]);
		pos+=16;
	}
	aes_sram_read(&data[pos-16]);

}

//------------------------------------------------------------------------------
/// AES ECB decryption
///	\param key for decryption
/// \param data Pointer the encrypted data (has to be a multiple of 16)
/// \param length	Length of data
//------------------------------------------------------------------------------
void aes_ecb_decrypt(unsigned char *key, unsigned char *data, unsigned char length){

	unsigned char pos;
	
	
	if(key == 0 || data == 0 || length == 0 || length%16 != 0) 
		return;

	// Write decryption key
	radio_aes_write_key(key);
	// New key is set
	new_key = 1;
	
	// Encrypt first 16 bytes
	aes_sram_rw(AES_DIR_DEC | AES_MODE_ECB, data, data);
	
	pos = 16;
	// Encrypt rest of the data
	while(pos < length){
		
		aes_sram_rw(AES_DIR_DEC | AES_MODE_ECB, &data[pos], &data[pos-16]);
		pos+=16;
	}
	aes_sram_read(&data[pos-16]);
}

//------------------------------------------------------------------------------
/// AES CBC MIC encryption
/// \param data Pointer the uncrypted data (has to be a multiple of 16)
/// \param length	Length of data
//------------------------------------------------------------------------------
void aes_mic_calculation(unsigned char *data, unsigned char length, unsigned char* mic){
	
	unsigned char pos;
		
	if(keyset == 0 || data == 0 || length == 0 || length%16 != 0) 
		return;

	// Encrypt first 16 bytes
	aes_sram_rw(AES_DIR_ENC | AES_MODE_CBC, data, data);
	pos = 16;
	// Encrypt rest of the data
	while(pos < length){
		
		aes_sram_rw(AES_DIR_ENC | AES_MODE_CBC, &data[pos], mic);
		pos+=16;
	}
	// Read MIC
	aes_sram_read(mic);
}

//------------------------------------------------------------------------------
/// AES CBC encryption
/// \param data Pointer the uncrypted data (has to be a multiple of 16)
/// \param length	Length of data
//------------------------------------------------------------------------------
void aes_cbc_encrypt(unsigned char *data, unsigned char length){
	
	unsigned char pos, tmp[18], j, i;
	
		
	if(keyset == 0 || data == 0 || length == 0 || length%16 != 0) 
		return;
	

	tmp[0] = AES_DIR_ENC | AES_MODE_CBC;
	tmp[17] = AES_DIR_ENC | AES_MODE_CBC | AES_REQUEST;
	
	for(i=0; i<16; i++)
		tmp[i+1] = data[i];

	aes_sram_write(AES_CTRL, tmp, 18);
	aes_sram_read(data);
	
	pos = 16;
	// Encrypt rest of the data
	while(pos < length){
		j=0;
		for(i=pos; i<pos+16; i++) {
			tmp[j+1] = data[i] ^ data[pos-16];
			j++;
		}
		aes_sram_write(AES_CTRL, tmp, 17);
		aes_sram_read(&data[pos]);
		pos+=16;
	}
}


//------------------------------------------------------------------------------
/// AES CBC decryption
/// This can not be called to decrypt data encrypted with the transceiver cbc unit
/// encrypted data since the aes unit does a XOR before encryption for calculating
/// the 802.15.4 MIC checksum.
///	\param key for decryption
/// \param data Pointer the encrypted data (has to be a multiple of 16)
/// \param length	Length of data
//------------------------------------------------------------------------------
void aes_cbc_decrypt(unsigned char *key, unsigned char *data, unsigned char length){
		
	unsigned char pos;
		
	if(key == 0 || data == 0 || length == 0 || length%16 != 0) 
		return;

	// Write decryption key
	radio_aes_write_key(key);
	// New key is set
	new_key = 1;
	
	// Encrypt first 16 bytes
	aes_sram_rw(AES_DIR_DEC | AES_MODE_CBC, data, data);
	
	pos = 16;
	// Encrypt rest of the data
	while(pos < length){
		
		aes_sram_rw(AES_DIR_DEC | AES_MODE_CBC, &data[pos], &data[pos-16]);
		pos+=16;
	}
	aes_sram_read(&data[pos-16]);
}

//------------------------------------------------------------------------------
/// Write AES Key to SRAM
/// This writes a new AES Key to SRAM.
/// \param key Pointer to a 16 byte array where the key should be stored
//------------------------------------------------------------------------------
void radio_aes_write_key(unsigned char *key) {
	unsigned char i;
	unsigned char txbuf[20];

	// Set key mode
	txbuf[0] = AES_MODE_KEY;
	
	
	for(i=0; i<16; i++)
		txbuf[i+1] = key[i];
	
	// Write to SRAM
	aes_sram_write(AES_CTRL, txbuf, 17);
	
	// Indicates that we are in key mode
	new_key = 1;
	keyset = 1;

	
}

//------------------------------------------------------------------------------
/// Read AES Key from SRAM
/// This reads the AES Key from SRAM. If there was already an encryption performed
/// then this reads the current AES round key that is needed for decryption of the
///	last encrypted data.
/// \param key Pointer to a 16 byte array where the key should be stored
//------------------------------------------------------------------------------
void radio_aes_read_key(unsigned char *key) {
	unsigned char txbuf[1];
	
	// Set key mode
	txbuf[0] = AES_MODE_KEY;

	// Perform a mode change to key mode
	aes_sram_write(AES_CTRL, txbuf, 1);
	
	// Read the current key and store it in the key buffer
	aes_sram_read(key);
	
	// Indicates that we are in key mode
	new_key = 1;

	
}

//------------------------------------------------------------------------------
/// Write SRAM access to the AES registers
/// This writes the key/data to the AES registers (only used for writing the key
///	since we can use fast SRAM access for writing the data)
/// 
/// \param data pointer to a 16 byte array where the read data should be stored
//------------------------------------------------------------------------------
void aes_sram_write(unsigned char addr, unsigned char *data, unsigned char length) {
	unsigned char i, txbuf[21];
	
	// Set SEL to low		
	deRF_ClearSEL();
	
	txbuf[0] = SPI_CMD_SW;
	txbuf[1] = addr;
	
	for(i = 0; i < length; i++) {
		txbuf[i+2] = data[i];
	}
	
	SPI_WriteBuffer(AT91C_BASE_SPI1, txbuf, length+2);

	// Wait for the operation to be finished
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
	// Set sel to high to complete SPI access
	deRF_SetSEL();	
}

//------------------------------------------------------------------------------
/// Read SRAM access to the AES registers
/// This reads the last key/data from the AES registers
/// 
/// \param data pointer to a 16 byte array where the read data should be stored
//------------------------------------------------------------------------------
void aes_sram_read(unsigned char *data) {
	
	unsigned char i, *read;
	delay_us(1);
	// Set SEL to low	
 	deRF_ClearSEL();

	// SRAM read mode	
	SPI_Write(AT91C_BASE_SPI1, 0, SPI_CMD_SR);
	SPI_Read(AT91C_BASE_SPI1); // Ignored

	// Set AES_STATE/AES_KEY register as starting point	(same address)
	SPI_Write(AT91C_BASE_SPI1, 0, AES_KEY);	
	SPI_Read(AT91C_BASE_SPI1); // Ignored

	// Write dummy byte
	SPI_Write(AT91C_BASE_SPI1, 0, 0);
	SPI_Read(AT91C_BASE_SPI1); // Ignored
	
	read = data;
	
	for(i = 1; i < 16; i++) {
		// Write dummy byte
		SPI_Write(AT91C_BASE_SPI1, 0, 0);
		// Read the data/key
		read[i-1] = SPI_Read(AT91C_BASE_SPI1);
	}
	// Write dummy byte
	SPI_Write(AT91C_BASE_SPI1, 0, 0);
	// Read last byte
	read[15] = SPI_Read(AT91C_BASE_SPI1);
	
	// Wait for the operation to be finished
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
	// Set sel to high to complete SPI access
	deRF_SetSEL();	
}

//------------------------------------------------------------------------------
/// Fast SRAM access to the AES registers
/// This function writes new data to encrypt/decrypt to the AES data registers
/// and reads the last encrypted/decrypted data simultaneously using only one
/// SPI access. 
/// See AT86RF231 Datasheet Chapter 11.1.5 for more information
/// 
/// \param txdata Pointer to a 16 byte array with the data to encrypt/decrypt
/// \param rxdata Pointer to a 16 byte array where the data should be stored
//------------------------------------------------------------------------------
void aes_sram_rw(unsigned char cmd, unsigned char *txdata, unsigned char *rxdata) {
	
	unsigned char i;
		
	// Set SEL to low
	deRF_ClearSEL();
	
	// SRAM write mode
	SPI_Write(AT91C_BASE_SPI1, 0, SPI_CMD_SW);
	SPI_Read(AT91C_BASE_SPI1);
	
	// If a new key was written before, then we have to change from key mode to
	// encryption/decryption mode
	if(new_key != 0) {
		// Set AES_CTRL register as start
		SPI_Write(AT91C_BASE_SPI1, 0, AES_CTRL);
		SPI_Read(AT91C_BASE_SPI1); //We have to read after a write access but we ignore this byte	
		
		// Write the command (encryption or decryption)
		SPI_Write(AT91C_BASE_SPI1, 0, cmd);
		SPI_Read(AT91C_BASE_SPI1); //We have to read after a write access but we ignore this byte
		
		new_key = 0;
	}
	else {
		// Set AES_STATE register as starting point
		SPI_Write(AT91C_BASE_SPI1, 0, AES_STATE);
		SPI_Read(AT91C_BASE_SPI1); //We have to read after a write access but we ignore this byte	
		
	}
	// Write the first byte that should be encrypted/decrypted
	SPI_Write(AT91C_BASE_SPI1, 0, txdata[0]);
	SPI_Read(AT91C_BASE_SPI1); //We have to read after a write access but we ignore this byte

	// Write the first byte that should be encrypted/decrypted
	SPI_Write(AT91C_BASE_SPI1, 0, txdata[1]);
	SPI_Read(AT91C_BASE_SPI1); //We have to read after a write access but we ignore this byte	

	
	for(i = 2; i < 16; i++) {
			// Write the other bytes from the 16 byte block that should be encrypted/decrypted
			SPI_Write(AT91C_BASE_SPI1, 0, txdata[i]);
			// Read the previously encrypted/decrypted data
			rxdata[i-2] = SPI_Read(AT91C_BASE_SPI1);
	}	
	
	// Write to AES_CMD mirror register that the block should now be encrypted/decrypted
	SPI_Write(AT91C_BASE_SPI1, 0, cmd | AES_REQUEST);
	// Read last byte from the previously encrypted/decrypted data
	rxdata[14] = SPI_Read(AT91C_BASE_SPI1);		

	// Write dummy to read last byte
	SPI_Write(AT91C_BASE_SPI1, 0, cmd | AES_REQUEST);
	// Read last byte from the previously encrypted/decrypted data
	rxdata[15] = SPI_Read(AT91C_BASE_SPI1);	
	
	// Wait for the operation to be finished
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
	// Set sel to high to complete SPI access
	deRF_SetSEL();	
	
	// AES operation takes 24 us to complete
	delay_us(24);
}

//------------------------------------------------------------------------------
/// Read AES status
/// This function reads the aes status byte
/// 
/// \param key Pointer to a 16 byte array where the key should be stored
//------------------------------------------------------------------------------
unsigned char readStatus(void) {
	
	unsigned char tmp;

	// Set SEL to low	
	deRF_ClearSEL();
	
	// SRAM read mode
 	SPI_Write(AT91C_BASE_SPI1, 0, SPI_CMD_SR);
 	tmp = SPI_Read(AT91C_BASE_SPI1);
	
	// Set status register addr
 	SPI_Write(AT91C_BASE_SPI1, 0, AES_STATUS);	
 	tmp = SPI_Read(AT91C_BASE_SPI1);
	
	// Write dummy
 	SPI_Write(AT91C_BASE_SPI1, 0, 0);	
 	tmp = SPI_Read(AT91C_BASE_SPI1);	
	
	// Write dummy
	SPI_Write(AT91C_BASE_SPI1, 0, 0);
	
	// Read status byte
 	tmp = SPI_Read(AT91C_BASE_SPI1);	
	
	while(!SPI_IsFinished(AT91C_BASE_SPI1));
	
	// Set SEL to high	
 	deRF_SetSEL();
	
	return tmp;
}
