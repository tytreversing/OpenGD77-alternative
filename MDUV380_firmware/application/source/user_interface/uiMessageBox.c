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
#include "utils.h"

static menuStatus_t menuMessageBoxExitCode;

static void handleEvent(uiEvent_t *ev);
static void updateScreen(bool forceRedraw);
static void displayMessage(void);
static void displayButtons(void);

menuStatus_t uiMessageBox(uiEvent_t *ev, bool isFirstRun)
{
	static uint32_t curm = 0;

	if (isFirstRun)
	{
		freqEnterReset();
		uiDataGlobal.MessageBox.keyPressed = 0;

		updateScreen(true);
		menuMessageBoxExitCode = MENU_STATUS_SUCCESS;
	}
	else
	{
		bool forceRedraw = false;

		menuMessageBoxExitCode = MENU_STATUS_SUCCESS;

		if (ev->events == NO_EVENT)
		{
			// We are entering digits, so update the screen as we have a cursor to blink
			if ((uiDataGlobal.MessageBox.type == MESSAGEBOX_TYPE_PIN_CODE) &&
					(uiDataGlobal.FreqEnter.index >= 0) && ((ev->time - curm) > 300))
			{
				curm = ev->time;
				forceRedraw = true;
			}

			if (forceRedraw)
			{
				updateScreen(false);
			}
		}
		else
		{
			if (ev->hasEvent)
			{
				handleEvent(ev);
			}
		}
	}

	return menuMessageBoxExitCode;
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
		updateScreen(true);
		return;
	}

	if (KEYCHECK_SHORTUP(ev->keys, KEY_GREEN))
	{
		int greenHandled = (uiDataGlobal.MessageBox.buttons != MESSAGEBOX_BUTTONS_DISMISS);
		bool res = false;

		// Ensure we have a complete pin code before calling the validator function
		if ((uiDataGlobal.MessageBox.type == MESSAGEBOX_TYPE_PIN_CODE) &&
				(uiDataGlobal.FreqEnter.index < uiDataGlobal.MessageBox.pinLength))
		{
			return;
		}

		uiDataGlobal.MessageBox.keyPressed = KEY_GREEN;

		// Call validator
		if (uiDataGlobal.MessageBox.validatorCallback)
		{
			res = uiDataGlobal.MessageBox.validatorCallback();

			if (uiDataGlobal.MessageBox.type == MESSAGEBOX_TYPE_PIN_CODE)
			{
				freqEnterReset();

				if (res == false)
				{
					menuMessageBoxExitCode |= MENU_STATUS_ERROR;
				}
			}
		}

		if (greenHandled && (((uiDataGlobal.MessageBox.validatorCallback != NULL) && res) || (uiDataGlobal.MessageBox.validatorCallback == NULL)))
		{
			uiDataGlobal.MessageBox.validatorCallback = NULL;
			menuSystemPopPreviousMenu();
			return;
		}
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		bool redHandled = (uiDataGlobal.MessageBox.buttons != MESSAGEBOX_BUTTONS_OK)
#if defined(PLATFORM_MD9600)
				&& (uiDataGlobal.MessageBox.buttons != MESSAGEBOX_BUTTONS_ENT)
#endif
				;
		bool res = false;

		uiDataGlobal.MessageBox.keyPressed = KEY_RED;

		// Call validator
		if (uiDataGlobal.MessageBox.validatorCallback)
		{
			res = uiDataGlobal.MessageBox.validatorCallback();
		}

		if (redHandled && (((uiDataGlobal.MessageBox.validatorCallback != NULL) && res) || (uiDataGlobal.MessageBox.validatorCallback == NULL)))
		{
			uiDataGlobal.MessageBox.validatorCallback = NULL;
			menuSystemPopPreviousMenu();
			return;
		}
	}
	else
	{
		if (uiDataGlobal.MessageBox.type == MESSAGEBOX_TYPE_PIN_CODE)
		{
			bool redraw = false;

			if ((uiDataGlobal.FreqEnter.index > 0) && KEYCHECK_PRESS(ev->keys, KEY_LEFT))
			{
				uiDataGlobal.FreqEnter.index--;
				uiDataGlobal.FreqEnter.digits[uiDataGlobal.FreqEnter.index] = '-';
				redraw = true;
			}

			if (uiDataGlobal.FreqEnter.index < uiDataGlobal.MessageBox.pinLength)
			{
				int keyval = menuGetKeypadKeyValue(ev, true);

				if (keyval != 99)
				{
					voicePromptsInit();
					voicePromptsAppendPrompt(PROMPT_0 + keyval);
					voicePromptsPlay();

					uiDataGlobal.FreqEnter.digits[uiDataGlobal.FreqEnter.index] = (char) keyval + '0';
					uiDataGlobal.FreqEnter.index++;
					redraw = true;
				}
			}

			if (redraw)
			{
				updateScreen(false);
			}
		}
	}
}

