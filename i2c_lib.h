/*
 * i2c_lib.c
 *
 *  Created on: Jul 2, 2025
 *      Authors: Artyom Skibitskiy
 * 		"HALоподобная" библиотека для работы с i2c
 * 
 */

#ifndef INC_I2C_LIB_H_
#define INC_I2C_LIB_H_

#pragma once
#include "app.h"
#include <stdbool.h>

#define I2C_MEMADD_SIZE_8BIT            0x00000001U
#define I2C_MEMADD_SIZE_16BIT           0x00000010U

typedef enum
{
	I2C_OK = 0x00U,
	I2C_ERROR = 0x01U,
	I2C_BUSY = 0x02U,
	I2C_ACK_TIMEOUT = 0x03U,
	I2C_nTRANS_TIMEOUT = 0x04U
} I2C_Status;

void initI2C(); // настройка порта C и I2C на PC0 и PC1
I2C_Status I2C_Transmit_Byte(uint8_t adr, uint8_t data, uint32_t timeout); // Передать байт устройству по адресу
I2C_Status I2C_Recieve_Byte(uint8_t adr, uint8_t *recieveByte, uint32_t timeout); // Принять байт от устройства по адресу
bool I2C_Ping(uint8_t adr, uint32_t timeout); // Проверка наличия устройства по адресу
I2C_Status I2C_Mem_Write(uint8_t adr, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t size, uint32_t timeout); // Запись массива во внешнюю память по адресу
I2C_Status I2C_Mem_Read(uint8_t adr, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t size, uint32_t timeout); // Считывание масива из внешней памяти

#endif /* INC_I2C_LIB_H_ */
