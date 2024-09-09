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
#if defined(PLATFORM_MD9600)
#include "hardware/ST7567.h"
#elif (defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017))
#include "hardware/HX8353E.h"
#else
#include "hardware/UC1701.h"
#endif
#include "hardware/HR-C6000.h"
#include "user_interface/uiGlobals.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiLocalisation.h"
#include "user_interface/uiUtilities.h"

#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
#define DMR_RX_AGC_MAX 16
#define FM_MIC_GAIN_MIN 1  // Limit to min 1, as 0: no audio
#define FM_MIC_GAIN_MAX 31
#elif defined(PLATFORM_MD9600)
#define DMR_RX_AGC_MAX 16
#define FM_MIC_GAIN_MIN 0
#define FM_MIC_GAIN_MAX 15
#else
#define DMR_RX_AGC_MAX 8
#define FM_MIC_GAIN_MIN 0
#define FM_MIC_GAIN_MAX 15
#endif

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);
static void applySettings(void);
static void exitCallback(void *data);

static menuStatus_t menuSoundExitCode = MENU_STATUS_SUCCESS;

enum
{
	OPTIONS_MENU_TIMEOUT_BEEP = 0,
	OPTIONS_MENU_BEEP_VOLUME,
	OPTIONS_MENU_DMR_BEEP,
	OPTIONS_MENU_RX_BEEP,
	OPTIONS_MENU_RX_TALKER_BEGIN_BEEP,
	OPTIONS_MIC_GAIN_DMR,
	OPTIONS_MIC_GAIN_FM,
	OPTIONS_VOX_THRESHOLD,
	OPTIONS_VOX_TAIL,
	OPTIONS_AUDIO_PROMPT_MODE,
	OPTIONS_AUDIO_DMR_RX_AGC,
#if defined(PLATFORM_MD9600)
	OPTIONS_SPEAKER_CLICK_SUPPRESS,
#endif
	NUM_SOUND_MENU_ITEMS
};

menuStatus_t menuSoundOptions(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.menuOptionsSetQuickkey = 0;
		menuDataGlobal.menuOptionsTimeout = 0;
		menuDataGlobal.newOptionSelected = true;
		menuDataGlobal.numItems = NUM_SOUND_MENU_ITEMS;

		if (originalNonVolatileSettings.magicNumber == 0xDEADBEEF)
		{
			// Store original settings, used on cancel event.
			memcpy(&originalNonVolatileSettings, &nonVolatileSettings, sizeof(settingsStruct_t));
		}

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->sound_options);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		menuSystemRegisterExitCallback(exitCallback, NULL);

		updateScreen(isFirstRun);
		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuSoundExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent || (menuDataGlobal.menuOptionsTimeout > 0))
		{
			handleEvent(ev);
		}
	}
	return menuSoundExitCode;
}


