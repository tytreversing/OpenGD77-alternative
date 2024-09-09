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

enum LOCK_STATE { LOCK_NONE = 0x00, LOCK_KEYPAD = 0x01, LOCK_PTT = 0x02, LOCK_BOTH = 0x03 };

static void updateScreen(bool update);
static void handleEvent(uiEvent_t *ev);

static bool lockDisplayed = false;
static const uint32_t TIMEOUT_MS = 500;
static int lockState = LOCK_NONE;
bool lockscreenIsRearming = false;

menuStatus_t menuLockScreen(uiEvent_t *ev, bool isFirstRun)
{
	static uint32_t m = 0;

	if (isFirstRun)
	{
		m = ticksGetMillis();

		updateScreen(lockDisplayed);
	}
	else
	{
		if ((lockDisplayed) && (
				((nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && (voicePromptsIsPlaying() == false)) ||
				((nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && ((ev->time - m) > TIMEOUT_MS))))
		{
			lockDisplayed = false;
			keyboardReset();
			menuSystemPopPreviousMenu();
			return MENU_STATUS_SUCCESS;
		}

		if (ev->hasEvent)
		{
			m = ticksGetMillis(); // reset timer on each key button/event.

			handleEvent(ev);
		}
	}
	return MENU_STATUS_SUCCESS;
}

static void redrawScreen(bool update, bool state)
{

	displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG_NOTIFICATION);

	if (update)
	{
		// Clear inner rect only
		displayFillRoundRect(5, 3, 118 + DISPLAY_H_EXTRA_PIXELS, DISPLAY_SIZE_Y - 8, 5, false);
	}
	else
	{
		// Clear whole screen
		displayClearBuf();
		displayDrawRoundRectWithDropShadow(4, 4, 120 + DISPLAY_H_EXTRA_PIXELS, DISPLAY_SIZE_Y - 6, 5, true);
	}

	displayThemeApply((state ? THEME_ITEM_FG_ERROR_NOTIFICATION : THEME_ITEM_FG_WARNING_NOTIFICATION), THEME_ITEM_BG_NOTIFICATION);

	if (state)
	{
		int bufferLen = strlen(currentLanguage->keypad) + 3 + strlen(currentLanguage->ptt) + 1;
		char buf[bufferLen];

		memset(buf, 0, bufferLen);

		if (keypadLocked)
		{
			strcat(buf, currentLanguage->keypad);
		}

		if (PTTLocked)
		{
			if (keypadLocked)
			{
				strcat(buf, " & ");
			}

			strcat(buf, currentLanguage->ptt);
		}
		buf[bufferLen - 1] = 0;

#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
		displayPrintCentered(16, buf, FONT_SIZE_3);
		displayPrintCentered(32, currentLanguage->locked, FONT_SIZE_3);
		displayPrintCentered(DISPLAY_SIZE_Y - 48, currentLanguage->press_sk2_plus_star, FONT_SIZE_1);
		displayPrintCentered(DISPLAY_SIZE_Y - 32, currentLanguage->to_unlock, FONT_SIZE_1);
#else
		displayPrintCentered(6, buf, FONT_SIZE_3);

  #if defined(PLATFORM_RD5R)
		displayPrintCentered(14, currentLanguage->locked, FONT_SIZE_3);
		displayPrintCentered(24, currentLanguage->press_sk2_plus_star, FONT_SIZE_1);
		displayPrintCentered(32, currentLanguage->to_unlock, FONT_SIZE_1);
  #else
		displayPrintCentered(22, currentLanguage->locked, FONT_SIZE_3);
		displayPrintCentered(40, currentLanguage->press_sk2_plus_star, FONT_SIZE_1);
		displayPrintCentered(48, currentLanguage->to_unlock, FONT_SIZE_1);
  #endif
#endif
		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		if (lockState & LOCK_KEYPAD)
		{
			voicePromptsAppendLanguageString(currentLanguage->keypad);
			voicePromptsAppendPrompt(PROMPT_SILENCE);
		}

		if (lockState & LOCK_PTT)
		{
			voicePromptsAppendLanguageString(currentLanguage->ptt);
			voicePromptsAppendPrompt(PROMPT_SILENCE);
		}

		voicePromptsAppendLanguageString(currentLanguage->locked);

		if (nonVolatileSettings.audioPromptMode > AUDIO_PROMPT_MODE_VOICE_LEVEL_2)
		{
			voicePromptsAppendPrompt(PROMPT_SILENCE);
			voicePromptsAppendLanguageString(currentLanguage->press_sk2_plus_star);
			voicePromptsAppendLanguageString(currentLanguage->to_unlock);
			voicePromptsAppendPrompt(PROMPT_SILENCE);
		}

		voicePromptsPlay();
	}
	else
	{
		displayPrintCentered((DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) / 2, currentLanguage->unlocked, FONT_SIZE_3);

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->unlocked);
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsPlay();
	}

	displayThemeResetToDefault();
	displayRender();
	lockDisplayed = true;
}

