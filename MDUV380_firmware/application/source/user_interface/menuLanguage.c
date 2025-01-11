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

#define CHAR_CONSTANTS_ONLY // Needed to get FONT_CHAR_* glyph offsets
#if defined(PLATFORM_MD9600)

		#include "hardware/ST7567_charset.h"

#elif (defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017))

		#include "hardware/HX8353E_charset.h"

#else

		#include "hardware/UC1701_charset.h"

#endif

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);
static menuStatus_t menuLanguageExitCode = MENU_STATUS_SUCCESS;



menuStatus_t menuLanguage(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.numItems = 2;

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->language);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		updateScreen(true);
		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuLanguageExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
	}
	return menuLanguageExitCode;
}

static void updateScreen(bool isFirstRun)
{
	int mNum = 0;

	displayClearBuf();
	menuDisplayTitle(currentLanguage->language);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		mNum = menuGetMenuOffset(languagesGetCount(), i);
		if (mNum == MENU_OFFSET_BEFORE_FIRST_ENTRY)
		{
			continue;
		}
		else if (mNum == MENU_OFFSET_AFTER_LAST_ENTRY)
		{
			break;
		}

		if (mNum < languagesGetCount())
		{
			menuDisplayEntry(i, mNum, (char *)languages[mNum].LANGUAGE_NAME, -1, THEME_ITEM_FG_MENU_ITEM, THEME_ITEM_FG_OPTIONS_VALUE, THEME_ITEM_BG);

			if (i == 0)
			{
				if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
				{
					char buffer[17];

					snprintf(buffer, 17, "%s", (char *)languages[mNum].LANGUAGE_NAME);

					if (isFirstRun == false)
					{
						voicePromptsInit();
					}

					voicePromptsAppendString(buffer);
					promptsPlayNotAfterTx();
				}
			}
		}
	}

	displayRender();
}

static void handleEvent(uiEvent_t *ev)
{
	if (ev->events & BUTTON_EVENT)
	{
		if (repeatVoicePromptOnSK1(ev))
		{
			return;
		}
	}

	if (ev->events & FUNCTION_EVENT)
	{
		if (ev->function == FUNC_REDRAW)
		{
			updateScreen(false);
		}
		else if ((QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU) && (QUICKKEY_ENTRYID(ev->function) < languagesGetCount()))
		{
			menuDataGlobal.currentItemIndex = QUICKKEY_ENTRYID(ev->function);
			settingsSetOptionBit(BIT_SECONDARY_LANGUAGE, ((menuDataGlobal.currentItemIndex > 0) ? true : false));
			currentLanguage = &languages[(settingsIsOptionBitSet(BIT_SECONDARY_LANGUAGE) ? 1 : 0)];
			settingsSaveIfNeeded(true);
			menuSystemLanguageHasChanged();
			menuSystemPopAllAndDisplayRootMenu();
		}
		return;
	}

	if (KEYCHECK_PRESS(ev->keys, KEY_DOWN) && (menuDataGlobal.numItems != 0))
	{
		menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, languagesGetCount());
		updateScreen(false);
		menuLanguageExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
	{
		menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, languagesGetCount());
		updateScreen(false);
		menuLanguageExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
	{
		settingsSetOptionBit(BIT_SECONDARY_LANGUAGE, ((menuDataGlobal.currentItemIndex > 0) ? true : false));
		currentLanguage = &languages[(settingsIsOptionBitSet(BIT_SECONDARY_LANGUAGE) ? 1 : 0)];
		settingsSaveIfNeeded(true);
		menuSystemLanguageHasChanged();
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
		saveQuickkeyMenuIndex(ev->keys.key, menuSystemGetCurrentMenuNumber(), menuDataGlobal.currentItemIndex, 0);
		return;
	}
}
