#include "stm32f4xx_conf.h"

#include "temp.h"
#include "flash.h"

#define COUNT(arr) (sizeof(arr) / sizeof(*arr))

//! @addtogroup temp
//! @{

//! @name I2C pin mapping
//! @{
#define I2CA             I2C1
#define I2CA_AF          GPIO_AF_I2C1
#define I2CA_CLK         RCC_APB1Periph_I2C1
#define I2CA_SDA_GPIO    GPIOB
#define I2CA_SDA_PIN     GPIO_Pin_7
#define I2CA_SDA_PINSRC  GPIO_PinSource7
#define I2CA_SCL_GPIO    GPIOB
#define I2CA_SCL_PIN     GPIO_Pin_8
#define I2CA_SCL_PINSRC  GPIO_PinSource8

#define I2CB             I2C3
#define I2CB_AF          GPIO_AF_I2C3
#define I2CB_CLK         RCC_APB1Periph_I2C3
#define I2CB_SDA_GPIO    GPIOC
#define I2CB_SDA_PIN     GPIO_Pin_9
#define I2CB_SDA_PINSRC  GPIO_PinSource9
#define I2CB_SCL_GPIO    GPIOA
#define I2CB_SCL_PIN     GPIO_Pin_8
#define I2CB_SCL_PINSRC  GPIO_PinSource8
//! @}

//! @name I2C Addresses for STTS751-0xxxx
//! @{
#define STTS_ADDR_0_7_5K 0b10010000
#define STTS_ADDR_0_12K  0b10010010
#define STTS_ADDR_0_20K  0b01110000
#define STTS_ADDR_0_33K  0b01110010
//! @}
//! @name I2C Addresses for STTS751-1xxxx
//! @{
#define STTS_ADDR_1_7_5K 0b10010100
#define STTS_ADDR_1_12K  0b10010110
#define STTS_ADDR_1_20K  0b01110100
#define STTS_ADDR_1_33K  0b01110110
//! @}

//! @name STTS751 Register map
//! @{

//! Temperature high
#define TEMP_ADDR_T_HI   0x00
//! Status
#define TEMP_ADDR_STAT   0x01
//! Temperature low
#define TEMP_ADDR_T_LO   0x02
//! Configuration
#define TEMP_ADDR_CONF   0x03
//! Conversion rate
#define TEMP_ADDR_CRATE  0x04
//! Temperature high limit high byte
#define TEMP_ADDR_HL_HI  0x05
//! Temperature high limit low byte
#define TEMP_ADDR_HL_LO  0x06
//! Temperature low limit high byte
#define TEMP_ADDR_LL_HI  0x07
//! Temperature low limit low byte
#define TEMP_ADDR_LL_LO  0x08
//! One-shot register
#define TEMP_ADDR_1SHOT  0x0F
//! THERM limit
#define TEMP_ADDR_TLIM   0x20
//! THERM hysteresis
#define TEMP_ADDR_THYST  0x21
//! SMBus timeout enable
#define TEMP_ADDR_SMTIM  0x22
//! Product ID
#define TEMP_ADDR_PID    0xFD
//! Manufacturer ID
#define TEMP_ADDR_MID    0xFE
//! Revision number
#define TEMP_ADDR_REV    0xFF
//! @}

/*!
 @brief Device configuration list

 Please define all devices to be used here.
 */
static temp_sensor_t temp_sensors[] = {
	{I2CA, STTS_ADDR_1_7_5K, 0, 0, 0},
	{I2CA, STTS_ADDR_1_12K, 0, 0, 0},
	{I2CA, STTS_ADDR_1_20K, 0, 0, 0},
	{I2CA, STTS_ADDR_1_33K, 0, 0, 0},
	{I2CB, STTS_ADDR_1_7_5K, 0, 0, 0},
	{I2CB, STTS_ADDR_1_12K, 0, 0, 0},
	{I2CB, STTS_ADDR_1_20K, 0, 0, 0},
	{I2CB, STTS_ADDR_1_33K, 0, 0, 0},
};

const int temp_num_sensors = COUNT(temp_sensors);

//! @}

/*!
 @brief Read a single byte from the provided address
 @param address The address in the peripheral memory to read from
 @return The value at that address
 */
static uint8_t read_byte(temp_sensor_t *restrict sensor, uint8_t address);

/*!
 @brief Initialize the device
 
 @pre temp_init() has run
 @post The device is configured for 12-bit precision
 
 This is intended to be done by temp_init() and involves I2C write
 */
static void sensor_init(temp_sensor_t *restrict sensor);

/*!
 @brief Write a single byte to the address specified

 @param address The address within the peripheral's memory to write to
 @param data    The data to write
 */
static void write_byte(temp_sensor_t *restrict sensor, uint8_t address, uint8_t data);

/////////////////////////////////////////////////////////
// Public functions
/////////////////////////////////////////////////////////

