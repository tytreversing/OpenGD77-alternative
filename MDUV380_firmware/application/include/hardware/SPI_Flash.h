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

#ifndef _OPENGD77_SPI_FLASH_H_
#define _OPENGD77_SPI_FLASH_H_

#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>

// uint32 status bits returned by SPI_Flash_readStatusRegisters()
#define SR_BUSY         0x00000001 // Erase/Write in Progress        // S0
#define SR_WEL          0x00000002 // Write Enable Latch             // S1
#define SR_BP0          0x00000004 // Block Protect Bit 1            // S2
#define SR_BP1          0x00000008 // Block Protect Bit 2            // S3
#define SR_BP2          0x00000010 // Block Protect Bit 3            // S4
#define SR_TB           0x00000020 // Top/Bottom Protect Bit         // S5
#define SR_SEC          0x00000040 // Sector Protect Bit             // S6
#define SR_SRP0         0x00000080 // Status Register Protect 0      // S7
#define SR_SRP1         0x00000100 // Status Register Protect 1      // S8
#define SR_QE           0x00000200 // Quad Enable                    // S9
#define SR_RESERVED_1   0x00000400 // Reserved                       // S10
#define SR_LB1          0x00000800 // Security Register Lock Bit 1   // S11
#define SR_LB2          0x00001000 // Security Register Lock Bit 2   // S12
#define SR_LB3          0x00002000 // Security Register Lock Bit 3   // S13
#define SR_CMP          0x00004000 // Complement Protect             // S14
#define SR_SUS          0x00008000 // Suspend Status                 // S15
#define SR_RESERVED_2   0x00010000 // Reserved                       // S16
#define SR_RESERVED_3   0x00020000 // Reserved                       // S17
#define SR_WPS          0x00040000 // Write Protect Selection        // S18
#define SR_RESERVED_4   0x00080000 // Reserved                       // S19
#define SR_RESERVED_5   0x00100000 // Reserved                       // S20
#define SR_DRV0         0x00200000 // Output Driver Strength 0       // S21
#define SR_DRV1         0x00400000 // Output Driver Strength 1       // S22
#define SR_HOLD_RST     0x00800000 // /HOLD or /RESET Function       // S23

extern uint8_t SPI_Flash_sectorbuffer[4096];
extern uint32_t flashChipPartNumber;

// Public functions
bool SPI_Flash_init(void);
bool SPI_Flash_read(uint32_t addrress,uint8_t *buf,int size);
bool SPI_Flash_write(uint32_t addr, uint8_t *dataBuf, int size);
bool SPI_Flash_writePage(uint32_t address,uint8_t *dataBuf);// page is 256 bytes
bool SPI_Flash_eraseSector(uint32_t address);// sector is 16 pages  = 4k bytes
uint8_t SPI_Flash_readManufacturer(void);// Not necessarily Winbond !
uint32_t SPI_Flash_readPartID(void);// Should be 4014 for 1M or 4017 for 8M
uint32_t SPI_Flash_readStatusRegisters(void);// May come in handy
bool SPI_Flash_readSecurityRegisters(int startBlock, uint8_t *dataBuf, int size);   // Used to read the calibration Data. Must read in blocks of 256 bytes. Blocks 0,1 or 2
uint8_t SPI_Flash_readSingleSecurityRegister(int addr);                                 // Used to read a single security register. Used for Display Type
//bool SPI_Flash_StateIsBusy(void);

#endif /* _OPENGD77_SPI_FLASH_H_ */