static void updateScreen(bool forceRedraw)
{
	static bool blink = false;
	static uint32_t blinkTime = 0;

	if (forceRedraw)
	{
		displayClearBuf();
	}

	switch (uiDataGlobal.MessageBox.type)
	{
		case MESSAGEBOX_TYPE_INFO:
		{
			if (uiDataGlobal.MessageBox.decoration != MESSAGEBOX_DECORATION_NONE)
			{
				displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG_NOTIFICATION);
				displayDrawRoundRectWithDropShadow(4, 4 + DISPLAY_V_OFFSET, DISPLAY_SIZE_X - (2 * 4), DISPLAY_SIZE_Y - (uiDataGlobal.MessageBox.buttons != MESSAGEBOX_BUTTONS_NONE ? FONT_SIZE_3_HEIGHT : 0) - 6 - DISPLAY_V_EXTRA_PIXELS, 5, true);
			}

			if (strlen(uiDataGlobal.MessageBox.message))
			{
				displayThemeApply(THEME_ITEM_FG_NOTIFICATION, THEME_ITEM_BG_NOTIFICATION);
				displayMessage();
			}

			displayButtons();
		}
		break;

		case MESSAGEBOX_TYPE_PIN_CODE:
		{
			char pinStr[17] = { 0 };
			int16_t xCursor = -1;
			int16_t yCursor = -1;

			if (forceRedraw)
			{
				displayThemeApply(THEME_ITEM_FG_TEXT_INPUT, THEME_ITEM_BG);
				displayMessage();

				uiDataGlobal.MessageBox.buttons = MESSAGEBOX_BUTTONS_OK;
				displayButtons();
			}
			else
			{
				// Clear input
				displayThemeResetToDefault();
				displayFillRect(0, TITLE_BOX_HEIGHT + 2, DISPLAY_SIZE_X,
						DISPLAY_SIZE_Y - TITLE_BOX_HEIGHT - FONT_SIZE_3_HEIGHT - 2, true);
			}

			memset(pinStr, 0, sizeof(pinStr));
			memcpy(pinStr, uiDataGlobal.FreqEnter.digits, SAFE_MIN(uiDataGlobal.MessageBox.pinLength, FREQ_ENTER_DIGITS_MAX));

			displayThemeApply(THEME_ITEM_FG_TEXT_INPUT, THEME_ITEM_BG);
			displayPrintCentered(DISPLAY_Y_POS_RX_FREQ, pinStr, FONT_SIZE_3);

			// Cursor
			if (uiDataGlobal.FreqEnter.index < uiDataGlobal.MessageBox.pinLength)
			{
				xCursor = ((DISPLAY_SIZE_X - (strlen(pinStr) * 8)) >> 1) + (uiDataGlobal.FreqEnter.index * 8);
				yCursor = DISPLAY_Y_POS_RX_FREQ + (FONT_SIZE_3_HEIGHT - 2);
			}

			if ((xCursor >= 0) && (yCursor >= 0))
			{
				displayDrawFastHLine(xCursor + 1, yCursor, 6, blink);

				if ((ticksGetMillis() - blinkTime) > 500)
				{
					blinkTime = ticksGetMillis();
					blink = !blink;
				}
			}
		}
		break;

		default:
			break;
	}

	displayThemeResetToDefault();
	displayRender();
}

