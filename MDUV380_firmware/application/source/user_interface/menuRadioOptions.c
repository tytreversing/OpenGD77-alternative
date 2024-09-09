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
#include "user_interface/uiUtilities.h"
#include "interfaces/wdog.h"
#include "utils.h"
#include "functions/rxPowerSaving.h"

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);
static void applySettings(void);
static void exitCallback(void *data);

static menuStatus_t menuOptionsExitCode = MENU_STATUS_SUCCESS;

enum
{
	RADIO_OPTIONS_MENU_TX_FREQ_LIMITS = 0U,
	RADIO_OPTIONS_MENU_DMR_MONITOR_CAPTURE_TIMEOUT,
	RADIO_OPTIONS_MENU_SCAN_DELAY,
	RADIO_OPTIONS_MENU_SCAN_STEP_TIME,
	RADIO_OPTIONS_MENU_SCAN_MODE,
	RADIO_OPTIONS_MENU_SCAN_ON_BOOT,
	RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_VHF,
#if ! (defined(PLATFORM_MD9600) || defined(PLATFORM_MD380))
	RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_220MHz,
#endif
	RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_UHF,
	RADIO_OPTIONS_MENU_PTT_TOGGLE,
	RADIO_OPTIONS_MENU_PRIVATE_CALLS,
	RADIO_OPTIONS_MENU_USER_POWER,
	RADIO_OPTIONS_MENU_DMR_CRC,
#if defined(PLATFORM_MDUV380) && !defined(PLATFORM_VARIANT_UV380_PLUS_10W)
	RADIO_OPTIONS_MENU_FORCE_10W,
#endif
	NUM_RADIO_OPTIONS_MENU_ITEMS
};

menuStatus_t menuRadioOptions(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.menuOptionsSetQuickkey = 0;
		menuDataGlobal.menuOptionsTimeout = 0;
		menuDataGlobal.newOptionSelected = true;
		menuDataGlobal.numItems = NUM_RADIO_OPTIONS_MENU_ITEMS;

		if (originalNonVolatileSettings.magicNumber == 0xDEADBEEF)
		{
			// Store original settings, used on cancel event.
			memcpy(&originalNonVolatileSettings, &nonVolatileSettings, sizeof(settingsStruct_t));
		}

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->radio_options);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		menuSystemRegisterExitCallback(exitCallback, NULL);

		updateScreen(true);
		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuOptionsExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent || (menuDataGlobal.menuOptionsTimeout > 0))
		{
			handleEvent(ev);
		}
	}
	return menuOptionsExitCode;
}

