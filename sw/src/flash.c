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

#define SECTOR_BYTES  (ADDR_FLASH_SECTOR_7 - ADDR_FLASH_SECTOR_6)
#define RESERVED_START (&num_sensors)
#define FLASH_END (ADDR_FLASH_SECTOR_11 + SECTOR_BYTES)

//! The size of FP storage in 32-bit words
#define AVAIL_WORDS (0x20000 - 1)
//! The current offset from the begining of #sensor_data to be writing
static uint32_t offset = 0;
//! Flag indicatiing that device is ready to write	
static int ready = 0;


//! @name In flash
//! @{
//! The following are actually located in flash and are not directly writable
//! This order is important, as it ensures that &num_sensors is the correct start address...

//! The number of sensors in the system; useful for data decoding
static uint32_t num_sensors __attribute__ ((section(".store")));
//! The actual sensor data
static float sensor_data[AVAIL_WORDS] __attribute__ ((section(".store")));
//! @}


/*!
 @brief Convert an address to a sector

 @param addr The address in flash
 @return The sector number
 */
static uint32_t get_sector(uint32_t addr);


void flash_init(uint32_t num_sensors_){
	uint32_t *i;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	                FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

	for(i = RESERVED_START; i < (RESERVED_START + AVAIL_WORDS); i += (SECTOR_BYTES / sizeof(float))){
		if(FLASH_EraseSector(get_sector((uint32_t)i), VoltageRange_3) != FLASH_COMPLETE)
			while(1);	
	}
	
	FLASH_ProgramWord((uint32_t)RESERVED_START, num_sensors_);
	
	offset = 0;
	ready = 1;
}

int flash_is_ready(void){
	return ready;
}

void flash_write_float(float data){
	uint32_t addr = (uint32_t)(sensor_data + offset);
	
	// Check to make sure we are still in range
	if(addr >= FLASH_END){
		ready = 0;
		return;
	}

	FLASH_ProgramWord(addr, *((uint32_t *)(&data)));
	offset += 1;
}

static uint32_t get_sector(uint32_t addr){
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
	if(addr < (ADDR_FLASH_SECTOR_11 + SECTOR_BYTES))
		return FLASH_Sector_11;
	while(1);
}
