/*
 * Copyright (C) 2020-2024 Roger Clark, VK3KYY / G4KYF
 *
 * Using some code from NXP examples
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

#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Power mode definition used in application. */
typedef enum _app_power_mode
{
    kAPP_PowerModeMin = 'A' - 1,
    kAPP_PowerModeVlpr,  /*!< Very low power run mode. All Kinetis chips. */
    kAPP_PowerModeRun,   /*!< Run mode. All Kinetis chips. */
    kAPP_PowerModeHsrun /*!< High-speed run mode. Chip-specific. */
} app_power_mode_t;

/*!
 * @brief Power mode user configuration structure.
 *
 * This structure defines Kinetis power mode with additional power options and specifies
 * transition to and out of this mode. Application may define multiple power modes and
 * switch between them.
 * List of power mode configuration structure members depends on power options available
 * for specific chip. Complete list contains:
 *  mode - Kinetis power mode. List of available modes is chip-specific. See power_manager_modes_t
 *   list of modes. This item is common for all Kinetis chips.
 *  enableLowPowerWakeUpOnInterrupt - When set to true, system exits to Run mode when any interrupt occurs while in
 *   Very low power run, Very low power wait or Very low power stop mode. This item is chip-specific.
 *  enablePowerOnResetDetection - When set to true, power on reset detection circuit is disabled in
 *   Very low leakage stop 0 mode. When set to false, circuit is enabled. This item is chip-specific.
 *  enableRAM2Powered - When set to true, RAM2 partition content is retained through Very low
 *   leakage stop 2 mode. When set to false, RAM2 partition power is disabled and memory content lost.
 *   This item is chip-specific.
 *  enableLowPowerOscillator - When set to true, the 1 kHz Low power oscillator is disabled in any
 *   Low leakage or Very low leakage stop mode. When set to false, oscillator is enabled in these modes.
 *   This item is chip-specific.
 */


extern app_power_mode_t clockManagerCurrentRunMode;


/* Value of the enum is critical, and controls the clock Multipler and divider
 * External clock frequency to the MCU is 12.288Mhz
 *
 * Clock speed calculation is (Lower bye + 24) / (upper byte + 1) * external clock
 *
 * So 0x0603 = (3 + 24) / ( 6 + 1 ) * 12.288 =  47.396Mhz
 *
 * See data sheet for max clock rates in both Run and HS Run modes.
 * */
typedef enum
{
	CLOCK_MANAGER_SPEED_UNDEF         = 0x0000,
	CLOCK_MANAGER_SPEED_RUN           = 0x0603,
	CLOCK_MANAGER_SPEED_HS_RUN        = 0x0205,
	CLOCK_MANAGER_RUN_SUSPEND_MODE    = 0x1F00,
	CLOCK_MANAGER_RUN_ECO_POWER_MODE  = 0x1F00,
} clockManagerSpeedSetting_t;

void clockManagerInit(void);
void clockManagerSetRunMode(uint8_t targetConfigIndex, clockManagerSpeedSetting_t clockSpeedSetting);
clockManagerSpeedSetting_t clockManagerGetRunMode(void);

#endif /* _POWER_MANAGER_H_ */
