#include "user_interface/uiGlobals.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiLocalisation.h"
#include "user_interface/uiUtilities.h"
#include "functions/settings.h"

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);

static menuStatus_t menuDMRIDExitCode = MENU_STATUS_SUCCESS;
static char alias[16];
static char buffer[16];
static uint32_t dmrID = 0;
static char namePos;
static char numPos;


enum
{
	MENU_ALIAS = 0,
	MENU_DMRID_VALUE,
	NUM_DMRID_ITEMS
};



menuStatus_t menuDMRID(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
		{
			int currentMenuNumber = menuSystemGetCurrentMenuNumber();
			memset(buffer, 0, 16);
            codeplugGetRadioName(alias);
            dmrID = codeplugGetUserDMRID();
            sprintf(buffer, "%d", dmrID);
			menuDataGlobal.currentMenuList = (menuItemNewData_t *)menuDataGlobal.data[currentMenuNumber]->items;
			menuDataGlobal.numItems = menuDataGlobal.data[currentMenuNumber]->numItems;
			menuDataGlobal.currentItemIndex = MENU_ALIAS;
			menuDataGlobal.numItems = NUM_DMRID_ITEMS;


			updateScreen(true);

			return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
		}
		else
		{
			menuDMRIDExitCode = MENU_STATUS_SUCCESS;

			if (ev->hasEvent)
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

static void updateScreen(bool isFirstRun)
{
	int mNum = 0;
	char buf[SCREEN_LINE_BUFFER_SIZE];
	const char *leftSide = NULL;// initialise to please the compiler
	const char *leftSideConst = NULL;// initialise to please the compiler
	const char *rightSideConst = NULL;// initialise to please the compiler
	char rightSideVar[SCREEN_LINE_BUFFER_SIZE];

	displayClearBuf();
	menuDisplayTitle(currentLanguage->dmrid);
	displayThemeResetToDefault();
	if (menuDataGlobal.currentItemIndex == MENU_ALIAS)
	{
		keypadAlphaEnable = true;
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
		leftSideConst = NULL;
		rightSideConst = NULL;
		rightSideVar[0] = 0;
		switch (mNum)
		{
			case MENU_ALIAS:
				leftSide = currentLanguage->aliastext;
				strncpy(rightSideVar, alias, SCREEN_LINE_BUFFER_SIZE);
				break;
			case MENU_DMRID_VALUE:
				leftSide = currentLanguage->dmridtext;
				strncpy(rightSideVar, buffer, SCREEN_LINE_BUFFER_SIZE);
				break;
		}
		snprintf(buf, SCREEN_LINE_BUFFER_SIZE, "%s:%s", leftSide, (rightSideVar[0] ? rightSideVar : (rightSideConst ? rightSideConst : "")));
		menuDisplayEntry(i, mNum, buf, (strlen(leftSide) + 1), THEME_ITEM_FG_MENU_ITEM, THEME_ITEM_FG_OPTIONS_VALUE, THEME_ITEM_BG);

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

	if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		menuSystemPopPreviousMenu();
		return;
	}
	if (KEYCHECK_PRESS(ev->keys, KEY_DOWN))
	{
	 	menuSystemMenuIncrement(&menuDataGlobal.currentItemIndex, NUM_DMRID_ITEMS);
	 	updateScreen(false);
	 	menuDMRIDExitCode |= MENU_STATUS_LIST_TYPE;
	}
	else if (KEYCHECK_PRESS(ev->keys, KEY_UP))
	{
	 	menuSystemMenuDecrement(&menuDataGlobal.currentItemIndex, NUM_DMRID_ITEMS);
	 	updateScreen(false);
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
				moveCursorRightInString(buffer, &numPos, 16, BUTTONCHECK_DOWN(ev, BUTTON_SK2));
				updateCursor(true);
				break;
		}
	}

}
