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
#include "interfaces/gps.h"
#include "usb/usb_com.h"

#if defined(HAS_GPS)

static void updateScreen(bool isFirstRun, bool forceRedraw);
static void handleEvent(uiEvent_t *ev);

#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
#define currentPageOnMD9600Is(p)  if (menuDataGlobal.currentItemIndex == (p))
#else
#define currentPageOnMD9600Is(p)
#endif

#define POLAR_GRAPHICS_X_OFFSET     0

#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
#define POLAR_GRAPHICS_Y_OFFSET    40
#define RSSI_X_OFFSET               0
#define RSSI_BARS_Y_POS            54
#define MAIDENHEAD_HDOP_Y_POS      48
#define NUM_BARS                   14
#define DOT_RADIUS                  1
#define MAIDENHEAD_HDOP_MARGIN      0

#define FONT_SIZE_2_HEIGHT          8

#define VALUES_FONT_SIZE            FONT_SIZE_3
#define VALUES_FONT_SIZE_SMALL      FONT_SIZE_2
#define UNITS_FONT_SIZE             FONT_SIZE_1
#define UNITS_Y_OFFSET              6

#define VALUES_FONT_WIDTH           8
#define VALUES_FONT_HEIGHT         16

#define DIRECTION_REGION_X_OFFSET   2
#define DIRECTION_REGION_H_OFFSET   5
#define CARDINALS_REGION_HEIGHT    (FONT_SIZE_2_HEIGHT - 1)

#define DIRECTION_SATS_X_POS       11
#define DIRECTION_SATS_Y_POS       34

#define DIRECTION_CARDINAL_OFFSET   8
#define DIRECTION_COURSE_X_POS     75
#define DIRECTION_COURSE_Y_POS     29

#define DIRECTION_ALT_X_POS        81
#define DIRECTION_ALT_Y_POS        34

#define DIRECTION_HDOP_X_POS       42
#define DIRECTION_HDOP_Y_POS       34

#define DIRECTION_HEADING_X_POS    28
#define DIRECTION_HEADING_Y_POS    39
#define DIRECTION_HEADING_RADIUS   24
#define TOP_ARROW_OFFSET           (DIRECTION_HEADING_RADIUS - 7)
#define BASE_ARROW_OFFSET         ((DIRECTION_HEADING_RADIUS >> 1) + 1)

#define DIRECTION_FIXTYPE_X_POS    25
#define DIRECTION_FIXTYPE_Y_POS    36

#define DIRECTION_SPEED_X_POS      65
#define DIRECTION_SPEED_Y_POS      47

#define MAX_RADIUS                 22

#define MAX_RSSI_Y_PIXELS          25

#else

#define POLAR_GRAPHICS_Y_OFFSET    72
#define RSSI_X_OFFSET               2
#define RSSI_BARS_Y_POS           120
#define MAIDENHEAD_HDOP_Y_POS      32
#define NUM_BARS                   17
#define DOT_RADIUS                  2
#define MAIDENHEAD_HDOP_MARGIN      8

#define VALUES_FONT_SIZE            FONT_SIZE_4
#define VALUES_FONT_SIZE_SMALL      FONT_SIZE_3
#define UNITS_FONT_SIZE             FONT_SIZE_2
#define UNITS_Y_OFFSET             18

#define VALUES_FONT_WIDTH          16
#define VALUES_FONT_HEIGHT         32

#define DIRECTION_REGION_X_OFFSET   6
#define DIRECTION_REGION_H_OFFSET  13
#define CARDINALS_REGION_HEIGHT     FONT_SIZE_3_HEIGHT

#define DIRECTION_SATS_X_POS      122
#define DIRECTION_SATS_Y_POS       30

#define DIRECTION_CARDINAL_OFFSET  10
#define DIRECTION_COURSE_X_POS     56
#define DIRECTION_COURSE_Y_POS     31

#define DIRECTION_ALT_X_POS        12
#define DIRECTION_ALT_Y_POS        71

#define DIRECTION_HDOP_X_POS      111
#define DIRECTION_HDOP_Y_POS       71

#define DIRECTION_HEADING_X_POS    27
#define DIRECTION_HEADING_Y_POS    39
#define DIRECTION_HEADING_RADIUS   25
#define TOP_ARROW_OFFSET           (DIRECTION_HEADING_RADIUS - 8)
#define BASE_ARROW_OFFSET          (DIRECTION_HEADING_RADIUS >> 1)

#define DIRECTION_FIXTYPE_X_POS    25
#define DIRECTION_FIXTYPE_Y_POS    36

#define DIRECTION_SPEED_X_POS      40
#define DIRECTION_SPEED_Y_POS      98

#define MAX_RADIUS                 44

#define MAX_RSSI_Y_PIXELS          59

#endif

typedef enum
{
	PAGE_COORDS = 0,
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
	PAGE_RSSI,
#else
	PAGE_RSSI = 0,
#endif
	PAGE_POLAR,
	PAGE_DIRECTION, // MD9600: Sat/HDOP/Alt, MD-UV380 everything in that page
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
	PAGE_DIRECTION_2, // Heading/Course
#endif
	PAGES_MAX
} Pages_t;

typedef enum
{
	FIELD_SATS_CLEAR   = (1 << 0),
	FIELD_SATS_ZERO    = (1 << 1),
	FIELD_COURSE_CLEAR = (1 << 2),
	FIELD_COURSE_ZERO  = (1 << 3),
	FIELD_ALT_CLEAR    = (1 << 4),
	FIELD_ALT_ZERO     = (1 << 5),
	FIELD_HDOP_CLEAR   = (1 << 6),
	FIELD_HDOP_ZERO    = (1 << 7),
	FIELD_SPEED_CLEAR  = (1 << 8),
	FIELD_SPEED_ZERO   = (1 << 9),
	FIELD_BACKGROUND   = (1 << 10),
	FIELD_ALL_CLEAR    = (FIELD_SATS_CLEAR | FIELD_COURSE_CLEAR | FIELD_ALT_CLEAR | FIELD_HDOP_CLEAR | FIELD_SPEED_CLEAR),
	FIELD_ALL_ZERO     = (FIELD_SATS_ZERO | FIELD_COURSE_ZERO | FIELD_ALT_ZERO | FIELD_HDOP_ZERO | FIELD_SPEED_ZERO)
} DirectionFieldsFlags_t;

typedef enum
{
	FIXTYPE_NONE  = (1 << 1),
	FIXTYPE_1D    = (1 << 2),
	FIXTYPE_2D    = (1 << 3),
	FIXTYPE_3D    = (1 << 4),
} DirectionFixtype_t;

static menuStatus_t menuZoneExitCode = MENU_STATUS_SUCCESS;
static uint8_t prevGPSState = 0xff;
static uint32_t updateTick = 0;
static uint16_t prevSatsInView = 0xFFFF;
static char directions[16][4];

