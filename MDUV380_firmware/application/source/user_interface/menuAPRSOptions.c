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
#if !defined(PLATFORM_GD77S)

#include "main.h"
#if defined(PLATFORM_MD9600)
#include "hardware/ST7567.h"
#elif (defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017))
#include "hardware/HX8353E.h"
#else
#include "hardware/UC1701.h"
#endif
#include "functions/aprs.h"
#include "functions/settings.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiLocalisation.h"
#include "user_interface/uiUtilities.h"

extern uint16_t initialIntervalsInSecs[];

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);
static void applySettings(void);
static void exitCallback(void *data);

// See https://thewakesileave.wordpress.com/2017/06/14/rediscovering-the-smartbeaconing-parameters/
#define SMART_BEACONING_PRESETS_MAX  7U
static const char *sbPresetsStr[] = { "Default", "Car", "e-Bike", "Bike", "Walking", "Sailing", "APRSdroid" };
static aprsSmartBeaconingSettings_t smartPresets[SMART_BEACONING_PRESETS_MAX] =
{
		{ // default
				.slowRate  = APRS_SMART_BEACON_SLOW_RATE_DEFAULT,
				.fastRate  = APRS_SMART_BEACON_FAST_RATE_DEFAULT,
				.lowSpeed  = APRS_SMART_BEACON_LOW_SPEED_DEFAULT,
				.highSpeed = APRS_SMART_BEACON_HIGH_SPEED_DEFAULT,
				.turnAngle = APRS_SMART_BEACON_TURN_ANGLE_DEFAULT,
				.turnSlope = APRS_SMART_BEACON_TURN_SLOPE_DEFAULT,
				.turnTime  = APRS_SMART_BEACON_TURN_TIME_DEFAULT,
		},
		{ // car
				.slowRate  = 30U,  // min
				.fastRate  = 180U, // s
				.lowSpeed  = 7U,   // kmph
				.highSpeed = 90U,  // kmph (max)
				.turnAngle = 30U,  // degree
				.turnSlope = 25U,  // * (10degree/speed)
				.turnTime  = 15U,  // s
		},
		{ // e-bike
				.slowRate  = 2U,   // min
				.fastRate  = 70U,  // s
				.lowSpeed  = 7U,   // kmph
				.highSpeed = 23U,  // kmph
				.turnAngle = 20U,  // degree
				.turnSlope = 24U,  // * (10degree/speed)
				.turnTime  = 8U,   // s
		},
		{ // bike
				.slowRate  = 2U,   // min
				.fastRate  = 60U,  // s
				.lowSpeed  = 5U,   // kmph
				.highSpeed = 14U,  // kmph
				.turnAngle = 20U,  // degree
				.turnSlope = 24U,  // * (10degree/speed)
				.turnTime  = 8U,   // s
		},
		{ // walking
				.slowRate  = 2U,   // min
				.fastRate  = 20U,  // s
				.lowSpeed  = 2U,   // kmph
				.highSpeed = 4U,   // kmph
				.turnAngle = 20U,  // degree
				.turnSlope = 24U,  // * (10degree/speed)
				.turnTime  = 5U,   // s
		},
		{ // sailing
				.slowRate  = 3U,   // min
				.fastRate  = 90U,  // s
				.lowSpeed  = 2U,   // kmph
				.highSpeed = 27U,  // kmph
				.turnAngle = 45U,  // degree
				.turnSlope = 25U,  // * (10degree/speed)
				.turnTime  = 15U,  // s
		},
		{ // APRSdroid
				.slowRate  = 20U,  // min
				.fastRate  = 60U,  // s
				.lowSpeed  = 5U,   // kmph
				.highSpeed = 90U,  // kmph (APRSdroid is set to 100km/h, out of bounds here)
				.turnAngle = 10U,  // degree
				.turnSlope = 24U,  // * (10degree/speed)
				.turnTime  = 15U,  // s
		}
};

static menuStatus_t menuAPRSExitCode = MENU_STATUS_SUCCESS;

enum
{
	OPTIONS_MENU_APRS_MODE = 0,
	OPTIONS_MENU_APRS_LOCATION,
	OPTIONS_MENU_APRS_INITIAL_INTERVAL,
	OPTIONS_MENU_APRS_DECAY,
	OPTIONS_MENU_APRS_COMPRESSED,
#if defined(RATE_MESSAGE_FEATURE)
	OPTIONS_MENU_APRS_MESSAGE_INTERVAL,
#endif
	OPTIONS_MENU_APRS_SLOW_RATE,
	OPTIONS_MENU_APRS_FAST_RATE,
	OPTIONS_MENU_APRS_LOW_SPEED,
	OPTIONS_MENU_APRS_HIGH_SPEED,
	OPTIONS_MENU_APRS_TURN_ANGLE,
	OPTIONS_MENU_APRS_TURN_SLOPE,
	OPTIONS_MENU_APRS_TURN_TIME,
	NUM_APRS_MENU_ITEMS
};

