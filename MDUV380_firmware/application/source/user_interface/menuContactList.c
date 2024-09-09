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

typedef enum
{
	MENU_CONTACT_LIST_DISPLAY = 0,
	MENU_CONTACT_LIST_CONFIRM,
	MENU_CONTACT_LIST_DELETED,
	MENU_CONTACT_LIST_TG_IN_RXGROUP
} contactListState_t;

typedef enum
{
	MENU_CONTACT_LIST_CONTACT_DIGITAL = 0,
	MENU_CONTACT_LIST_CONTACT_DTMF
} contactListContactType_t;


static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);

static struct_codeplugContact_t     contact;
static struct_codeplugDTMFContact_t dtmfContact;

static contactListContactType_t contactListType = MENU_CONTACT_LIST_CONTACT_DIGITAL;
static uint32_t contactCallType = CONTACT_CALLTYPE_TG;
static contactListState_t contactListDisplayState = MENU_CONTACT_LIST_DISPLAY;
static contactListState_t contactListOverrideState = MENU_CONTACT_LIST_DISPLAY;
static int menuContactListTimeout = 0; // Action result screen autohide timeout (or it will instantly disappear if RED or GREEN is pressed)
static menuStatus_t menuContactListExitCode = MENU_STATUS_SUCCESS;
static menuStatus_t menuContactListSubMenuExitCode = MENU_STATUS_SUCCESS;


static const char *calltypeVoices[3] = { NULL, NULL, NULL };

// Apply contact + its TS on selection for TX (contact list of quick list).
static void overrideWithSelectedContact(void)
{
	menuPrivateCallClear();
	setOverrideTGorPC(contactListContactData.tgNumber, (contactListContactData.callType == CONTACT_CALLTYPE_PC));
	// Contact has a TS override set
	if ((contactListContactData.reserve1 & CODEPLUG_CONTACT_FLAG_NO_TS_OVERRIDE) == 0x00)
	{
		int ts = ((contactListContactData.reserve1 & CODEPLUG_CONTACT_FLAG_TS_OVERRIDE_TIMESLOT_MASK) >> 1);
		trxSetDMRTimeSlot(ts, true);
		tsSetManualOverride(((menuSystemGetRootMenuNumber() == UI_CHANNEL_MODE) ? CHANNEL_CHANNEL : (CHANNEL_VFO_A + nonVolatileSettings.currentVFONumber)), (ts + 1));
	}
}