menuStatus_t menuGPS(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuDataGlobal.numItems = PAGES_MAX;

		// Prepare directions from cardinals
		char *cardinals = (char *)currentLanguage->symbols;
		char *N = &cardinals[0];
		char *S = &cardinals[1];
		char *E = &cardinals[2];
		char *W = &cardinals[3];

		sprintf(directions[0], "%c", *N);
		sprintf(directions[1], "%c%c%c", *N, *N, *E);
		sprintf(directions[2], "%c%c", *N, *E);
		sprintf(directions[3], "%c%c%c", *E, *N, *E);
		sprintf(directions[4], "%c", *E);
		sprintf(directions[5], "%c%c%c", *E, *S, *E);
		sprintf(directions[6], "%c%c", *S, *E);
		sprintf(directions[7], "%c%c%c", *S, *S, *E);
		sprintf(directions[8], "%c", *S);
		sprintf(directions[9], "%c%c%c", *S, *S, *W);
		sprintf(directions[10], "%c%c", *S, *W);
		sprintf(directions[11], "%c%c%c", *W, *S, *W);
		sprintf(directions[12], "%c", *W);
		sprintf(directions[13], "%c%c%c", *W, *N, *W);
		sprintf(directions[14], "%c%c", *N, *W);
		sprintf(directions[15], "%c%c%c", *N, *N, *W);

		prevGPSState = 0xff;

		voicePromptsInit();
		voicePromptsAppendPrompt(PROMPT_SILENCE);
		voicePromptsAppendLanguageString(currentLanguage->gps);
		voicePromptsAppendLanguageString(currentLanguage->menu);
		voicePromptsAppendPrompt(PROMPT_SILENCE);

		updateScreen(true, false);

		return (MENU_STATUS_LIST_TYPE | MENU_STATUS_SUCCESS);
	}
	else
	{
		menuZoneExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
		else
		{
			updateTick++;
			// Limits updateScreen() calling frequency rate to every second, if a new GPS info is available, GPS_STATUS_TIME_UPDATED excepted.
			// The functions called by updateScreen() are also filtering and limiting the screen redrawing, depending on the displayed page.
			if ((updateTick > 1000) &&
					(gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_FIXTYPE_UPDATED | GPS_STATUS_POSITION_UPDATED |
							GPS_STATUS_HDOP_UPDATED | GPS_STATUS_COURSE_UPDATED | GPS_STATUS_SPEED_UPDATED | GPS_STATUS_HEIGHT_UPDATED |
							GPS_STATUS_GPS_SATS_UPDATED | GPS_STATUS_BD_SATS_UPDATED)))
			{
				updateTick = 0;
				updateScreen(false, false);
			}
		}
	}
	return menuZoneExitCode;
}

static int displayBars(gpsSatellitesData_t *satellitesArray, uint16_t satCount, bool numbersInBold, int dispPosition, float spacing, themeItem_t fgColour, themeItem_t bgColour)
{
	char buf[3];
#if ! defined(HAS_COLOURS)
	uint8_t numberFont = (numbersInBold ? FONT_SIZE_1_BOLD : FONT_SIZE_1);
#else
	uint8_t numberFont = FONT_SIZE_1;
#endif

	for (uint16_t i = 0; i < satCount; i++)
	{
		if (satellitesArray[i].RSSI > 0)
		{
			int xPos = RSSI_X_OFFSET + 2 + dispPosition * spacing;

			if (xPos > (DISPLAY_SIZE_X - spacing + 2))
			{
				break;
			}

			uint16_t sNumber = (satellitesArray[i].Number % 100); // limits to 2 last digits
			// Scale down (max @ 50dB), clipping higher
			int8_t rssi = SAFE_MIN(MAX_RSSI_Y_PIXELS, ((int8_t)((MAX_RSSI_Y_PIXELS * 0.020) * satellitesArray[i].RSSI)));

			displayThemeApply(THEME_ITEM_FG_GPS_NUMBER, THEME_ITEM_BG);
			sprintf(buf, "%d", (sNumber % 10));
			displayPrintAt(xPos + 1, RSSI_BARS_Y_POS, buf, numberFont);

			sprintf(buf, "%d", (sNumber / 10));
			displayPrintAt(xPos + 1, RSSI_BARS_Y_POS - 8, buf, numberFont);

			displayThemeApply(fgColour, bgColour);
			displayFillRect(xPos, RSSI_BARS_Y_POS - 12  - 2 - rssi, spacing - 2, rssi, false);
			displayThemeResetToDefault();

			dispPosition++;

			if (dispPosition > (NUM_BARS - 1))
			{
				return dispPosition;
			}
		}
	}

	return dispPosition;
}

static uint16_t getTotalNumberOfUsableSats(void)
{
	uint16_t n = 0;

	for (uint16_t i = 0; i < gpsData.SatsInViewGP; i++)
	{
		n += ((gpsData.GPSatellites[i].RSSI > 0) ? 1 : 0);
	}

	for (uint16_t i = 0; i < gpsData.SatsInViewBD; i++)
	{
		n += ((gpsData.BDSatellites[i].RSSI > 0) ? 1 : 0);
	}

	return n;
}

static void displayDirectionFixType(DirectionFixtype_t type)
{
	uint16_t fgColour, bgColour;

	displayGetForegroundAndBackgroundColours(&fgColour, &bgColour);
	displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);

	displayFillCircle(DIRECTION_HEADING_X_POS, DIRECTION_HEADING_Y_POS, (DIRECTION_HEADING_RADIUS >> 2) + 1, ((type & FIXTYPE_NONE) != 0));

	if (type & (FIXTYPE_1D | FIXTYPE_2D | FIXTYPE_3D))
	{
		displayDrawCircle(DIRECTION_HEADING_X_POS, DIRECTION_HEADING_Y_POS, (DIRECTION_HEADING_RADIUS >> 2) + 1, true);
	}

	if (type & (FIXTYPE_2D | FIXTYPE_3D))
	{
		displayThemeResetToDefault();
		displayPrintAt(DIRECTION_FIXTYPE_X_POS, DIRECTION_FIXTYPE_Y_POS, ((type & FIXTYPE_2D) ? "2" : "3"), FONT_SIZE_2);
	}

	displaySetForegroundAndBackgroundColours(fgColour, bgColour);
}

