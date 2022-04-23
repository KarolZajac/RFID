
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
uint8_t rc522_checkCard(uint8_t *id);
