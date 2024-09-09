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
#include "user_interface/uiUtilities.h"
#include "user_interface/uiLocalisation.h"

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);
static void setZoneToUserSelection(void);

static menuStatus_t menuZoneExitCode = MENU_STATUS_SUCCESS;

menuStatus_t menuZoneList(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.numItems = codeplugZonesGetCount();
		menuDataGlobal.currentItemIndex = nonVolatileSettings.currentZone;

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->zone);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		updateScreen(true);
		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuZoneExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
	}
	return menuZoneExitCode;
}

static void updateScreen(bool isFirstRun)
{
	char nameBuf[17];
	int mNum;
	struct_codeplugZone_t zoneBuf;

	displayClearBuf();
	menuDisplayTitle(currentLanguage->zones);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		if (menuDataGlobal.numItems <= (i + 1))
		{
			break;
		}

		mNum = menuGetMenuOffset(menuDataGlobal.numItems, i);
		if (mNum == MENU_OFFSET_BEFORE_FIRST_ENTRY)
		{
			continue;
		}
		else if (mNum == MENU_OFFSET_AFTER_LAST_ENTRY)
		{
			break;
		}

		codeplugZoneGetDataForNumber(mNum, &zoneBuf);
		codeplugUtilConvertBufToString(zoneBuf.name, nameBuf, 16);// need to convert to zero terminated string

		menuDisplayEntry(i, mNum, (char *)nameBuf, 0, THEME_ITEM_FG_ZONE_NAME, THEME_ITEM_COLOUR_NONE, THEME_ITEM_BG);

		if (i == 0)
		{
			if (!isFirstRun)
			{
				voicePromptsInit();
			}

			if (strcmp(nameBuf,currentLanguage->all_channels) == 0)
			{
				voicePromptsAppendLanguageString(currentLanguage->all_channels);
			}
			else
			{
				voicePromptsAppendString(nameBuf);
			}

			promptsPlayNotAfterTx();
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
		else if ((QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU) && (QUICKKEY_LONGENTRYID(ev->function) > 0) && (QUICKKEY_LONGENTRYID(ev->function) <= menuDataGlobal.numItems))
		{
			menuDataGlobal.currentItemIndex = QUICKKEY_LONGENTRYID(ev->function) - 1;
			setZoneToUserSelection();
		}

		return;
	}

	if (KEYCHECK_PRESS(ev->keys, KEY_DOWN))
	{
		menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, menuDataGlobal.numItems);
		updateScreen(false);
		menuZoneExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
	{
		menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, menuDataGlobal.numItems);
		updateScreen(false);
		menuZoneExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
	{
		setZoneToUserSelection();
		return;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		menuSystemPopPreviousMenu();
		return;
	}
	else if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
	{
		saveQuickkeyMenuLongValue(ev->keys.key, menuSystemGetCurrentMenuNumber(), menuDataGlobal.currentItemIndex + 1);
		return;
	}
}


static void setZoneToUserSelection(void)
{
	settingsSet(nonVolatileSettings.overrideTG, 0); // remove any TG override
	settingsSet(nonVolatileSettings.currentZone, (int16_t) menuDataGlobal.currentItemIndex);
	settingsSet(nonVolatileSettings.currentIndexInTRxGroupList[SETTINGS_CHANNEL_MODE], 0);// Since we are switching zones the TRx Group index should be reset
	channelScreenChannelData.rxFreq = 0x00; // Flag to the Channel screen that the channel data is now invalid and needs to be reloaded

	settingsSaveIfNeeded(true);
	menuSystemPopAllAndDisplaySpecificRootMenu(UI_CHANNEL_MODE, true);
	uiDataGlobal.displayQSOState = QSO_DISPLAY_DEFAULT_SCREEN; // Force screen redraw
}