static void displayDirectionInfoBackground(DirectionFieldsFlags_t flags)
{
	//
	// Full background redrawing
	//
	if (flags & FIELD_BACKGROUND)
	{
		// Clear background
		displayFillRect(0, 16, DISPLAY_SIZE_X, (DISPLAY_SIZE_Y - 16), true);

		displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);

		currentPageOnMD9600Is(PAGE_DIRECTION)
		{
			// Sats in view
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			displayPrintAt(DIRECTION_SATS_X_POS - 3, (DIRECTION_SATS_Y_POS - 7), currentLanguage->satellite_short, FONT_SIZE_1);
			displayDrawFastHLine((DIRECTION_SATS_X_POS - 7), (DIRECTION_SATS_Y_POS - 9), 5, true);
			displayDrawFastVLine((DIRECTION_SATS_X_POS - 7), (DIRECTION_SATS_Y_POS - 8), FONT_SIZE_3_HEIGHT + 6, true);
			displayDrawFastHLine((DIRECTION_SATS_X_POS - 7), ((DIRECTION_SATS_Y_POS - 8) + FONT_SIZE_3_HEIGHT + 6), ((2 * 8) + 7), true);
			displayDrawFastVLine(((DIRECTION_SATS_X_POS - 7) + ((2 * 8) + 7)), ((DIRECTION_SATS_Y_POS - 4) + (FONT_SIZE_3_HEIGHT - 1)), 4, true);
#else
			displayPrintAt(DIRECTION_SATS_X_POS, (DIRECTION_SATS_Y_POS - 3), currentLanguage->satellite_short, FONT_SIZE_2);
			displayDrawFastHLine((DIRECTION_SATS_X_POS - 6), (DIRECTION_SATS_Y_POS - 5), 5, true);
			displayDrawFastVLine((DIRECTION_SATS_X_POS - 6), (DIRECTION_SATS_Y_POS - 4), FONT_SIZE_4_HEIGHT, true);
			displayDrawFastHLine((DIRECTION_SATS_X_POS - 6), ((DIRECTION_SATS_Y_POS - 4) + FONT_SIZE_4_HEIGHT), ((2 * 16) + 7), true);
			displayDrawFastVLine(((DIRECTION_SATS_X_POS - 6) + ((2 * 16) + 7)), ((DIRECTION_SATS_Y_POS - 4) + (FONT_SIZE_4_HEIGHT - 4)), 5, true);
#endif

			// Altitude
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			displayPrintAt(DIRECTION_ALT_X_POS - 3, (DIRECTION_ALT_Y_POS - 7), currentLanguage->altitude, FONT_SIZE_1);
			displayPrintAt((DIRECTION_ALT_X_POS + (4 * 8) + 2), (DIRECTION_ALT_Y_POS + 6), "m", FONT_SIZE_1);
			displayDrawFastHLine((DIRECTION_ALT_X_POS - 7), (DIRECTION_ALT_Y_POS - 9), 5, true);
			displayDrawFastVLine((DIRECTION_ALT_X_POS - 7), (DIRECTION_ALT_Y_POS - 8), FONT_SIZE_3_HEIGHT + 6, true);
			displayDrawFastHLine((DIRECTION_ALT_X_POS - 7), ((DIRECTION_ALT_Y_POS - 8) + FONT_SIZE_3_HEIGHT + 6), ((4 * 8) + 7 + 8), true);
			displayDrawFastVLine(((DIRECTION_ALT_X_POS - 7) + ((4 * 8) + 7) + 8), ((DIRECTION_ALT_Y_POS - 4) + (FONT_SIZE_3_HEIGHT - 1)), 4, true);
#else
			displayPrintAt(DIRECTION_ALT_X_POS, (DIRECTION_ALT_Y_POS - 3), currentLanguage->altitude, FONT_SIZE_2);
			displayPrintAt((DIRECTION_ALT_X_POS + (4 * 16) + 2), (DIRECTION_ALT_Y_POS + 18), "m", FONT_SIZE_2);
			displayDrawFastHLine((DIRECTION_ALT_X_POS - 6), (DIRECTION_ALT_Y_POS - 5), 5, true);
			displayDrawFastVLine((DIRECTION_ALT_X_POS - 6), (DIRECTION_ALT_Y_POS - 4), FONT_SIZE_4_HEIGHT, true);
			displayDrawFastHLine((DIRECTION_ALT_X_POS - 6), ((DIRECTION_ALT_Y_POS - 4) + FONT_SIZE_4_HEIGHT), ((4 * 16) + 7 + 10), true);
			displayDrawFastVLine(((DIRECTION_ALT_X_POS - 6) + ((4 * 16) + 7) + 10), ((DIRECTION_ALT_Y_POS - 4) + (FONT_SIZE_4_HEIGHT - 4)), 5, true);
#endif

			// HDOP
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			displayPrintAt(DIRECTION_HDOP_X_POS - 3, (DIRECTION_HDOP_Y_POS - 7), "HDOP", FONT_SIZE_1);
			displayPrintAt((DIRECTION_HDOP_X_POS + (2 * 8) + 2), (DIRECTION_HDOP_Y_POS + 6), "m", FONT_SIZE_1);
			displayDrawFastHLine((DIRECTION_HDOP_X_POS - 7), (DIRECTION_HDOP_Y_POS - 9), 5, true);
			displayDrawFastVLine((DIRECTION_HDOP_X_POS - 7), (DIRECTION_HDOP_Y_POS - 8), FONT_SIZE_3_HEIGHT + 6, true);
			displayDrawFastHLine((DIRECTION_HDOP_X_POS - 7), ((DIRECTION_HDOP_Y_POS - 8) + FONT_SIZE_3_HEIGHT + 6), ((2 * 8) + 7 + 8), true);
			displayDrawFastVLine(((DIRECTION_HDOP_X_POS - 7) + ((2 * 8) + 7) + 8), ((DIRECTION_HDOP_Y_POS - 4) + (FONT_SIZE_3_HEIGHT - 1)), 4, true);
#else
			displayPrintAt(DIRECTION_HDOP_X_POS, (DIRECTION_HDOP_Y_POS - 3), "HDOP", FONT_SIZE_2);
			displayPrintAt((DIRECTION_HDOP_X_POS + (2 * 16) + 2), (DIRECTION_HDOP_Y_POS + 18), "m", FONT_SIZE_2);
			displayDrawFastHLine((DIRECTION_HDOP_X_POS - 6), (DIRECTION_HDOP_Y_POS - 5), 5, true);
			displayDrawFastVLine((DIRECTION_HDOP_X_POS - 6), (DIRECTION_HDOP_Y_POS - 4), FONT_SIZE_4_HEIGHT, true);
			displayDrawFastHLine((DIRECTION_HDOP_X_POS - 6), ((DIRECTION_HDOP_Y_POS - 4) + FONT_SIZE_4_HEIGHT), ((2 * 16) + 7 + 10), true);
			displayDrawFastVLine(((DIRECTION_HDOP_X_POS - 6) + ((2 * 16) + 7) + 10), ((DIRECTION_HDOP_Y_POS - 4) + (FONT_SIZE_4_HEIGHT - 4)), 5, true);
#endif

		}

		currentPageOnMD9600Is(PAGE_DIRECTION_2)
		{
			// Course
			char deg[2] = { 176, 0 };
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			displayPrintAt((DIRECTION_COURSE_X_POS + (3 * 8)), (DIRECTION_COURSE_Y_POS + 1), deg, FONT_SIZE_1);
			displayFillRect(((DIRECTION_COURSE_X_POS + 12) - 12 - 5), (DIRECTION_COURSE_Y_POS - 6), 4, 2, false);
			displayFillRect(((DIRECTION_COURSE_X_POS + 12) + 12 + 1), (DIRECTION_COURSE_Y_POS - 6), 4, 2, false);
			displayDrawFastVLine((DIRECTION_COURSE_X_POS - 7), (((DIRECTION_COURSE_Y_POS - 3) + FONT_SIZE_3_HEIGHT + 2) - 4), 4, true);
			displayDrawFastHLine((DIRECTION_COURSE_X_POS - 6), ((DIRECTION_COURSE_Y_POS - 3) + FONT_SIZE_3_HEIGHT + 1), ((3 * 8) + 7 + 7), true);
			displayDrawFastVLine(((DIRECTION_COURSE_X_POS - 1) + ((3 * 8) + 7 + 2)), (DIRECTION_COURSE_Y_POS - 8), (FONT_SIZE_3_HEIGHT + 7), true);
			displayDrawFastHLine(((DIRECTION_COURSE_X_POS - 1) + ((3 * 8) + 7 + 2) - 4), (DIRECTION_COURSE_Y_POS - 9), 5, true);
#else
			displayPrintAt((DIRECTION_COURSE_X_POS + (3 * 16) + 1), (DIRECTION_COURSE_Y_POS + 5), deg, FONT_SIZE_2);
			displayFillRect(((DIRECTION_COURSE_X_POS + 24) - 12 - 8), (DIRECTION_COURSE_Y_POS - 4), 7, 2, false);
			displayFillRect(((DIRECTION_COURSE_X_POS + 24) + 12 + 1), (DIRECTION_COURSE_Y_POS - 4), 7, 2, false);
			displayDrawFastVLine((DIRECTION_COURSE_X_POS - 2),  (((DIRECTION_COURSE_Y_POS - 5) + FONT_SIZE_4_HEIGHT) - 4), 5, true);
			displayDrawFastHLine((DIRECTION_COURSE_X_POS - 1), ((DIRECTION_COURSE_Y_POS - 5) + FONT_SIZE_4_HEIGHT), ((3 * 16) + 7 + 2), true);
			displayDrawFastVLine(((DIRECTION_COURSE_X_POS - 1) + ((3 * 16) + 7 + 2)), (DIRECTION_COURSE_Y_POS - 5), (FONT_SIZE_4_HEIGHT + 1), true);
			displayDrawFastHLine(((DIRECTION_COURSE_X_POS - 1) + ((3 * 16) + 7 + 2) - 4), (DIRECTION_COURSE_Y_POS - 6), 5, true);
#endif
			// Heading
			{

				for (int16_t i = 0; i < 360; i += 15)
				{
					int16_t x1 = (int16_t)(DIRECTION_HEADING_X_POS + (DIRECTION_HEADING_RADIUS) * cos((double)(i * DEG_TO_RAD)));
					int16_t y1 = (int16_t)(DIRECTION_HEADING_Y_POS + (DIRECTION_HEADING_RADIUS) * sin((double)(i * DEG_TO_RAD)));

					int16_t x2 = (int16_t)(DIRECTION_HEADING_X_POS + (DIRECTION_HEADING_RADIUS - ((i % 45) ? 2 : 4)) * cos((double)(i * DEG_TO_RAD)));
					int16_t y2 = (int16_t)(DIRECTION_HEADING_Y_POS + (DIRECTION_HEADING_RADIUS - ((i % 45) ? 2 : 4)) * sin((double)(i * DEG_TO_RAD)));

					if (i == 270)
					{
						displayThemeApply(THEME_ITEM_FG_GPS_COLOUR, THEME_ITEM_BG);
						displayDrawLine((x1 - 1), y1, (x2 - 1), y2, true);
					}

					displayDrawLine(x1, y1, x2, y2, true);

					if (i == 270)
					{
						displayDrawLine((x1 + 1), y1, (x2 + 1), y2, true);
						displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);
					}
				}

				displayDrawCircle(DIRECTION_HEADING_X_POS, DIRECTION_HEADING_Y_POS, DIRECTION_HEADING_RADIUS, true);
				displayDirectionFixType(FIXTYPE_NONE);
			}


			// Speed
			displayPrintAt((DIRECTION_SPEED_X_POS + (3 * VALUES_FONT_WIDTH) + 2), (DIRECTION_SPEED_Y_POS + UNITS_Y_OFFSET), "km/h", UNITS_FONT_SIZE);
		}

		displayThemeResetToDefault();
	}

	//
	// Region clearing
	//
	currentPageOnMD9600Is(PAGE_DIRECTION)
	{
		if (flags & (FIELD_SATS_CLEAR | FIELD_SATS_ZERO))
		{
			displayFillRect(DIRECTION_SATS_X_POS, (DIRECTION_SATS_Y_POS + DIRECTION_REGION_X_OFFSET), (2 * VALUES_FONT_WIDTH), (VALUES_FONT_HEIGHT - DIRECTION_REGION_H_OFFSET), true);
		}

		if (flags & (FIELD_ALT_CLEAR | FIELD_ALT_ZERO))
		{
			displayFillRect(DIRECTION_ALT_X_POS, (DIRECTION_ALT_Y_POS + DIRECTION_REGION_X_OFFSET), (4 * VALUES_FONT_WIDTH), (VALUES_FONT_HEIGHT - DIRECTION_REGION_H_OFFSET), true);
		}

		if (flags & (FIELD_HDOP_CLEAR | FIELD_HDOP_ZERO))
		{
			displayFillRect(DIRECTION_HDOP_X_POS, (DIRECTION_HDOP_Y_POS + DIRECTION_REGION_X_OFFSET), (2 * VALUES_FONT_WIDTH), (VALUES_FONT_HEIGHT - DIRECTION_REGION_H_OFFSET), true);
		}
	}

	currentPageOnMD9600Is(PAGE_DIRECTION_2)
	{
		if (flags & (FIELD_COURSE_CLEAR | FIELD_COURSE_ZERO))
		{
			displayFillRect(((DIRECTION_COURSE_X_POS + ((VALUES_FONT_WIDTH * 3) >> 1)) - 12), (DIRECTION_COURSE_Y_POS - DIRECTION_CARDINAL_OFFSET), (3 * 8), CARDINALS_REGION_HEIGHT, true);
			displayFillRect(DIRECTION_COURSE_X_POS, (DIRECTION_COURSE_Y_POS + DIRECTION_REGION_X_OFFSET), (3 * VALUES_FONT_WIDTH), (VALUES_FONT_HEIGHT - DIRECTION_REGION_H_OFFSET), true);
			displayFillCircle(DIRECTION_HEADING_X_POS, DIRECTION_HEADING_Y_POS, (DIRECTION_HEADING_RADIUS - 6), false);
			displayDirectionFixType(FIXTYPE_NONE);
		}

		if (flags & (FIELD_SPEED_CLEAR | FIELD_SPEED_ZERO))
		{
			displayFillRect(DIRECTION_SPEED_X_POS, (DIRECTION_SPEED_Y_POS + DIRECTION_REGION_X_OFFSET), (3 * VALUES_FONT_WIDTH), (VALUES_FONT_HEIGHT - DIRECTION_REGION_H_OFFSET), true);
		}
	}

	//
	// Zeroing values
	//
	currentPageOnMD9600Is(PAGE_DIRECTION)
	{
		if (flags & FIELD_SATS_ZERO)
		{
			displayPrintAt(DIRECTION_SATS_X_POS, DIRECTION_SATS_Y_POS, "--", VALUES_FONT_SIZE);
		}

		if (flags & FIELD_ALT_ZERO)
		{
			displayPrintAt(DIRECTION_ALT_X_POS, DIRECTION_ALT_Y_POS, "----", VALUES_FONT_SIZE);
		}

		if (flags & FIELD_HDOP_ZERO)
		{
			displayPrintAt(DIRECTION_HDOP_X_POS, DIRECTION_HDOP_Y_POS, "--", VALUES_FONT_SIZE);
		}
	}

	currentPageOnMD9600Is(PAGE_DIRECTION_2)
	{
		if (flags & FIELD_COURSE_ZERO)
		{
			displayPrintAt(((DIRECTION_COURSE_X_POS + ((VALUES_FONT_WIDTH * 3) >> 1)) - 12), (DIRECTION_COURSE_Y_POS - DIRECTION_CARDINAL_OFFSET), "---", VALUES_FONT_SIZE_SMALL);
			displayPrintAt(DIRECTION_COURSE_X_POS, DIRECTION_COURSE_Y_POS, "---", VALUES_FONT_SIZE);
		}

		if (flags & FIELD_SPEED_ZERO)
		{
			displayPrintAt(DIRECTION_SPEED_X_POS, DIRECTION_SPEED_Y_POS, "---", VALUES_FONT_SIZE);
		}
	}
}

