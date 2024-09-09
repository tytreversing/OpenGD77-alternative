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
#include "main.h"
#include <stdbool.h>
#include <interfaces/clockManager.h>
#include "interfaces/hr-c6000_spi.h"
#include "interfaces/i2c.h"
#include "usbd_def.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern volatile bool usbIsResetting;

volatile clockManagerSpeedSetting_t currentClockSpeedSetting = CLOCK_MANAGER_SPEED_UNDEF;

/*
static void clockManagerSetHSI(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}

  // -1- Select HSI as system clock source
  RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}
 */

static bool clockManagerSetHSE(bool isEco)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 72;
	RCC_OscInitStruct.PLL.PLLQ = 3;

	if (isEco)
	{
		RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV6;
		htim1.Init.Period = 4000;// change this to 4000 later to prevent display backlight flashing
	}
	else
	{
		RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
		htim1.Init.Period = 7100;
	}

	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		return false;
	}

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		return false;
	}

	//Initializes the CPU, AHB and APB buses clocks

	if (isEco)
	{
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
		{
			return false;
		}
	}
	else
	{
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
		{
			return false;
		}
	}

	return true;
}

clockManagerSpeedSetting_t clockManagerGetRunMode(void)
{
	return currentClockSpeedSetting;
}

void clockManagerUsbRequired(void)
{
	if (usbIsResetting && (clockManagerGetRunMode() != CLOCK_MANAGER_SPEED_RUN))
	{
		clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_SPEED_RUN);
	}
}

void clockManagerSetRunMode(uint8_t targetConfigIndex, clockManagerSpeedSetting_t clockSpeedSetting)
{
	if (currentClockSpeedSetting != clockSpeedSetting)
	{
		if ((hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) && (clockSpeedSetting == CLOCK_MANAGER_RUN_ECO_POWER_MODE))
		{
			return;
		}

		__HAL_RCC_SYSCLK_CONFIG(RCC_CFGR_SWS_HSI);//Switch clock to internal, as bootloader already set it to HSE and otherwise HSE can't be configured in SystemClock_Config()

		switch(clockSpeedSetting)
		{
			case CLOCK_MANAGER_RUN_ECO_POWER_MODE:
				clockManagerSetHSE(true);
				break;

			case CLOCK_MANAGER_SPEED_RUN:
			case CLOCK_MANAGER_SPEED_HS_RUN:
				clockManagerSetHSE(false);
			default:
				break;
		}

		currentClockSpeedSetting = clockSpeedSetting;
	}
}

