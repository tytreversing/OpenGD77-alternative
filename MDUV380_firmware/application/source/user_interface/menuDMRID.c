#include "user_interface/uiGlobals.h"
#include "functions/trx.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiUtilities.h"
#include "user_interface/uiLocalisation.h"

static void updateScreen(bool isFirstRun, bool updateScreen);
static void updateCursor(bool moved);
static void handleEvent(uiEvent_t *ev);


static menuStatus_t menuDMRIDExitCode = MENU_STATUS_SUCCESS;


static char digits[9];
static char alias[12];
static int namePos;
static int numPos;
static uint32_t dmrID = 0;
static bool preview = false;


enum
{
	MENU_DMRID_VALUE = 0,
	MENU_ALIAS,
	NUM_DMRID_ITEMS
};


static int menuDMRIDTimeout;



menuStatus_t menuDMRID(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
        int currentMenuNumber = menuSystemGetCurrentMenuNumber();
		memset(digits, 0, 9);
        codeplugGetRadioName(alias);
        dmrID = codeplugGetUserDMRID();
        sprintf(digits, "%d", dmrID);
	    menuDataGlobal.currentMenuList = (menuItemNewData_t *)menuDataGlobal.data[currentMenuNumber]->items;
	    menuDataGlobal.numItems = menuDataGlobal.data[currentMenuNumber]->numItems;
		menuDataGlobal.currentItemIndex = MENU_DMRID_VALUE;
		menuDataGlobal.numItems = NUM_DMRID_ITEMS;
		updateScreen(isFirstRun, true);
		updateCursor(true);

		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuDMRIDExitCode = MENU_STATUS_SUCCESS;

		updateCursor(false);
		if (ev->hasEvent || (menuDMRIDTimeout > 0))
		{
			handleEvent(ev);
		}
	}
	return menuDMRIDExitCode;
}

static void updateCursor(bool moved)
{

    displayThemeApply(THEME_ITEM_BG, THEME_ITEM_BG_MENU_ITEM_SELECTED);

	switch (menuDataGlobal.currentItemIndex)
	{
		case MENU_ALIAS:
			menuUpdateCursor(namePos, moved, true);
			break;

		case MENU_DMRID_VALUE:
			menuUpdateCursor(numPos, moved, true);
			break;
	}

	displayThemeResetToDefault();

}

static void updateScreen(bool isFirstRun, bool allowedToSpeakUpdate)
{
	int mNum = 0;
	char buf[SCREEN_LINE_BUFFER_SIZE];
	const char *leftSide = NULL;// initialise to please the compiler
	char rightSideVar[SCREEN_LINE_BUFFER_SIZE];


	displayThemeResetToDefault();
	displayClearBuf();
	menuDisplayTitle(currentLanguage->dmrid);



	if (menuDataGlobal.currentItemIndex == MENU_ALIAS)
	{
		keypadAlphaEnable = true;
		onlyLatin = true;
	}
	else
	{
		keypadAlphaEnable = false;
	}

	for (int i = MENU_START_ITERATION_VALUE; i <= MENU_END_ITERATION_VALUE; i++)
	{
		mNum = menuGetMenuOffset(NUM_DMRID_ITEMS, i);
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

		rightSideVar[0] = 0;
		switch (mNum)
		{
			case MENU_ALIAS:
				leftSide = currentLanguage->aliastext;
				snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%s", alias);
				namePos = strlen(leftSide) + strlen(rightSideVar);
				if (!preview)
					namePos++;
				break;

			case MENU_DMRID_VALUE:
				leftSide = currentLanguage->dmridtext;
                snprintf(rightSideVar, SCREEN_LINE_BUFFER_SIZE, "%s", digits);
                numPos = strlen(leftSide) + 1 + strlen(rightSideVar);
				break;
		}
		snprintf(buf, SCREEN_LINE_BUFFER_SIZE, "%s:%s", leftSide, rightSideVar);
		menuDisplayEntry(i, mNum, buf, (strlen(leftSide) + 1 + strlen(rightSideVar)), THEME_ITEM_FG_MENU_ITEM, THEME_ITEM_FG_OPTIONS_VALUE, THEME_ITEM_BG);

	}

	displayRender();
}