static void displayDirectionCourseZeroing(void)
{
	DirectionFixtype_t ft = FIXTYPE_1D;

	if (gpsData.Status & (GPS_STATUS_2D_FIX | GPS_STATUS_3D_FIX))
	{
		ft &= ~FIXTYPE_1D;
		ft |= ((gpsData.Status & GPS_STATUS_2D_FIX) ? FIXTYPE_2D : FIXTYPE_3D);
	}

	displayDirectionInfoBackground(FIELD_COURSE_ZERO);
	displayDirectionFixType(ft);
}

static bool displayDirectionInfo(bool isFirstRun, bool forceRedraw)
{
	char buffer[SCREEN_LINE_BUFFER_SIZE];

	if (gpsData.Status & GPS_STATUS_HAS_FIX)
	{
		uint16_t inViewAndUsableSats = getTotalNumberOfUsableSats();

		if (isFirstRun || forceRedraw || (prevSatsInView != inViewAndUsableSats) ||
				(gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_COURSE_UPDATED |
						GPS_STATUS_SPEED_UPDATED | GPS_STATUS_HDOP_UPDATED | GPS_STATUS_FIXTYPE_UPDATED | GPS_STATUS_HEIGHT_UPDATED)))
		{
			// Draw the background
			if (isFirstRun || forceRedraw)
			{
				displayDirectionInfoBackground(FIELD_BACKGROUND);
			}

			// Update, only clear screen regions
			if (((isFirstRun || forceRedraw) == false) &&
					((gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_COURSE_UPDATED |
							GPS_STATUS_SPEED_UPDATED | GPS_STATUS_HDOP_UPDATED | GPS_STATUS_FIXTYPE_UPDATED | GPS_STATUS_HEIGHT_UPDATED))))
			{
				DirectionFieldsFlags_t f = 0;

				if (gpsData.Status & GPS_STATUS_COURSE_UPDATED)
				{
					f |= FIELD_COURSE_CLEAR;
				}

				if (gpsData.Status & GPS_STATUS_SPEED_UPDATED)
				{
					f |= FIELD_SPEED_CLEAR;
				}

				if (gpsData.Status & GPS_STATUS_HDOP_UPDATED)
				{
					f |= FIELD_HDOP_CLEAR;
				}

				if (gpsData.Status & GPS_STATUS_HEIGHT_UPDATED)
				{
					f |= FIELD_ALT_CLEAR;
				}

				displayDirectionInfoBackground(f);
			}

			if (isFirstRun || forceRedraw || (prevSatsInView != inViewAndUsableSats))
			{
				currentPageOnMD9600Is(PAGE_DIRECTION)
				{
					displayDirectionInfoBackground(FIELD_SATS_CLEAR);
					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%2u", inViewAndUsableSats);
					displayPrintAt(DIRECTION_SATS_X_POS, DIRECTION_SATS_Y_POS, buffer, VALUES_FONT_SIZE);
				}
			}

			currentPageOnMD9600Is(PAGE_DIRECTION_2)
			{
				if (isFirstRun || forceRedraw || (gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_FIXTYPE_UPDATED | GPS_STATUS_COURSE_UPDATED)))
				{
					DirectionFixtype_t ft = FIXTYPE_1D;

					if (gpsData.Status & (GPS_STATUS_2D_FIX | GPS_STATUS_3D_FIX))
					{
						ft &= ~FIXTYPE_1D;
						ft |= ((gpsData.Status & GPS_STATUS_2D_FIX) ? FIXTYPE_2D : FIXTYPE_3D);
					}

					displayDirectionFixType(ft);
				}

				if (isFirstRun || forceRedraw || (gpsData.Status & GPS_STATUS_COURSE_UPDATED))
				{
					int dir = (int)(((gpsData.CourseInHundredthDeg * 1E-2) + 11.25f) / 22.5f);

					if (gpsData.SpeedInHundredthKn > GPS_SPEED_THRESHOLD_MIN)
					{
						int16_t x1, x2, x3, y1, y2, y3;

						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s", directions[dir % 16]);
						displayPrintAt(((DIRECTION_COURSE_X_POS + ((VALUES_FONT_WIDTH * 3) >> 1)) - ((strlen(buffer) * 8) >> 1)), (DIRECTION_COURSE_Y_POS - DIRECTION_CARDINAL_OFFSET), buffer, VALUES_FONT_SIZE_SMALL);

						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%03u", (uint16_t)(gpsData.CourseInHundredthDeg * 1E-2));
						displayPrintAt(DIRECTION_COURSE_X_POS, DIRECTION_COURSE_Y_POS, buffer, VALUES_FONT_SIZE);

						displayThemeApply(THEME_ITEM_FG_GPS_COLOUR, THEME_ITEM_BG);

						// Needle
						float needle = ((gpsData.CourseInHundredthDeg * 1E-2) - 90.0);

						if (needle > 360.0)
						{
							needle -= 360.0;
						}

						needle *= DEG_TO_RAD;

						x1 = (int16_t)(DIRECTION_HEADING_X_POS + 1 + TOP_ARROW_OFFSET * cos(needle));
						y1 = (int16_t)(DIRECTION_HEADING_Y_POS + 1 + TOP_ARROW_OFFSET * sin(needle));

						x2 = (int16_t)(DIRECTION_HEADING_X_POS + 1 + BASE_ARROW_OFFSET * cos(needle + (30 * DEG_TO_RAD)));
						y2 = (int16_t)(DIRECTION_HEADING_Y_POS + 1 + BASE_ARROW_OFFSET * sin(needle + (30 * DEG_TO_RAD)));

						x3 = (int16_t)(DIRECTION_HEADING_X_POS + 1 + BASE_ARROW_OFFSET * cos(needle - (30 * DEG_TO_RAD)));
						y3 = (int16_t)(DIRECTION_HEADING_Y_POS + 1 + BASE_ARROW_OFFSET * sin(needle - (30 * DEG_TO_RAD)));

						displayFillTriangle(x1, y1, x2, y2, x3, y3, true);
						displayThemeResetToDefault();
					}
					else
					{
						displayDirectionCourseZeroing();
					}
				}

				if (isFirstRun || forceRedraw || (gpsData.Status & GPS_STATUS_SPEED_UPDATED))
				{
					uint16_t speed = (uint16_t)((gpsData.SpeedInHundredthKn * 1E-2) * KMPH_PER_KNOT);

					// Speed < 1km/h and Course is not not updated => display zero course
					if ((speed == 0) && ((isFirstRun || forceRedraw) == false) && ((gpsData.Status & GPS_STATUS_COURSE_UPDATED) == 0))
					{
						displayDirectionCourseZeroing();
					}

					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%03u", speed);
					displayPrintAt(DIRECTION_SPEED_X_POS, DIRECTION_SPEED_Y_POS, buffer, VALUES_FONT_SIZE);
				}
			}

			currentPageOnMD9600Is(PAGE_DIRECTION)
			{
				if (isFirstRun || forceRedraw || (gpsData.Status & GPS_STATUS_HDOP_UPDATED))
				{
					if (gpsData.AccuracyInCm < 100)
					{
						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s", "<1");
					}
					else
					{
						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%2u", SAFE_MIN((gpsData.AccuracyInCm / 100), 99));
					}

					displayPrintAt(DIRECTION_HDOP_X_POS, DIRECTION_HDOP_Y_POS, buffer, VALUES_FONT_SIZE);
				}
			}

			if (isFirstRun || forceRedraw || (gpsData.Status & GPS_STATUS_HEIGHT_UPDATED))
			{
				currentPageOnMD9600Is(PAGE_DIRECTION)
				{
					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%4d", gpsData.HeightInM);
					displayPrintAt(DIRECTION_ALT_X_POS, DIRECTION_ALT_Y_POS, buffer, VALUES_FONT_SIZE);
				}
			}

			return true;
		}
	}
	else
	{
		//                               |--> fix got lost
		if (isFirstRun || forceRedraw || (gpsData.Status & GPS_STATUS_FIX_UPDATED))
		{
			displayDirectionInfoBackground((FIELD_BACKGROUND | FIELD_ALL_ZERO));
			return true;
		}
	}

	return false;
}

