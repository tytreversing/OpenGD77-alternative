/*
 * Copyright (C) 2021-2024 Roger Clark, VK3KYY / G4KYF
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
#include "main.h"
#include "interfaces/batteryRAM.h"
#include "interfaces/stm32f4xx_ll_pwr.h"

const uint32_t RAM_SIZE = 0x1000;
/*
 * Note.
 * Do not optimise this to use memcpy, because it didn't seem to work
 */
bool batteryRAM_Read(uint32_t offset, uint8_t *buf, uint32_t size)
{
	if ((offset + size) <= RAM_SIZE)
	{
		for(uint32_t i = 0; i < size; i++)
		{
			*buf++ = *(__IO uint8_t *)(BKPSRAM_BASE + (offset + i));
		}

		return true;
	}

	return false;
}

/*
 * Note.
 * Do not optimise this to use memcpy, because it didn't seem to work
 */
bool batteryRAM_Write(uint32_t offset, uint8_t *buf, uint32_t size)
{
	if ((offset + size) <= RAM_SIZE)
	{
		for(uint32_t i = 0; i < size; i++)
		{
			*(__IO uint8_t *)(BKPSRAM_BASE + (offset + i)) = *buf++;
		}

		return true;
	}

	return false;
}

void batteryRAM_Init(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_BKPSRAM_CLK_ENABLE();

	LL_PWR_EnableBkUpRegulator();
	while (!LL_PWR_IsActiveFlag_BRR());
}
