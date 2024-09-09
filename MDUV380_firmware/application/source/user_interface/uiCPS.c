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
#include "functions/trx.h"
#include "user_interface/menuSystem.h"
#include "functions/rxPowerSaving.h"
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#include "hardware/radioHardwareInterface.h"
#endif

static void handleTick(void);

typedef enum
{
	CPS_LED_NONE,
	CPS_LED_RED,
	CPS_LED_GREEN
} blinkLed_t;

#define PIT_COUNTS_PER_UPDATE  500

static blinkLed_t ledType = CPS_LED_NONE;
static ticksTimer_t nextUpdateTimer;
static bool ledState = false;
static int radioMode;
static bool radioBandWidth;


menuStatus_t uiCPS(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.numItems = 0;
		rxPowerSavingSetState(ECOPHASE_POWERSAVE_INACTIVE);
		radioMode = trxGetMode();
		radioBandWidth = trxGetBandwidthIs25kHz();
		trxSetModeAndBandwidth(RADIO_MODE_NONE, radioBandWidth);
		ticksTimerStart(&nextUpdateTimer, PIT_COUNTS_PER_UPDATE);
	}
	else
	{
		if (ticksTimerHasExpired(&nextUpdateTimer))
		{
			ticksTimerStart(&nextUpdateTimer, PIT_COUNTS_PER_UPDATE);
			handleTick();
		}
	}

	return MENU_STATUS_SUCCESS;
}

void uiCPSUpdate(uiCPSCommand_t command, int x, int y, ucFont_t fontSize, ucTextAlign_t alignment, bool isInverted, char *szMsg)
{
	switch(command)
	{
		case CPS2UI_COMMAND_CLEARBUF:
			displayClearBuf();
			break;
		case CPS2UI_COMMAND_PRINT:
			displayPrintCore(x, y, szMsg, fontSize, alignment, isInverted);
			break;
		case CPS2UI_COMMAND_RENDER_DISPLAY:
			displayRender();
			displayLightTrigger(true);
			break;
		case CPS2UI_COMMAND_BACKLIGHT:
			displayLightTrigger(true);
			break;
		case CPS2UI_COMMAND_GREEN_LED:
			ledType = CPS_LED_GREEN;// flash green LED
			break;
		case CPS2UI_COMMAND_RED_LED:
			ledType = CPS_LED_RED;// flash red LED
			break;
		case CPS2UI_COMMAND_END:
		    LedWrite(LED_GREEN, 0);
		    LedWrite(LED_RED, 0);
		    ledState = false;
		    ledType = CPS_LED_NONE;
		    trxSetRX();// Rx would be turned off at start of CPS by setting radio mode to none
		    trxSetModeAndBandwidth(radioMode, radioBandWidth);
		    menuSystemPopPreviousMenu();
			break;
		default:
			break;
	}
}

static void handleTick(void)
{
	switch(ledType)
	{
		case CPS_LED_GREEN:
			ledState = !ledState;
			LedWrite(LED_GREEN, (ledState ? 1 : 0));
			break;

		case CPS_LED_RED:
			ledState = !ledState;
			LedWrite(LED_RED, (ledState ? 1 : 0));
			break;

		case CPS_LED_NONE:
		default:
			break;
	}
}
