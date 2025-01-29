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
#include "interfaces/pit.h"
#include "functions/satellite.h"
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#include "interfaces/batteryAndPowerManagement.h"
#include "semphr.h"
#include "interfaces/gps.h"
#endif


static const uint8_t daysPerMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
static char keypadInputDigits[17]; // HHMMSS + terminator (Displayed as HH:MM:SS, or YYYY:MM:DD or LAT.LATIT,LON.LONGI)
static int keypadInputDigitsLength = 0;
static menuStatus_t menuRadioInfosExitCode = MENU_STATUS_SUCCESS;

static struct tm timeAndDate;
bool latLonIsSouthernHemisphere = false;
bool latLonIsWesternHemisphere = false;







static int displayMode = RADIO_INFOS_CURRENT_TIME;


static void updateScreen(uiEvent_t *ev, bool forceRedraw);
static void handleEvent(uiEvent_t *ev);
static void updateVoicePrompts(bool spellIt, bool firstRun);
static uint32_t menuRadioInfosNextUpdateTime = 0;

static uint32_t inputDigitsLonToFixed_10_5(void)
{
	uint32_t inPart =		((keypadInputDigits[6] - '0') * 100) +
							((keypadInputDigits[7] - '0') *  10) +
							((keypadInputDigits[8] - '0') *   1);

	uint32_t decimalPart =	((keypadInputDigits[9] - '0')  * 10000) +
							((keypadInputDigits[10] - '0') *  1000) +
							((keypadInputDigits[11] - '0') *   100) +
							((keypadInputDigits[12] - '0') *    10);

	uint32_t fixedVal = (inPart << 23) + decimalPart;
	if (latLonIsWesternHemisphere)
	{
		fixedVal |= 0x80000000;// set MSB to indicate negative number / southern hemisphere
	}
	return fixedVal;
}

static uint32_t inputDigitsLatToFixed_10_5(void)
{
	uint32_t inPart =		((keypadInputDigits[0] - '0') *  10) +
							((keypadInputDigits[1] - '0') *   1);
	uint32_t decimalPart =	((keypadInputDigits[2] - '0') * 10000) +
							((keypadInputDigits[3] - '0') *  1000) +
							((keypadInputDigits[4] - '0') *   100)+
							((keypadInputDigits[5] - '0') *    10);

	uint32_t fixedVal = (inPart << 23) + decimalPart;
	if (latLonIsSouthernHemisphere)
	{
		fixedVal |= 0x80000000;// set MSB to indicate negative number / southrern hemisphere
	}
	return fixedVal;
}

menuStatus_t menuRadioInfos(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		keypadInputDigits[0] = 0;
		keypadInputDigitsLength = 0;
		menuDataGlobal.numItems = NUM_RADIO_INFOS_MENU_ITEMS;
		displayClearBuf();
		menuDisplayTitle(currentLanguage->radio_info);
		displayRenderRows(0, 2);

		if (nonVolatileSettings.locationLat != SETTINGS_UNITIALISED_LOCATION_LAT)
		{
			if (nonVolatileSettings.locationLat & 0x80000000)
			{
				latLonIsSouthernHemisphere = true;
			}
			if (nonVolatileSettings.locationLon & 0x80000000)
			{
				latLonIsWesternHemisphere = true;
			}
		}

		updateScreen(ev, true);

		updateVoicePrompts(true, true);
	}
	else
	{
		if (ev->time > menuRadioInfosNextUpdateTime)
		{
			menuRadioInfosNextUpdateTime = ev->time + 500;
			updateScreen(ev, false);// update the screen each 500ms to show any changes to the battery voltage or low battery
		}

		menuRadioInfosExitCode = MENU_STATUS_SUCCESS;

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
	}
	return menuRadioInfosExitCode;
}

