#include "stm32f10x.h"
#include "i2c.h"
#include "si2c.h"

SI2C_TypeDef si2c;

void My_I2C1_Init(void);
void My_OnBoardLED_Init(void);

int main(void)
{
	// 硬件I2C的使用
	// My_I2C1_Init();
	// My_OnBoardLED_Init();
	// uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf,0xa5};

	// My_I2C_SendBytes(I2C1, 0x78, commands, 5);

	// uint8_t rcvd;
	
	// My_I2C_ReceiveBytes(I2C1, 0x78, &rcvd, 1);
	
	// if((rcvd & (0x01 << 6)) == 0)
	// {
	// 	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	// }
	// else
	// {
	// 	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	// }

	// while(1)
	// {
	// }


	// 软I2C的使用
	si2c.SCL_GPIOx = GPIOB;
	si2c.SCL_GPIO_Pin = GPIO_Pin_6;
	si2c.SDA_GPIOx = GPIOB;
	si2c.SDA_GPIO_Pin = GPIO_Pin_7;
	My_SI2C_Init(&si2c);
	
	My_OnBoardLED_Init();
	
	uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	
	My_SI2C_SendBytes(&si2c, 0x78, commands, 5);
	
	uint8_t rcvd;
	
	My_SI2C_ReceiveBytes(&si2c, 0x78, &rcvd, 1);
	
	if((rcvd & (0x01 << 6)) == 0)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}
	
	while(1)
	{
	}

}

void My_I2C1_Init(void)
{
	// 初始化PB6和PB7为I2C1的SCL和SDA引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	// 初始化I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// 复位
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	// 取消复位
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_ClockSpeed = 400000; // 400kHz
	I2C_Init(I2C1, &I2C_InitStructure);

	I2C_Cmd(I2C1, ENABLE); // 使能I2C1
}


void My_OnBoardLED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}





