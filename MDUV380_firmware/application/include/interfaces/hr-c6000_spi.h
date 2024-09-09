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

#ifndef _OPENGD77_SPI_H_
#define _OPENGD77_SPI_H_

#include <FreeRTOS.h>
#include <task.h>

enum _generic_status
{
    kStatus_Success = 0,
    kStatus_Fail,
    kStatus_ReadOnly,
    kStatus_OutOfRange,
    kStatus_InvalidArgument,
    kStatus_Timeout,
    kStatus_NoTransferInProgress,
};


void SPIInit(void);
void SPI0Read(uint8_t *txBuf,uint8_t *rxBuf,uint8_t length);
void SPI0Write(uint8_t *txBuf,uint8_t length);
int SPI0WritePageRegByte(uint8_t page, uint8_t reg, uint8_t val);
int SPI0WritePageRegByteExtended(uint8_t page, uint16_t reg, uint8_t val);
int SPI0ReadPageRegByte(uint8_t page, uint8_t reg, volatile uint8_t *val);
int SPI0ClearPageRegByteWithMask(uint8_t page, uint8_t reg, uint8_t mask, uint8_t val);
int SPI0WritePageRegByteArray(uint8_t page, uint8_t reg, const uint8_t *values, uint8_t length);
int SPI0ReadPageRegByteArray(uint8_t page, uint8_t reg, volatile uint8_t *values, uint8_t length);

int SPI1WritePageRegByteArray(uint8_t page, uint8_t reg, const uint8_t *values, uint8_t length);
int SPI1ReadPageRegByteArray(uint8_t page, uint8_t reg, volatile uint8_t *values, uint8_t length);

void SPI0Setup(void);
void SPI1Setup(void);

#endif /* _OPENGD77_SPI_H_ */
