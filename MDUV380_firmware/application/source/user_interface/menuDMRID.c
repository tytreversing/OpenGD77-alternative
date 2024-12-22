#include "user_interface/uiGlobals.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiLocalisation.h"
#include "user_interface/uiUtilities.h"

static void updateScreen(bool isFirstRun);
static void handleEvent(uiEvent_t *ev);

static menuStatus_t menuDMRIDExitCode = MENU_STATUS_SUCCESS;

menuStatus_t menuDMRID(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
		{
			const char *menuName = NULL;
			int currentMenuNumber = menuSystemGetCurrentMenuNumber();

			menuDataGlobal.currentMenuList = (menuItemNewData_t *)menuDataGlobal.data[currentMenuNumber]->items;
			menuDataGlobal.numItems = menuDataGlobal.data[currentMenuNumber]->numItems;



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

static void updateScreen(bool isFirstRun)
{
	int mNum;
	const char *mName = currentLanguage->dmrid;

	displayClearBuf();
	menuDisplayTitle(mName);



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
}