static bool displayRSSI(bool isFirstRun, bool forceRedraw)
{
	float spacing = 9.0;
	int dispPosition = 0;

	if (isFirstRun || forceRedraw || (gpsData.Status & (GPS_STATUS_GPS_SATS_UPDATED | GPS_STATUS_BD_SATS_UPDATED)))
	{
		// Update, only clear screen regions
		if (((isFirstRun || forceRedraw) == false) && (gpsData.Status & (GPS_STATUS_GPS_SATS_UPDATED | GPS_STATUS_BD_SATS_UPDATED)))
		{
			// Bars.
			displayFillRect(0, (RSSI_BARS_Y_POS - 12 - 2 - MAX_RSSI_Y_PIXELS), DISPLAY_SIZE_X, MAX_RSSI_Y_PIXELS, true);
			// Numbers
			displayFillRect(0, (RSSI_BARS_Y_POS - 8), DISPLAY_SIZE_X, (8 * 2), true);
		}

		if (isFirstRun || forceRedraw)
		{
			for(int i = 0; i < NUM_BARS; i++)
			{
				int xPos = 2 + (i * spacing);

				displayFillRect(RSSI_X_OFFSET + xPos, RSSI_BARS_Y_POS - 12 - 2, spacing - 2, 2, false);
			}
		}

		if ((gpsData.SatsInViewGP + gpsData.SatsInViewBD) > 0)
		{
			dispPosition = displayBars(gpsData.GPSatellites, gpsData.SatsInViewGP, false, dispPosition, spacing, THEME_ITEM_FG_GPS_COLOUR, THEME_ITEM_BG);
			displayBars(gpsData.BDSatellites, gpsData.SatsInViewBD, true, dispPosition, spacing, THEME_ITEM_FG_BD_COLOUR, THEME_ITEM_BG);
		}

		return true;
	}

	return false;
}

