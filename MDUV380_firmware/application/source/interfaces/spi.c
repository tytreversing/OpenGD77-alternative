/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
 * Copyright (C) 2019-2024 Roger Clark, VK3KYY / G4KYF
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. Use of this source code or binary releases for commercial purposes is strictly forbidden. This includes, without limitation,
 *    incorporation in a commercial product or incorporation into a product or project which allows commercial use.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdbool.h>
#include "interfaces/hr-c6000_spi.h"
#include "main.h"



volatile bool SPI0inUse = false;
volatile bool SPI1inUse = false;

void SPIInit(void)
{

}

void SPI0Setup(void)
{
	//SPIO is a bit-banged interface on the MD9600
}

void SPI1Setup(void)
{
	//SPI1 is a hardware Interface on the MD9600
}

int SPI0WritePageRegByte(uint8_t page, uint8_t reg, uint8_t val)
{
	uint8_t txBuf[3];
	UBaseType_t SavedInterruptStatus;

	if (SPI0inUse)
	{
		return -1;
	}
	SavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	SPI0inUse = true;

	txBuf[0] = page;
	txBuf[1] = reg;
	txBuf[2] = val;

	SPI0Write(txBuf,3);

	SPI0inUse = false;
	taskEXIT_CRITICAL_FROM_ISR(SavedInterruptStatus);
	return 0;
}


int SPI0WritePageRegByteExtended(uint8_t page, uint16_t reg, uint8_t val)
{
	uint8_t txBuf[4];
	UBaseType_t SavedInterruptStatus;

	if (SPI0inUse)
	{
		return -1;
	}
	SavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	SPI0inUse = true;

	txBuf[0] = page | 0x40;
	txBuf[2] = (reg >> 8) & 0x07;
	txBuf[1] = reg & 0xFF;
	txBuf[3] = val;

	SPI0Write(txBuf,4);

	SPI0inUse = false;
	taskEXIT_CRITICAL_FROM_ISR(SavedInterruptStatus);
	return 0;
}




int SPI0ReadPageRegByte(uint8_t page, uint8_t reg, volatile uint8_t *val)
{
	uint8_t rxBuf[3];
	uint8_t txBuf[3];
	UBaseType_t SavedInterruptStatus;

	if (SPI0inUse)
	{
		return -1;
	}
	SavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	SPI0inUse = true;

	txBuf[0] = page | 0x80;
	txBuf[1] = reg;
	txBuf[2] = 0xFF;

	SPI0Read(txBuf, rxBuf, 3);

	*val = rxBuf[2];

	SPI0inUse = false;
	taskEXIT_CRITICAL_FROM_ISR(SavedInterruptStatus);

	return 0;
}

int SPI0ClearPageRegByteWithMask(uint8_t page, uint8_t reg, uint8_t mask, uint8_t val)
{
	int status;
	uint8_t tmp_val;

	status = SPI0ReadPageRegByte(page, reg, &tmp_val);

	if (status == kStatus_Success)
	{
		tmp_val = val | (tmp_val & mask);
		status = SPI0WritePageRegByte(page, reg, tmp_val);
	}

	return status;
}

int SPI0WritePageRegByteArray(uint8_t page, uint8_t reg, const uint8_t *values, uint8_t length)
{
	const int SPI0_PAGE_WRITE_BUFFER_SIZE = 128 + 2;
	uint8_t txBuf[SPI0_PAGE_WRITE_BUFFER_SIZE + 2];
	UBaseType_t SavedInterruptStatus;

	if (length > SPI0_PAGE_WRITE_BUFFER_SIZE)
	{
		return kStatus_InvalidArgument;
	}

	if (SPI0inUse)
	{
		return -1;
	}

	SavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	SPI0inUse = true;

	txBuf[0] = page;
	txBuf[1] = reg;
	memcpy(txBuf + 2, values, length);

	SPI0Write(txBuf, length + 2);

	SPI0inUse = false;
	taskEXIT_CRITICAL_FROM_ISR(SavedInterruptStatus);
	return 0;
}

int SPI0ReadPageRegByteArray(uint8_t page, uint8_t reg, volatile uint8_t *values, uint8_t length)
{
	uint8_t rxBuf[0x60 + 2];
	uint8_t txBuf[0x60 + 2];
	UBaseType_t SavedInterruptStatus;

	if (length > 0x60)
	{
		return kStatus_InvalidArgument;
	}

	if (SPI0inUse)
	{
		return -1;
	}
	SavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	SPI0inUse = true;

	txBuf[0] = page | 0x80;
	txBuf[1] = reg;

	SPI0Read(txBuf, rxBuf, length + 2);

	for (int i = 0; i < length; i++)
	{
		values[i] = rxBuf[i + 2];
	}


	SPI0inUse = false;
	taskEXIT_CRITICAL_FROM_ISR(SavedInterruptStatus);
	return 0;
}

