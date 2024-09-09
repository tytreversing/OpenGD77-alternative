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

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);

static menuStatus_t menuDisplayListExitCode = MENU_STATUS_SUCCESS;

menuStatus_t menuDisplayMenuList(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		const char *menuName = NULL;
		int currentMenuNumber = menuSystemGetCurrentMenuNumber();

		menuDataGlobal.currentMenuList = (menuItemNewData_t *)menuDataGlobal.data[currentMenuNumber]->items;
		menuDataGlobal.numItems = menuDataGlobal.data[currentMenuNumber]->numItems;

		if ((currentMenuNumber != MENU_MAIN_MENU) && (menuDataGlobal.controlData.stackPosition >= 1))
		{
			if (menuDataGlobal.data[menuDataGlobal.controlData.stack[menuDataGlobal.controlData.stackPosition - 1]] != NULL)
			{
				int lastIndex = menuSystemGetLastItemIndex(menuDataGlobal.controlData.stackPosition - 1);

				if (lastIndex != -1)
				{
					menuName = (currentLanguage->LANGUAGE_NAME +
							(menuDataGlobal.data[menuDataGlobal.controlData.stack[menuDataGlobal.controlData.stackPosition - 1]]->items[lastIndex].stringOffset * LANGUAGE_TEXTS_LENGTH));
				}
			}
		}

		voicePromptsInit();
		if (menuName)
		{
			voicePromptsAppendPrompt(PROMPT_SILENCE);
			voicePromptsAppendLanguageString((const char *)menuName);
		}
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		updateScreen(true);

		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuDisplayListExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
	}
	return menuDisplayListExitCode;
}

static void updateScreen(bool isFirstRun)
{
	int mNum;
	const char *mName = currentLanguage->menu;

	displayClearBuf();

	// Apply some menu title override(s)
	switch (menuSystemGetCurrentMenuNumber())
	{
		case MENU_CONTACTS_MENU:
			mName = currentLanguage->contacts;
			break;
		case MENU_OPTIONS:
			mName = currentLanguage->options;
			break;
	}

	menuDisplayTitle(mName);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		mNum = menuGetMenuOffset(menuDataGlobal.numItems, i);
		if (mNum == MENU_OFFSET_BEFORE_FIRST_ENTRY)
		{
			continue;
		}
		else if (mNum == MENU_OFFSET_AFTER_LAST_ENTRY)
		{
			break;
		}

		if (mNum < menuDataGlobal.numItems)
		{
			if (menuDataGlobal.currentMenuList[mNum].stringOffset >= 0)
			{
				const char *menuName = (currentLanguage->LANGUAGE_NAME + (menuDataGlobal.currentMenuList[mNum].stringOffset * LANGUAGE_TEXTS_LENGTH));
				menuDisplayEntry(i, mNum, menuName, 0, THEME_ITEM_FG_MENU_ITEM, THEME_ITEM_COLOUR_NONE, THEME_ITEM_BG);

				if (i == 0)
				{
					if (!isFirstRun)
					{
						voicePromptsInit();
					}

					voicePromptsAppendLanguageString(menuName);
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
		else if ((QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU) && (QUICKKEY_ENTRYID(ev->function) < menuDataGlobal.numItems))
		{
			menuDataGlobal.currentItemIndex = QUICKKEY_ENTRYID(ev->function);
			updateScreen(false);
		}
		return;
	}

	if (KEYCHECK_PRESS(ev->keys, KEY_DOWN))
	{
		menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, menuDataGlobal.numItems);
		updateScreen(false);
		menuDisplayListExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
	{
		menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, menuDataGlobal.numItems);
		updateScreen(false);
		menuDisplayListExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
	{
		if (menuDataGlobal.currentMenuList[menuDataGlobal.currentItemIndex].menuNum != -1)
		{
			menuSystemPushNewMenu(menuDataGlobal.currentMenuList[menuDataGlobal.currentItemIndex].menuNum);
		}
		return;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		menuSystemPopPreviousMenu();
		return;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_STAR) && (menuSystemGetCurrentMenuNumber() == MENU_MAIN_MENU))
	{
		keypadLocked = true;
		menuSystemPopAllAndDisplayRootMenu();
		menuSystemPushNewMenu(UI_LOCK_SCREEN);
		return;
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_HASH) && (menuSystemGetCurrentMenuNumber() == MENU_MAIN_MENU))
	{
		PTTLocked = !PTTLocked;
		menuSystemPopAllAndDisplayRootMenu();
		menuSystemPushNewMenu(UI_LOCK_SCREEN);
		return;
	}
	else if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2)))
	{
		if (menuDataGlobal.currentMenuList[menuDataGlobal.currentItemIndex].menuNum != -1)
		{
			saveQuickkeyMenuIndex(ev->keys.key, menuDataGlobal.currentMenuList[menuDataGlobal.currentItemIndex].menuNum, 0, 0);
		}
		return;
	}
}
