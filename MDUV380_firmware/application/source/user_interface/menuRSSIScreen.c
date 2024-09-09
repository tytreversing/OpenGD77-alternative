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
#include "functions/calibration.h"
#include "functions/trx.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiUtilities.h"
#include "user_interface/uiLocalisation.h"
#include "hardware/radioHardwareInterface.h"


#if defined(PLATFORM_MD2017)
#define SECONDARY_DISPLAY_OFFSET  50
#endif


static menuStatus_t menuRSSIExitCode = MENU_STATUS_SUCCESS;
//static calibrationRSSIMeter_t rssiCalibration; // UNUSED
static void updateScreen(bool forceRedraw, bool firstRun);
static void handleEvent(uiEvent_t *ev);
static void updateVoicePrompts(bool flushIt, bool spellIt);

static int dBm[RADIO_DEVICE_MAX] =
{
		0
#if defined(PLATFORM_MD2017)
		, 0
#endif
};

static uint8_t rawSignal[RADIO_DEVICE_MAX] =
{
		0
#if defined(PLATFORM_MD2017)
		, 0
#endif
};

static uint8_t rawNoise[RADIO_DEVICE_MAX] =
{
		0
#if defined(PLATFORM_MD2017)
		, 0
#endif
};

static bool displayRawValues = false;

static const int barX = 9;
DECLARE_SMETER_ARRAY(rssiMeterBar, (DISPLAY_SIZE_X - (barX - 1)));

menuStatus_t menuRSSIScreen(uiEvent_t *ev, bool isFirstRun)
{
	static uint32_t m = 0;

	if (isFirstRun)
	{
		//calibrationGetRSSIMeterParams(&rssiCalibration); // UNUSED
		menuDataGlobal.numItems = 0;
		displayClearBuf();
		menuDisplayTitle(currentLanguage->rssi);
		displayRenderRows(0, 2);

		updateScreen(true, true);
	}
	else
	{
		menuRSSIExitCode = MENU_STATUS_SUCCESS;
		if (ev->hasEvent)
		{
			handleEvent(ev);
		}

		if((ev->time - m) > RSSI_UPDATE_COUNTER_RELOAD)
		{
			m = ev->time;
			updateScreen(false, false);
		}
	}

	return menuRSSIExitCode;
}

// Returns S-Unit 0..9..10(S9+10dB)..15(S9+60)
static int32_t getSignalStrength(int dbm)
{
	for (int8_t i = 15; i >= 0; i--)
	{
		if (dbm >= DBM_LEVELS[i])
		{
			return i;
		}
	}

	return 0;
}

static void drawMeterGraticule(int16_t vOffset)
{
	// Draw S-Meter outer frame
	displayDrawRect((barX - 2), (vOffset - 2), (DISPLAY_SIZE_X - (barX - 2)), (8 + 4), true);
	// Clear the right V line of the frame
	displayDrawFastVLine((DISPLAY_SIZE_X - 1), (vOffset - 1), (8 + 2), false);
	// S9+xx H Dots
	for (int16_t i = ((barX - 2) + (rssiMeterBar[9] * 2) + 1); i < DISPLAY_SIZE_X; i += 4)
	{
		displayDrawFastHLine(i, (vOffset - 2), 2, false);
	}
	// +10..60dB
	displayFillRect(((barX - 2) + (rssiMeterBar[9] * 2) + 2), (vOffset + 8) + 2,
			(DISPLAY_SIZE_X - ((barX - 2) + (rssiMeterBar[9] * 2) + 2)), 2, false);

	// Draw S, Numbers and ticks
	displayPrintAt(1, vOffset, "S", FONT_SIZE_1_BOLD);

	int xPos;
	int currentMode = trxGetMode();

	for (uint8_t i = 0; i < 10; i++)
	{
		// Scale the bar graph so values S0 - S9 take 70% of the scale width, and signals above S9 take the last 30%
		// On DMR the max signal is S9+10, so teh entire bar can be the sale scale
		// ON FM signals above S9, the scale is compressed to 2/STRONG_SIGNAL_RESCALE
		if ((i <= 9) || (currentMode == RADIO_MODE_DIGITAL))
		{
			xPos = rssiMeterBar[i];
		}
		else
		{
			xPos = ((rssiMeterBar[i] - rssiMeterBar[9]) / STRONG_SIGNAL_RESCALE) + rssiMeterBar[9];
		}
		xPos *= 2;

		// V ticks
		displayDrawFastVLine(((barX - 2) + xPos), (vOffset + 8) + 2, ((i % 2) ? 3 : 1), ((i < 10) ? true : false));

		if ((i % 2) && (i < 10))
		{
			char buf[2];

			sprintf(buf, "%d", i);
			displayPrintAt(((((barX - 2) + xPos) - 2) - 1)/* FONT_2 H offset */, vOffset + 15
#if defined(PLATFORM_RD5R)
					-1
#endif
					, buf, FONT_SIZE_2);
		}
	}
}

