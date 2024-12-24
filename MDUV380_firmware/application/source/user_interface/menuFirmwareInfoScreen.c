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


enum
{
	FIRMWARE_INFO_BUILD_DETAILS = 0 /* then all credits pages */
};

#if defined(PLATFORM_RD5R)
#define maxDisplayedCreditsLines  3
#elif defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#define maxDisplayedCreditsLines  11 // Does work with DM-1701 Y screen shrinkage
#else
#define maxDisplayedCreditsLines  5
#endif

static const char *creditTexts[] =
{
		"OpenGD77 RUS:", "Дмитрий Вишнев", "https://opengd77rus.ru", "", "Шрифт Terminus:", "Димитар Жеков", "",  "OpenGD77:", "Roger VK3KYY", "Daniel F1RMB", "Kai DG4KLU", "Colin G4EML", "Alex DL4LEX", "Jason VK7ZJA (SK)"
};
static const int maxCredits = (sizeof(creditTexts) / sizeof(creditTexts[0]));
static const int maxCreditsPages = (maxCredits / maxDisplayedCreditsLines) + ((maxCredits % maxDisplayedCreditsLines) == 0 ? 0 : 1);

static void displayCredits(bool playVP, uint32_t pageNumber);
static void displayBuildDetails(bool playVP);
static void updateScreen(bool playVP);
static void handleEvent(uiEvent_t *ev);
static int displayMode = FIRMWARE_INFO_BUILD_DETAILS;
static uint32_t menuFirmwareInfoNextUpdateTime;
static bool blink = false;

menuStatus_t menuFirmwareInfoScreen(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.numItems = 0;
		updateScreen(isFirstRun);
	}
	else
	{
		if (ev->time > menuFirmwareInfoNextUpdateTime)
		{
			menuFirmwareInfoNextUpdateTime = ev->time + 500;
			updateScreen(false);
		}

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
	}
	return MENU_STATUS_SUCCESS;
}

static void updateScreen(bool playVP)
{
	switch(displayMode)
	{
		case FIRMWARE_INFO_BUILD_DETAILS:
			displayBuildDetails(playVP);
			break;

		default:
			displayCredits(playVP, displayMode);
			break;
	}

	blink = !blink;
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

	if (EVENTCHECK_SHORTUP(ev->keys))
	{
		switch(ev->keys.key)
		{
			case KEY_RED:
				menuSystemPopPreviousMenu();
				return;
				break;

			case KEY_UP:
				if (displayMode > FIRMWARE_INFO_BUILD_DETAILS)
				{
					displayMode--;
					updateScreen(true);
				}
				break;

			case KEY_DOWN:
				if (displayMode < maxCreditsPages)
				{
					displayMode++;
					updateScreen(true);
				}
				break;
		}
	}
}

#if defined(STM32F405xx) && ! defined(PLATFORM_MD9600)
#if 0
static uint32_t cpuGetUnique32(uint32_t byteNumber)
{
	return (*(__IO uint32_t *) (0x1FFF7A10 + 4 * (byteNumber)));
}
#endif
#endif

static void displayBuildDetails(bool playVP)
{
#if !defined(PLATFORM_GD77S)
	char versionBuf[SCREEN_LINE_BUFFER_SIZE];
	const char *radioModel = currentLanguage->openGD77;
	char dateTimeBuf[SCREEN_LINE_BUFFER_SIZE];

	displayClearBuf();

	sprintf(dateTimeBuf, "%d%02d%02d", BUILD_YEAR, BUILD_MONTH, BUILD_DAY);



#if defined(PLATFORM_RD5R)
	displayPrintCentered(0, radioModel, FONT_SIZE_3);
	displayPrintCentered(10, currentLanguage->built, FONT_SIZE_2);
	displayPrintCentered(20, dateTimeBuf , FONT_SIZE_2);
	displayPrintCentered(30, versionBuf, FONT_SIZE_2);
#else
	displayPrintCentered(5, radioModel, FONT_SIZE_3);
	displayPrintCentered(20, currentLanguage->built, FONT_SIZE_2);
	displayPrintCentered(30, dateTimeBuf , FONT_SIZE_2);
	//displayPrintCentered(40, versionBuf, FONT_SIZE_2);
#endif

// STM32 platforms (Genuine or Clone)
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_MD9600) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
	char cpuTypeBuf[SCREEN_LINE_BUFFER_SIZE] = {0};

#if defined(PLATFORM_MD9600) || defined(PLATFORM_VARIANT_UV380_PLUS_10W)
	strncpy(cpuTypeBuf,
#if defined(MD9600_VERSION_1)
			"HW v1 "
#elif defined(MD9600_VERSION_2)
			"HW v2 "
#elif defined(MD9600_VERSION_4)
			"HW v4 "
#elif defined(MD9600_VERSION_5)
			"HW v5 "
#elif defined(PLATFORM_VARIANT_UV380_PLUS_10W)
			"Plus 10W "
#endif
	, SCREEN_LINE_BUFFER_SIZE);
#endif

	strncat(cpuTypeBuf, (NumInterruptPriorityBits == 4) ? "CPU:STM" : "CPU:TYT", (SCREEN_LINE_BUFFER_SIZE - (strlen(cpuTypeBuf) - 1)));
	displayPrintCentered(50, cpuTypeBuf , FONT_SIZE_2);
#endif

	if (playVP && (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD))
	{
		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(radioModel);
		voicePromptsAppendLanguageString(currentLanguage->built);
		voicePromptsAppendString(dateTimeBuf);
		voicePromptsAppendLanguageString(currentLanguage->gitCommit);
		voicePromptsAppendString(versionBuf);
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
		voicePromptsAppendString(cpuTypeBuf);
#endif
		promptsPlayNotAfterTx();
	}

#if defined(STM32F405xx) && ! defined(PLATFORM_MD9600)
	sprintf(cpuTypeBuf, "CPU Sig  :0x%04X", cpuGetSignature());
	displayPrintAt(8, 64, cpuTypeBuf , FONT_SIZE_2);
	sprintf(cpuTypeBuf, "CPU Rev  :0x%04X", cpuGetRevision());
	displayPrintAt(8, 74, cpuTypeBuf , FONT_SIZE_2);
	sprintf(cpuTypeBuf, "CPU Pack :0x%04X", cpuGetPackage());
	displayPrintAt(8, 84, cpuTypeBuf , FONT_SIZE_2);
	sprintf(cpuTypeBuf, "CPU Flash:%dkb", cpuGetFlashSize());
	displayPrintAt(8, 94, cpuTypeBuf , FONT_SIZE_2);
#endif

	displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 1), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
	displayRender();
#endif
}

static void displayCredits(bool playVP, uint32_t pageNumber)
{
	if (playVP && (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD))
	{
		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->credits);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		promptsPlayNotAfterTx();
	}

	displayClearBuf();
	menuDisplayTitle(currentLanguage->credits);

	pageNumber = (pageNumber - 1) * maxDisplayedCreditsLines;

	for(int i = pageNumber, y = 0; (i < (pageNumber + maxDisplayedCreditsLines)) && (i < maxCredits); i++, y++)
	{
		displayPrintCentered(y * 8 + 16, (char *)creditTexts[i], FONT_SIZE_1);
	}

	if ((maxCreditsPages > 1) && (pageNumber <= maxCreditsPages))
	{
		displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 1), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
	}

	displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 5), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);

	displayRender();
}
