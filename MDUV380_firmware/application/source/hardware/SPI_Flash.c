/*
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

#include "hardware/SPI_Flash.h"
#include "interfaces/gpio.h"
#include <string.h>
#include "main.h"

// private functions
static bool spi_flash_busy(void);

static void spi_flash_setWriteEnable(bool cmd);
static inline void spi_flash_enable(void);
static inline void spi_flash_disable(void);

#if defined(PLATFORM_MD9600)
#define HANDLE_SPI  hspi2
__attribute__((section(".data.$RAM2"))) uint8_t SPI_Flash_sectorbuffer[4096];
#elif defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#define HANDLE_SPI  hspi1
__attribute__((section(".ccmram"))) uint8_t SPI_Flash_sectorbuffer[4096];
#else
#error SPI FLASH: unsupported platform
#endif


// COMMANDS. Not all implemented or used
#define W_EN            0x06    // write enable
#define W_DE            0x04    // write disable
#define R_SR1           0x05    // read status register 1
#define W_SR1           0x01    // write status register 1
#define R_SR2           0x35    // read status register 2
#define W_SR2           0x31    // write status register 2
#define R_SR3           0x15    // read status register 3
#define W_SR3           0x11    // write status register 3
#define PAGE_PGM        0x02    // page program
#define QPAGE_PGM       0x32    // quad input page program
#define BLK_E_64K       0xD8    // block erase 64KB
#define BLK_E_32K       0x52    // block erase 32KB
#define SECTOR_E        0x20    // sector erase 4KB
#define CHIP_ERASE      0xc7    // chip erase
#define CHIP_ERASE2     0x60    // same as CHIP_ERASE
#define E_SUSPEND       0x75    // erase suspend
#define E_RESUME        0x7a    // erase resume
#define PWR_DWN         0xb9    // power down
#define HIGH_PERF_M     0xa3    // high performance mode
#define CONT_R_RST      0xff    // continuous read mode reset
#define RELEASE         0xab    // release power down or HPM/Dev ID (deprecated)
#define R_MANUF_ID      0x90    // read Manufacturer and Dev ID (deprecated)
#define R_UNIQUE_ID     0x4b    // read unique ID (suggested)
#define R_JEDEC_ID      0x9f    // read JEDEC ID = Manuf+ID (suggested)
#define READ_DATA       0x03    // read one or more bytes
#define FAST_READ       0x0b    // read one or more bytes at highest possible frequency
#define R_SEC_REGS      0x48    //read security registers


#define WINBOND_MANUF   0xef


uint32_t flashChipPartNumber;

bool SPI_Flash_init(void)
{
	HAL_GPIO_WritePin(SPI_Flash_CS_GPIO_Port, SPI_Flash_CS_Pin, GPIO_PIN_SET); // Disable

    flashChipPartNumber = SPI_Flash_readPartID();

    // 4014 25Q80 8M bits 1M bytes, used in the GD-77
    // 4015 25Q16 16M bits 2M bytes, used in the Baofeng DM-1801 ?
    // 4017 25Q64 64M bits. Used in Roger's special GD-77 radios modified on the TYT production line
    // 4018 25Q128 128M bits. MD9600 / MDUV380 / MD380 etc
    return (flashChipPartNumber == 0x4018);
}

// Returns false for failed
// Note. There is no error checking that the device is not initially busy.
bool SPI_Flash_read(uint32_t addr, uint8_t *dataBuf, int size)
{
  uint8_t commandBuf[4]= { READ_DATA, addr >> 16, addr >> 8, addr };// command

  spi_flash_enable();
  HAL_SPI_Transmit(&HANDLE_SPI, commandBuf, 4, HAL_MAX_DELAY);
  HAL_SPI_Receive(&HANDLE_SPI, dataBuf, size, HAL_MAX_DELAY);
  spi_flash_disable();

  return true;
}

bool SPI_Flash_write(uint32_t addr, uint8_t *dataBuf, int size)
{
	bool retVal = true;
	int flashWritePos = addr;
	int flashSector;
	int flashEndSector;
	int bytesToWriteInCurrentSector = size;

	flashSector	= flashWritePos / 4096;
	flashEndSector = (flashWritePos + size) / 4096;

	if (flashSector != flashEndSector)
	{
		bytesToWriteInCurrentSector = (flashEndSector * 4096) - flashWritePos;
	}

	if (bytesToWriteInCurrentSector != 4096)
	{
		SPI_Flash_read(flashSector * 4096, SPI_Flash_sectorbuffer, 4096);
	}
	uint8_t *writePos = SPI_Flash_sectorbuffer + flashWritePos - (flashSector * 4096);
	memcpy(writePos, dataBuf, bytesToWriteInCurrentSector);

	retVal = SPI_Flash_eraseSector(flashSector * 4096);
	if (!retVal)
	{
		return false;
	}

	for (int i = 0; i < 16; i++)
	{
		retVal = SPI_Flash_writePage(flashSector * 4096 + i * 256, SPI_Flash_sectorbuffer + i * 256);
		if (!retVal)
		{
			return false;
		}
	}

	if (flashSector != flashEndSector)
	{
		uint8_t *bufPusOffset = (uint8_t *) dataBuf + bytesToWriteInCurrentSector;
		bytesToWriteInCurrentSector = size - bytesToWriteInCurrentSector;

		SPI_Flash_read(flashEndSector * 4096, SPI_Flash_sectorbuffer, 4096);
		memcpy(SPI_Flash_sectorbuffer, (uint8_t *) bufPusOffset, bytesToWriteInCurrentSector);

		retVal = SPI_Flash_eraseSector(flashEndSector * 4096);

		if (!retVal)
		{
			return false;
		}

		for (int i = 0; i < 16; i++)
		{
			retVal = SPI_Flash_writePage(flashEndSector * 4096 + i * 256, SPI_Flash_sectorbuffer + i * 256);

			if (!retVal)
			{
				return false;
			}
		}
	}

	return true;
}

uint32_t SPI_Flash_readStatusRegisters(void)
{
	uint8_t cmdVal = R_SR1;
	uint8_t r1 = 0x0, r2 = 0x0, r3 = 0x0;

	spi_flash_enable();
	HAL_SPI_Transmit(&HANDLE_SPI, &cmdVal, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&HANDLE_SPI, &r1, 1, HAL_MAX_DELAY);
	spi_flash_disable();

	cmdVal = R_SR2;
	spi_flash_enable();
	HAL_SPI_Transmit(&HANDLE_SPI, &cmdVal, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&HANDLE_SPI, &r2, 1, HAL_MAX_DELAY);
	spi_flash_disable();

	cmdVal = R_SR3;
	spi_flash_enable();
	HAL_SPI_Transmit(&HANDLE_SPI, &cmdVal, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&HANDLE_SPI, &r3, 1, HAL_MAX_DELAY);
	spi_flash_disable();

	return (r3 << 16) | (r2 << 8) | r1;
}

uint8_t SPI_Flash_readManufacturer(void)
{
	uint8_t commandBuf[4] = { R_JEDEC_ID, 0x00, 0x00, 0x00 };
	uint8_t recBuf[4];

	spi_flash_enable();
	HAL_SPI_TransmitReceive(&HANDLE_SPI, commandBuf, recBuf, 4, HAL_MAX_DELAY);
	spi_flash_disable();

	return recBuf[1];
}

uint32_t SPI_Flash_readPartID(void)
{
	uint8_t commandBuf[4] = { R_JEDEC_ID, 0x00, 0x00, 0x00 };
	uint8_t recBuf[4];

	spi_flash_enable();
	HAL_SPI_TransmitReceive(&HANDLE_SPI, commandBuf, recBuf, 4, HAL_MAX_DELAY);
	spi_flash_disable();

	return (recBuf[2] << 8) | recBuf[3];
}

bool SPI_Flash_writePage(uint32_t addr_start,uint8_t *dataBuf)
{
	bool isBusy;
	int waitCounter = 5;// Worst case is something like 3mS
	uint8_t commandBuf[4]= { PAGE_PGM, addr_start >> 16, addr_start >> 8, 0x00 } ;

	spi_flash_setWriteEnable(true);

	spi_flash_enable();

	HAL_SPI_Transmit(&HANDLE_SPI, commandBuf, 4, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&HANDLE_SPI, dataBuf, 0x100, HAL_MAX_DELAY);

	spi_flash_disable();

	do
	{
		osDelay(1);
		isBusy = spi_flash_busy();
	} while ((waitCounter-- > 0) && isBusy);

	return !isBusy;
}

// Returns true if erased and false if failed.
bool SPI_Flash_eraseSector(uint32_t addr_start)
{
	int waitCounter = 500;// erase can take up to 500 mS
	bool isBusy;
	uint8_t commandBuf[4] = { SECTOR_E, addr_start >> 16, addr_start >> 8, 0x00 };

	spi_flash_setWriteEnable(true); // it calls spi_flash_{enable/disable}() by itself

	spi_flash_enable();
	HAL_SPI_Transmit(&HANDLE_SPI, commandBuf, 4, HAL_MAX_DELAY);
	spi_flash_disable();

	do
	{
		osDelay(1);
		isBusy = spi_flash_busy();
	} while ((waitCounter-- > 0) && isBusy);

	return !isBusy;// If still busy after
}

static inline void spi_flash_enable(void)
{
	HAL_GPIO_WritePin(SPI_Flash_CS_GPIO_Port, SPI_Flash_CS_Pin, GPIO_PIN_RESET);
}

static void spi_flash_disable(void)
{
	HAL_GPIO_WritePin(SPI_Flash_CS_GPIO_Port, SPI_Flash_CS_Pin, GPIO_PIN_SET);
}

static bool spi_flash_busy(void)
{
	uint8_t r1;
	uint8_t cmdVal = R_SR1;

	spi_flash_enable();
	HAL_SPI_Transmit(&HANDLE_SPI, &cmdVal, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&HANDLE_SPI, &r1, 1, HAL_MAX_DELAY);
	spi_flash_disable();

	return (r1 & SR_BUSY);
}

static void spi_flash_setWriteEnable(bool cmd)
{
	uint8_t cmdValue = (cmd ? W_EN : W_DE);

	spi_flash_enable();
	HAL_SPI_Transmit(&HANDLE_SPI, &cmdValue, 1, HAL_MAX_DELAY);
	spi_flash_disable();
}

bool SPI_Flash_readSecurityRegisters(int startBlock, uint8_t *dataBuf, int size)
{
	const uint32_t addrs[] = { 0x1000, 0x2000, 0x3000 };
	const int securityBlockSize = 256;

	int numberofblocks = size / securityBlockSize;

	for (uint8_t i = startBlock; i < numberofblocks + 1; i++)
	{
		uint32_t addr = addrs[i];
		uint8_t commandBuf[5] = { R_SEC_REGS, ((addr >> 16) & 0xFF), ((addr >> 8) & 0xFF), (addr & 0xFF), 0x00 };

		spi_flash_enable();
		HAL_SPI_Transmit(&HANDLE_SPI, commandBuf, 5, HAL_MAX_DELAY);
		HAL_SPI_Receive(&HANDLE_SPI, dataBuf + ((i - startBlock) * securityBlockSize), securityBlockSize, HAL_MAX_DELAY);
		spi_flash_disable();
	}

	return true;
}

uint8_t SPI_Flash_readSingleSecurityRegister(int addr)
{
	  uint8_t value;
	  uint8_t commandBuf[5] = { R_SEC_REGS, ((addr >> 16) & 0xFF), ((addr >> 8) & 0xFF), (addr & 0xFF), 0x00 };

	  spi_flash_enable();
	  HAL_SPI_Transmit(&HANDLE_SPI, commandBuf, 5, HAL_MAX_DELAY);
	  HAL_SPI_Receive(&HANDLE_SPI, &value, 1, HAL_MAX_DELAY);
	  spi_flash_disable();

      return value;
}

#if 0
bool SPI_Flash_StateIsBusy(void)
{
	HAL_SPI_StateTypeDef state = HAL_SPI_GetState(&HANDLE_SPI);
	return ((state == HAL_SPI_STATE_BUSY) ||
			(state == HAL_SPI_STATE_BUSY_TX) ||
			(state == HAL_SPI_STATE_BUSY_RX) ||
			(state == HAL_SPI_STATE_BUSY_TX_RX));
}
#endif

