
#include "inttypes.h"

/* Commands */
#define PCD_IDLE            0x00
#define PCD_AUTHENT           0x0E
#define PCD_TRANSCEIVE          0x0C
#define PCD_CALCCRC			0x03

/* Mifare_One card command word */
#define PICC_REQIDL           	0x26
#define PICC_ANTICOLL        	0x93
#define PICC_AUTHNT_KEY_A		0x60
#define PICC_READ 				0x30


void rc522_init(void);
uint8_t rc522_registerToByteRead(uint8_t reg);
uint8_t rc522_registerToByteWrite(uint8_t reg);
uint8_t rc522_readReg(uint8_t reg);
void rc522_setRegBitMask(uint8_t reg,  uint8_t mask);
void rc522_clearRegBitMask(uint8_t reg,  uint8_t mask);
void rc522_writeReg(uint8_t reg, uint8_t val);
void rc522_reset(void);
void antennaOn(void);
void antennaOff(void);
uint8_t rc522_request(uint8_t reqMode, uint8_t *tagType);
uint8_t rc522_toCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen);
uint8_t rc522_antiColl(uint8_t *serNum);
void rc522_calculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData);
uint8_t rc522_checkCard(uint8_t *id);