void SPI0Write(uint8_t *txBuf, uint8_t length)
{
	uint8_t val;

	//Set the CS pin Low
	HAL_GPIO_WritePin(DMR_SPI_CS_GPIO_Port,DMR_SPI_CS_Pin, GPIO_PIN_RESET);
	for(int v = 0; v < length; v++)
	{
		val = txBuf[v];
		for (register int i = 0; i < 8; i++)
		{
			HAL_GPIO_WritePin(DMR_SPI_MOSI_GPIO_Port, DMR_SPI_MOSI_Pin, ((val & 0x80) != 0));

#ifdef SPI_0_DELAYS
			for(volatile int x=0;x<1;x++);
#endif

			HAL_GPIO_WritePin(DMR_SPI_CLK_GPIO_Port, DMR_SPI_CLK_Pin, GPIO_PIN_RESET);
#ifdef SPI_0_DELAYS
			for(volatile int x=0;x<1;x++);
#endif

			val = val << 1;
			HAL_GPIO_WritePin(DMR_SPI_CLK_GPIO_Port, DMR_SPI_CLK_Pin, GPIO_PIN_SET);

#ifdef SPI_0_DELAYS
			for(volatile int x=0;x<1;x++);
#endif
		}
	}

	//Set the CS pin high again
	HAL_GPIO_WritePin(DMR_SPI_CS_GPIO_Port, DMR_SPI_CS_Pin, GPIO_PIN_SET);
}

void SPI0Read(uint8_t *txBuf, uint8_t *rxBuf, uint8_t length)
{
	uint8_t val;
	uint8_t rxval;

	//Set the CS pin Low
	HAL_GPIO_WritePin(DMR_SPI_CS_GPIO_Port,DMR_SPI_CS_Pin, GPIO_PIN_RESET);

	for(int v = 0; v < length; v++)
	{
		val = txBuf[v];
		rxval = 0;
		for (register int i = 0; i < 8; i++)
		{
			HAL_GPIO_WritePin(DMR_SPI_MOSI_GPIO_Port, DMR_SPI_MOSI_Pin, ((val & 0x80) != 0));
			HAL_GPIO_WritePin(DMR_SPI_CLK_GPIO_Port, DMR_SPI_CLK_Pin, GPIO_PIN_RESET);
			val = val << 1;
			rxval = (rxval << 1) + (HAL_GPIO_ReadPin(DMR_SPI_MISO_GPIO_Port, DMR_SPI_MISO_Pin) != 0);
			HAL_GPIO_WritePin(DMR_SPI_CLK_GPIO_Port, DMR_SPI_CLK_Pin, GPIO_PIN_SET);

		}
		rxBuf[v] = rxval;
	}

	//Set the CS pin high again
	HAL_GPIO_WritePin(DMR_SPI_CS_GPIO_Port,DMR_SPI_CS_Pin, GPIO_PIN_SET);
}


int SPI1WritePageRegByteArray(uint8_t page, uint8_t reg, const uint8_t *values, uint8_t length)
{
	uint8_t txBuf[32 + 2];

	if (length > 32)
	{
		return kStatus_InvalidArgument;
	}

	if (SPI1inUse)
	{
		return -1;
	}
	SPI1inUse = true;

//	dspi_transfer_t masterXfer;
	int status = kStatus_Fail;

	txBuf[0] = page;
	txBuf[1] = reg;
	memcpy(txBuf + 2, values, length);

	HAL_GPIO_WritePin(V_SPI_CS_GPIO_Port, V_SPI_CS_Pin, GPIO_PIN_RESET);
	status = HAL_SPI_Transmit(&hspi2, txBuf, length + 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(V_SPI_CS_GPIO_Port, V_SPI_CS_Pin, GPIO_PIN_SET);

	SPI1inUse = false;

	return status;
}

int SPI1ReadPageRegByteArray(uint8_t page, uint8_t reg, volatile uint8_t *values, uint8_t length)
{
	uint8_t rxBuf[32 + 2];
	uint8_t txBuf[32 + 2];

	if (length > 32)
	{
		return kStatus_InvalidArgument;
	}

	if (SPI1inUse)
	{
		return -1;
	}
	SPI1inUse = true;

//	dspi_transfer_t masterXfer;
	HAL_StatusTypeDef status;

	txBuf[0] = page | 0x80;
	txBuf[1] = reg;

	HAL_GPIO_WritePin(V_SPI_CS_GPIO_Port, V_SPI_CS_Pin, GPIO_PIN_RESET);
	status = HAL_SPI_TransmitReceive(&hspi2, txBuf, rxBuf, length + 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(V_SPI_CS_GPIO_Port, V_SPI_CS_Pin, GPIO_PIN_SET);

	if (status == HAL_OK)
	{
		for (int i = 0; i < length; i++)
		{
			values[i] = rxBuf[i + 2];
		}
	}

	SPI1inUse = false;

	return status;
}