static uint8_t getIncValue(uiEvent_t *ev, uint8_t value, uint8_t mStep, uint8_t max)
{
	int16_t v = value;

	return (uint8_t) SAFE_MIN(((int16_t)max), (BUTTONCHECK_DOWN(ev, BUTTON_SK2) ? (v + mStep) : (v + 1)));
}

static uint8_t getDecValue(uiEvent_t *ev, uint8_t value, uint8_t mStep, uint8_t min)
{
	int16_t v = value;

	return (uint8_t) SAFE_MAX(((int16_t)min), (BUTTONCHECK_DOWN(ev, BUTTON_SK2) ? (v - mStep) : (v - 1)));
}

menuStatus_t menuAPRSOptions(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.menuOptionsSetQuickkey = 0;
		menuDataGlobal.menuOptionsTimeout = 0;
		menuDataGlobal.newOptionSelected = true;
		menuDataGlobal.numItems = NUM_APRS_MENU_ITEMS;

		if (*((uint32_t *)&aprsSettingsCopy.smart.slowRate) == 0xDEADBEEF)
		{
			// Store original APRS settings, used on cancel event.
			aprsBeaconingGetSettings(&aprsSettingsCopy);
		}

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->aprs_options);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		menuSystemRegisterExitCallback(exitCallback, NULL);

		updateScreen(isFirstRun);
		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuAPRSExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent || (menuDataGlobal.menuOptionsTimeout > 0))
		{
			handleEvent(ev);
		}
	}
	return menuAPRSExitCode;
}