static void displaySatellitesPolar(gpsSatellitesData_t *satellitesArray, bool circle)
{
	float az, elFactor, s, c;
	int x, y;

	for (int i = 0; ((i < 32) && (satellitesArray[i].Number != 0)); i++)
	{
		if (satellitesArray[i].RSSI > 0)
		{
			az = (M_PI * satellitesArray[i].Az) / 180.0;

			if (satellitesArray[i].El <= 0)
			{
				elFactor = 0;// put in the middle of the screen if there is no real elevation data
			}
			else
			{
				elFactor = (1 - (fabs(satellitesArray[i].El) / 90)) * MAX_RADIUS;
			}

			s = sin(az);
			c = cos(az);
			x = POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2) + (s * elFactor);
			y = POLAR_GRAPHICS_Y_OFFSET - (c * elFactor);
#if ! defined(HAS_COLOURS)
			if (circle)
			{
				displayFillCircle(x, y, (DOT_RADIUS + 1), false);
				displayDrawCircle(x, y, (DOT_RADIUS + 1), true);
			}
			else
#endif
			{
				displayFillCircle(x, y, (DOT_RADIUS + 1), true);
			}
		}
	}
}

static bool displayPolar(bool isFirstRun, bool forceRedraw)
{
	if (isFirstRun || forceRedraw || (gpsData.Status & (GPS_STATUS_GPS_SATS_UPDATED | GPS_STATUS_BD_SATS_UPDATED)))
	{
		displayThemeApply(THEME_ITEM_FG_POLAR_DRAWING, THEME_ITEM_BG);

		// Update, only clear screen region
		if (((isFirstRun || forceRedraw) == false) && (gpsData.Status & (GPS_STATUS_GPS_SATS_UPDATED | GPS_STATUS_BD_SATS_UPDATED)))
		{
			displayFillCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), POLAR_GRAPHICS_Y_OFFSET, MAX_RADIUS, false);
		}

		displayDrawCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), POLAR_GRAPHICS_Y_OFFSET, MAX_RADIUS, true);
		displayDrawCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), POLAR_GRAPHICS_Y_OFFSET, (MAX_RADIUS / 3 ) * 2, true);
		displayDrawCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), POLAR_GRAPHICS_Y_OFFSET, (MAX_RADIUS / 3 ), true);

		displayDrawFastVLine(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2),
				(POLAR_GRAPHICS_Y_OFFSET) - (MAX_RADIUS + 4), (MAX_RADIUS + 4) * 2, true);
		displayDrawFastHLine((POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2)) - (MAX_RADIUS + 4),
				(POLAR_GRAPHICS_Y_OFFSET), (MAX_RADIUS + 4) * 2, true);

		if ((gpsData.SatsInViewGP + gpsData.SatsInViewBD) > 0)
		{
			displayThemeApply(THEME_ITEM_FG_GPS_COLOUR, THEME_ITEM_BG);
			displaySatellitesPolar(gpsData.GPSatellites, false);
			displayThemeApply(THEME_ITEM_FG_BD_COLOUR, THEME_ITEM_BG);
			displaySatellitesPolar(gpsData.BDSatellites, true);
		}

		displayThemeResetToDefault();

		return true;
	}

	return false;
}

