extern "C" {
	#include "stm32f4xx_conf.h"
}
#include "temp.h"


#define I2CA_AF          GPIO_AF_I2C1
#define I2CA_CLK         RCC_APB1Periph_I2C1
#define I2CA_SDA_GPIO    GPIOB
#define I2CA_SDA_PIN     GPIO_Pin_7
#define I2CA_SDA_PINSRC  GPIO_PinSource7
#define I2CA_SCL_GPIO    GPIOB
#define I2CA_SCL_PIN     GPIO_Pin_8
#define I2CA_SCL_PINSRC  GPIO_PinSource8

#define I2CB_AF          GPIO_AF_I2C3
#define I2CB_CLK         RCC_APB1Periph_I2C3
#define I2CB_SDA_GPIO    GPIOC
#define I2CB_SDA_PIN     GPIO_Pin_9
#define I2CB_SDA_PINSRC  GPIO_PinSource9
#define I2CB_SCL_GPIO    GPIOA
#define I2CB_SCL_PIN     GPIO_Pin_8
#define I2CB_SCL_PINSRC  GPIO_PinSource8

#define TEMP_ADDR_T_HI   0x00
#define TEMP_ADDR_STAT   0x01
#define TEMP_ADDR_T_LO   0x02
#define TEMP_ADDR_CONF   0x03
#define TEMP_ADDR_CRATE  0x04

TemperatureSensor TemperatureSensor::sensors[] = {
	TemperatureSensor(I2CA, 0b01110110),
	//{I2CA, 0b10010110, 0, 0},
	//{I2CB, 0b10010110, 0, 0},
	//{I2CB, 0b01110110, 0, 0},
};

/////////////////////////////////////////////////////////
// Public functions
/////////////////////////////////////////////////////////

TemperatureSensor::TemperatureSensor(I2C_TypeDef *I2Cx, uint8_t devicex) :
	I2C(I2Cx),
	device(devicex),
	num_readings(0)
{}

void TemperatureSensor::read_status(){
	status = read_byte(TEMP_ADDR_STAT);
}

float TemperatureSensor::read(){
	uint8_t buffer[2];
	buffer[0] = read_byte(TEMP_ADDR_T_HI);
	buffer[1] = read_byte(TEMP_ADDR_T_LO);
	temperature = ((float)(buffer[0] & 0x7F)) + ((float)buffer[1])/256.0;
	if(buffer[0] & 0x80)
		temperature = -temperature;
	num_readings += 1;
	return temperature;
}


/////////////////////////////////////////////////////////
// Private functions
/////////////////////////////////////////////////////////

void TemperatureSensor::init(void){
	write_byte(TEMP_ADDR_CONF, 0b00001100);
}

uint8_t TemperatureSensor::read_byte(uint8_t address){
	uint8_t data;
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C, ENABLE);
	while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C, device, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C, address);
	while (I2C_GetFlagStatus(I2C, I2C_FLAG_BTF) == RESET);
	I2C_GenerateSTART(I2C, ENABLE);
	while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C, device, I2C_Direction_Receiver);
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_ADDR) == RESET);
	I2C_AcknowledgeConfig(I2C, DISABLE);
	(void)I2C->SR2;
	I2C_GenerateSTOP(I2C, ENABLE);
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_RXNE) == RESET);
	data = I2C_ReceiveData(I2C);
	while(I2C->CR1 & I2C_CR1_STOP);
	I2C_AcknowledgeConfig(I2C, ENABLE);
	I2C_ClearFlag(I2C, I2C_FLAG_AF);
	return data;
}

void TemperatureSensor::write_byte(uint8_t address, uint8_t data){
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C, ENABLE);
	while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C, device, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C, address);
	while (I2C_GetFlagStatus(I2C, I2C_FLAG_BTF) == RESET);
	I2C_SendData(I2C, data);
	I2C_GenerateSTOP(I2C, ENABLE);
	while (I2C_GetFlagStatus(I2C, I2C_FLAG_TXE) == RESET);
	(void)I2C->SR2;
}

/////////////////////////////////////////////////////////
// Static functions
/////////////////////////////////////////////////////////

void TemperatureSensor::hw_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	TemperatureSensor *sensor;
	
	RCC_APB1PeriphClockCmd(I2CA_CLK | I2CB_CLK, ENABLE);
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

	
	GPIO_PinAFConfig(I2CA_SDA_GPIO, I2CA_SDA_PINSRC, I2CA_AF);
	GPIO_PinAFConfig(I2CA_SCL_GPIO, I2CA_SCL_PINSRC, I2CA_AF);
	GPIO_PinAFConfig(I2CB_SDA_GPIO, I2CB_SDA_PINSRC, I2CB_AF);
	GPIO_PinAFConfig(I2CB_SCL_GPIO, I2CB_SCL_PINSRC, I2CB_AF);
	
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

	for(sensor = sensors; sensor < sensors + COUNT(sensors); sensor++){
		sensor->init();
	}
}

void TemperatureSensor::read_all(void){
	TemperatureSensor *sensor;
	for(sensor = sensors; sensor < sensors + num_sensors; sensor++){
		sensor->read();
	}
}

const int TemperatureSensor::num_sensors = COUNT(sensors);

