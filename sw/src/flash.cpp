#include "flash.h"

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */ 

#define RESERVED_START (&num_sensors)
#define SECTOR_SIZE    (ADDR_FLASH_SECTOR_7 - ADDR_FLASH_SECTOR_6)

const uint32_t Flash::avail_words = 0x20000 - 1;

uint32_t Flash::offset = 0;
bool Flash::ready = false;

// This order is important, as it ensures that &num_sensors is the correct start address...
uint32_t Flash::num_sensors __attribute__ ((section(".store")));
float Flash::sensor_data[avail_words] __attribute__ ((section(".store")));


void Flash::init(uint32_t num_sensors_){
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	                FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

	for(auto i = (uint32_t)RESERVED_START; i < (uint32_t)(RESERVED_START + avail_words); i += SECTOR_SIZE){
		if(FLASH_EraseSector(get_sector(i), VoltageRange_3) != FLASH_COMPLETE)
			while(1);	
	}
	
	FLASH_ProgramWord((uint32_t)RESERVED_START, num_sensors_);
	
	offset = 0;
	ready = true;
}

void Flash::write_float(float data){
	// Check to make sure we are still in range
	if(offset >= avail_words)
		return;
	FLASH_ProgramWord((uint32_t)(sensor_data + offset), *((uint32_t *)(&data)));
	offset += 1;
}

uint32_t Flash::get_sector(uint32_t addr){
	if(addr < ADDR_FLASH_SECTOR_0)
		while(1); // Ooops...
	if(addr < ADDR_FLASH_SECTOR_1)
		return FLASH_Sector_0;
	if(addr < ADDR_FLASH_SECTOR_2)
		return FLASH_Sector_1;
	if(addr < ADDR_FLASH_SECTOR_3)
		return FLASH_Sector_2;
	if(addr < ADDR_FLASH_SECTOR_4)
		return FLASH_Sector_3;
	if(addr < ADDR_FLASH_SECTOR_5)
		return FLASH_Sector_4;
	if(addr < ADDR_FLASH_SECTOR_6)
		return FLASH_Sector_5;
	if(addr < ADDR_FLASH_SECTOR_7)
		return FLASH_Sector_6;
	if(addr < ADDR_FLASH_SECTOR_8)
		return FLASH_Sector_7;
	if(addr < ADDR_FLASH_SECTOR_9)
		return FLASH_Sector_8;
	if(addr < ADDR_FLASH_SECTOR_10)
		return FLASH_Sector_9;
	if(addr < ADDR_FLASH_SECTOR_11)
		return FLASH_Sector_10;
	if(addr < (ADD_FLASH_SECTOR_11 + SECTOR_SIZE))
		return FLASH_Sector_11;
	while(1);
}