static void updateScreen(bool isFirstRun)
{
	int mNum = 0;
	char buf[SCREEN_LINE_BUFFER_SIZE];
	const char *leftSide = NULL; // initialise to please the compiler
	const char *rightSideConst = NULL; // initialise to please the compiler
	char rightSideVar[SCREEN_LINE_BUFFER_SIZE];
	voicePrompt_t rightSideUnitsPrompt;
	const char *rightSideUnitsStr;

	displayClearBuf();
	bool settingOption = uiQuickKeysShowChoices(buf, SCREEN_LINE_BUFFER_SIZE, currentLanguage->aprs_options);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		if ((settingOption == false) || (i == 0))
		{
			mNum = menuGetMenuOffset(NUM_APRS_MENU_ITEMS, i);
			if (mNum == MENU_OFFSET_BEFORE_FIRST_ENTRY)
			{
				continue;
			}
			else if (mNum == MENU_OFFSET_AFTER_LAST_ENTRY)
			{
				break;
			}

			buf[0] = 0;
			leftSide = NULL;
			rightSideConst = NULL;
			rightSideVar[0] = 0;
			rightSideUnitsPrompt = PROMPT_SILENCE; // use PROMPT_SILENCE as flag that the unit has not been set
			rightSideUnitsStr = NULL;

			switch (mNum)
			{
				case OPTIONS_MENU_APRS_MODE:
					{
						const char *aprsModes[] = { currentLanguage->off, currentLanguage->manual, currentLanguage->ptt, currentLanguage->Auto, currentLanguage->aprs_smart };
						leftSide = currentLanguage->mode;
						rightSideConst = aprsModes[(uint32_t)aprsSettingsCopy.mode];
					}
					break;

				case OPTIONS_MENU_APRS_LOCATION:
					{
						const char *aprsLocations[] = { currentLanguage->aprs_channel, currentLanguage->gps };
						leftSide = currentLanguage->location;
						rightSideConst = aprsLocations[(((aprsSettingsCopy.state & 0x06) >> 1) / 2)];
					}
					break;

				case OPTIONS_MENU_APRS_INITIAL_INTERVAL:
					{
						double secs = (double)(initialIntervalsInSecs[aprsSettingsCopy.initialInterval] / 60.0);
						uint8_t mInt = (uint8_t)secs;
						uint8_t mDec = (uint8_t)((secs - (double)mInt) * 1E1);

						leftSide = currentLanguage->aprs_interval;

						if (mDec > 0)
						{
							snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u.%u", mInt, mDec);
						}
						else
						{
							snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", mInt);
						}

						rightSideUnitsPrompt = PROMPT_MINUTES;
						rightSideUnitsStr = "min";
					}
					break;

				case OPTIONS_MENU_APRS_DECAY:
					leftSide = currentLanguage->aprs_decay;
					rightSideConst = ((aprsSettingsCopy.state & APRS_BEACONING_STATE_DECAY_ALGO_ENABLED) ? currentLanguage->on : currentLanguage->off);
					break;

				case OPTIONS_MENU_APRS_COMPRESSED:
					leftSide = currentLanguage->aprs_compress;
					rightSideConst = ((aprsSettingsCopy.state & APRS_BEACONING_STATE_COMPRESSED_FORMAT) ? currentLanguage->on : currentLanguage->off);
					break;

#if defined(RATE_MESSAGE_FEATURE)
				case OPTIONS_MENU_APRS_MESSAGE_INTERVAL:
					leftSide = currentLanguage->aprs_message_interval;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.messageInterval);
					break;
#endif
				case OPTIONS_MENU_APRS_SLOW_RATE:
					leftSide = currentLanguage->aprs_slow_rate;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.smart.slowRate);
					rightSideUnitsPrompt = PROMPT_MINUTES;
					rightSideUnitsStr = "min";
					break;

				case OPTIONS_MENU_APRS_FAST_RATE:
					leftSide = currentLanguage->aprs_fast_rate;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.smart.fastRate);
					rightSideUnitsPrompt = PROMPT_SECONDS;
					rightSideUnitsStr = "s";
					break;

				case OPTIONS_MENU_APRS_LOW_SPEED:
					leftSide = currentLanguage->aprs_low_speed;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.smart.lowSpeed);
					rightSideUnitsPrompt = PROMPT_KMPH;
					rightSideUnitsStr = "km/h";
					break;

				case OPTIONS_MENU_APRS_HIGH_SPEED:
					leftSide = currentLanguage->aprs_high_speed;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.smart.highSpeed);
					rightSideUnitsPrompt = PROMPT_KMPH;
					rightSideUnitsStr = "km/h";
					break;

				case OPTIONS_MENU_APRS_TURN_ANGLE:
					{
						char unitStr[SCREEN_LINE_BUFFER_SIZE];

						leftSide = currentLanguage->aprs_turn_angle;
						snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.smart.turnAngle);
						rightSideUnitsPrompt = PROMPT_DEGREES;
						sprintf(unitStr, "%c", 176);
						rightSideUnitsStr = unitStr;
					}
					break;

				case OPTIONS_MENU_APRS_TURN_SLOPE:
					{
						char unitStr[SCREEN_LINE_BUFFER_SIZE];

						leftSide = currentLanguage->aprs_turn_slope;
						snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", (aprsSettingsCopy.smart.turnSlope * 10));
						sprintf(unitStr, "%c/v", 176);
						rightSideUnitsStr = unitStr;
					}
					break;

				case OPTIONS_MENU_APRS_TURN_TIME:
					leftSide = currentLanguage->aprs_turn_time;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%u", aprsSettingsCopy.smart.turnTime);
					rightSideUnitsPrompt = PROMPT_SECONDS;
					rightSideUnitsStr = "s";
					break;
			}

			snprintf(buf, SCREEN_LINE_BUFFER_SIZE, "%s:%s", leftSide, (rightSideVar[0] ? rightSideVar : (rightSideConst ? rightSideConst : "")));

			if (i == 0)
			{
				bool wasPlaying = voicePromptsIsPlaying();

				if (!isFirstRun && (menuDataGlobal.menuOptionsSetQuickkey == 0))
				{
					voicePromptsInit();
				}

				if (!wasPlaying || (menuDataGlobal.newOptionSelected || (menuDataGlobal.menuOptionsTimeout > 0)))
				{
					voicePromptsAppendLanguageString(leftSide);
				}

				if ((rightSideVar[0] != 0) || ((rightSideVar[0] == 0) && (rightSideConst == NULL)))
				{
					voicePromptsAppendString(rightSideVar);
				}
				else
				{
					voicePromptsAppendLanguageString(rightSideConst);
				}

				if (rightSideUnitsPrompt != PROMPT_SILENCE)
				{
					voicePromptsAppendPrompt(rightSideUnitsPrompt);
				}

				if (rightSideUnitsStr != NULL)
				{
					strncat(rightSideVar, rightSideUnitsStr, SCREEN_LINE_BUFFER_SIZE);
				}

				if (menuDataGlobal.menuOptionsTimeout != -1)
				{
					promptsPlayNotAfterTx();
				}
				else
				{
					menuDataGlobal.menuOptionsTimeout = 0; // clear flag indicating that a QuickKey has just been set
				}
			}

			// QuickKeys
			if (menuDataGlobal.menuOptionsTimeout > 0)
			{
				menuDisplaySettingOption(leftSide, (rightSideVar[0] ? rightSideVar : rightSideConst));
			}
			else
			{
				if (rightSideUnitsStr != NULL)
				{
					strncat(buf, rightSideUnitsStr, SCREEN_LINE_BUFFER_SIZE);
				}

				menuDisplayEntry(i, mNum, buf, (strlen(leftSide) + 1), THEME_ITEM_FG_MENU_ITEM, THEME_ITEM_FG_OPTIONS_VALUE, THEME_ITEM_BG);
			}
		}
	}

	displayRender();
}