static void handleEvent(uiEvent_t *ev)
{
	char dLen = strlen(digits);
	char sLen = strlen(alias);
    char promptLen = strlen(currentLanguage->aliastext) + 1;

	if (ev->events & BUTTON_EVENT)
	{
		if (repeatVoicePromptOnSK1(ev))
		{
			return;
		}
	}

	if ((ev->events & FUNCTION_EVENT) && (ev->function == FUNC_REDRAW))
	{
		updateScreen(false, false);
		return;
	}


	if (ev->events & KEY_EVENT)
		{
			if (KEYCHECK_PRESS(ev->keys, KEY_DOWN))
			{
				menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, NUM_DMRID_ITEMS);
				updateScreen(false, true);
				menuDMRIDExitCode |= MENU_STATUS_LIST_TYPE;
			}
			else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
			{
				menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, NUM_DMRID_ITEMS);
				updateScreen(false, true);
				menuDMRIDExitCode |= MENU_STATUS_LIST_TYPE;
			}

			else if (KEYCHECK_SHORTUP(ev->keys, KEY_RIGHT)
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
						|| KEYCHECK_SHORTUP(ev->keys, KEY_ROTARY_INCREMENT)
#endif
				)
			{
				switch(menuDataGlobal.currentItemIndex)
				{
					case MENU_ALIAS:
						moveCursorRightInString(alias, &namePos, 16, BUTTONCHECK_DOWN(ev, BUTTON_SK2));
						updateCursor(true);
						break;

					case MENU_DMRID_VALUE:
					    break;


					}
					updateScreen(false, false);
				}

			else if (KEYCHECK_SHORTUP(ev->keys, KEY_LEFT)
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
						|| KEYCHECK_SHORTUP(ev->keys, KEY_ROTARY_DECREMENT)
#endif
				)
			{
				switch(menuDataGlobal.currentItemIndex)
				{
					case MENU_ALIAS:
						if (sLen > 0)
						{
							alias[sLen - 1] = 0x00;
						}
						updateCursor(true);
						break;

					case MENU_DMRID_VALUE:
						if (dLen > 0)
						{
							digits[dLen - 1] = 0x00;
						}
						updateCursor(true);
						break;
				}
				updateScreen(false, false);
			}
			else if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN)) //сохраняем
			{
                    codeplugSetRadioName(alias);
                    codeplugSetUserDMRID(strtol(digits, NULL, 10));
                    uiDataGlobal.userDMRId = codeplugGetUserDMRID();
				    onlyLatin = false;
					soundSetMelody(MELODY_ACK_BEEP);
				    updateScreen(false, false);
				    uiNotificationShow(NOTIFICATION_TYPE_MESSAGE, NOTIFICATION_ID_MESSAGE, 1000, currentLanguage->contact_saved, true);
				    menuSystemPopPreviousMenu();
					return;
			}
			else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
			{
				onlyLatin = false;
				menuSystemPopPreviousMenu();
				return;
			}
			else
			{
				if (menuDataGlobal.currentItemIndex == MENU_DMRID_VALUE)
				{
					if (dLen < NUM_PC_OR_TG_DIGITS)
					{
						int keyval = menuGetKeypadKeyValue(ev, true);

						char c[2] = {0, 0};

						if (keyval != 99)
						{
							c[0] = keyval + '0';
							strcat(digits, c);
						}
						updateScreen(false, false);
					}

				}
				else if (menuDataGlobal.currentItemIndex == MENU_ALIAS)
				{
					if ((ev->keys.event == KEY_MOD_PREVIEW) && (namePos < 8 + promptLen))
					{
						preview = true;
						alias[namePos - promptLen] = ev->keys.key;
						updateScreen(false, false);
					}
					else if ((ev->keys.event == KEY_MOD_PRESS) && (namePos < 8 + promptLen))
					{
						alias[namePos - promptLen] = ev->keys.key;
						if (namePos < 8 + promptLen)
						{
							preview = false;
							namePos++;
						}

						updateScreen(false, false);
					}
				}
			}

	}
}


