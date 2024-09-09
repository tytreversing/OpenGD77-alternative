/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
 * Copyright (C) 2019-2024 Roger Clark, VK3KYY / G4KYF
 *                         Daniel Caujolle-Bert, F1RMB
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
#include "interfaces/gpio.h"
#include "io/LEDs.h"

#if ! defined(PLATFORM_GD77S)
uint8_t LEDsState[NUM_LEDS] = { 0, 0 };
#endif


void LEDsInit(void)
{
#if defined(PLATFORM_MD9600) || defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
	gpioInitLEDs();
#endif

	LedWrite(LED_GREEN, 0);
	LedWrite(LED_RED, 0);

#if defined(PLATFORM_RD5R)
	GPIO_PinWrite(GPIO_Torch, Pin_Torch, 0);
#endif
}

void LedWrite(LEDs_t theLED, uint8_t output)
{
#if ! defined(PLATFORM_GD77S)
	LEDsState[theLED] = output;

	if (settingsIsOptionBitSet(BIT_ALL_LEDS_DISABLED) == 0)
#endif
	{
		LedWriteDirect(theLED, output);
	}
}

uint8_t LedRead(LEDs_t theLED)
{
#if defined(PLATFORM_GD77S)
	return GPIO_PinRead(((theLED == LED_GREEN) ? GPIO_LEDgreen : GPIO_LEDred), ((theLED == LED_GREEN) ? Pin_LEDgreen : Pin_LEDred));
#else
	return LEDsState[theLED];
#endif
}

void LedWriteDirect(LEDs_t theLED, uint8_t output)
{
#if defined(PLATFORM_GD77S)
	GPIO_PinWrite(((theLED == LED_GREEN) ? GPIO_LEDgreen : GPIO_LEDred), ((theLED == LED_GREEN) ? Pin_LEDgreen : Pin_LEDred), output);
#else
	LEDsState[theLED] = output;

#if ! defined(PLATFORM_MD9600)
	if (theLED == LED_GREEN)
	{
#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
		GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, output);
#else
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, output);
#endif
	}
	else
	{
#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
		GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, output);
#else
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, output);
#endif
	}
#endif // ! MD9600
#endif // ! GD77S
}

#if defined(PLATFORM_RD5R)
// Baofeng DM-5R torch LED
static bool torchState = false;

void torchToggle(void)
{
	torchState = !torchState;
	GPIO_PinWrite(GPIO_Torch, Pin_Torch, torchState);
}
#endif