static void handleEvent(uiEvent_t *ev)
{
	bool isDirty = false;

	if (ev->events & BUTTON_EVENT)
	{
		if (repeatVoicePromptOnSK1(ev))
		{
			return;
		}
	}

	if ((menuDataGlobal.menuOptionsTimeout > 0) && (!BUTTONCHECK_DOWN(ev, BUTTON_SK2)))
	{
		if (voicePromptsIsPlaying() == false)
		{
			menuDataGlobal.menuOptionsTimeout--;
			if (menuDataGlobal.menuOptionsTimeout == 0)
			{
				applySettings();
				menuSystemPopPreviousMenu();
				return;
			}
		}
	}

	if (ev->events & FUNCTION_EVENT)
	{
		isDirty = true;
		if (ev->function == FUNC_REDRAW)
		{
			updateScreen(false);
			return;
		}
		else if ((QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU) && (QUICKKEY_ENTRYID(ev->function) < NUM_APRS_MENU_ITEMS))
		{
			menuDataGlobal.currentItemIndex = QUICKKEY_ENTRYID(ev->function);
		}

		if ((QUICKKEY_FUNCTIONID(ev->function) != 0))
		{
			menuDataGlobal.menuOptionsTimeout = 1000;
		}
	}

	if ((ev->events & KEY_EVENT) && (menuDataGlobal.menuOptionsSetQuickkey == 0) && (menuDataGlobal.menuOptionsTimeout == 0))
	{
		if (KEYCHECK_PRESS(ev->keys, KEY_DOWN) && (menuDataGlobal.numItems != 0))
		{
			isDirty = true;
			menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, NUM_APRS_MENU_ITEMS);
			menuDataGlobal.newOptionSelected = true;
			menuAPRSExitCode |= MENU_STATUS_LIST_TYPE;
		}
		else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
		{
			isDirty = true;
			menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, NUM_APRS_MENU_ITEMS);
			menuDataGlobal.newOptionSelected = true;
			menuAPRSExitCode |= MENU_STATUS_LIST_TYPE;
		}
		else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
		{
			applySettings();
			menuSystemPopAllAndDisplayRootMenu();
			return;
		}
		else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
		{
			menuSystemPopPreviousMenu();
			return;
		}
		else if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, (BUTTON_SK1 | BUTTON_SK2)) == 0))
		{
			int value = (ev->keys.key - '0');

			if ((value >= 0) && (value < SMART_BEACONING_PRESETS_MAX))
			{
				memcpy(&aprsSettingsCopy.smart, &smartPresets[value], sizeof(aprsSmartBeaconingSettings_t));
				uiNotificationShow(NOTIFICATION_TYPE_MESSAGE, NOTIFICATION_ID_MESSAGE, 1000, sbPresetsStr[value], false);
				isDirty = true;
			}
		}
		else if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
		{
			menuDataGlobal.menuOptionsSetQuickkey = ev->keys.key;
			isDirty = true;
		}
	}
	if ((ev->events & (KEY_EVENT | FUNCTION_EVENT)) && (menuDataGlobal.menuOptionsSetQuickkey == 0))
	{
		if (KEYCHECK_PRESS(ev->keys, KEY_RIGHT)
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
				|| KEYCHECK_SHORTUP(ev->keys, KEY_ROTARY_INCREMENT)
#endif
				|| (QUICKKEY_FUNCTIONID(ev->function) == FUNC_RIGHT))
		{
			isDirty = true;
			menuDataGlobal.newOptionSelected = false;
			switch (menuDataGlobal.currentItemIndex)
			{
				case OPTIONS_MENU_APRS_MODE:
					if (aprsSettingsCopy.mode < APRS_BEACONING_MODE_SMART_BEACONING)
					{
						aprsSettingsCopy.mode++;
					}
					break;

				case OPTIONS_MENU_APRS_LOCATION:
					if ((aprsSettingsCopy.state & 0x06) < APRS_BEACONING_STATE_LOCATION_MAX)
					{
						aprsSettingsCopy.state = ((aprsSettingsCopy.state & ~0x06) | ((aprsSettingsCopy.state & 0x06) << 1));
					}
					break;

				case OPTIONS_MENU_APRS_INITIAL_INTERVAL:
					if (aprsSettingsCopy.initialInterval < APRS_BEACON_INITIAL_INTERVAL_MAX)
					{
						aprsSettingsCopy.initialInterval++;
					}
					break;

				case OPTIONS_MENU_APRS_DECAY:
					if ((aprsSettingsCopy.state & APRS_BEACONING_STATE_DECAY_ALGO_ENABLED) == 0)
					{
						aprsSettingsCopy.state |= APRS_BEACONING_STATE_DECAY_ALGO_ENABLED;
					}
					break;

				case OPTIONS_MENU_APRS_COMPRESSED:
					if ((aprsSettingsCopy.state & APRS_BEACONING_STATE_COMPRESSED_FORMAT) == 0)
					{
						aprsSettingsCopy.state |= APRS_BEACONING_STATE_COMPRESSED_FORMAT;
					}
					break;

#if defined(RATE_MESSAGE_FEATURE)
				case OPTIONS_MENU_APRS_MESSAGE_INTERVAL:
					aprsSettingsCopy.messageInterval = getIncValue(ev, aprsSettingsCopy.messageInterval, 4, APRS_BEACON_MESSAGE_INTERVAL_MAX);
					break;
#endif
				case OPTIONS_MENU_APRS_SLOW_RATE:
					aprsSettingsCopy.smart.slowRate = getIncValue(ev, aprsSettingsCopy.smart.slowRate, 10, APRS_SMART_BEACON_SLOW_RATE_MAX);
					break;

				case OPTIONS_MENU_APRS_FAST_RATE:
					aprsSettingsCopy.smart.fastRate = getIncValue(ev, aprsSettingsCopy.smart.fastRate, 10, APRS_SMART_BEACON_FAST_RATE_MAX);
					break;

				case OPTIONS_MENU_APRS_LOW_SPEED:
					aprsSettingsCopy.smart.lowSpeed = getIncValue(ev, aprsSettingsCopy.smart.lowSpeed, 4, APRS_SMART_BEACON_LOW_SPEED_MAX);
					break;

				case OPTIONS_MENU_APRS_HIGH_SPEED:
					aprsSettingsCopy.smart.highSpeed = getIncValue(ev, aprsSettingsCopy.smart.highSpeed, 10, APRS_SMART_BEACON_HIGH_SPEED_MAX);
					break;

				case OPTIONS_MENU_APRS_TURN_ANGLE:
					aprsSettingsCopy.smart.turnAngle = getIncValue(ev, aprsSettingsCopy.smart.turnAngle, 10, APRS_SMART_BEACON_TURN_ANGLE_MAX);
					break;

				case OPTIONS_MENU_APRS_TURN_SLOPE:
					aprsSettingsCopy.smart.turnSlope = getIncValue(ev, aprsSettingsCopy.smart.turnSlope, 10, APRS_SMART_BEACON_TURN_SLOPE_MAX);
					break;

				case OPTIONS_MENU_APRS_TURN_TIME:
					aprsSettingsCopy.smart.turnTime = getIncValue(ev, aprsSettingsCopy.smart.turnTime, 10, APRS_SMART_BEACON_TURN_TIME_MAX);
					break;
			}
		}
		else if (KEYCHECK_PRESS(ev->keys, KEY_LEFT)
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
				|| KEYCHECK_SHORTUP(ev->keys, KEY_ROTARY_DECREMENT)
#endif
				|| (QUICKKEY_FUNCTIONID(ev->function) == FUNC_LEFT))
		{
			isDirty = true;
			menuDataGlobal.newOptionSelected = false;
			switch (menuDataGlobal.currentItemIndex)
			{
				case OPTIONS_MENU_APRS_MODE:
					if (aprsSettingsCopy.mode > APRS_BEACONING_MODE_OFF)
					{
						aprsSettingsCopy.mode--;
					}
					break;

				case OPTIONS_MENU_APRS_LOCATION:
					if ((aprsSettingsCopy.state & 0x06) > APRS_BEACONING_STATE_LOCATION_MIN)
					{
						aprsSettingsCopy.state = ((aprsSettingsCopy.state & ~0x06) | ((aprsSettingsCopy.state & 0x06) >> 1));
					}
					break;

				case OPTIONS_MENU_APRS_INITIAL_INTERVAL:
					if (aprsSettingsCopy.initialInterval > APRS_BEACON_INITIAL_INTERVAL_MIN)
					{
						aprsSettingsCopy.initialInterval--;
					}
					break;

				case OPTIONS_MENU_APRS_DECAY:
					if ((aprsSettingsCopy.state & APRS_BEACONING_STATE_DECAY_ALGO_ENABLED) != 0)
					{
						aprsSettingsCopy.state &= ~APRS_BEACONING_STATE_DECAY_ALGO_ENABLED;
					}
					break;

				case OPTIONS_MENU_APRS_COMPRESSED:
					if ((aprsSettingsCopy.state & APRS_BEACONING_STATE_COMPRESSED_FORMAT) != 0)
					{
						aprsSettingsCopy.state &= ~APRS_BEACONING_STATE_COMPRESSED_FORMAT;
					}
					break;

#if defined(RATE_MESSAGE_FEATURE)
				case OPTIONS_MENU_APRS_MESSAGE_INTERVAL:
					aprsSettingsCopy.messageInterval = getDecValue(ev, aprsSettingsCopy.messageInterval, 4, APRS_BEACON_MESSAGE_INTERVAL_MIN);
					break;
#endif
				case OPTIONS_MENU_APRS_SLOW_RATE:
					aprsSettingsCopy.smart.slowRate = getDecValue(ev, aprsSettingsCopy.smart.slowRate, 10, APRS_SMART_BEACON_SLOW_RATE_MIN);
					break;

				case OPTIONS_MENU_APRS_FAST_RATE:
					aprsSettingsCopy.smart.fastRate = getDecValue(ev, aprsSettingsCopy.smart.fastRate, 10, APRS_SMART_BEACON_FAST_RATE_MIN);
					break;

				case OPTIONS_MENU_APRS_LOW_SPEED:
					aprsSettingsCopy.smart.lowSpeed = getDecValue(ev, aprsSettingsCopy.smart.lowSpeed, 4, APRS_SMART_BEACON_LOW_SPEED_MIN);
					break;

				case OPTIONS_MENU_APRS_HIGH_SPEED:
					aprsSettingsCopy.smart.highSpeed = getDecValue(ev, aprsSettingsCopy.smart.highSpeed, 10, APRS_SMART_BEACON_HIGH_SPEED_MIN);
					break;

				case OPTIONS_MENU_APRS_TURN_ANGLE:
					aprsSettingsCopy.smart.turnAngle = getDecValue(ev, aprsSettingsCopy.smart.turnAngle, 10, APRS_SMART_BEACON_TURN_ANGLE_MIN);
					break;

				case OPTIONS_MENU_APRS_TURN_SLOPE:
					aprsSettingsCopy.smart.turnSlope = getDecValue(ev, aprsSettingsCopy.smart.turnSlope, 10, APRS_SMART_BEACON_TURN_SLOPE_MIN);
					break;

				case OPTIONS_MENU_APRS_TURN_TIME:
					aprsSettingsCopy.smart.turnTime = getDecValue(ev, aprsSettingsCopy.smart.turnTime, 10, APRS_SMART_BEACON_TURN_TIME_MIN);
					break;
			}
		}
		else if ((ev->keys.event & KEY_MOD_PRESS) && (menuDataGlobal.menuOptionsTimeout > 0))
		{
			menuDataGlobal.menuOptionsTimeout = 0;
			resetOriginalSettingsData();
			menuSystemPopPreviousMenu();
			return;
		}
	}

	if (uiQuickKeysIsStoring(ev))
	{
		uiQuickKeysStore(ev, &menuAPRSExitCode);
		isDirty = true;
	}

	if (isDirty)
	{
		updateScreen(false);
	}
}

static void applySettings(void)
{
	aprsBeaconingSetSettings(&aprsSettingsCopy);
	settingsSaveIfNeeded(true);
}

static void exitCallback(void *data)
{
	resetOriginalSettingsData();
}

#endif // PLATFORM_GD77S