static void updateScreen(bool isFirstRun)
{
	int mNum = 0;
	char buf[SCREEN_LINE_BUFFER_SIZE];
	const char *leftSide = NULL;// initialise to please the compiler
	const char *rightSideConst = NULL;// initialise to please the compiler
	char rightSideVar[SCREEN_LINE_BUFFER_SIZE];
	voicePrompt_t rightSideUnitsPrompt;
	const char *rightSideUnitsStr;

	displayClearBuf();
	bool settingOption = uiQuickKeysShowChoices(buf, SCREEN_LINE_BUFFER_SIZE, currentLanguage->sound_options);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		if ((settingOption == false) || (i == 0))
		{
			mNum = menuGetMenuOffset(NUM_SOUND_MENU_ITEMS, i);
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
			rightSideUnitsPrompt = PROMPT_SILENCE;// use PROMPT_SILENCE as flag that the unit has not been set
			rightSideUnitsStr = NULL;

			switch(mNum)
			{
				case OPTIONS_MENU_TIMEOUT_BEEP:
					leftSide = currentLanguage->timeout_beep;
					if (nonVolatileSettings.audioPromptMode == AUDIO_PROMPT_MODE_SILENT)
					{
						rightSideConst = currentLanguage->n_a;
					}
					else
					{
						if (nonVolatileSettings.txTimeoutBeepX5Secs != 0)
						{
							snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d", nonVolatileSettings.txTimeoutBeepX5Secs * 5);
							rightSideUnitsPrompt = PROMPT_SECONDS;
							rightSideUnitsStr = "s";
						}
						else
						{
							rightSideConst = currentLanguage->n_a;
						}
					}
					break;
				case OPTIONS_MENU_BEEP_VOLUME: // Beep volume reduction
					leftSide = currentLanguage->beep_volume;
					if (nonVolatileSettings.audioPromptMode == AUDIO_PROMPT_MODE_SILENT)
					{
						rightSideConst = currentLanguage->n_a;
					}
					else
					{
						snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%ddB", (2 - nonVolatileSettings.beepVolumeDivider) * 3);
						soundBeepVolumeDivider = nonVolatileSettings.beepVolumeDivider;
					}

					break;
				case OPTIONS_MENU_DMR_BEEP:
					leftSide = currentLanguage->dmr_beep;
					if (nonVolatileSettings.audioPromptMode == AUDIO_PROMPT_MODE_SILENT)
					{
						rightSideConst = currentLanguage->n_a;
					}
					else
					{
						const char *beepTX[] = { currentLanguage->none, currentLanguage->start, currentLanguage->stop, currentLanguage->both };
						rightSideConst = beepTX[(nonVolatileSettings.beepOptions & 0x03)];
					}
					break;
				case OPTIONS_MENU_RX_BEEP:
					leftSide = currentLanguage->rx_beep;
					if (nonVolatileSettings.audioPromptMode == AUDIO_PROMPT_MODE_SILENT)
					{
						rightSideConst = currentLanguage->n_a;
					}
					else
					{
						const char *beepRX[] = { currentLanguage->none, currentLanguage->carrier, currentLanguage->talker, currentLanguage->both };
						rightSideConst = beepRX[((nonVolatileSettings.beepOptions >> 2) & 0x03)];
					}
					break;
				case OPTIONS_MENU_RX_TALKER_BEGIN_BEEP:
					leftSide = currentLanguage->talker;
					if ((nonVolatileSettings.audioPromptMode == AUDIO_PROMPT_MODE_SILENT) ||
							((nonVolatileSettings.beepOptions & BEEP_RX_TALKER) == 0) || (((nonVolatileSettings.beepOptions >> 2) & 0x03) == 0))
					{
						rightSideConst = currentLanguage->n_a;
					}
					else
					{
						const char *beepRXTalker[] = { currentLanguage->end_only, currentLanguage->both };
						rightSideConst = beepRXTalker[((nonVolatileSettings.beepOptions & BEEP_RX_TALKER_BEGIN) >> 4)];
					}
					break;
				case OPTIONS_MIC_GAIN_DMR: // DMR Mic gain
					leftSide = currentLanguage->dmr_mic_gain;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%ddB", (nonVolatileSettings.micGainDMR - SETTINGS_DMR_MIC_ZERO) * 3);
					break;
				case OPTIONS_MIC_GAIN_FM: // FM Mic gain
					leftSide = currentLanguage->fm_mic_gain;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%ddB", (nonVolatileSettings.micGainFM - SETTINGS_FM_MIC_ZERO) * 3);
					break;
				case OPTIONS_VOX_THRESHOLD:
					leftSide = currentLanguage->vox_threshold;
					snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d", nonVolatileSettings.voxThreshold);
					break;
				case OPTIONS_VOX_TAIL:
					leftSide = currentLanguage->vox_tail;
					if (nonVolatileSettings.voxThreshold != 0)
					{
						float tail = (nonVolatileSettings.voxTailUnits * 0.5);
						uint8_t secs = (uint8_t)tail;
						uint8_t fracSec = (tail - secs) * 10;

						snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%d.%d", secs, fracSec);
						rightSideUnitsPrompt = PROMPT_SECONDS;
						rightSideUnitsStr = "s";
					}
					else
					{
						rightSideConst = currentLanguage->n_a;
					}
					break;
				case OPTIONS_AUDIO_PROMPT_MODE:
					{
						leftSide = currentLanguage->audio_prompt;
						const char *audioPromptOption[] = { currentLanguage->silent, currentLanguage->beep, currentLanguage->no_keys,
								currentLanguage->voice_prompt_level_1, currentLanguage->voice_prompt_level_2, currentLanguage->voice_prompt_level_3 };
						rightSideConst = audioPromptOption[nonVolatileSettings.audioPromptMode];
					}
					break;
				case OPTIONS_AUDIO_DMR_RX_AGC:
					leftSide = currentLanguage->dmr_rx_agc;
					if (nonVolatileSettings.DMR_RxAGC != 0)
					{
						snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%ddB", ((nonVolatileSettings.DMR_RxAGC - 1) * 3));
					}
					else
					{
						rightSideConst = currentLanguage->off;
					}
					break;
#if defined(PLATFORM_MD9600)
				case OPTIONS_SPEAKER_CLICK_SUPPRESS:
					leftSide = currentLanguage->speaker_click_suppress;
					rightSideConst = (settingsIsOptionBitSet(BIT_SPEAKER_CLICK_SUPPRESS) ? currentLanguage->on : currentLanguage->off);
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
		else if ((QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU) && (QUICKKEY_ENTRYID(ev->function) < NUM_SOUND_MENU_ITEMS))
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
			menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, NUM_SOUND_MENU_ITEMS);
			menuDataGlobal.newOptionSelected = true;
			menuSoundExitCode |= MENU_STATUS_LIST_TYPE;
		}
		else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
		{
			isDirty = true;
			menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, NUM_SOUND_MENU_ITEMS);
			menuDataGlobal.newOptionSelected = true;
			menuSoundExitCode |= MENU_STATUS_LIST_TYPE;
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
				case OPTIONS_MENU_TIMEOUT_BEEP:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if (nonVolatileSettings.txTimeoutBeepX5Secs < 4)
						{
							settingsIncrement(nonVolatileSettings.txTimeoutBeepX5Secs, 1);
						}
					}
					break;
				case OPTIONS_MENU_BEEP_VOLUME:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if (nonVolatileSettings.beepVolumeDivider > 0)
						{
							settingsDecrement(nonVolatileSettings.beepVolumeDivider, 1);
						}
					}
					break;
				case OPTIONS_MENU_DMR_BEEP:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if ((nonVolatileSettings.beepOptions & 0x03) < (BEEP_TX_START | BEEP_TX_STOP))
						{
							uint8_t opts = (nonVolatileSettings.beepOptions >> 2);
							uint8_t v = (nonVolatileSettings.beepOptions & 0x03) + 1;

							opts = ((opts << 2) | v);

							settingsSet(nonVolatileSettings.beepOptions, opts);
						}
					}
					break;
				case OPTIONS_MENU_RX_BEEP:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if ((nonVolatileSettings.beepOptions & 0x0C) < (BEEP_RX_CARRIER | BEEP_RX_TALKER))
						{
							uint8_t opts = (nonVolatileSettings.beepOptions >> 4);
							uint8_t txBeeps = (nonVolatileSettings.beepOptions & 0x03);
							uint8_t v = ((nonVolatileSettings.beepOptions >> 2) & 0x03) + 1;

							opts = ((opts << 4) | (v << 2) | txBeeps);

							settingsSet(nonVolatileSettings.beepOptions, opts);
						}
					}
					break;
				case OPTIONS_MENU_RX_TALKER_BEGIN_BEEP:
					if ((nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT) &&
							((nonVolatileSettings.beepOptions & BEEP_RX_TALKER) != 0))
					{
						if ((nonVolatileSettings.beepOptions & BEEP_RX_TALKER_BEGIN) == 0)
						{
							uint8_t opts = (nonVolatileSettings.beepOptions | BEEP_RX_TALKER_BEGIN);

							settingsSet(nonVolatileSettings.beepOptions, opts);
						}
					}
					break;
				case OPTIONS_MIC_GAIN_DMR: // DMR Mic gain
					if (nonVolatileSettings.micGainDMR < 15)
					{
						settingsIncrement(nonVolatileSettings.micGainDMR, 1);
						HRC6000SetMicGainDMR(nonVolatileSettings.micGainDMR);
					}
					break;
				case OPTIONS_MIC_GAIN_FM: // FM Mic gain
					if (nonVolatileSettings.micGainFM < FM_MIC_GAIN_MAX)
					{
						settingsIncrement(nonVolatileSettings.micGainFM, 1);
						trxSetMicGainFM(nonVolatileSettings.micGainFM);
					}
					break;
				case OPTIONS_VOX_THRESHOLD:
					if (nonVolatileSettings.voxThreshold < 30)
					{
						settingsIncrement(nonVolatileSettings.voxThreshold, 1);
						voxSetParameters(nonVolatileSettings.voxThreshold, nonVolatileSettings.voxTailUnits);
					}
					break;
				case OPTIONS_VOX_TAIL:
					if (nonVolatileSettings.voxTailUnits < 10) // 5 seconds max
					{
						settingsIncrement(nonVolatileSettings.voxTailUnits, 1);
						voxSetParameters(nonVolatileSettings.voxThreshold, nonVolatileSettings.voxTailUnits);
					}
					break;
				case OPTIONS_AUDIO_PROMPT_MODE:
					if (nonVolatileSettings.audioPromptMode <= ((voicePromptDataIsLoaded ? NUM_AUDIO_PROMPT_MODES : AUDIO_PROMPT_MODE_VOICE_THRESHOLD) - 2))
					{
						settingsIncrement(nonVolatileSettings.audioPromptMode, 1);
					}
					break;
				case OPTIONS_AUDIO_DMR_RX_AGC:
					if (nonVolatileSettings.DMR_RxAGC < DMR_RX_AGC_MAX)
					{
						settingsIncrement(nonVolatileSettings.DMR_RxAGC, 1);
					}
					break;