static void updateScreen(uiEvent_t *ev, bool forceRedraw)
{
	static bool blink = false;
	bool renderArrowOnly = true;
	char buffer[SCREEN_LINE_BUFFER_SIZE];

	switch (displayMode)
	{
		case RADIO_INFOS_CURRENT_TIME:
			{
				displayClearBuf();
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s%s", currentLanguage->time, ((nonVolatileSettings.timezone & 0x80) ? "" : " UTC"));
				menuDisplayTitle(buffer);

				if (keypadInputDigitsLength == 0)
				{
					time_t_custom t = uiDataGlobal.dateTimeSecs + ((nonVolatileSettings.timezone & 0x80) ? ((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60) : 0);

					gmtime_r_Custom(&t, &timeAndDate);

					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%02u:%02u:%02u", timeAndDate.tm_hour, timeAndDate.tm_min, timeAndDate.tm_sec);

					if (voicePromptsIsPlaying() == false)
					{
						updateVoicePrompts(false, false);
					}
				}
				else
				{
					strcpy(buffer,"__:__:__");
					int bufPos = 0;
					for (int i = 0; i < keypadInputDigitsLength; i++)
					{
						buffer[bufPos++] = keypadInputDigits[i];
						if ((bufPos == 2) || (bufPos == 5))
						{
							bufPos++;
						}
					}

					displayThemeApply(THEME_ITEM_FG_TEXT_INPUT, THEME_ITEM_BG);
				}

				displayPrintCentered((DISPLAY_SIZE_Y / 2) - 8, buffer, FONT_SIZE_4);
				renderArrowOnly = false;

				displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);
				// Up/Down blinking arrow
				displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 1), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
				displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 5), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
			}
			break;

		case RADIO_INFOS_DATE:
			displayClearBuf();
			snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s %s", currentLanguage->date, ((nonVolatileSettings.timezone & 0x80) ? "" : "UTC"));
			menuDisplayTitle(buffer);

			if (keypadInputDigitsLength == 0)
			{
				time_t_custom t = uiDataGlobal.dateTimeSecs + ((nonVolatileSettings.timezone & 0x80) ? ((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60) : 0);

				gmtime_r_Custom(&t, &timeAndDate);
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%04u-%02u-%02u", (timeAndDate.tm_year + 1900), (timeAndDate.tm_mon + 1), timeAndDate.tm_mday);

				if (voicePromptsIsPlaying() == false)
				{
					updateVoicePrompts(false, false);
				}
			}
			else
			{
				strcpy(buffer, "____-__-__");

				int bufPos = 0;
				for(int i = 0; i < keypadInputDigitsLength; i++)
				{
					buffer[bufPos++] = keypadInputDigits[i];
					if ((bufPos == 4) || (bufPos == 7))
					{
						bufPos++;
					}
				}
				displayThemeApply(THEME_ITEM_FG_TEXT_INPUT, THEME_ITEM_BG);
			}

			displayPrintCentered((DISPLAY_SIZE_Y / 2) - 8, buffer, FONT_SIZE_3);

			renderArrowOnly = false;

			displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);
			// Up/Down blinking arrow
			displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 1), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
			displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 5), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
			break;

		case RADIO_INFOS_LOCATION:
			displayClearBuf();
			menuDisplayTitle(currentLanguage->location);
			{
				char maidenheadBuf[7];

				if (keypadInputDigitsLength == 0)
				{
					bool locIsValid = (nonVolatileSettings.locationLat != SETTINGS_UNITIALISED_LOCATION_LAT);

					buildLocationAndMaidenheadStrings(buffer, maidenheadBuf, locIsValid);

					if (locIsValid == false)
					{
						displayPrintCentered((DISPLAY_SIZE_Y / 2) + 8, currentLanguage->not_set, FONT_SIZE_3);
					}

					if (voicePromptsIsPlaying() == false)
					{
						updateVoicePrompts(false, false);
					}
				}
				else
				{
					int bufPos = 0;

					sprintf(buffer, "__.____%c ___.____%c",
							currentLanguageGetSymbol(latLonIsSouthernHemisphere ? SYMBOLS_SOUTH : SYMBOLS_NORTH),
							currentLanguageGetSymbol(latLonIsWesternHemisphere ? SYMBOLS_WEST : SYMBOLS_EAST));

					for(int i = 0; i < keypadInputDigitsLength; i++)
					{
						buffer[bufPos++] = keypadInputDigits[i];

						if ((bufPos == 2) || (bufPos == 12))
						{
							bufPos++;
						}
						else if (bufPos == 7)
						{
							bufPos +=2;
						}
					}

					maidenheadBuf[0] = 0;
					displayThemeApply(THEME_ITEM_FG_TEXT_INPUT, THEME_ITEM_BG);
				}

				displayPrintCentered((DISPLAY_SIZE_Y / 2) - 8, buffer,
#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
						FONT_SIZE_3
#else
						FONT_SIZE_1
#endif
				);
				displayPrintCentered(((DISPLAY_SIZE_Y / 4) * 3) - 8 , maidenheadBuf, FONT_SIZE_3);
			}
			renderArrowOnly = false;

			displayThemeApply(THEME_ITEM_FG_DECORATION, THEME_ITEM_BG);
			// Up/Down blinking arrow
			displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 1), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
			displayFillTriangle(63 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 5), 59 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), 67 + DISPLAY_H_OFFSET, (DISPLAY_SIZE_Y - 3), blink);
			break;


	}

	blink = !blink;
	displayThemeResetToDefault();

	displayRenderRows((renderArrowOnly ? (DISPLAY_NUMBER_OF_ROWS - 1) : 0), DISPLAY_NUMBER_OF_ROWS);
}

