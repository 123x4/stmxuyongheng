#include "stm32f10x.h"

void delay_us(uint32_t us);

void My_SI2C_Init(void);

void scl_write(uint8_t level);
void sda_write(uint8_t level);

uint8_t sda_read(void);

void SendStart(void);
void SendStop(void);

uint8_t SendByte(uint8_t Byte);
uint8_t ReceiveByte(uint8_t Ack);

int My_SI2C_SendBytes(uint8_t Addr, uint8_t *pData, uint16_t Size);
int My_SI2C_ReceiveBytes(uint8_t Addr, uint8_t *pBuffer, uint16_t Size);

int main(void)
{
	My_SI2C_Init();
	
	uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	
	My_SI2C_SendBytes(0x78, commands, 5);
	
	while(1)
	{
	}
}

void delay_us(uint32_t us)
{
	uint32_t n = us * 8;
	
	for(uint32_t i=0; i<n; i++);
}

//初始化PA0和PA1为开漏输出模式，作为I2C的SCL和SDA
void My_SI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
}
// 向SCL写0和1来控制高低电平
void scl_write(uint8_t level)
{
	// PA0
	if(level == 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	}
}
// 向SDA写0和1来传输数据
void sda_write(uint8_t level)
{
	// PA1
	if(level == 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
	}	
}

uint8_t sda_read(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_SET)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// 发送起始位
void SendStart(void)
{
	sda_write(0);
	delay_us(1);
}
// 发送停止位
void SendStop(void)
{
	// SCL为低电平时才允许SDA去写
	scl_write(0);
	// SCL拉低后，SDA拉低
	sda_write(0);
	delay_us(1);
	
	// 拉高SCL
	scl_write(1);
	delay_us(1);
	
	// SCL高电平期间，SDA拉高
	sda_write(1);
	delay_us(1);
}

uint8_t SendByte(uint8_t Byte)
{
	for(int8_t i=7; i>=0; i--)
	{
		scl_write(0);
		if((Byte & (0x01 << i)) != 0)
		{
			sda_write(1);
		}
		else
		{
			sda_write(0);
		}
		delay_us(1);
		scl_write(1);
		delay_us(1);
	}
	
	// 读取ACK或者NAK
	scl_write(0);
	sda_write(1);
	delay_us(1);
	scl_write(1);
	delay_us(1);
	return sda_read();
}

uint8_t ReceiveByte(uint8_t Ack)
{
	uint8_t byte = 0;
	
	for(int8_t i=7; i>=0; i--)
	{
		scl_write(0);
		sda_write(1);
		delay_us(1);
		scl_write(1);
		delay_us(1);
		
		if(sda_read() != 0)
		{
			byte |= (0x01 << i);
		}
	}
	
	scl_write(0);
	sda_write(!Ack);
	delay_us(1);
	scl_write(1);
	delay_us(1);
	
	return byte;
}


int My_SI2C_SendBytes(uint8_t Addr, uint8_t *pData, uint16_t Size)
{
	SendStart();
	
	if(SendByte(Addr & 0xfe) != 0)
	{
		SendStop();
		return -1;
	}
	
	for(uint32_t i=0; i<Size; i++)
	{
		if(SendByte(pData[i]) != 0)
		{
			SendStop();
			return -2;
		}
	}
	
	SendStop();
	
	return 0;
}


int My_SI2C_ReceiveBytes(uint8_t Addr, uint8_t *pBuffer, uint16_t Size)
{
	SendStart();
	
	if(SendByte(Addr | 0x01) != 0)
	{
		SendStop();
		return -1;
	}
	
	for(uint32_t i=0; i<Size-1; i++)
	{
	 	pBuffer[i] = ReceiveByte(1);
	}
	
	pBuffer[Size-1] = ReceiveByte(0);
	
	SendStop();
	return 0;
}











