static void updateScreen(bool forceRedraw, bool isFirstRun)
{
	char buffer[LOCATION_TEXT_BUFFER_SIZE];
	int barWidth;
	int rssi[RADIO_DEVICE_MAX];
	int16_t yValuePos, yBarPos;

	for(RadioDevice_t device = RADIO_DEVICE_PRIMARY; device < RADIO_DEVICE_MAX; device++)
	{
		rssi[device] = dBm[device] = trxGetRSSIdBm(device);
		rawSignal[device] = trxGetSignalRaw(device);
		rawNoise[device] = trxGetNoiseRaw(device);
	}

	if (isFirstRun && (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD))
	{
		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->rssi);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		updateVoicePrompts(false, true);
	}

	if (forceRedraw)
	{
		yBarPos = DISPLAY_Y_POS_RSSI_BAR;

		displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);
		// Clear whole drawing region
		displayFillRect(0, 14, DISPLAY_SIZE_X, DISPLAY_SIZE_Y - 14, true);

		for(RadioDevice_t device = RADIO_DEVICE_PRIMARY; device < RADIO_DEVICE_MAX; device++)
		{
			drawMeterGraticule(yBarPos);
#if defined(PLATFORM_MD2017)
			yBarPos += SECONDARY_DISPLAY_OFFSET;
#endif
		}

		displayThemeResetToDefault();
	}
	else
	{
		yValuePos = DISPLAY_Y_POS_RSSI_VALUE;

		for(RadioDevice_t device = RADIO_DEVICE_PRIMARY; device < RADIO_DEVICE_MAX; device++)
		{
			// Clear dBm region value
			displayFillRect((displayRawValues ? 0 : ((DISPLAY_SIZE_X - (7 * 8)) >> 1)), yValuePos, (displayRawValues ? DISPLAY_SIZE_X : (7 * 8)), FONT_SIZE_3_HEIGHT, true);
#if defined(PLATFORM_MD2017)
			yValuePos += SECONDARY_DISPLAY_OFFSET;
#endif
		}
	}

	yValuePos = DISPLAY_Y_POS_RSSI_VALUE;
	yBarPos = DISPLAY_Y_POS_RSSI_BAR;

	for(RadioDevice_t device = RADIO_DEVICE_PRIMARY; device < RADIO_DEVICE_MAX; device++)
	{
		if (displayRawValues)
		{
			snprintf(buffer, LOCATION_TEXT_BUFFER_SIZE, "%d%s [%u %u]", dBm[device], "dBm", rawSignal[device], rawNoise[device]);
		}
		else
		{
			snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%d%s", dBm[device], "dBm");
		}
		displayPrintCentered(yValuePos, buffer, FONT_SIZE_3);

#if 0 // DEBUG
		sprintf(buffer, "%d", currentRadioDevice->trxRxSignal);
		displayFillRect((DISPLAY_SIZE_X - (4 * 8)), yValuePos, (4 * 8), 8, true);
		ucPrintCore((DISPLAY_SIZE_X - ((strlen(buffer) + 1) * 8)), yValuePos, buffer, FONT_SIZE_2, TEXT_ALIGN_RIGHT, false);
#endif

		if ((rssi[device] > SMETER_S9) && (trxGetMode() == RADIO_MODE_ANALOG))
		{
			// In Analog mode, the max RSSI value from the hardware is over S9+60.
			// So scale this to fit in the last 30% of the display
			rssi[device] = ((rssi[device] - SMETER_S9) / STRONG_SIGNAL_RESCALE) + SMETER_S9;
		}
		// Scale the entire bar by 2.
		// Because above S9 the values are scaled to 1/5.
		// This can result in the signal below S9 being doubled in scale (depending on STRONG_SIGNAL_RESCALE)
		// Signals above S9 the scales is compressed to 2/STRONG_SIGNAL_RESCALE.
		rssi[device] = (rssi[device] - SMETER_S0) * 2;

		barWidth = ((rssi[device] * rssiMeterBarNumUnits) / rssiMeterBarDivider);
		barWidth = CLAMP((barWidth - 1), 0, (DISPLAY_SIZE_X - barX));

		if (barWidth)
		{
			displayThemeApply(THEME_ITEM_FG_RSSI_BAR, THEME_ITEM_BG);
			displayFillRect(barX, yBarPos, barWidth, 8, false);
			displayThemeResetToDefault();
		}

		// Clear the end of the bar area, if needed
		if (barWidth < (DISPLAY_SIZE_X - barX))
		{
			displayFillRect(barX + barWidth, yBarPos, (DISPLAY_SIZE_X - barX) - barWidth, 8, true);
		}

#if defined(HAS_COLOURS)
		if (rssi[device] > SMETER_S9)
		{
			int xPos;

			xPos = (rssiMeterBar[9] * 2);

			if (barWidth > xPos)
			{
				displayThemeApply(THEME_ITEM_FG_RSSI_BAR_S9P, THEME_ITEM_BG);
				displayFillRect((barX + xPos), yBarPos, (barWidth - xPos), 8, false);
				displayThemeResetToDefault();
			}
		}
#endif

#if defined(PLATFORM_MD2017)
		yValuePos += SECONDARY_DISPLAY_OFFSET;
		yBarPos += SECONDARY_DISPLAY_OFFSET;
#endif
	}

	if (forceRedraw || uiNotificationIsVisible())
	{
		displayRender();
	}
	else
	{
		int16_t yStartValuePos = DISPLAY_Y_POS_RSSI_VALUE;
		int16_t yStartBarPos = DISPLAY_Y_POS_RSSI_BAR;

		// Y end positions
		yValuePos = DISPLAY_Y_POS_RSSI_VALUE + FONT_SIZE_3_HEIGHT;
		yBarPos = DISPLAY_Y_POS_RSSI_BAR + 8;

#if defined(PLATFORM_RD5R)
#warning CHECK ME (end pos)
		displayRenderRows((DISPLAY_Y_POS_RSSI_VALUE / 8), (yValuePos / 8) + 1);
#else

		for(RadioDevice_t device = RADIO_DEVICE_PRIMARY; device < RADIO_DEVICE_MAX; device++)
		{
			displayRenderRows((yStartValuePos / 8), (yValuePos / 8) + 1);
			displayRenderRows((yStartBarPos / 8), (yBarPos / 8) + 1);

#if defined(PLATFORM_MD2017)
			yStartValuePos += SECONDARY_DISPLAY_OFFSET;
			yStartBarPos += SECONDARY_DISPLAY_OFFSET;
			yValuePos += SECONDARY_DISPLAY_OFFSET;
			yBarPos += SECONDARY_DISPLAY_OFFSET;
#endif
		}
#endif
	}
}