static void updateScreen(bool isFirstRun)
{
	int mNum = 0;
	char buf[SCREEN_LINE_BUFFER_SIZE];
	const char *leftSide = NULL;// initialize to please the compiler
	const char *rightSideConst = NULL;// initialize to please the compiler
	char rightSideVar[SCREEN_LINE_BUFFER_SIZE];
	voicePrompt_t rightSideUnitsPrompt;
	const char *rightSideUnitsStr;

	displayClearBuf();
	bool settingOption = uiQuickKeysShowChoices(buf, SCREEN_LINE_BUFFER_SIZE, currentLanguage->radio_options);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		if ((settingOption == false) || (i == 0))
		{
			mNum = menuGetMenuOffset(NUM_RADIO_OPTIONS_MENU_ITEMS, i);
			if (mNum == MENU_OFFSET_BEFORE_FIRST_ENTRY)
			{
				continue;
			}
			else if (mNum == MENU_OFFSET_AFTER_LAST_ENTRY)
			{
				break;
			}

			buf[0] = 0;
			buf[2] = 0;
			leftSide = NULL;
			rightSideConst = NULL;
			rightSideVar[0] = 0;
			rightSideUnitsPrompt = PROMPT_SILENCE;// use PROMPT_SILENCE as flag that the unit has not been set
			rightSideUnitsStr = NULL;

			switch(mNum)
			{
				case RADIO_OPTIONS_MENU_TX_FREQ_LIMITS:// Tx Freq limits
					leftSide = currentLanguage->band_limits;
					switch(nonVolatileSettings.txFreqLimited)
					{
						case BAND_LIMITS_NONE:
							rightSideConst = currentLanguage->off;
							break;
						case BAND_LIMITS_ON_LEGACY_DEFAULT:
							rightSideConst = currentLanguage->on;
							break;
						case BAND_LIMITS_FROM_CPS:
							strcpy(rightSideVar,"CPS");
							break;
					}

					break;
				case RADIO_OPTIONS_MENU_DMR_MONITOR_CAPTURE_TIMEOUT:// DMR filtr timeout repeat
					leftSide = currentLanguage->dmr_filter_timeout;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d", nonVolatileSettings.dmrCaptureTimeout);
					rightSideUnitsPrompt = PROMPT_SECONDS;
					rightSideUnitsStr = "s";
					break;
				case RADIO_OPTIONS_MENU_SCAN_DELAY:// Scan hold and pause time
					leftSide = currentLanguage->scan_delay;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d", nonVolatileSettings.scanDelay);
					rightSideUnitsPrompt = PROMPT_SECONDS;
					rightSideUnitsStr = "s";
					break;
				case RADIO_OPTIONS_MENU_SCAN_STEP_TIME:// Scan step time
					leftSide = currentLanguage->scan_dwell_time;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d", settingsGetScanStepTimeMilliseconds());
					rightSideUnitsPrompt = PROMPT_MILLISECONDS;
					rightSideUnitsStr = "ms";
					break;
				case RADIO_OPTIONS_MENU_SCAN_MODE:// scanning mode
					leftSide = currentLanguage->scan_mode;
					{
						const char *scanModes[] = { currentLanguage->hold, currentLanguage->pause, currentLanguage->stop };
						rightSideConst = scanModes[nonVolatileSettings.scanModePause];
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_ON_BOOT:
					leftSide = currentLanguage->scan_on_boot;
					rightSideConst = (settingsIsOptionBitSet(BIT_SCAN_ON_BOOT_ENABLED) ? currentLanguage->on : currentLanguage->off);
					break;
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_VHF:
					leftSide = currentLanguage->squelch_VHF;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d%%", (nonVolatileSettings.squelchDefaults[RADIO_BAND_VHF] - 1) * 5);// 5% steps
					break;
#if ! (defined(PLATFORM_MD9600) || defined(PLATFORM_MD380))
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_220MHz:
					leftSide = currentLanguage->squelch_220;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d%%", (nonVolatileSettings.squelchDefaults[RADIO_BAND_220MHz] - 1) * 5);// 5% steps
					break;
#endif
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_UHF:
					leftSide = currentLanguage->squelch_UHF;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d%%", (nonVolatileSettings.squelchDefaults[RADIO_BAND_UHF] - 1) * 5);// 5% steps
					break;
				case RADIO_OPTIONS_MENU_PTT_TOGGLE:
					leftSide = currentLanguage->ptt_toggle;
					rightSideConst = (settingsIsOptionBitSet(BIT_PTT_LATCH) ? currentLanguage->on : currentLanguage->off);
					break;
				case RADIO_OPTIONS_MENU_PRIVATE_CALLS:
					leftSide = currentLanguage->private_call_handling;
					const char *allowPCOptions[] = { currentLanguage->off, currentLanguage->on, currentLanguage->ptt, currentLanguage->Auto};
					rightSideConst = allowPCOptions[nonVolatileSettings.privateCalls];
					break;
				case RADIO_OPTIONS_MENU_USER_POWER:
					leftSide = currentLanguage->user_power;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d", (nonVolatileSettings.userPower));
					break;
				case RADIO_OPTIONS_MENU_DMR_CRC:
					leftSide = currentLanguage->dmr_crc;
					rightSideConst = (settingsIsOptionBitSet(BIT_DMR_CRC_IGNORED) ? currentLanguage->off : currentLanguage->on);
					break;
#if defined(PLATFORM_MDUV380) && !defined(PLATFORM_VARIANT_UV380_PLUS_10W)
				case RADIO_OPTIONS_MENU_FORCE_10W:
					leftSide = currentLanguage->mode;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%s", (settingsIsOptionBitSet(BIT_FORCE_10W_RADIO) ? "10" : "5"));
					rightSideUnitsPrompt = PROMPT_WATTS;
					rightSideUnitsStr = "W";
					break;
#endif
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
					menuDataGlobal.menuOptionsTimeout = 0;// clear flag indicating that a QuickKey has just been set
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
		else if ((QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU) && (QUICKKEY_ENTRYID(ev->function) < NUM_RADIO_OPTIONS_MENU_ITEMS))
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
			menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, NUM_RADIO_OPTIONS_MENU_ITEMS);
			menuDataGlobal.newOptionSelected = true;
			menuOptionsExitCode |= MENU_STATUS_LIST_TYPE;
		}
		else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
		{
			isDirty = true;
			menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, NUM_RADIO_OPTIONS_MENU_ITEMS);
			menuDataGlobal.newOptionSelected = true;
			menuOptionsExitCode |= MENU_STATUS_LIST_TYPE;
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
			switch(menuDataGlobal.currentItemIndex)
			{
				case RADIO_OPTIONS_MENU_TX_FREQ_LIMITS:
					if (nonVolatileSettings.txFreqLimited < BAND_LIMITS_FROM_CPS)
					{
						settingsIncrement(nonVolatileSettings.txFreqLimited, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_DMR_MONITOR_CAPTURE_TIMEOUT:
					if (nonVolatileSettings.dmrCaptureTimeout < 90)
					{
						settingsIncrement(nonVolatileSettings.dmrCaptureTimeout, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_DELAY:
					if (nonVolatileSettings.scanDelay < 30)
					{
						settingsIncrement(nonVolatileSettings.scanDelay, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_STEP_TIME:
					if (nonVolatileSettings.scanStepTime < 15)  // <30> + (15 * 30ms) MAX
					{
						settingsIncrement(nonVolatileSettings.scanStepTime, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_MODE:
					if (nonVolatileSettings.scanModePause < SCAN_MODE_STOP)
					{
						settingsIncrement(nonVolatileSettings.scanModePause, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_ON_BOOT:
					if (settingsIsOptionBitSet(BIT_SCAN_ON_BOOT_ENABLED) == false)
					{
						settingsSetOptionBit(BIT_SCAN_ON_BOOT_ENABLED, true);
					}
					break;
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_VHF:
					if (nonVolatileSettings.squelchDefaults[RADIO_BAND_VHF] < CODEPLUG_MAX_VARIABLE_SQUELCH)
					{
						settingsIncrement(nonVolatileSettings.squelchDefaults[RADIO_BAND_VHF], 1);
					}
					break;
#if ! (defined(PLATFORM_MD9600) || defined(PLATFORM_MD380))
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_220MHz:
					if (nonVolatileSettings.squelchDefaults[RADIO_BAND_220MHz] < CODEPLUG_MAX_VARIABLE_SQUELCH)
					{
						settingsIncrement(nonVolatileSettings.squelchDefaults[RADIO_BAND_220MHz], 1);
					}
					break;
#endif
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_UHF:
					if (nonVolatileSettings.squelchDefaults[RADIO_BAND_UHF] < CODEPLUG_MAX_VARIABLE_SQUELCH)
					{
						settingsIncrement(nonVolatileSettings.squelchDefaults[RADIO_BAND_UHF], 1);
					}
					break;
				case RADIO_OPTIONS_MENU_PTT_TOGGLE:
					if (settingsIsOptionBitSet(BIT_PTT_LATCH) == false)
					{
						settingsSetOptionBit(BIT_PTT_LATCH, true);
					}
					break;
				case RADIO_OPTIONS_MENU_PRIVATE_CALLS:
					// Note. Currently the "AUTO" option is not available
					if (nonVolatileSettings.privateCalls < ALLOW_PRIVATE_CALLS_PTT)
					{
						settingsIncrement(nonVolatileSettings.privateCalls, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_USER_POWER:
					{
						int newVal = (int)nonVolatileSettings.userPower;

						// Not the real max value of 4096, but trxUpdate_PA_DAC_Drive() will auto limit it to 4096
						// and it makes the logic easier and there is no functional difference
						newVal = SAFE_MIN((newVal + (BUTTONCHECK_DOWN(ev, BUTTON_SK2) ? 10 : 100)), 4100);

						settingsSet(nonVolatileSettings.userPower, newVal);
						trxUpdate_PA_DAC_Drive();
					}
					break;
				case RADIO_OPTIONS_MENU_DMR_CRC:
					if (settingsIsOptionBitSet(BIT_DMR_CRC_IGNORED))
					{
						settingsSetOptionBit(BIT_DMR_CRC_IGNORED, false);
					}
					break;
#if defined(PLATFORM_MDUV380) && !defined(PLATFORM_VARIANT_UV380_PLUS_10W)
				case RADIO_OPTIONS_MENU_FORCE_10W:
					if (settingsIsOptionBitSet(BIT_FORCE_10W_RADIO) == false)
					{
						settingsSetOptionBit(BIT_FORCE_10W_RADIO, true);
					}
					break;
#endif
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
			switch(menuDataGlobal.currentItemIndex)
			{
				case RADIO_OPTIONS_MENU_TX_FREQ_LIMITS:
					if (nonVolatileSettings.txFreqLimited > BAND_LIMITS_NONE)
					{
						settingsDecrement(nonVolatileSettings.txFreqLimited, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_DMR_MONITOR_CAPTURE_TIMEOUT:
					if (nonVolatileSettings.dmrCaptureTimeout > 2) // min 2s
					{
						settingsDecrement(nonVolatileSettings.dmrCaptureTimeout, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_DELAY:
					if (nonVolatileSettings.scanDelay > 1)
					{
						settingsDecrement(nonVolatileSettings.scanDelay, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_STEP_TIME:
					if (nonVolatileSettings.scanStepTime > 0)
					{
						settingsDecrement(nonVolatileSettings.scanStepTime, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_MODE:
					if (nonVolatileSettings.scanModePause > SCAN_MODE_HOLD)
					{
						settingsDecrement(nonVolatileSettings.scanModePause, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_SCAN_ON_BOOT:
					if (settingsIsOptionBitSet(BIT_SCAN_ON_BOOT_ENABLED))
					{
						settingsSetOptionBit(BIT_SCAN_ON_BOOT_ENABLED, false);
					}
					break;
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_VHF:
					if (nonVolatileSettings.squelchDefaults[RADIO_BAND_VHF] > 1)
					{
						settingsDecrement(nonVolatileSettings.squelchDefaults[RADIO_BAND_VHF], 1);
					}
					break;
#if ! (defined(PLATFORM_MD9600) || defined(PLATFORM_MD380))
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_220MHz:
					if (nonVolatileSettings.squelchDefaults[RADIO_BAND_220MHz] > 1)
					{
						settingsDecrement(nonVolatileSettings.squelchDefaults[RADIO_BAND_220MHz], 1);
					}
					break;
#endif
				case RADIO_OPTIONS_MENU_SQUELCH_DEFAULT_UHF:
					if (nonVolatileSettings.squelchDefaults[RADIO_BAND_UHF] > 1)
					{
						settingsDecrement(nonVolatileSettings.squelchDefaults[RADIO_BAND_UHF], 1);
					}
					break;
				case RADIO_OPTIONS_MENU_PTT_TOGGLE:
					if (settingsIsOptionBitSet(BIT_PTT_LATCH))
					{
						settingsSetOptionBit(BIT_PTT_LATCH, false);
					}
					break;
				case RADIO_OPTIONS_MENU_PRIVATE_CALLS:
					if (nonVolatileSettings.privateCalls > 0)
					{
						settingsDecrement(nonVolatileSettings.privateCalls, 1);
					}
					break;
				case RADIO_OPTIONS_MENU_USER_POWER:
					{
						int newVal = (int)nonVolatileSettings.userPower;

						// Not the real max value of 4096, but trxUpdate_PA_DAC_Drive() will auto limit it to 4096
						// and it makes the logic easier and there is no functional difference
						newVal = SAFE_MAX((newVal - (BUTTONCHECK_DOWN(ev, BUTTON_SK2) ? 10 : 100)), 0);

						settingsSet(nonVolatileSettings.userPower, newVal);
						trxUpdate_PA_DAC_Drive();
					}
					break;
				case RADIO_OPTIONS_MENU_DMR_CRC:
					if (settingsIsOptionBitSet(BIT_DMR_CRC_IGNORED) == false)
					{
						settingsSetOptionBit(BIT_DMR_CRC_IGNORED, true);
					}
					break;
#if defined(PLATFORM_MDUV380) && !defined(PLATFORM_VARIANT_UV380_PLUS_10W)
				case RADIO_OPTIONS_MENU_FORCE_10W:
					if (settingsIsOptionBitSet(BIT_FORCE_10W_RADIO))
					{
						settingsSetOptionBit(BIT_FORCE_10W_RADIO, false);
					}
					break;
#endif
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
		uiQuickKeysStore(ev, &menuOptionsExitCode);
		isDirty = true;
	}

	if (isDirty)
	{
		updateScreen(false);
	}
}

static void applySettings(void)
{
	settingsSaveIfNeeded(true);
	resetOriginalSettingsData();
}

static void exitCallback(void *data)
{
	if (originalNonVolatileSettings.magicNumber != 0xDEADBEEF)
	{
		// Restore original settings.
		memcpy(&nonVolatileSettings, &originalNonVolatileSettings, sizeof(settingsStruct_t));
		settingsSaveIfNeeded(true);
		trxUpdate_PA_DAC_Drive();

		resetOriginalSettingsData();
	}
}

