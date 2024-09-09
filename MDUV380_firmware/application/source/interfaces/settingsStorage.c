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
#include "main.h"
#include "interfaces/batteryRAM.h"
#include "interfaces/settingsStorage.h"

#define STORAGE_BASE_ADDRESS         (0x6000 + 0x4B /* After "Last Used Channel In Zone" */)
#define USE_PERMANENT_STORAGE
#if defined(STM32F405xx)
#define SETTINGS_START_ADDRESS        0x02
#endif

bool settingsStorageRead(uint8_t *buf, uint32_t size)
{
#if !defined(STM32F405xx) || defined(USE_PERMANENT_STORAGE)
	return EEPROM_Read(STORAGE_BASE_ADDRESS, buf, size);
#else
	return batteryRAM_Read(SETTINGS_START_ADDRESS,buf,size);
#endif
}

bool settingsStorageWrite(uint8_t *buf, uint32_t size)
{
#if !defined(STM32F405xx) || defined(USE_PERMANENT_STORAGE)
	return EEPROM_Write(STORAGE_BASE_ADDRESS, buf, size);
#else
	return batteryRAM_Write(SETTINGS_START_ADDRESS,buf,size);
#endif
}
