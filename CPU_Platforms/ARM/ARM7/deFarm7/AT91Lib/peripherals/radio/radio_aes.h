#ifndef RADIO_AES_H
#define	RADIO_AES_H

typedef enum {
	
	AES_STATUS	= 0x82,
	AES_CTRL = 0x83,
	AES_KEY = 0x84,
	AES_STATE = 0x84,
	AES_CTRL_MIRROR = 0x94
	
} aes_register_t;

typedef enum {
	AES_DIR_ENC = 0x00,
	AES_DIR_DEC = 0x08,
	AES_MODE_ECB = 0x00,
	AES_MODE_KEY = 0x10,
	AES_MODE_CBC = 0x20,
	AES_REQUEST = 0x80
} aes_cmd_t;



void aes_ecb_encrypt(unsigned char *data, unsigned char length);
void aes_ecb_decrypt(unsigned char *key, unsigned char *data, unsigned char length);
void aes_mic_calculation(unsigned char *data, unsigned char length, unsigned char* mic);
void aes_cbc_encrypt(unsigned char *data, unsigned char length);
void aes_cbc_decrypt(unsigned char *key, unsigned char *data, unsigned char length);
void radio_aes_write_key(unsigned char *key);
void radio_aes_read_key(unsigned char *key);
unsigned char readStatus(void);
#endif