static bool displayCoords(bool isFirstRun, bool forceRedraw)
{
	char buffer[LOCATION_TEXT_BUFFER_SIZE]; // larger screen is available on MD-UV380 and friends

	if (gpsData.Status & GPS_STATUS_HAS_FIX)
	{
		if (isFirstRun || forceRedraw || (gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_POSITION_UPDATED | GPS_STATUS_HDOP_UPDATED | GPS_STATUS_HEIGHT_UPDATED)))
		{
			char maidenheadBuf[7];
			char *p;

			// Update, only clear screen regions
			if (((isFirstRun || forceRedraw) == false) && (gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_POSITION_UPDATED | GPS_STATUS_HDOP_UPDATED | GPS_STATUS_HEIGHT_UPDATED)))
			{
				displayFillRect(0, 16, DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
				displayFillRect(0, 32, DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
#endif
				displayFillRect(0, MAIDENHEAD_HDOP_Y_POS, DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
			}

			buildLocationAndMaidenheadStrings(buffer, maidenheadBuf, true);

			p = strchr(buffer, ' ');

			*p = 0; // split locations Lat/Lon parts

#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			// Lat: Centered
			displayPrintAt(((DISPLAY_SIZE_X - (8 * 8)) >> 1), 16, buffer, FONT_SIZE_3);

			// Lon: Centered
			displayPrintAt(((DISPLAY_SIZE_X - (9 * 8)) >> 1), 32, (p + 1), FONT_SIZE_3);
#else
			// Lat: Left
			displayPrintAt(5, 16, buffer, FONT_SIZE_3);

			// Lon: Right
			displayPrintAt((DISPLAY_SIZE_X - ((9 * 8) + 5)), 16, (p + 1), FONT_SIZE_3);
#endif

			displayPrintAt(MAIDENHEAD_HDOP_MARGIN, MAIDENHEAD_HDOP_Y_POS, maidenheadBuf, FONT_SIZE_3);

			if (gpsData.AccuracyInCm < 100)
			{
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s", "+/-<1m");
			}
			else
			{
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "+/-%dm", (gpsData.AccuracyInCm / 100));
			}

			displayPrintAt((DISPLAY_SIZE_X - ((strlen(buffer) * 8) + MAIDENHEAD_HDOP_MARGIN)), MAIDENHEAD_HDOP_Y_POS, buffer, FONT_SIZE_3);

			return true;
		}
	}

	return false;
}

