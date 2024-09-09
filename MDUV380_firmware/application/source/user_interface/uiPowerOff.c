/*
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
#include "user_interface/uiGlobals.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiLocalisation.h"
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#include "interfaces/batteryAndPowerManagement.h"
#include "hardware/radioHardwareInterface.h"
#endif

static void updateScreen(void);
static void handleEvent(uiEvent_t *ev);
static uint32_t initialEventTime;
const uint32_t POWEROFF_DURATION_MILLISECONDS = 500;

menuStatus_t uiPowerOff(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		updateScreen();
		initialEventTime = ev->time;
	}
	else
	{
		handleEvent(ev);
	}
	return MENU_STATUS_SUCCESS;
}

static void updateScreen(void)
{
	displayClearBuf();
	displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG_NOTIFICATION);
	displayDrawRoundRectWithDropShadow(4, 4, 120 + DISPLAY_H_EXTRA_PIXELS, DISPLAY_SIZE_Y - 6, 5, true);

	displayThemeApply(THEME_ITEM_FG_WARNING_NOTIFICATION, THEME_ITEM_BG_NOTIFICATION);
	displayPrintCentered(((DISPLAY_SIZE_Y / 3) - (FONT_SIZE_3_HEIGHT / 2)), currentLanguage->power_off, FONT_SIZE_3);
	displayThemeResetToDefault();
	displayRender();
}

static void handleEvent(uiEvent_t *ev)
{
	if (
#if defined(PLATFORM_MD9600)
			(LedRead(LED_GREEN) == 0) &&
#elif defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			powerRotarySwitchIsOn() &&
#elif defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) // not for RD5R
			(GPIO_PinRead(GPIO_Power_Switch, Pin_Power_Switch) == 0) &&
#endif
			(batteryVoltage > CUTOFF_VOLTAGE_LOWER_HYST)
	)
	{
		// I think this is to handle if the power button is turned back on during shutdown
		menuSystemPopPreviousMenu();
		initialEventTime = 0; // Reset timeout
		return;
	}

	if ((ev->time - initialEventTime) > POWEROFF_DURATION_MILLISECONDS)
	{
		powerOffFinalStage(false, false);
	}

}