static void displayMessage(void)
{
	char msg[17];
	int y = 0;
	bool decoration = (uiDataGlobal.MessageBox.decoration != MESSAGEBOX_DECORATION_NONE);
	char *p = strchr(uiDataGlobal.MessageBox.message, '\n');
	int linesCount = (p ? 1 : 0);
	int maxLines = (decoration ? 3 : 4);

	if (uiDataGlobal.MessageBox.type != MESSAGEBOX_TYPE_PIN_CODE)
	{
		if (uiDataGlobal.MessageBox.buttons != MESSAGEBOX_BUTTONS_NONE)
		{
			maxLines--;
		}

		while (p)
		{
			linesCount++;

			if (linesCount == maxLines)
			{
				break;
			}

			p = strchr(p + 1, '\n');
		}

		// Y start
		y = (DISPLAY_SIZE_Y - (uiDataGlobal.MessageBox.buttons != MESSAGEBOX_BUTTONS_NONE ? FONT_SIZE_3_HEIGHT : 0) - (linesCount * FONT_SIZE_3_HEIGHT)) >> 1;
	}

	if ((uiDataGlobal.MessageBox.type != MESSAGEBOX_TYPE_PIN_CODE) && linesCount)
	{
		int len = strlen(uiDataGlobal.MessageBox.message);
		char *pb = uiDataGlobal.MessageBox.message;
		char *pe = &uiDataGlobal.MessageBox.message[0] + len;
		int cnt;

		p = strchr(uiDataGlobal.MessageBox.message, '\n');

		while (linesCount > 1)
		{
			cnt = SAFE_MIN((p - pb), (decoration ? 14 : 16));

			memcpy(msg, pb, cnt);
			msg[cnt] = '\0';

			displayPrintCentered(y, msg, FONT_SIZE_3);
			y += FONT_SIZE_3_HEIGHT;

			pb = p + 1;
			p = strchr(pb, '\n');

			linesCount--;
		}

		if (pb < pe)
		{
			cnt = SAFE_MIN((pe - pb), (decoration ? 14 : 16));

			p++;
			memcpy(msg, pb, cnt);
			msg[cnt] = '\0';

			displayPrintCentered(y, msg, FONT_SIZE_3);
		}
	}
	else
	{
		y = ((DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) >> 1);

		if (uiDataGlobal.MessageBox.type == MESSAGEBOX_TYPE_PIN_CODE)
		{
			decoration = true; // Override decoration

			// Display title
			displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG_MENU_NAME);
			displayDrawRoundRectWithDropShadow(2, 2, (DISPLAY_SIZE_X - 6), TITLE_BOX_HEIGHT, 3, true);
			displayThemeApply(THEME_ITEM_FG_MENU_NAME, THEME_ITEM_BG_MENU_NAME);

			y = 3;
		}

		snprintf(msg, (decoration ? 15 : 17), "%s", uiDataGlobal.MessageBox.message);
		displayPrintCentered(y, msg, FONT_SIZE_3);
	}

}

static void displayButtons(void)
{
	switch (uiDataGlobal.MessageBox.buttons)
	{
		case MESSAGEBOX_BUTTONS_OK:
			displayDrawChoice(CHOICE_OK, false);
			break;
#if defined(PLATFORM_MD9600)
		case MESSAGEBOX_BUTTONS_ENT:
			displayDrawChoice(CHOICE_ENT, false);
			break;
#endif
		case MESSAGEBOX_BUTTONS_YESNO:
			displayDrawChoice(CHOICE_YESNO, false);
			break;
		case MESSAGEBOX_BUTTONS_DISMISS:
			displayDrawChoice(CHOICE_DISMISS, false);
			break;
		default:
			break;
	}
}