static bool displayGPSData(bool isFirstRun, bool forceRedraw)
{
	char buffer[SCREEN_LINE_BUFFER_SIZE];
	bool res = false;

	if (nonVolatileSettings.gps > GPS_MODE_OFF)
	{
		switch(menuDataGlobal.currentItemIndex)
		{
			case PAGE_COORDS: // with PAGE_RSSI on MD-UV380
				res = displayCoords(isFirstRun, forceRedraw);
#if !(defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12))
				res |= displayRSSI(isFirstRun, forceRedraw);
#endif
				break;

#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			case PAGE_RSSI:
				res = displayRSSI(isFirstRun, forceRedraw);
				break;
#endif

			case PAGE_POLAR:
				res = displayPolar(isFirstRun, forceRedraw);
				break;

			case PAGE_DIRECTION:
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			case PAGE_DIRECTION_2:
#endif
				res = displayDirectionInfo(isFirstRun, forceRedraw);
				break;

			case PAGES_MAX:
				break;
		}

		//
		// Update the Voice Prompts
		//
		if (gpsData.Status & GPS_STATUS_HAS_FIX)
		{
			bool vpIsPlaying = voicePromptsIsPlaying();
			uint16_t inViewAndUsableSats = getTotalNumberOfUsableSats();

			if (((isFirstRun || vpIsPlaying) == false) &&
					((prevSatsInView != inViewAndUsableSats) || (gpsData.Status & (GPS_STATUS_FIX_UPDATED | GPS_STATUS_POSITION_UPDATED | GPS_STATUS_HEIGHT_UPDATED))))
			{
				prevSatsInView = inViewAndUsableSats;

				if (gpsData.Status & GPS_STATUS_FIX_UPDATED)
				{
					if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
					{
						voicePromptsInitWithOverride();
					}
					else
					{
						soundSetMelody(MELODY_ACK_BEEP);
					}
				}
				else
				{
					voicePromptsInit();
				}

				if (!vpIsPlaying)
				{
					if (gpsData.Status & GPS_STATUS_HAS_POSITION)
					{
						char locationBuffer[LOCATION_TEXT_BUFFER_SIZE];
						char maidenheadBuf[7];

						buildLocationAndMaidenheadStrings(locationBuffer, maidenheadBuf, true);

						voicePromptsAppendLanguageString(currentLanguage->location);
						voicePromptsAppendString(locationBuffer);
						voicePromptsAppendPrompt(PROMPT_SILENCE);
						voicePromptsAppendPrompt(PROMPT_SILENCE);
						voicePromptsAppendPrompt(PROMPT_SILENCE);
						voicePromptsAppendString(maidenheadBuf);
					}

					if (prevSatsInView != 0xFFFF)
					{
						voicePromptsAppendLanguageString(currentLanguage->satellite);
						voicePromptsAppendInteger(prevSatsInView);
					}

					if (gpsData.Status & GPS_STATUS_HAS_HEIGHT)
					{
						voicePromptsAppendLanguageString(currentLanguage->altitude);
						voicePromptsAppendInteger(gpsData.HeightInM);
					}
				}
			}
		}
		else
		{
			//                               |--> fix got lost
			if (isFirstRun || forceRedraw || (gpsData.Status & GPS_STATUS_FIX_UPDATED))
			{
				prevSatsInView = 0xFFFF;

#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
				if (menuDataGlobal.currentItemIndex < PAGE_RSSI)   // Do not display anything on RSSI, Polar and Direction* screens
#else
				if (menuDataGlobal.currentItemIndex < PAGE_POLAR)  // Do not display anything on Polar and Direction* screens
#endif
				{
					// Update, only clear screen regions
					if (((isFirstRun || forceRedraw) == false) && (gpsData.Status & (GPS_STATUS_FIX_UPDATED)))
					{
						displayFillRect(0, 16, DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
						displayFillRect(0, 32, DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
#endif
						displayFillRect(0, MAIDENHEAD_HDOP_Y_POS, DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
					}

					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s", currentLanguage->gps_acquiring);
					displayPrintCentered(16, buffer, FONT_SIZE_3);
				}

				if (!isFirstRun)
				{
					if (gpsData.Status & GPS_STATUS_FIX_UPDATED)
					{
						if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
						{
							voicePromptsInitWithOverride();
						}
						else
						{
							soundSetMelody(MELODY_NACK_BEEP);
						}
					}
					else
					{
						voicePromptsInit();
					}
				}

				voicePromptsAppendLanguageString(currentLanguage->gps_acquiring);

				res = true;
			}

		}
	}
	else
	{
		if (isFirstRun || forceRedraw || (prevGPSState != nonVolatileSettings.gps))
		{
			if ((isFirstRun || forceRedraw) == false) // only clear if the GPS power status has changed
			{
				displayFillRect(0, (((DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) >> 1) + (FONT_SIZE_3_HEIGHT >> 1)), DISPLAY_SIZE_X, FONT_SIZE_3_HEIGHT, true);
			}

			if (nonVolatileSettings.gps == GPS_MODE_OFF)
			{
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s %s", currentLanguage->gps, currentLanguage->off);
				displayThemeApply(THEME_ITEM_FG_WARNING_NOTIFICATION, THEME_ITEM_BG);
				displayPrintCentered(((DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) >> 1) + (FONT_SIZE_3_HEIGHT >> 1), buffer, FONT_SIZE_3);
				displayThemeResetToDefault();

				if (forceRedraw == false)
				{
					voicePromptsAppendLanguageString(currentLanguage->gps);
					voicePromptsAppendLanguageString(currentLanguage->off);
				}
			}
			else
			{
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s %s", currentLanguage->no, currentLanguage->gps);
				displayThemeApply(THEME_ITEM_FG_ERROR_NOTIFICATION, THEME_ITEM_BG);
				displayPrintCentered(((DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) >> 1) + (FONT_SIZE_3_HEIGHT >> 1), buffer, FONT_SIZE_3);
				displayThemeResetToDefault();

				if (forceRedraw == false)
				{
					voicePromptsAppendLanguageString(currentLanguage->no);
					voicePromptsAppendLanguageString(currentLanguage->gps);
				}
			}

			res = true;
		}
	}

	// First run
	if ((isFirstRun || (gpsData.Status & GPS_STATUS_FIX_UPDATED)) && (voicePromptsIsPlaying() == false))
	{
		promptsPlayNotAfterTx();
	}

	return res;
}

static void updateScreen(bool isFirstRun, bool forceRedraw)
{
	bool res;

	if (isFirstRun || forceRedraw)
	{
		char buffer[SCREEN_LINE_BUFFER_SIZE];

		displayClearBuf();
		menuDisplayTitle(currentLanguage->gps);

		snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%d/%d", (menuDataGlobal.currentItemIndex + 1), PAGES_MAX);
		displayThemeApply(THEME_ITEM_FG_MENU_NAME, THEME_ITEM_BG);
		displayPrintCore(0, 3, buffer, FONT_SIZE_1, TEXT_ALIGN_RIGHT, false);
		displayThemeResetToDefault();
	}

	res = displayGPSData(isFirstRun, forceRedraw);

	if (isFirstRun || forceRedraw || res)
	{
		displayRender();
	}

	// GPS power mode (Off/On/NMEA) has changed, display update is now done, store the new state.
	if (prevGPSState != nonVolatileSettings.gps)
	{
		prevGPSState = nonVolatileSettings.gps;
	}

	// Clear all the *_UPDATED flags.
	gpsData.Status &= ~(GPS_STATUS_FIX_UPDATED | GPS_STATUS_FIXTYPE_UPDATED | GPS_STATUS_POSITION_UPDATED |
			GPS_STATUS_HDOP_UPDATED | GPS_STATUS_COURSE_UPDATED | GPS_STATUS_SPEED_UPDATED | GPS_STATUS_HEIGHT_UPDATED |
			GPS_STATUS_GPS_SATS_UPDATED | GPS_STATUS_BD_SATS_UPDATED);
}

static void handleEvent(uiEvent_t *ev)
{
	bool isDirty = false;

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
			isDirty = true;
			goto eventHanderExit;
		}
		else if (QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU)
		{
			menuDataGlobal.currentItemIndex = QUICKKEY_ENTRYID(ev->function);
			isDirty = true;
			goto eventHanderExit;
		}
	}

	if (KEYCHECK_SHORTUP(ev->keys, KEY_UP))
	{
		if (menuDataGlobal.currentItemIndex > PAGE_COORDS)
		{
			menuDataGlobal.currentItemIndex--;
			isDirty = true;
		}
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_DOWN))
	{
		if (menuDataGlobal.currentItemIndex < (PAGES_MAX - 1))
		{
			menuDataGlobal.currentItemIndex++;
			isDirty = true;
		}
	}
	else if ((KEYCHECK_SHORTUP(ev->keys, KEY_GREEN) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
			// Let the operator to set the GPS power status to GPS_MODE_ON only, on longpress GREEN (A/B mic button also).
			|| (KEYCHECK_LONGDOWN(ev->keys, KEY_GREEN) && (nonVolatileSettings.gps > GPS_NOT_DETECTED) && (nonVolatileSettings.gps < GPS_MODE_ON))
#endif
	)
	{
		gpsOnUsingQuickKey(true);
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
	{
		gpsOnUsingQuickKey(false);
	}
	else if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		menuSystemPopPreviousMenu();
	}
	else if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2)))
	{
		saveQuickkeyMenuIndex(ev->keys.key, menuSystemGetCurrentMenuNumber(), menuDataGlobal.currentItemIndex, 0);
	}

	eventHanderExit:

	if (isDirty)
	{
		updateScreen(false, true);
	}
}
#endif
