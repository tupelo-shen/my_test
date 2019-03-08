#ifndef __EEPROM_DRIVER_H__
#define __EEPROM_DRIVER_H__

/*
 * public interfaces for operating EEPROM
 */
int          DRV_EEPROM_Initialize(void);
int          DRV_EEPROM_WriteData(unsigned short addr , char* data, unsigned int num);
int          DRV_EEPROM_ReadData(unsigned short addr , char* data, unsigned int num);

#endif // __EEPROM_DRIVER_H__
