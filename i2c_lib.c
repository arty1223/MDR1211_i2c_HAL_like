/*
 * i2c_lib.c
 *
 *  Created on: Jul 2, 2025
 *      Authors: Artyom Skibitskiy
 * 		Библиотека для работы с i2c
 */

#include "i2c_lib.h"

void initI2C()
{
	RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTC | RST_CLK_PCLK_I2C), ENABLE);
	I2C_InitTypeDef I2C_InitStruct; // Структуры для настройки переферии.
	PORT_InitTypeDef PortInit;
	/* Настройка пинов PORTC 0,1 (I2C_SCL,I2C_SDA) другие выводы - см. документацию*/
	PORT_StructInit(&PortInit);
	PortInit.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1);
	PortInit.PORT_FUNC = PORT_FUNC_ALTER;
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;
	PortInit.PORT_PULL_UP = PORT_PULL_UP_ON;
	PortInit.PORT_PD = PORT_PD_OPEN;
	PORT_Init(MDR_PORTC, &PortInit);

	/* Включение I2C */
	I2C_Cmd(ENABLE);

	/* Заполняем структуру для I2C с нужными нам параметрами */
	I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.I2C_ClkDiv = 159;
	I2C_InitStruct.I2C_Speed = I2C_SPEED_UP_TO_400KHz;
	I2C_Init(&I2C_InitStruct);
}

// адрес, байт данных, таймаут на всю передачу.
I2C_Status I2C_Transmit_Byte(uint8_t adr, uint8_t data, uint32_t timeout)
{
	uint32_t tickstart = GetTick();

	while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET)
	{ // ждем освобождения линии в течении 25 мсек
		if (GetTick() - tickstart >= 25)
		{
			return I2C_BUSY;
		}
	}
	I2C_Send7bitAddress(adr, I2C_Direction_Transmitter);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}

	I2C_SendByte(data);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}

	I2C_SendSTOP();
	return I2C_OK;
}

I2C_Status I2C_Recieve_Byte(uint8_t adr, uint8_t *recieveByte, uint32_t timeout)
{
	uint32_t tickstart = GetTick();

	while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET)
	{ // ждем освобождения линии в течении 25 мсек
		if (GetTick() - tickstart >= 25)
		{
			return I2C_BUSY;
		}
	}
	I2C_Send7bitAddress(adr, I2C_Direction_Receiver);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}

	I2C_StartReceiveData(I2C_Send_to_Slave_ACK);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}
	*recieveByte = I2C_GetReceivedData();
	I2C_SendSTOP();
	return I2C_OK;
}

bool I2C_Ping(uint8_t adr, uint32_t timeout)
{
	uint32_t tickstart = GetTick();

	while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET)
	{ // ждем освобождения линии в течении 25 мсек
		if (GetTick() - tickstart >= 25)
		{
			return false;
		}
	}
	I2C_Send7bitAddress(adr, I2C_Direction_Transmitter);
	I2C_SendSTOP();
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return false;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return false;
		}
	}
	return true;
}

I2C_Status I2C_Mem_Write(uint8_t adr, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	uint32_t tickstart = GetTick();

	while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET)
	{ // ждем освобождения линии в течении 25 мсек
		if (GetTick() - tickstart >= 25)
		{
			return I2C_BUSY;
		}
	}
	I2C_Send7bitAddress(adr, I2C_Direction_Transmitter);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}

	if (MemAddSize == I2C_MEMADD_SIZE_8BIT) // выбор 8-битного адреса
	{
		I2C_SendByte(MemAddress);
		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_nTRANS_TIMEOUT;
			}
		}
		while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_ACK_TIMEOUT;
			}
		}
	}
	else // выбор  16-битного адреса
	{
		uint16_t tMemAddr = MemAddress;
		for (int i = 0; i < 2; i++)
		{
			I2C_SendByte(tMemAddr >> 8); // сначала отправится старший байт
			while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
			{
				if (GetTick() - tickstart >= timeout)
				{
					return I2C_nTRANS_TIMEOUT;
				}
			}
			while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
			{
				if (GetTick() - tickstart >= timeout)
				{
					return I2C_ACK_TIMEOUT;
				}
			}
			tMemAddr <<= 8; // затем нижний
		}
	}

	for (int i = 0; i < size; i++) // запись данных
	{
		I2C_SendByte(pData[i]);
		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_nTRANS_TIMEOUT;
			}
		}
		while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_ACK_TIMEOUT;
			}
		}
	}
	I2C_SendSTOP();
	return I2C_OK;
}

I2C_Status I2C_Mem_Read(uint8_t adr, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	uint32_t tickstart = GetTick();

	while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET)
	{ // ждем освобождения линии в течении 25 мсек
		if (GetTick() - tickstart >= 25)
		{
			return I2C_BUSY;
		}
	}
	I2C_Send7bitAddress(adr, I2C_Direction_Transmitter);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}

	if (MemAddSize == I2C_MEMADD_SIZE_8BIT) // выбор 8-битного адреса
	{
		I2C_SendByte(MemAddress);
		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_nTRANS_TIMEOUT;
			}
		}
		while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_ACK_TIMEOUT;
			}
		}
	}
	else // выбор  16-битного адреса
	{
		uint16_t tMemAddr = MemAddress;
		for (int i = 0; i < 2; i++)
		{
			I2C_SendByte(tMemAddr >> 8); // сначала отправится старший байт
			while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
			{
				if (GetTick() - tickstart >= timeout)
				{
					return I2C_nTRANS_TIMEOUT;
				}
			}
			while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
			{
				if (GetTick() - tickstart >= timeout)
				{
					return I2C_ACK_TIMEOUT;
				}
			}
			tMemAddr << 8; // затем нижний
		}
	}

	I2C_Send7bitAddress(adr, I2C_Direction_Receiver);
	while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_nTRANS_TIMEOUT;
		}
	}
	while (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) != SET)
	{
		if (GetTick() - tickstart >= timeout)
		{
			return I2C_ACK_TIMEOUT;
		}
	}

	// чтение
	for (int i = 0; i < size; i++)
	{
		tickstart = GetTick();
		if (size - i == 1)
		{
			I2C_StartReceiveData(I2C_Send_to_Slave_NACK);
		}
		else
		{
			I2C_StartReceiveData(I2C_Send_to_Slave_ACK);
		}
		while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET)
		{
			if (GetTick() - tickstart >= timeout)
			{
				return I2C_nTRANS_TIMEOUT;
			}
		}
		pData[i] = I2C_GetReceivedData();
	}
	I2C_SendSTOP();
	return I2C_OK;
}