void temp_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	int i;

	// Enable I2C clocks
	RCC_APB1PeriphClockCmd(I2CA_CLK | I2CB_CLK, ENABLE);
	// Enable all GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
	                       RCC_AHB1Periph_GPIOB |
	                       RCC_AHB1Periph_GPIOC |
	                       RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	
	// I2CA_SDA
	GPIO_InitStructure.GPIO_Pin = I2CA_SDA_PIN;
	GPIO_Init(I2CA_SDA_GPIO, &GPIO_InitStructure);

	// I2CA_SCL
	GPIO_InitStructure.GPIO_Pin = I2CA_SCL_PIN;
	GPIO_Init(I2CA_SCL_GPIO, &GPIO_InitStructure);
	
	// I2CB_SDA
	GPIO_InitStructure.GPIO_Pin = I2CB_SDA_PIN;
	GPIO_Init(I2CB_SDA_GPIO, &GPIO_InitStructure);
	
	// I2CB_SCL
	GPIO_InitStructure.GPIO_Pin = I2CB_SCL_PIN;
	GPIO_Init(I2CB_SCL_GPIO, &GPIO_InitStructure);

	// AF configuration	
	GPIO_PinAFConfig(I2CA_SDA_GPIO, I2CA_SDA_PINSRC, I2CA_AF);
	GPIO_PinAFConfig(I2CA_SCL_GPIO, I2CA_SCL_PINSRC, I2CA_AF);
	GPIO_PinAFConfig(I2CB_SDA_GPIO, I2CB_SDA_PINSRC, I2CB_AF);
	GPIO_PinAFConfig(I2CB_SCL_GPIO, I2CB_SCL_PINSRC, I2CB_AF);
	
	// I2C configuration
	I2C_DeInit(I2CA);
	I2C_DeInit(I2CB);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;

	I2C_Init(I2CA, &I2C_InitStructure);
	I2C_Init(I2CB, &I2C_InitStructure);
	
	I2C_Cmd(I2CA, ENABLE);
	I2C_Cmd(I2CB, ENABLE);

	for(i = 0; i < temp_num_sensors; i++){
		sensor_init(&temp_sensors[i]);
	};
}

void temp_read_all(void){
	static float results[COUNT(temp_sensors)];
	float *result = results;
	temp_sensor_t *sensor;
	// Just read each sensor in series
	for(sensor = temp_sensors; sensor < temp_sensors + temp_num_sensors; sensor++, result++){
		*result = temp_read(sensor);
		if(flash_is_ready())
			flash_write_float(*result);
	}
}

uint8_t temp_read_status(temp_sensor_t *restrict sensor){
	sensor->status = read_byte(sensor, TEMP_ADDR_STAT);
	return sensor->status;
}

float temp_read(temp_sensor_t *restrict sensor){
	uint8_t buffer[2];
	buffer[0] = read_byte(sensor, TEMP_ADDR_T_HI);
	buffer[1] = read_byte(sensor, TEMP_ADDR_T_LO);
	sensor->temperature = ((float)(buffer[0] & 0x7F)) + ((float)buffer[1])/256.0;
	if(buffer[0] & 0x80)
		sensor->temperature = -sensor->temperature;
	sensor->num_readings += 1;
	return sensor->temperature;
}


/////////////////////////////////////////////////////////
// Private functions
/////////////////////////////////////////////////////////

static void sensor_init(temp_sensor_t *restrict sensor){
	write_byte(sensor, TEMP_ADDR_CONF, 0b00001100);
}

static uint8_t read_byte(temp_sensor_t *restrict sensor, uint8_t address){
	uint8_t data;
	while(I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_BUSY));
	I2C_GenerateSTART(sensor->I2C, ENABLE);
	while (!I2C_CheckEvent(sensor->I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(sensor->I2C, sensor->device, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(sensor->I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(sensor->I2C, address);
	while (I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_BTF) == RESET);
	I2C_GenerateSTART(sensor->I2C, ENABLE);
	while (!I2C_CheckEvent(sensor->I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(sensor->I2C, sensor->device, I2C_Direction_Receiver);
	while(I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_ADDR) == RESET);
	I2C_AcknowledgeConfig(sensor->I2C, DISABLE);
	(void)sensor->I2C->SR2;
	I2C_GenerateSTOP(sensor->I2C, ENABLE);
	while(I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_RXNE) == RESET);
	data = I2C_ReceiveData(sensor->I2C);
	while(sensor->I2C->CR1 & I2C_CR1_STOP);
	I2C_AcknowledgeConfig(sensor->I2C, ENABLE);
	I2C_ClearFlag(sensor->I2C, I2C_FLAG_AF);
	return data;
}

static void write_byte(temp_sensor_t *restrict sensor, uint8_t address, uint8_t data){
	while(I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_BUSY));
	I2C_GenerateSTART(sensor->I2C, ENABLE);
	while (!I2C_CheckEvent(sensor->I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(sensor->I2C, sensor->device, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(sensor->I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(sensor->I2C, address);
	while (I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_BTF) == RESET);
	I2C_SendData(sensor->I2C, data);
	I2C_GenerateSTOP(sensor->I2C, ENABLE);
	while (I2C_GetFlagStatus(sensor->I2C, I2C_FLAG_TXE) == RESET);
	(void)sensor->I2C->SR2;
}