static void handleEvent(uiEvent_t *ev)
{
	if (ev->events & FUNCTION_EVENT)
	{
		if (ev->function == FUNC_REDRAW)
		{
			updateScreen(ev, true);
			return;
		}
		else if (QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU)
		{
			displayMode = QUICKKEY_ENTRYID(ev->function);

			updateScreen(ev, true);
			updateVoicePrompts(true, false);
			return;
		}
	}

	if (ev->events & BUTTON_EVENT)
	{
		if (repeatVoicePromptOnSK1(ev))
		{
			return;
		}

	}

	if ((KEYCHECK_SHORTUP(ev->keys, KEY_GREEN) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
#if defined(PLATFORM_MD9600)
			// Let the operator to set the GPS power status to GPS_MODE_ON only, on longpress GREEN (A/B mic button also).
			|| (KEYCHECK_LONGDOWN(ev->keys, KEY_GREEN) && (nonVolatileSettings.gps > GPS_NOT_DETECTED) && (nonVolatileSettings.gps < GPS_MODE_ON))
#endif
	)
	{
		if (displayMode == RADIO_INFOS_LOCATION)
		{
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			gpsOnUsingQuickKey(true);
#endif
		}

		return;
	}

	if ((ev->keys.event & (KEY_MOD_UP | KEY_MOD_LONG)) == KEY_MOD_UP)
	{
		switch(ev->keys.key)
		{
			case KEY_GREEN:
				if (keypadInputDigitsLength == 0)
				{
					menuSystemPopPreviousMenu();
				}
				else
				{
					switch(displayMode)
					{
						case RADIO_INFOS_DATE:
						{
							// get the current date time because the time is constantly changing
							time_t_custom t = uiDataGlobal.dateTimeSecs + ((nonVolatileSettings.timezone & 0x80) ? ((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60) : 0);
							gmtime_r_Custom(&t, &timeAndDate);// get the current date time as the date may have changed since starting to enter the time.

							timeAndDate.tm_mon		= (((keypadInputDigits[4] - '0') * 10) + (keypadInputDigits[5] - '0')) - 1;
							timeAndDate.tm_mday	= ((keypadInputDigits[6] - '0') * 10) + (keypadInputDigits[7] - '0');

							timeAndDate.tm_year = (((keypadInputDigits[0] - '0') * 1000) +
									((keypadInputDigits[1] - '0') * 100) +
									((keypadInputDigits[2] - '0') * 10) +
									((keypadInputDigits[3] - '0'))) - 1900;

							uiSetUTCDateTimeInSecs(mktime_custom(&timeAndDate) - ((nonVolatileSettings.timezone & 0x80)?((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60):0));
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
							setRtc_custom(uiDataGlobal.dateTimeSecs);
#endif
							keypadInputDigits[0] = 0;// clear digits
							keypadInputDigitsLength = 0;

							menuSatelliteScreenClearPredictions(false);

							nextKeyBeepMelody = (int16_t *)MELODY_ACK_BEEP;

							updateScreen(ev, true);
						}
							break;

						case RADIO_INFOS_LOCATION:
							if (keypadInputDigitsLength == 13)
							{
								settingsSet(nonVolatileSettings.locationLat, inputDigitsLatToFixed_10_5());
								settingsSet(nonVolatileSettings.locationLon, inputDigitsLonToFixed_10_5());

								keypadInputDigits[0] = 0;// clear digits
								keypadInputDigitsLength = 0;
								menuSatelliteScreenClearPredictions(false);
								aprsBeaconingInvalidateFixedPosition();
								updateScreen(ev, true);
								nextKeyBeepMelody = (int16_t *)MELODY_ACK_BEEP;
							}
							else
							{
								nextKeyBeepMelody = (int16_t *)MELODY_NACK_BEEP;
							}
							break;

						default:
						{
							const int multipliers[6] = { 36000, 3600, 600, 60, 10, 1 };

							if (keypadInputDigitsLength == 6)
							{
								int newTime = 0;

								for(int i = 0; i < 6; i++)
								{
									newTime += (keypadInputDigits[i] - '0') * multipliers[i];
								}
								if (displayMode == RADIO_INFOS_CURRENT_TIME)
								{
									PIT2SecondsCounter = 0;// Stop PIT2SecondsCounter rolling over during the next operations
									time_t_custom t = uiDataGlobal.dateTimeSecs + ((nonVolatileSettings.timezone & 0x80)?((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60):0);
									gmtime_r_Custom(&t, &timeAndDate);// get the current date time as the date may have changed since starting to enter the time.
									timeAndDate.tm_hour 	= ((keypadInputDigits[0] - '0') * 10) + (keypadInputDigits[1] - '0');
									timeAndDate.tm_min		= ((keypadInputDigits[2] - '0') * 10) + (keypadInputDigits[3] - '0');
									timeAndDate.tm_sec		= ((keypadInputDigits[4] - '0') * 10) + (keypadInputDigits[5] - '0');

									PIT2SecondsCounter = 0;//Synchronise PIT2SecondsCounter
									uiSetUTCDateTimeInSecs(mktime_custom(&timeAndDate) - ((nonVolatileSettings.timezone & 0x80)?((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60):0));
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
									setRtc_custom(uiDataGlobal.dateTimeSecs);
#endif
									menuSatelliteScreenClearPredictions(false);
								}
								else
								{
									// alarm
									uiDataGlobal.SatelliteAndAlarmData.alarmTime = newTime;

									if (BUTTONCHECK_DOWN(ev, BUTTON_SK2))
									{
										uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_CLOCK;
										powerOffFinalStage(true, false);
									}
								}
								keypadInputDigits[0] = 0;// clear digits
								keypadInputDigitsLength = 0;
								updateScreen(ev, true);
								nextKeyBeepMelody = (int16_t *)MELODY_ACK_BEEP;
							}
							break;
						}
					}
				}
				return;
				break;

			case KEY_RED:
				if ((displayMode == RADIO_INFOS_LOCATION) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
				{
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
					gpsOnUsingQuickKey(false);
#endif
					return;
				}
				else if (((displayMode == RADIO_INFOS_CURRENT_TIME) || (displayMode == RADIO_INFOS_DATE) || (displayMode == RADIO_INFOS_LOCATION))
					&& (keypadInputDigitsLength != 0))
				{
					keypadInputDigits[0] = 0;
					keypadInputDigitsLength = 0;
					updateScreen(ev, true);
				}
				else
				{
					menuSystemPopPreviousMenu();
				}

				return;
				break;
		}
	}


	if (ev->keys.event & KEY_MOD_PRESS)
	{
		switch(ev->keys.key)
		{
			case KEY_DOWN:
				if (keypadInputDigitsLength != 0)
				{
					if (displayMode == RADIO_INFOS_LOCATION)
					{
						char buf[2] = { 0, 0 };

						if (BUTTONCHECK_DOWN(ev, BUTTON_SK2))
						{
							latLonIsWesternHemisphere = true;
							buf[0] = currentLanguageGetSymbol(latLonIsWesternHemisphere ? SYMBOLS_WEST : SYMBOLS_EAST);
						}
						else
						{
							latLonIsSouthernHemisphere = true;
							buf[0] = currentLanguageGetSymbol(latLonIsSouthernHemisphere ? SYMBOLS_SOUTH : SYMBOLS_NORTH);
						}
						voicePromptsInit();
						voicePromptsAppendString(buf);
						voicePromptsPlay();
						updateScreen(ev, false);
					}
					return;
				}
				if (displayMode < (NUM_RADIO_INFOS_MENU_ITEMS - 1))
				{
					displayMode++;
					updateScreen(ev, true);
					updateVoicePrompts(true, false);
				}
				break;

			case KEY_UP:
				if (keypadInputDigitsLength != 0)
				{
					if (displayMode == RADIO_INFOS_LOCATION)
					{
						char buf[2] = { 0, 0 };

						if (BUTTONCHECK_DOWN(ev, BUTTON_SK2))
						{
							latLonIsWesternHemisphere = false;
							buf[0] = currentLanguageGetSymbol(latLonIsWesternHemisphere ? SYMBOLS_WEST : SYMBOLS_EAST);
						}
						else
						{
							latLonIsSouthernHemisphere = false;
							buf[0] = currentLanguageGetSymbol(latLonIsSouthernHemisphere ? SYMBOLS_SOUTH : SYMBOLS_NORTH);
						}
						voicePromptsInit();
						voicePromptsAppendString(buf);
						voicePromptsPlay();
						updateScreen(ev, false);
					}
					return;
				}

				if (displayMode > RADIO_INFOS_CURRENT_TIME)
				{
					displayMode--;
					updateScreen(ev, true);
					updateVoicePrompts(true, false);
				}
				break;

			case KEY_LEFT:
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			case KEY_ROTARY_DECREMENT:
#endif
				switch(displayMode)
				{
					case RADIO_INFOS_CURRENT_TIME:
					case RADIO_INFOS_DATE:
					case RADIO_INFOS_LOCATION:
						{
							if (keypadInputDigitsLength > 0)
							{
								keypadInputDigits[keypadInputDigitsLength - 1] = 0;
								keypadInputDigitsLength--;
							}
							updateScreen(ev, true);
						}
						break;
				}
				break;

			case KEY_RIGHT:
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			case KEY_ROTARY_INCREMENT:
#endif
				break;

			default:
				if (BUTTONCHECK_DOWN(ev, BUTTON_SK2) == 0) // Filtering for QuickKey
				{
					int keyval = menuGetKeypadKeyValue(ev, true);
					if (keyval != 99)
					{
						int maxDigitsOnThisScreen;
						switch(displayMode)
						{
							case RADIO_INFOS_DATE:
								maxDigitsOnThisScreen = 8;
							break;
							case RADIO_INFOS_LOCATION:
								maxDigitsOnThisScreen = 13;
								break;
							case RADIO_INFOS_CURRENT_TIME:
								maxDigitsOnThisScreen = 6;
								break;
							default:
								return;// No other screens have key entry
								break;
						}

						if (keypadInputDigitsLength < maxDigitsOnThisScreen)
						{
							const uint8_t MAX_DIGIT_VALUE[3][9] = {
									{ 2, 9, 5, 9, 5, 9, 0, 0, 0 },
									{ 2, 9, 5, 9, 5, 9, 0, 0, 0 },
									{ 2, 9, 5, 9, 5, 9, 0, 0, 0 }
							};
							const uint8_t MAX_YEAR_DIGIT_VALUES[5] = { 2, 0, 4, 9, 1 };
							int maxDigValue = 9;
							int minDigValue = 0;

							switch(displayMode)
							{
								case RADIO_INFOS_DATE:
									switch(keypadInputDigitsLength)
									{
										case 0:
										case 1:
										case 2:
										case 3:
										case 4:
											// Year digits, and first digit of the month
											maxDigValue = MAX_YEAR_DIGIT_VALUES[keypadInputDigitsLength];
											break;
										case 5:
											// second digit of the month
											maxDigValue = (keypadInputDigits[4] == '1') ? 2 : 9;
											if (keypadInputDigits[4] == '0')
											{
												minDigValue = 1;
											}
											break;
										case 6:
											// first digit of the day
											maxDigValue = daysPerMonth[(((keypadInputDigits[4] - '0') * 10) + (keypadInputDigits[5] - '0')) - 1] / 10;
											break;
										case 7:
											{
												// second digit of the day
												uint32_t month = (((keypadInputDigits[4] - '0') * 10) + (keypadInputDigits[5] - '0'));

												if (month == 2)
												{
													// handle leap years
													uint32_t year = 0;
													uint32_t multiplier = 1000;

													for(int i = 0; i < 4; i++)
													{
														year += (keypadInputDigits[i] - '0') * multiplier;
														multiplier /= 10;
													}

													if ((keypadInputDigits[6] - '0') == 2) // 2_: Feb, 28 or 29 days (leap year checking)
													{
														if ((year % 4) == 0)
														{
															maxDigValue = 9;
														}
														else
														{
															maxDigValue = 8;
														}
													}
												}
												else
												{
													if ((keypadInputDigits[6] - '0') < 3)
													{
														maxDigValue = 9;
													}
													else
													{
														maxDigValue = daysPerMonth[month - 1] % 10;
													}
												}

												if (keypadInputDigits[6] == '0')
												{
													minDigValue = 1;
												}
											}
											break;
									}

								break;
								case RADIO_INFOS_LOCATION:

									maxDigValue = 9;// default value if no override is applied below

									switch(keypadInputDigitsLength)
									{
										case 1:
										case 2:
										case 3:
										case 4:
										case 5:
											if (keypadInputDigits[0] == '9')
											{
												maxDigValue = 0;
											}
											break;
										case 6:
											maxDigValue = 1;
											break;
										case 7:
											if (keypadInputDigits[6] == '1')
											{
												maxDigValue = 8;
											}
											break;
										case 8:
										case 9:
										case 10:
										case 11:
										case 12:
											if ((keypadInputDigits[6] == '1') && (keypadInputDigits[7] == '8'))
											{
												maxDigValue = 0;
											}
											break;
									}

									break;
								case RADIO_INFOS_CURRENT_TIME:
									if (keypadInputDigitsLength == 1 && (keypadInputDigits[0] == '2'))
									{
										maxDigValue = 3;
									}
									else
									{
										maxDigValue = MAX_DIGIT_VALUE[0][keypadInputDigitsLength];
									}
									break;
							}

							if ((keyval <=  maxDigValue) && (keyval >= minDigValue))
							{
								char c[2] = {0, 0};
								c[0] = keyval + '0';

								if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
								{
									voicePromptsInit();
									switch(displayMode)
									{
										case RADIO_INFOS_LOCATION:
											switch(keypadInputDigitsLength)
											{
												case 0:
													if (latLonIsSouthernHemisphere)
													{
														voicePromptsAppendPrompt(PROMPT_MINUS);
													}
													break;
												case 5:
													if (latLonIsWesternHemisphere)
													{
														voicePromptsAppendPrompt(PROMPT_MINUS);
													}
													break;
											}
										break;
									}
									voicePromptsAppendString(c);

									switch(displayMode)
									{
										case RADIO_INFOS_LOCATION:
											if ((keypadInputDigitsLength == 1) || (keypadInputDigitsLength == 8))
											{
												voicePromptsAppendPrompt(PROMPT_POINT);
											}
											else if (keypadInputDigitsLength == 5)
											{
												char buf[2] = { 0, 0 };
												buf[0] = currentLanguageGetSymbol(latLonIsSouthernHemisphere ? SYMBOLS_SOUTH : SYMBOLS_NORTH);
												voicePromptsAppendString(buf);
											}
											else if (keypadInputDigitsLength == 12)
											{
												char buf[2] = { 0, 0 };
												buf[0] = currentLanguageGetSymbol(latLonIsWesternHemisphere ? SYMBOLS_WEST : SYMBOLS_EAST);
												voicePromptsAppendString(buf);
											}
										break;
									}
									voicePromptsPlay();
								}

								strcat(keypadInputDigits, c);
								keypadInputDigitsLength++;
								updateScreen(ev, true);
								nextKeyBeepMelody = (int16_t *)MELODY_KEY_BEEP;
							}
							else
							{
								if (keypadInputDigitsLength != 0)
								{
									nextKeyBeepMelody = (int16_t *)MELODY_NACK_BEEP;
								}
							}
						}
						else
						{
							nextKeyBeepMelody = (int16_t *)MELODY_NACK_BEEP;
						}
					}
				}
				break;
		}
	}


	if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2)))
	{
		saveQuickkeyMenuIndex(ev->keys.key, menuSystemGetCurrentMenuNumber(), displayMode, 0);
	}
}

void menuRadioInfosInit(void)
{
}


static void updateVoicePrompts(bool spellIt, bool firstRun)
{
	if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
	{
		char buffer[LOCATION_TEXT_BUFFER_SIZE];

		voicePromptsInit();

		if (firstRun)
		{
			voicePromptsAppendPrompt(PROMPT_SILENCE);
			voicePromptsAppendLanguageString(currentLanguage->radio_info);
			voicePromptsAppendLanguageString(currentLanguage->menu);
			voicePromptsAppendPrompt(PROMPT_SILENCE);
		}

		switch (displayMode)
		{
			case RADIO_INFOS_CURRENT_TIME:
				voicePromptsAppendLanguageString(currentLanguage->time);
				if (!(nonVolatileSettings.timezone & 0x80))
				{
					voicePromptsAppendString("UTC");
				}
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%02u %02u%02u", timeAndDate.tm_hour, timeAndDate.tm_min, timeAndDate.tm_sec);
				voicePromptsAppendString(buffer);
			break;
			case RADIO_INFOS_LOCATION:
				voicePromptsAppendLanguageString(currentLanguage->location);
				if (nonVolatileSettings.locationLat != SETTINGS_UNITIALISED_LOCATION_LAT)
				{
					char maidenheadBuf[7];

					buildLocationAndMaidenheadStrings(buffer, maidenheadBuf, true);
					voicePromptsAppendString(buffer);
					voicePromptsAppendPrompt(PROMPT_SILENCE);
					voicePromptsAppendPrompt(PROMPT_SILENCE);
					voicePromptsAppendPrompt(PROMPT_SILENCE);
					voicePromptsAppendString(maidenheadBuf);
				}
				else
				{
					voicePromptsAppendLanguageString(currentLanguage->not_set);
				}
			break;
			case RADIO_INFOS_DATE:
				voicePromptsAppendLanguageString(currentLanguage->date);
				snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%04u %02u %02u", (timeAndDate.tm_year + 1900),(timeAndDate.tm_mon + 1),timeAndDate.tm_mday);
				voicePromptsAppendString(buffer);
			break;
		}

		if (spellIt)
		{
			promptsPlayNotAfterTx();
		}
	}
}