static void handleEvent(uiEvent_t *ev)
{
	if ((ev->events & FUNCTION_EVENT) && (ev->function == FUNC_REDRAW))
	{
		updateScreen(true, false);
		return;
	}

	if (ev->events & BUTTON_EVENT)
	{
		bool wasPlaying = false;

		if (BUTTONCHECK_SHORTUP(ev, BUTTON_SK1) && (ev->keys.key == 0))
		{
			// Stop playback or update signal strength
			if ((wasPlaying = voicePromptsIsPlaying()) == false)
			{
				updateVoicePrompts(true, false);
			}
		}

		if (repeatVoicePromptOnSK1(ev))
		{
			if (wasPlaying && voicePromptsIsPlaying())
			{
				voicePromptsTerminate();
			}
			return;
		}
	}

	if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN) || KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		menuSystemPopPreviousMenu();
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_STAR))
	{
		displayRawValues = !displayRawValues;
		updateScreen(true, false);
	}
	else if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2)))
	{
		saveQuickkeyMenuIndex(ev->keys.key, menuSystemGetCurrentMenuNumber(), 0, 0);
	}
}

static void updateVoicePrompts(bool flushIt, bool spellIt)
{
	if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
	{
		uint8_t S = getSignalStrength(dBm[RADIO_DEVICE_PRIMARY]);

		if (flushIt)
		{
			voicePromptsInit();
		}

		voicePromptsAppendPrompt(PROMPT_S);
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		if (S > 9)
		{
			voicePromptsAppendPrompt(PROMPT_9);
			voicePromptsAppendPrompt(PROMPT_PLUS);
			voicePromptsAppendInteger(10 * ((S - 10) + 1));
		}
		else
		{
			voicePromptsAppendPrompt(PROMPT_0 + S);
		}

		if (spellIt)
		{
			promptsPlayNotAfterTx();
		}
	}
}
