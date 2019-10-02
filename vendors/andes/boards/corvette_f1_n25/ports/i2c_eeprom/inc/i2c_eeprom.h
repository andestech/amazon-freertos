int32_t EEPROM_Initial(void);
int32_t EEPROM_Write(uint32_t addr, uint8_t* buf, uint32_t len);
int32_t EEPROM_Read(uint32_t addr, uint8_t* buf, uint32_t len);
void EEPROM_Uninitial(void);