static void reloadContactList(contactListContactType_t type)
{
	menuDataGlobal.numItems = (type == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? codeplugContactsGetCount(contactCallType) : codeplugDTMFContactsGetCount();

	if (menuDataGlobal.numItems > 0)
	{
		if (menuDataGlobal.currentItemIndex >= menuDataGlobal.numItems)
		{
			menuDataGlobal.currentItemIndex = 0;
		}
		uiDataGlobal.currentSelectedContactIndex = (type == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				? codeplugContactGetDataForNumberInType(menuDataGlobal.currentItemIndex + 1, contactCallType, &contactListContactData)
				: codeplugDTMFContactGetDataForNumber(menuDataGlobal.currentItemIndex + 1, &contactListDTMFContactData);
	}
	else
	{
		uiDataGlobal.currentSelectedContactIndex = 0;
	}
}

menuStatus_t menuContactList(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		calltypeVoices[0] = currentLanguage->group_call;
		calltypeVoices[1] = currentLanguage->private_call;
		calltypeVoices[2] = currentLanguage->all_call;

		// Do not override the timeout on error (e.g. "already in tg list")
		if (contactListOverrideState != MENU_CONTACT_LIST_TG_IN_RXGROUP)
		{
			menuContactListTimeout = 0;
		}

		if (contactListOverrideState == MENU_CONTACT_LIST_DISPLAY)
		{
			if (uiDataGlobal.currentSelectedContactIndex == 0)
			{
				int currentMenu = menuSystemGetCurrentMenuNumber();

				// Shows digital contact list if called from "contact list" menu entry, or from <SK2>+# in digital.
				// Otherwise displays DTMF contact list
				contactListType = ((currentMenu == MENU_CONTACT_LIST) || ((currentMenu == MENU_CONTACT_QUICKLIST) && (trxGetMode() != RADIO_MODE_ANALOG))) ? MENU_CONTACT_LIST_CONTACT_DIGITAL : MENU_CONTACT_LIST_CONTACT_DTMF;
				contactCallType = CONTACT_CALLTYPE_TG;

				dtmfSequenceReset();
			}
			else
			{
				if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				{
					codeplugContactGetDataForIndex(uiDataGlobal.currentSelectedContactIndex, &contactListContactData);
					contactCallType = contactListContactData.callType;
				}
				else
				{
					codeplugDTMFContactGetDataForIndex(uiDataGlobal.currentSelectedContactIndex, &contactListDTMFContactData);
				}
			}

			reloadContactList(contactListType);
			contactListDisplayState = MENU_CONTACT_LIST_DISPLAY;

			voicePromptsInit();
			if (menuSystemGetCurrentMenuNumber() == MENU_CONTACT_QUICKLIST)
			{
				if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				{
					voicePromptsAppendLanguageString(currentLanguage->dmr_contacts);
					voicePromptsAppendPrompt(PROMPT_SILENCE);
					voicePromptsAppendLanguageString(calltypeVoices[contactCallType]);
				}
				else
				{
					voicePromptsAppendLanguageString(currentLanguage->dtmf_contact_list);
				}
				voicePromptsAppendPrompt(PROMPT_SILENCE);
			}
			else
			{
				if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				{
					voicePromptsAppendLanguageString(calltypeVoices[contactCallType]);
					voicePromptsAppendPrompt(PROMPT_SILENCE);
				}
			}
		}
		else
		{
			if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
			{
				codeplugContactGetDataForIndex(uiDataGlobal.currentSelectedContactIndex, &contactListContactData);
			}
			else
			{
				codeplugDTMFContactGetDataForIndex(uiDataGlobal.currentSelectedContactIndex, &contactListDTMFContactData);
			}

			contactListDisplayState = contactListOverrideState;
			contactListOverrideState = MENU_CONTACT_LIST_DISPLAY;

		}

		updateScreen(true);
		menuContactListExitCode = (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuContactListExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent || (menuContactListTimeout > 0))
		{
			handleEvent(ev);
		}
	}

	// Play the DTMF contact sequence from Quicklist
	dtmfSequenceTick(false);

	return menuContactListExitCode;
}

static void updateScreen(bool isFirstRun)
{
	char nameBuf[33];
	int mNum;
	int idx;
	const char *calltypeName[] = { currentLanguage->group_call, currentLanguage->private_call, currentLanguage->all_call, "DTMF" };

	displayClearBuf();

	switch (contactListDisplayState)
	{
		case MENU_CONTACT_LIST_DISPLAY:
			menuDisplayTitle((char *) calltypeName[((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? contactCallType : 3)]);

			if (menuDataGlobal.numItems == 0)
			{
				displayThemeApply(THEME_ITEM_FG_WARNING_NOTIFICATION, THEME_ITEM_BG);
				displayPrintCentered((DISPLAY_SIZE_Y / 2), currentLanguage->list_empty, FONT_SIZE_3);
				displayThemeResetToDefault();

				voicePromptsAppendLanguageString(currentLanguage->list_empty);
			}
			else
			{
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

					idx = (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
							? codeplugContactGetDataForNumberInType(mNum + 1, contactCallType, &contact)
							: codeplugDTMFContactGetDataForNumber(mNum + 1, &dtmfContact);

					if (idx > 0)
					{
						codeplugUtilConvertBufToString(((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? contact.name : dtmfContact.name), nameBuf, 16); // need to convert to zero terminated string
						menuDisplayEntry(i, mNum, (char*) nameBuf, 0, THEME_ITEM_FG_CHANNEL_CONTACT, THEME_ITEM_COLOUR_NONE, THEME_ITEM_BG);
					}

					if (i == 0)
					{
						if (strlen(nameBuf))
						{
							voicePromptsAppendString(nameBuf);
						}
						else
						{
							voicePromptsAppendLanguageString(currentLanguage->name);
							voicePromptsAppendPrompt(PROMPT_SILENCE);
							voicePromptsAppendLanguageString(currentLanguage->none);
						}
					}
				}

			}
			promptsPlayNotAfterTx();
			break;

		case MENU_CONTACT_LIST_CONFIRM:
			codeplugUtilConvertBufToString(contactListContactData.name, nameBuf, 16);
			menuDisplayTitle(nameBuf);
			displayPrintCentered(16, currentLanguage->delete_contact_qm, FONT_SIZE_3);
			displayDrawChoice(CHOICE_YESNO, false);
			break;

		case MENU_CONTACT_LIST_DELETED:
			codeplugUtilConvertBufToString(contactListContactData.name, nameBuf, 16);
			displayPrintCentered(16, currentLanguage->contact_deleted, FONT_SIZE_3);
			displayDrawChoice(CHOICE_DISMISS, false);
			break;

		case MENU_CONTACT_LIST_TG_IN_RXGROUP:
			codeplugUtilConvertBufToString(contactListContactData.name, nameBuf, 16);
			menuDisplayTitle(nameBuf);
			displayPrintCentered(16, currentLanguage->contact_used, FONT_SIZE_3);
			displayPrintCentered((DISPLAY_SIZE_Y/2), currentLanguage->in_tg_list, FONT_SIZE_3);
			displayDrawChoice(CHOICE_DISMISS, false);
			break;
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

	if ((ev->events & FUNCTION_EVENT) && (ev->function == FUNC_REDRAW))
	{
		updateScreen(false);
		return;
	}

	// DTMF sequence is playing, stop it.
	if (dtmfSequenceIsKeying() && ((ev->keys.key != 0) || BUTTONCHECK_DOWN(ev, BUTTON_PTT)
#if ! defined(PLATFORM_RD5R)
													|| BUTTONCHECK_DOWN(ev, BUTTON_ORANGE)
#endif
	))
	{
		dtmfSequenceStop();
		keyboardReset();
		return;
	}

	switch (contactListDisplayState)
	{
		case MENU_CONTACT_LIST_DISPLAY:
			if (KEYCHECK_PRESS(ev->keys, KEY_DOWN))
			{
				menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, menuDataGlobal.numItems);
				uiDataGlobal.currentSelectedContactIndex = (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
						? codeplugContactGetDataForNumberInType(menuDataGlobal.currentItemIndex + 1, contactCallType, &contactListContactData)
						: codeplugDTMFContactGetDataForNumber(menuDataGlobal.currentItemIndex + 1, &contactListDTMFContactData);
				voicePromptsInit();
				updateScreen(false);
				menuContactListExitCode |= MENU_STATUS_LIST_TYPE;
			}
			else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
			{
				menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, menuDataGlobal.numItems);
				uiDataGlobal.currentSelectedContactIndex = (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
						? codeplugContactGetDataForNumberInType(menuDataGlobal.currentItemIndex + 1, contactCallType, &contactListContactData)
						: codeplugDTMFContactGetDataForNumber(menuDataGlobal.currentItemIndex + 1, &contactListDTMFContactData);
				voicePromptsInit();
				updateScreen(false);
				menuContactListExitCode |= MENU_STATUS_LIST_TYPE;
			}
			else if (KEYCHECK_SHORTUP(ev->keys, KEY_HASH))
			{
				if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				{
					contactCallType = (contactCallType + 1) % (CONTACT_CALLTYPE_ALL + 1);
					reloadContactList(contactListType);

					voicePromptsInit();
					voicePromptsAppendLanguageString(calltypeVoices[contactCallType]);
					voicePromptsAppendPrompt(PROMPT_SILENCE);

					updateScreen(false);
				}
			}
			else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
			{
				int currentMenu = menuSystemGetCurrentMenuNumber();
				int currentMode = trxGetMode();

				if (currentMenu == MENU_CONTACT_QUICKLIST)
				{
					if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
					{
						if (menuDataGlobal.numItems > 0)
						{
							if (currentMode == RADIO_MODE_DIGITAL)
							{
								overrideWithSelectedContact();
								uiDataGlobal.currentSelectedContactIndex = 0;
								announceItem(PROMPT_SEQUENCE_CONTACT_TG_OR_PC, PROMPT_THRESHOLD_3);
								menuSystemPopAllAndDisplayRootMenu();
							}
							else
							{
								menuContactListExitCode |= MENU_STATUS_ERROR;
							}
						}
						return;
					}
					else // MENU_CONTACT_LIST_CONTACT_DTMF
					{
						if (currentMode == RADIO_MODE_ANALOG)
						{
							dtmfSequencePrepare(contactListDTMFContactData.code, true);
						}
						else
						{
							menuContactListExitCode |= MENU_STATUS_ERROR;
						}
						return;
					}
				}

				// Display submenu for DTMF contact list
				if (menuDataGlobal.numItems > 0) // display action list only if contact list is non empty
				{
					if ((currentMenu == MENU_CONTACT_LIST) ||
							((currentMenu == MENU_DTMF_CONTACT_LIST) && (currentMode == RADIO_MODE_ANALOG)))
					{
						menuSystemPushNewMenu(MENU_CONTACT_LIST_SUBMENU);
					}
					else
					{
						menuContactListExitCode |= MENU_STATUS_ERROR;
					}
				}
				return;
			}
			else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
			{
				uiDataGlobal.currentSelectedContactIndex = 0;
				menuSystemPopPreviousMenu();
				return;
			}

			if ((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) &&
					(KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2))))
			{
				saveQuickkeyContactIndex(ev->keys.key, (uint16_t)contactListContactData.NOT_IN_CODEPLUGDATA_indexNumber);
				return;
			}

			break;

		case MENU_CONTACT_LIST_CONFIRM:
			if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
			{
				memset(contact.name, 0xff, 16);
				contact.tgNumber = 0;
				contact.callType = 0xff;
				codeplugContactSaveDataForIndex(uiDataGlobal.currentSelectedContactIndex, &contact);
				uiDataGlobal.currentSelectedContactIndex = 0;
				menuContactListTimeout = 2000;
				contactListDisplayState = MENU_CONTACT_LIST_DELETED;
				reloadContactList(contactListType);
				updateScreen(false);
				voicePromptsAppendLanguageString(currentLanguage->contact_deleted);
				voicePromptsPlay();
			}
			else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
			{
				contactListDisplayState = MENU_CONTACT_LIST_DISPLAY;
				voicePromptsInit(); // flush the VP buffer
				reloadContactList(contactListType);
				updateScreen(false);
			}
			break;

		case MENU_CONTACT_LIST_DELETED:
		case MENU_CONTACT_LIST_TG_IN_RXGROUP:
			// Countdown after the VP has finished to play, if VP is enabled.
			if (((nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && (voicePromptsIsPlaying() == false)) ||
					(nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD))
			{
				menuContactListTimeout -= ((menuContactListTimeout > 0) ? 1 : 0);
			}

			if ((menuContactListTimeout == 0) || KEYCHECK_SHORTUP(ev->keys, KEY_GREEN) || KEYCHECK_SHORTUP(ev->keys, KEY_RED))
			{
				menuContactListTimeout = 0;
				contactListDisplayState = MENU_CONTACT_LIST_DISPLAY;
				voicePromptsInit(); // flush the VP buffer
				reloadContactList(contactListType);
				updateScreen(false);
			}
			break;
	}
}

enum CONTACT_LIST_QUICK_MENU_ITEMS
{
	CONTACT_LIST_QUICK_MENU_SELECT = 0,
	CONTACT_LIST_QUICK_MENU_EDIT,
	CONTACT_LIST_QUICK_MENU_DELETE,
	NUM_CONTACT_LIST_QUICK_MENU_ITEMS    // The last item in the list is used so that we automatically get a total number of items in the list
};

static void updateSubMenuScreen(void)
{
	int mNum = 0;
	char buf[SCREEN_LINE_BUFFER_SIZE];
	const char *langTextConst = NULL;// initialise to please the compiler

	voicePromptsInit();

	displayClearBuf();

	codeplugUtilConvertBufToString((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? contactListContactData.name : contactListDTMFContactData.name, buf, 16);
	menuDisplayTitle(buf);

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		mNum = menuGetMenuOffset(((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? NUM_CONTACT_LIST_QUICK_MENU_ITEMS : 1), i);
		if (mNum == MENU_OFFSET_BEFORE_FIRST_ENTRY)
		{
			continue;
		}
		else if (mNum == MENU_OFFSET_AFTER_LAST_ENTRY)
		{
			break;
		}

		buf[0] = 0;

		switch(mNum)
		{
			case CONTACT_LIST_QUICK_MENU_SELECT:
				langTextConst = currentLanguage->select_tx;
				break;

			case CONTACT_LIST_QUICK_MENU_EDIT:
				langTextConst = (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? currentLanguage->edit_contact : NULL;
				break;

			case CONTACT_LIST_QUICK_MENU_DELETE:
				langTextConst = (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? currentLanguage->delete_contact : NULL;
				break;
		}

		if (langTextConst != NULL)
		{
			strncpy(buf, langTextConst, SCREEN_LINE_BUFFER_SIZE);
		}
		else
		{
			strncpy(buf, " ", SCREEN_LINE_BUFFER_SIZE);
		}

		if ((i == 0) && (langTextConst != NULL))
		{
			voicePromptsAppendLanguageString(langTextConst);
			promptsPlayNotAfterTx();
		}

		menuDisplayEntry(i, mNum, buf, 0, THEME_ITEM_FG_MENU_ITEM, THEME_ITEM_COLOUR_NONE, THEME_ITEM_BG);
	}

	displayRender();
}

static void handleSubMenuEvent(uiEvent_t *ev)
{
	if (ev->events & BUTTON_EVENT)
	{
		if (repeatVoicePromptOnSK1(ev))
		{
			return;
		}
	}

	if ((ev->events & FUNCTION_EVENT) && (ev->function == FUNC_REDRAW))
	{
		updateSubMenuScreen();
		return;
	}

	// DTMF sequence is playing, stop it.
	if (dtmfSequenceIsKeying() && ((ev->keys.key != 0) || BUTTONCHECK_DOWN(ev, BUTTON_PTT)
#if ! defined(PLATFORM_RD5R)
													|| BUTTONCHECK_DOWN(ev, BUTTON_ORANGE)
#endif
	))
	{
		dtmfSequenceStop();
		keyboardReset();
		return;
	}

	if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		menuSystemPopPreviousMenu();
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
	{
		contactListOverrideState = MENU_CONTACT_LIST_DISPLAY;
		switch (menuDataGlobal.currentItemIndex)
		{
			case CONTACT_LIST_QUICK_MENU_SELECT:
				if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				{
					if (trxGetMode() == RADIO_MODE_DIGITAL)
					{
						overrideWithSelectedContact();
						uiDataGlobal.currentSelectedContactIndex = 0;
						announceItem(PROMPT_SEQUENCE_CONTACT_TG_OR_PC, PROMPT_THRESHOLD_3);
						uiDataGlobal.VoicePrompts.inhibitInitial = true;
						menuSystemPopAllAndDisplayRootMenu();
					}
					else
					{
						menuContactListSubMenuExitCode |= MENU_STATUS_ERROR;
					}
					return;
				}
				else
				{
					dtmfSequencePrepare(contactListDTMFContactData.code, true);
				}
				break;

			case CONTACT_LIST_QUICK_MENU_EDIT:
				if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
				{
					menuSystemSetCurrentMenu(MENU_CONTACT_DETAILS);
				}
				break;

			case CONTACT_LIST_QUICK_MENU_DELETE:
				if (uiDataGlobal.currentSelectedContactIndex > 0)
				{
					if (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL)
					{
						voicePromptsInit();
						if ((contactListContactData.callType == CONTACT_CALLTYPE_TG) &&
								codeplugContactGetRXGroup(contactListContactData.NOT_IN_CODEPLUGDATA_indexNumber))
						{
							menuContactListTimeout = 2000;
							contactListOverrideState = MENU_CONTACT_LIST_TG_IN_RXGROUP;
							voicePromptsAppendLanguageString(currentLanguage->contact_used);
							voicePromptsAppendLanguageString(currentLanguage->in_tg_list);
						}
						else
						{
							contactListOverrideState = MENU_CONTACT_LIST_CONFIRM;
							voicePromptsAppendLanguageString(currentLanguage->delete_contact_qm);
						}
						voicePromptsPlay();
					}
					menuSystemPopPreviousMenu();
				}
				break;
		}
	}
	else if (KEYCHECK_PRESS(ev->keys, KEY_DOWN))
	{
		menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, ((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? NUM_CONTACT_LIST_QUICK_MENU_ITEMS : 1));
		updateSubMenuScreen();
	}
	else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
	{
		menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, ((contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) ? NUM_CONTACT_LIST_QUICK_MENU_ITEMS : 1));
		updateSubMenuScreen();
	}

	if ((menuDataGlobal.currentItemIndex == CONTACT_LIST_QUICK_MENU_SELECT) && (contactListType == MENU_CONTACT_LIST_CONTACT_DIGITAL) &&
			(KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2))))
	{
		saveQuickkeyContactIndex(ev->keys.key, (uint16_t)contactListContactData.NOT_IN_CODEPLUGDATA_indexNumber);
		return;
	}

}

menuStatus_t menuContactListSubMenu(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.currentItemIndex = 0;
		updateSubMenuScreen();
		keyboardInit();
		menuContactListSubMenuExitCode = (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuContactListSubMenuExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent)
		{
			handleSubMenuEvent(ev);
		}
	}

	dtmfSequenceTick(true);

	return menuContactListSubMenuExitCode;
}