static void updateScreen(bool updateOnly)
{
	bool keypadChanged = false;
	bool PTTChanged = false;

	if (keypadLocked)
	{
		if ((lockState & LOCK_KEYPAD) == 0)
		{
			keypadChanged = true;
			lockState |= LOCK_KEYPAD;
		}
	}
	else
	{
		if ((lockState & LOCK_KEYPAD))
		{
			keypadChanged = true;
			lockState &= ~LOCK_KEYPAD;
		}
	}

	if (PTTLocked)
	{
		if ((lockState & LOCK_PTT) == 0)
		{
			PTTChanged = true;
			lockState |= LOCK_PTT;
		}
	}
	else
	{
		if ((lockState & LOCK_PTT))
		{
			PTTChanged = true;
			lockState &= ~LOCK_PTT;
		}
	}

	if (updateOnly)
	{
		if (keypadChanged || PTTChanged)
		{
			redrawScreen(updateOnly, ((lockState & LOCK_KEYPAD) || (lockState & LOCK_PTT)));
		}
		else
		{
			if (lockDisplayed == false)
			{
				redrawScreen(updateOnly, false);
			}
		}
	}
	else
	{
		// Draw everything
		redrawScreen(false, keypadLocked || PTTLocked);
	}
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

	if (KEYCHECK_DOWN(ev->keys, KEY_STAR) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
	{
		if ((nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && voicePromptsIsPlaying())
		{
			voicePromptsTerminate();
		}

		keypadLocked = false;
		PTTLocked = false;
		lockDisplayed = false;
#if ! defined(PLATFORM_GD77S)
		ticksTimerStart(&autolockTimer, (nonVolatileSettings.autolockTimer * 30000U));
#endif
		lockscreenIsRearming = true;
		menuSystemPopAllAndDisplayRootMenu();
		lockscreenIsRearming = false;
		menuSystemPushNewMenu(UI_LOCK_SCREEN);
	}
	else if ((nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && voicePromptsIsPlaying() && (ev->keys.key != 0) && (ev->keys.event & KEY_MOD_UP))
	{
#if defined(PLATFORM_MD2017)
		bool trackballEvent = ((ev->keys.event == (KEY_MOD_UP | KEY_MOD_PRESS)) && (
				((ev->keys.key == KEY_UP) || (ev->keys.key == KEY_DOWN) ||
						(ev->keys.key == KEY_LEFT) || (ev->keys.key == KEY_RIGHT))));

		if (trackballEvent == false)
#endif
		{
			// Cancel the voice on any key event (that hides the lock screen earlier), except for the trackball on MD-2017
			voicePromptsTerminate();
		}
	}
}

void menuLockScreenPop(void)
{
	lockDisplayed = false;

	if (menuSystemGetCurrentMenuNumber() == UI_LOCK_SCREEN)
	{
		menuSystemPopPreviousMenu();
	}
}