#if defined(PLATFORM_MD9600)
				case OPTIONS_SPEAKER_CLICK_SUPPRESS:
					settingsSetOptionBit(BIT_SPEAKER_CLICK_SUPPRESS, true);
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
				case OPTIONS_MENU_TIMEOUT_BEEP:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if (nonVolatileSettings.txTimeoutBeepX5Secs > 0)
						{
							settingsDecrement(nonVolatileSettings.txTimeoutBeepX5Secs, 1);
						}
					}
					break;
				case OPTIONS_MENU_BEEP_VOLUME:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if (nonVolatileSettings.beepVolumeDivider < 10)
						{
							settingsIncrement(nonVolatileSettings.beepVolumeDivider, 1);
						}
					}
					break;
				case OPTIONS_MENU_DMR_BEEP:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if ((nonVolatileSettings.beepOptions & 0x03) > BEEP_TX_NONE)
						{
							uint8_t opts = (nonVolatileSettings.beepOptions >> 2);
							uint8_t v = (nonVolatileSettings.beepOptions & 0x03) - 1;

							opts = ((opts << 2) | v);

							settingsSet(nonVolatileSettings.beepOptions, opts);
						}
					}
					break;
				case OPTIONS_MENU_RX_BEEP:
					if (nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT)
					{
						if ((nonVolatileSettings.beepOptions & 0x0C) >= BEEP_RX_CARRIER)
						{
							uint8_t opts = (nonVolatileSettings.beepOptions >> 4);
							uint8_t txBeeps = (nonVolatileSettings.beepOptions & 0x03);
							uint8_t v = ((nonVolatileSettings.beepOptions >> 2) & 0x03) - 1;

							opts = ((opts << 4) | (v << 2) | txBeeps);

							settingsSet(nonVolatileSettings.beepOptions, opts);
						}
					}
					break;
				case OPTIONS_MENU_RX_TALKER_BEGIN_BEEP:
					if ((nonVolatileSettings.audioPromptMode != AUDIO_PROMPT_MODE_SILENT) &&
							((nonVolatileSettings.beepOptions & BEEP_RX_TALKER) != 0))
					{
						if (nonVolatileSettings.beepOptions & BEEP_RX_TALKER_BEGIN)
						{
							uint8_t opts = nonVolatileSettings.beepOptions;

							opts &= ~BEEP_RX_TALKER_BEGIN;

							settingsSet(nonVolatileSettings.beepOptions, opts);
						}
					}
					break;
				case OPTIONS_MIC_GAIN_DMR: // DMR Mic gain
					if (nonVolatileSettings.micGainDMR > 0)
					{
						settingsDecrement(nonVolatileSettings.micGainDMR, 1);
						HRC6000SetMicGainDMR(nonVolatileSettings.micGainDMR);
					}
					break;
				case OPTIONS_MIC_GAIN_FM: // FM Mic gain
					if (nonVolatileSettings.micGainFM > FM_MIC_GAIN_MIN)
					{
						settingsDecrement(nonVolatileSettings.micGainFM, 1);
						trxSetMicGainFM(nonVolatileSettings.micGainFM);
					}
					break;
				case OPTIONS_VOX_THRESHOLD:
					// threshold of 1 is too low. So only allow the value to go down to 2.
					if (nonVolatileSettings.voxThreshold > 2)
					{
						settingsDecrement(nonVolatileSettings.voxThreshold, 1);
						voxSetParameters(nonVolatileSettings.voxThreshold, nonVolatileSettings.voxTailUnits);
					}
					break;
				case OPTIONS_VOX_TAIL:
					if (nonVolatileSettings.voxTailUnits > 1) // 500ms tail minimum.
					{
						settingsDecrement(nonVolatileSettings.voxTailUnits, 1);
						voxSetParameters(nonVolatileSettings.voxThreshold, nonVolatileSettings.voxTailUnits);
					}
					break;
				case OPTIONS_AUDIO_PROMPT_MODE:
					if (nonVolatileSettings.audioPromptMode > AUDIO_PROMPT_MODE_SILENT)
					{
						// Stop the voice prompt playback as soon as the level is set to 'Beep'
						if ((nonVolatileSettings.audioPromptMode == AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && voicePromptsIsPlaying())
						{
							voicePromptsTerminate();
						}

						settingsDecrement(nonVolatileSettings.audioPromptMode, 1);
					}
					break;
				case OPTIONS_AUDIO_DMR_RX_AGC:
					if (nonVolatileSettings.DMR_RxAGC > 0)
					{
						settingsDecrement(nonVolatileSettings.DMR_RxAGC, 1);

						if (nonVolatileSettings.DMR_RxAGC == 0)
						{
#if defined(PLATFORM_MD9600) || defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
							HRC6000SetDmrRxGain(0);
#else
							HRC6000SetDmrAGCGain(0);
#endif
						}
					}
					break;
#if defined(PLATFORM_MD9600)
				case OPTIONS_SPEAKER_CLICK_SUPPRESS:
					settingsSetOptionBit(BIT_SPEAKER_CLICK_SUPPRESS, false);
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
		uiQuickKeysStore(ev, &menuSoundExitCode);
		isDirty = true;
	}

	if (isDirty)
	{
		updateScreen(false);
	}
}

static void applySettings(void)
{
	// All parameters has already been applied
	settingsSaveIfNeeded(true);
	resetOriginalSettingsData();
}

static void exitCallback(void *data)
{
	if (originalNonVolatileSettings.magicNumber != 0xDEADBEEF)
	{
		// Restore original settings.
		memcpy(&nonVolatileSettings, &originalNonVolatileSettings, sizeof(settingsStruct_t));
		soundBeepVolumeDivider = nonVolatileSettings.beepVolumeDivider;
		HRC6000SetMicGainDMR(nonVolatileSettings.micGainDMR);
		trxSetMicGainFM(nonVolatileSettings.micGainFM);
		voxSetParameters(nonVolatileSettings.voxThreshold, nonVolatileSettings.voxTailUnits);
		settingsSaveIfNeeded(true);
		resetOriginalSettingsData();
	}
}
