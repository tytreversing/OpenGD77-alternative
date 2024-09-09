/*
 * Copyright (C) 2019-2024 Roger Clark, VK3KYY / G4KYF
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
#include <time.h>
#include "user_interface/uiGlobals.h"
#include "utils.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiLocalisation.h"
#include "user_interface/uiUtilities.h"
#include "interfaces/clockManager.h"
#include "interfaces/pit.h"
#include "functions/satellite.h"
#include "hardware/radioHardwareInterface.h"
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#include "interfaces/batteryAndPowerManagement.h"
#endif
#if defined(USING_EXTERNAL_DEBUGGER)
#include "SeggerRTT/RTT/SEGGER_RTT.h"
#endif

#if defined(PLATFORM_RD5R)
#define NUM_PASSES_TO_DISPLAY_ON_LIST_SCREEN 2
#elif defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#if defined(PLATFORM_VARIANT_DM1701)
#define NUM_PASSES_TO_DISPLAY_ON_LIST_SCREEN 5
#else
#define NUM_PASSES_TO_DISPLAY_ON_LIST_SCREEN 6
#endif
#else
#define NUM_PASSES_TO_DISPLAY_ON_LIST_SCREEN 3
#endif


static const uint32_t ALARM_OFFSET_SECS = 60;

enum
{
	SATELLITE_SCREEN_ALL_PREDICTIONS_LIST,
	SATELLITE_SCREEN_SELECTED_SATELLITE,
	SATELLITE_SCREEN_SELECTED_SATELLITE_POLAR,
	SATELLITE_SCREEN_SELECTED_SATELLITE_PREDICTION,
	NUM_SATELLITE_SCREEN_ITEMS
};

static void handleEvent(uiEvent_t *ev);
static void updateScreen(uiEvent_t *ev, bool firstRun, bool announceVP);
static bool calculatePredictionsForSatelliteIndex(int satelliteIndex);
static void loadKeps(void);
static int menuSatelliteFindNextSatellite(void);
static void exitCallback(void *data);
static void selectSatellite(uint32_t selectedSatellite);
static void calculateActiveSatelliteData(bool forceFrequencyUpdate);

static bool hasSatelliteKeps = false;
static bool satelliteVisible = false;
static struct_codeplugChannel_t satelliteChannelData = { .rxFreq = 0, .sql = 10U };
static struct tm timeAndDate;
static int displayMode = SATELLITE_SCREEN_ALL_PREDICTIONS_LIST;
static uint32_t nextCalculationTime = 0;
static uint32_t menuSatelliteScreenNextUpdateTime;
static char azelBuffer[SCREEN_LINE_BUFFER_SIZE];
static uint32_t numSatellitesLoaded;
static predictionStateMachineData_t currentPrediction;
static int currentlyDisplayedListPosition = 0;
static int predictionsListNumSatellitePassesDisplayed = 0;
static int rxIntPart;
static int rxDecPart;
static int txIntPart;
static int txDecPart;
static int predictionsListSelectedSatellite;
static satelliteResults_t currentSatelliteResults;
static int currentlyPredictingSatellite = 0;
static int numTotalSatellitesPredicted = 0;
static satelliteData_t *predictingSat;
static uint32_t nextAlarmBeepTime = 0;
static bool hasRecalculated;
static bool hasSelectedSatellite = false;
satelliteFreq_t currentSatelliteFreqIndex = SATELLITE_VOICE_FREQ;

menuStatus_t menuSatelliteScreen(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		struct tm buildDateTime;

		predictionsListSelectedSatellite = 0;
		currentlyPredictingSatellite = 0;

		currentPrediction.state = PREDICTION_STATE_NONE;
		currentlyDisplayedListPosition = 0;

		if (!hasSelectedSatellite)
		{
			if (nonVolatileSettings.locationLat == SETTINGS_UNITIALISED_LOCATION_LAT)
			{
				// Use the quickkeys system to enter the Radio Infos screen at a specific page
				menuSystemPopPreviousMenu();
				menuSystemPushNewMenu(MENU_RADIO_INFOS);
				ev->hasEvent = true;
				ev->function = QUICKKEY_MENUVALUE(MENU_RADIO_INFOS, RADIO_INFOS_LOCATION, 0);
				ev->events = FUNCTION_EVENT;
				menuSystemCallCurrentMenuTick(ev);
				return MENU_STATUS_SUCCESS;
			}

			memset(&buildDateTime, 0x00, sizeof(struct tm));// clear entire struct
			buildDateTime.tm_mday = BUILD_DAY;  /* day of the month, 1 to 31 */
			buildDateTime.tm_mon = BUILD_MONTH - 1;   /* months since January, 0 to 11 */
			buildDateTime.tm_year = BUILD_YEAR - 1900;  /* years since 1900 */

			time_t_custom buildYYMMDDasUtcTime = mktime_custom(&buildDateTime) - (48 * 3600);// subtract 2 days, in case of problems between build TZ and TZ of user. 2 days is conservative in case of strange TZ's

			if (uiDataGlobal.dateTimeSecs < buildYYMMDDasUtcTime)
			{
				// Use the quickkeys system to enter the Radio Infos screen at a specific page
				menuSystemPopPreviousMenu();
				menuSystemPushNewMenu(MENU_RADIO_INFOS);
				ev->hasEvent = true;
				ev->function = QUICKKEY_MENUVALUE(MENU_RADIO_INFOS, RADIO_INFOS_DATE, 0);
				ev->events = FUNCTION_EVENT;
				menuSystemCallCurrentMenuTick(ev);
				return MENU_STATUS_SUCCESS;
			}

			if (!hasSatelliteKeps)
			{
				loadKeps();
			}

			if (hasSatelliteKeps) // No Keps data, no computation
			{
				// user may have changed the location
				satelliteSetObserverLocation(
						latLongFixed32ToDouble(nonVolatileSettings.locationLat),
						latLongFixed32ToDouble(nonVolatileSettings.locationLon),
						0);// Use zero for height, as this seems to make virtually no difference to the calculations. We may however need to change this to some more average height for the ham radio population
			}

			satelliteChannelData.rxFreq = 0;
			satelliteChannelData.txFreq = 0;
			satelliteChannelData.rxTone = CODEPLUG_CSS_TONE_NONE;
			satelliteChannelData.chMode = RADIO_MODE_NONE;
			codeplugChannelSetFlag(&satelliteChannelData, CHANNEL_FLAG_BW_25K, 1);
		}

		currentChannelData = &satelliteChannelData;// Now make it available to the rest of the firmware e.g the Tx screen
		trxSetModeAndBandwidth(satelliteChannelData.chMode, true);
		trxSetRxCSS(RADIO_DEVICE_PRIMARY, satelliteChannelData.rxTone);// Never any Rx CTCSS

		currentActiveSatellite = &satelliteDataNative[uiDataGlobal.SatelliteAndAlarmData.currentSatellite];
		menuSatelliteScreenNextUpdateTime = (ev->time - 1U);

		menuSystemRegisterExitCallback(exitCallback, NULL);

		calculateActiveSatelliteData(true);
		nextCalculationTime = (ev->time + 1000U); // 1000 milliseconds

		updateScreen(ev, true, true);

	}
	else
	{
		if (hasSatelliteKeps)
		{
			if (numTotalSatellitesPredicted == 0)
			{
				clockManagerSetRunMode(kAPP_PowerModeHsrun, CLOCK_MANAGER_SPEED_HS_RUN);
			}

			while (!calculatePredictionsForSatelliteIndex(currentlyPredictingSatellite))
			{
				vTaskDelay((0 / portTICK_PERIOD_MS));
			}

			predictingSat = &satelliteDataNative[currentlyPredictingSatellite];

			if (numTotalSatellitesPredicted < numSatellitesLoaded)
			{
				if (predictingSat->predictions.passes[predictingSat->predictions.numPassBeingPredicted].valid == PREDICTION_RESULT_LIMIT)
				{
					currentlyPredictingSatellite++;
					numTotalSatellitesPredicted++;

					updateScreen(ev, false, true);
					if (numTotalSatellitesPredicted == numSatellitesLoaded)
					{
						if (settingsIsOptionBitSet(BIT_SATELLITE_MANUAL_AUTO))
						{
							// need to set the current satellite to the new first satellite in the predictions list.
							int foundSatellite = menuSatelliteFindNextSatellite();
							if (foundSatellite != -1)
							{
								selectSatellite(foundSatellite);
							}
						}
						clockManagerSetRunMode(kAPP_PowerModeHsrun,CLOCK_MANAGER_SPEED_RUN);
					}
				}
			}
			else
			{
				// keep sweeping in case any of the satellites goes LOS and the predictions need to be re-run.
				currentlyPredictingSatellite = ((currentlyPredictingSatellite + 1) % numSatellitesLoaded);
			}

			if (ev->time > nextCalculationTime)
			{
				calculateActiveSatelliteData(false);
				nextCalculationTime = ev->time + 1000; // 1000 milliseconds
			}

			if ((menuSatelliteScreenNextUpdateTime != 0) && (ev->time > menuSatelliteScreenNextUpdateTime))
			{
				updateScreen(ev, false, false);
			}

			if ((uiDataGlobal.SatelliteAndAlarmData.alarmType == ALARM_TYPE_SATELLITE) &&
				(uiDataGlobal.dateTimeSecs >= uiDataGlobal.SatelliteAndAlarmData.alarmTime))
			{
				if (ev->time > nextAlarmBeepTime)
				{
					if  (uiDataGlobal.dateTimeSecs >= (uiDataGlobal.SatelliteAndAlarmData.alarmTime + ALARM_OFFSET_SECS))
					{
						// alarm has been beeping for ALARM_OFFSET_SECS and has not been cancelled.
						// So presume the radio is unattended, turn the alarm off, and put it back into suspend.
						uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_CANCELLED;
#if ! defined(PLATFORM_MD9600)
						powerOffFinalStage(true, false);
#endif
					}
					else
					{
						soundSetMelody(MELODY_QUICKKEYS_CLEAR_ACK_BEEP);
					}

					nextAlarmBeepTime = ev->time + 1000;
				}
			}
		}

		if (ev->hasEvent)
		{
			handleEvent(ev);
		}
	}
	return MENU_STATUS_SUCCESS;
}

static void updateScreen(uiEvent_t *ev, bool firstRun, bool announceVP)
{
	if (hasSatelliteKeps == false)
	{
		if (firstRun)
		{
			displayClearBuf();

			menuDisplayTitle(currentLanguage->satellite);
			displayPrintCentered((DISPLAY_SIZE_Y / 2) - 6, currentLanguage->list_empty, FONT_SIZE_2);

			voicePromptsInit();
			voicePromptsAppendLanguageString(currentLanguage->satellite);
			voicePromptsAppendLanguageString(currentLanguage->list_empty);
			voicePromptsPlay();

			displayRender();
		}

		return;
	}
	else
	{
		char buffer[SCREEN_LINE_BUFFER_SIZE];

		// prevent all annoucements when returning from the Tx screen
		if (menuSystemGetPreviouslyPushedMenuNumber(false) == UI_TX_SCREEN)
		{
			announceVP = false;
		}

		if (firstRun)
		{
			displayClearBuf();

			// Do not display the menu title if a list of passes is available, as that list uses the
			// whole screen (and the screen flickers on the MD*-3x0)
			if ((displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST) &&
					((numTotalSatellitesPredicted != numSatellitesLoaded) || (predictionsListNumSatellitePassesDisplayed == 0)))
			{
				menuDisplayTitle(currentLanguage->satellite);
			}

			displayRender();
		}

		if (numTotalSatellitesPredicted != numSatellitesLoaded)
		{
			displayClearRows(2, 6, false);

			displayPrintCentered(16, currentLanguage->predicting, FONT_SIZE_3);
			displayDrawRect(2,
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
					(DISPLAY_SIZE_Y / 4)
#else
					(DISPLAY_SIZE_Y / 2)
#endif
					, DISPLAY_SIZE_X - 2, 12, true);
			displayFillRect(3,
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
					(DISPLAY_SIZE_Y / 4)
#else
					(DISPLAY_SIZE_Y / 2)
#endif
					, ((DISPLAY_SIZE_X - 2) * numTotalSatellitesPredicted) / numSatellitesLoaded, 12, false);
			menuSatelliteScreenNextUpdateTime = 0;// Don't do time based update

			displayRenderRows(2, 6);

			if (firstRun)
			{
				bool wasPlaying = voicePromptsIsPlaying();
				voicePromptsInit();
				if (!wasPlaying)
				{
					voicePromptsAppendLanguageString(currentLanguage->satellite);
				}
				voicePromptsAppendLanguageString(currentLanguage->predicting);
				voicePromptsPlay();
			}
			return;
		}

		// Don't stop/cut the current VP
		if (announceVP && voicePromptsIsPlaying())
		{
			announceVP = false;
		}

		switch(displayMode)
		{
			case SATELLITE_SCREEN_SELECTED_SATELLITE:
			{
				const char *freqNames[] = { currentLanguage->voice_prompt_level_1, currentLanguage->APRS, "CW Rx" };// Temporary hard coded names, will eventually need new language strings

				if(hasRecalculated || announceVP)
				{
					displayClearBuf();
				}

				uiUtilityRenderHeader(false, false);

				if (hasRecalculated || announceVP)
				{
					// calculate these now for display later...
					snprintf(azelBuffer, SCREEN_LINE_BUFFER_SIZE, "%s:%3d%c %s:%3d%c", currentLanguage->azimuth, currentSatelliteResults.azimuthAsInteger, 176, currentLanguage->elevation, currentSatelliteResults.elevationAsInteger, 176);
					displayPrintCore(0, (DISPLAY_SIZE_Y / 4), currentActiveSatellite->name, FONT_SIZE_2, TEXT_ALIGN_LEFT, false);

					displayPrintCore(0, (DISPLAY_SIZE_Y / 4), freqNames[currentSatelliteFreqIndex], FONT_SIZE_2, TEXT_ALIGN_RIGHT, false);

					displayPrintCentered((DISPLAY_SIZE_Y / 2)
#if defined(PLATFORM_RD5R)
							- 4
#else
							- 6
#endif
							, azelBuffer, FONT_SIZE_2);

					if (currentSatelliteResults.freqs[currentSatelliteFreqIndex].rxFreq != 0)
					{
						rxIntPart = currentSatelliteResults.freqs[currentSatelliteFreqIndex].rxFreq / 1E6;
						rxDecPart = (currentSatelliteResults.freqs[currentSatelliteFreqIndex].rxFreq - (rxIntPart * 1E6)) / 10;
						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "R:%3d.%05u",rxIntPart,rxDecPart);
						displayPrintCentered((DISPLAY_SIZE_Y / 2) + 6, buffer, FONT_SIZE_2);

						if (currentSatelliteResults.freqs[currentSatelliteFreqIndex].txFreq != 0)
						{
							txIntPart = currentSatelliteResults.freqs[currentSatelliteFreqIndex].txFreq / 1E6;
							txDecPart = (currentSatelliteResults.freqs[currentSatelliteFreqIndex].txFreq - (txIntPart * 1E6)) / 10;

							snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "T:%3d.%05u",txIntPart,txDecPart);
							displayPrintCentered((DISPLAY_SIZE_Y / 2) + 16, buffer, FONT_SIZE_2);
						}
					}

					displayRender();// render the whole screen;
				}
				else
				{
					displayRenderRows(0,2);
				}

				if (announceVP || hasRecalculated)
				{
					char vpBuffer[SCREEN_LINE_BUFFER_SIZE];

					if (menuSystemGetPreviouslyPushedMenuNumber(false) != UI_TX_SCREEN)
					{
						if (announceVP)
						{
							voicePromptsInit();
							voicePromptsAppendString(currentActiveSatellite->name);

							if (currentSatelliteFreqIndex == SATELLITE_OTHER_FREQ)
							{
								voicePromptsAppendString(freqNames[currentSatelliteFreqIndex]);
							}
							else
							{
								voicePromptsAppendLanguageString(freqNames[currentSatelliteFreqIndex]);
							}

							voicePromptsAppendLanguageString(currentLanguage->azimuth);
							snprintf(vpBuffer, SCREEN_LINE_BUFFER_SIZE, "%3d%c", currentSatelliteResults.azimuthAsInteger, 176);
							voicePromptsAppendString(vpBuffer);
							voicePromptsAppendLanguageString(currentLanguage->elevation);
							snprintf(vpBuffer, SCREEN_LINE_BUFFER_SIZE, "%3d%c", currentSatelliteResults.elevationAsInteger, 176);
							voicePromptsAppendString(vpBuffer);

							voicePromptsPlay();
						}
					}
				}

				menuSatelliteScreenNextUpdateTime = (ev->time + 250U);
			}
			break;

			case SATELLITE_SCREEN_SELECTED_SATELLITE_POLAR:
			{
				satellitePass_t *displayedPredictionPass  = &currentActiveSatellite->predictions.passes[currentActiveSatellite->predictions.selectedPassNumber];
				const int DOT_RADIUS = 2;
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
				const int MAX_RADIUS = 44;
#else
				const int MAX_RADIUS = ((DISPLAY_SIZE_Y / 2) - 4);
#endif
				float az, elFactor, s, c, lastX = 0, lastY = 0, x, y;
				satelliteResults_t results;
				int16_t satX = INT16_MIN, satY = INT16_MIN;
				time_t_custom displayedPassTimeDiff = displayedPredictionPass->satelliteAOS - uiDataGlobal.dateTimeSecs;

				if (displayedPredictionPass->valid == PREDICTION_RESULT_OK)
				{
					volatile uint32_t startTime;
#define POLAR_GRAPHICS_X_OFFSET  24

					if(hasRecalculated || announceVP)
					{
						struct tm dispTimeAndDate;

						 // below 1 minute displayedPassTimeDiff is used.
						if (displayedPassTimeDiff >= 60)
						{
							gmtime_r_Custom(&displayedPassTimeDiff, &dispTimeAndDate);
						}

						displayClearBuf();

						snprintf(buffer, 8,"%s", currentActiveSatellite->name);
						displayThemeApply(THEME_ITEM_FG_CHANNEL_NAME, THEME_ITEM_BG);
						displayPrintAt(4, (DISPLAY_SIZE_Y / 2) - 4, buffer, FONT_SIZE_2);

						if (announceVP)
						{
							voicePromptsAppendString(currentActiveSatellite->name);
						}

						displayThemeApply(THEME_ITEM_FG_POLAR_DRAWING, THEME_ITEM_BG);
						displayDrawCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), DISPLAY_SIZE_Y / 2, MAX_RADIUS, true);
						displayDrawCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), DISPLAY_SIZE_Y / 2, (MAX_RADIUS / 3 ) * 2, true);
						displayDrawCircle(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2), DISPLAY_SIZE_Y / 2, (MAX_RADIUS / 3 ), true);

						displayDrawFastVLine(POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2),
								(DISPLAY_SIZE_Y / 2) - (MAX_RADIUS + 4), (MAX_RADIUS + 4) * 2, true);
						displayDrawFastHLine((POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2)) - (MAX_RADIUS + 4),
								(DISPLAY_SIZE_Y / 2), (MAX_RADIUS + 4) * 2, true);
						displayThemeResetToDefault();

						if ((uiDataGlobal.dateTimeSecs < displayedPredictionPass->satelliteAOS) || (uiDataGlobal.dateTimeSecs > displayedPredictionPass->satelliteLOS))
						{
							startTime = displayedPredictionPass->satelliteAOS;

							snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"%s:%02d%c", currentLanguage->maximum, satelliteGetMaximumElevation(currentActiveSatellite, currentActiveSatellite->predictions.selectedPassNumber), 176);
							displayPrintAt(4, (DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) - 4 , buffer, FONT_SIZE_3);

							if (displayedPassTimeDiff >= 60)
							{
								if (displayedPassTimeDiff >= 3600)
								{
									snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"-%u:%02u:%02u", dispTimeAndDate.tm_hour, dispTimeAndDate.tm_min, dispTimeAndDate.tm_sec);
									displayPrintAt(((dispTimeAndDate.tm_hour > 9) ? 2 : 4), 4, buffer, FONT_SIZE_2);
								}
								else
								{
									// between 60 and 3599 seconds
									snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"-%u:%02u", dispTimeAndDate.tm_min, dispTimeAndDate.tm_sec);
									displayPrintAt(8, 4, buffer, FONT_SIZE_2);
								}
							}
							else
							{
								snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"-%02us", displayedPassTimeDiff);
								displayPrintAt(12, 4, buffer, FONT_SIZE_3);
							}

							if (announceVP)
							{
								voicePromptsAppendLanguageString(currentLanguage->inHHMMSS);

								if (displayedPassTimeDiff >= 60)
								{
									if (displayedPassTimeDiff >= 3600)
									{
										voicePromptsAppendInteger(dispTimeAndDate.tm_hour);
										voicePromptsAppendLanguageString(currentLanguage->hours);

										voicePromptsAppendInteger(dispTimeAndDate.tm_min);
										voicePromptsAppendPrompt(PROMPT_MINUTES);
									}
									else
									{
										// between 60 and 3599 seconds
										voicePromptsAppendInteger(dispTimeAndDate.tm_min);
										voicePromptsAppendPrompt(PROMPT_MINUTES);

										voicePromptsAppendInteger(dispTimeAndDate.tm_sec);
										voicePromptsAppendPrompt(PROMPT_SECONDS);
									}
								}
								else
								{
									voicePromptsAppendInteger(displayedPassTimeDiff);
									voicePromptsAppendPrompt(PROMPT_SECONDS);
								}

								voicePromptsAppendLanguageString(currentLanguage->maximum);
								snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"%d%c", satelliteGetMaximumElevation(currentActiveSatellite, currentActiveSatellite->predictions.selectedPassNumber), 176);
								voicePromptsAppendString(buffer);
							}
						}
						else
						{
							startTime = uiDataGlobal.dateTimeSecs;
							snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"%s:%03d%c", currentLanguage->azimuth, currentSatelliteResults.azimuthAsInteger, 176);
							displayPrintAt(4, 6, buffer, FONT_SIZE_3);

							if (announceVP)
							{
								voicePromptsAppendLanguageString(currentLanguage->azimuth);
								snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"%3d%c", currentSatelliteResults.azimuthAsInteger, 176);
								voicePromptsAppendString(buffer);
							}

							snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"%s:%02u%c", currentLanguage->elevation, currentSatelliteResults.elevationAsInteger, 176);
							displayPrintAt(4, (DISPLAY_SIZE_Y - FONT_SIZE_3_HEIGHT) - 4 , buffer, FONT_SIZE_3);

							if (announceVP)
							{
								voicePromptsAppendLanguageString(currentLanguage->elevation);
								snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,"%2d%c", currentSatelliteResults.elevationAsInteger, 176);
								voicePromptsAppendString(buffer);
							}
						}

						for(uint32_t t = startTime; t < displayedPredictionPass->satelliteLOS; t += 30)
						{
							satelliteCalculateForDateTimeSecs(currentActiveSatellite, t, &results, SATELLITE_PREDICTION_LEVEL_TIME_EL_AND_AZ);
							az = deg2rad(results.azimuthAsInteger);
							elFactor = (1 - (fabs(results.elevationAsInteger) / 90)) * MAX_RADIUS;
							s = sin(az);
							c = cos(az);
							x = POLAR_GRAPHICS_X_OFFSET + (DISPLAY_SIZE_X / 2) + (s * elFactor);
							y = (DISPLAY_SIZE_Y / 2) - (c * elFactor);// NOTE... Subtract value from Y center - for North at top of screen

							if (t == startTime)
							{
								// Postpone satellite spot drawing, to avoid the path drawn on top of it
								satX = (int16_t)x;
								satY = (int16_t)y;
							}
							else
							{
								displayDrawLine((int16_t)lastX, (int16_t)lastY, (int16_t)x, (int16_t)y, true);
							}

							lastX = x;
							lastY = y;
						}

						if ((satX != INT16_MIN) && (satY != INT16_MIN))
						{
							displayThemeApply(THEME_ITEM_FG_SATELLITE_COLOUR, THEME_ITEM_BG);
							displayFillCircle(satX, satY, (DOT_RADIUS + 1), true);
							displayThemeResetToDefault();
						}
					}

					const uint32_t S_METER_BAR_WIDTH = 8;
					// draw S meter with range S0 to S9.
					int rssi = MIN(MAX(0, trxGetRSSIdBm(RADIO_DEVICE_PRIMARY) - SMETER_S0), (SMETER_S9 - SMETER_S0));

					rssi = (rssi * DISPLAY_SIZE_Y) / (SMETER_S9 - SMETER_S0);

					displayThemeApply(THEME_ITEM_FG_RSSI_BAR, THEME_ITEM_BG);
					displayFillRect(DISPLAY_SIZE_X - S_METER_BAR_WIDTH, 0                     , S_METER_BAR_WIDTH, DISPLAY_SIZE_Y - rssi  , true);
					displayFillRect(DISPLAY_SIZE_X - S_METER_BAR_WIDTH, DISPLAY_SIZE_Y - rssi , S_METER_BAR_WIDTH, rssi                   , false);
					displayThemeResetToDefault();

					menuSatelliteScreenNextUpdateTime = (ev->time + 250U);
				}
				else
				{
					char buf[SCREEN_LINE_BUFFER_SIZE];

					displayClearBuf();
					displayPrintCentered(4, currentActiveSatellite->name, FONT_SIZE_3);
					snprintf(buf, SCREEN_LINE_BUFFER_SIZE, "%s: %s", currentLanguage->pass, currentLanguage->none);
					displayPrintCentered((DISPLAY_SIZE_Y / 2) + 4, buf, FONT_SIZE_3);

					if (announceVP)
					{
						voicePromptsAppendString(currentActiveSatellite->name);
						voicePromptsAppendPrompt(PROMPT_SILENCE);
						voicePromptsAppendLanguageString(currentLanguage->pass);
						voicePromptsAppendLanguageString(currentLanguage->none);
					}

					menuSatelliteScreenNextUpdateTime = 0;// don't update again
				}

				if (announceVP)
				{
					voicePromptsPlay();
				}

				displayRender();
			}
			break;

			case SATELLITE_SCREEN_SELECTED_SATELLITE_PREDICTION:
			{
				satellitePass_t *displayedPredictionPass  = &currentActiveSatellite->predictions.passes[currentActiveSatellite->predictions.selectedPassNumber];

				displayClearBuf();
				voicePromptsInit();

				displayPrintCentered(4, currentActiveSatellite->name, FONT_SIZE_3);
				if (currentActiveSatellite->predictions.numPasses > 0)
				{
					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s %u / %u", currentLanguage->pass, (currentActiveSatellite->predictions.selectedPassNumber + 1), currentActiveSatellite->predictions.numPasses);
					displayPrintCentered((DISPLAY_SIZE_Y / 4) + 4,buffer, FONT_SIZE_2);

					if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
					{
						voicePromptsAppendString(currentActiveSatellite->name);
						voicePromptsAppendLanguageString(currentLanguage->pass);
						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%u", (currentActiveSatellite->predictions.selectedPassNumber + 1));
						voicePromptsAppendString(buffer);
					}
				}

				if (displayedPredictionPass->valid == PREDICTION_RESULT_OK)
				{
					time_t_custom AOS =  displayedPredictionPass->satelliteAOS + ((nonVolatileSettings.timezone & 0x80) ? ((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60) : 0);

					struct tm passDateTime;
					gmtime_r_Custom(&AOS, &passDateTime);
					char buffer[SCREEN_LINE_BUFFER_SIZE];

					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%02u:%02u:%02u %s", passDateTime.tm_hour, passDateTime.tm_min, passDateTime.tm_sec, ((nonVolatileSettings.timezone & 0x80) ? "" : "UTC"));
					displayPrintCentered((DISPLAY_SIZE_Y / 2) + 4, buffer, FONT_SIZE_2);

					if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
					{
						voicePromptsAppendLanguageString(currentLanguage->time);
						voicePromptsAppendString(buffer);
					}

					snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s:%2d%c %3u:%02us", currentLanguage->elevation, satelliteGetMaximumElevation(currentActiveSatellite, currentActiveSatellite->predictions.selectedPassNumber), 176,
							( displayedPredictionPass->satellitePassDuration / 60), (displayedPredictionPass->satellitePassDuration % 60)) ;
					displayPrintCentered(((DISPLAY_SIZE_Y * 3) / 4), buffer, FONT_SIZE_2);

					if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
					{
						voicePromptsAppendLanguageString(currentLanguage->maximum);

						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%2d%c",  satelliteGetMaximumElevation(currentActiveSatellite, currentActiveSatellite->predictions.selectedPassNumber), 176);
						voicePromptsAppendString(buffer);

						voicePromptsAppendPrompt(PROMPT_DURATION);
						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%u", ( displayedPredictionPass->satellitePassDuration / 60)) ;
						voicePromptsAppendString(buffer);
						voicePromptsAppendLanguageString(currentLanguage->minutes);

						snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%u", (displayedPredictionPass->satellitePassDuration % 60)) ;
						voicePromptsAppendString(buffer);
						voicePromptsAppendLanguageString(currentLanguage->seconds);

					}
				}

				if ((displayedPredictionPass->valid == PREDICTION_RESULT_LIMIT) && (currentActiveSatellite->predictions.numPasses == 0))
				{
					displayPrintCentered((DISPLAY_SIZE_Y / 2) + 4, currentLanguage->list_empty, FONT_SIZE_3);

					voicePromptsAppendString(currentActiveSatellite->name);
					voicePromptsAppendPrompt(PROMPT_SILENCE);
					voicePromptsAppendLanguageString(currentLanguage->list_empty);
				}

				if (announceVP)
				{
					voicePromptsPlay();
				}

				menuSatelliteScreenNextUpdateTime = 0;// Don't do time based update
				displayRender();
			}
			break;

			case SATELLITE_SCREEN_ALL_PREDICTIONS_LIST:
				{
					time_t_custom passTime;
					satelliteData_t *foundSat = NULL;
					int foundPassNumber;
					satelliteData_t *seatchingSat;
					time_t_custom AOS;
					uint32_t pass;
					int numSatellitesFound = 0;
					int totalPredictions = 0;
					int foundSatelliteIndex = -1;

					predictionsListNumSatellitePassesDisplayed = 0;
					predictionsListSelectedSatellite = 0;

					displayClearBuf();

					for(int i = 0; i< numSatellitesLoaded; i++)
					{
						satelliteDataNative[i].predictions.listDisplayPassSearchStartIndex = 0;
						totalPredictions += satelliteDataNative[i].predictions.numPasses;
					}

					while((predictionsListNumSatellitePassesDisplayed < NUM_PASSES_TO_DISPLAY_ON_LIST_SCREEN) && (numSatellitesFound < totalPredictions))
					{
						foundPassNumber = -1;
						passTime = 0xFFFFFFFF;// highest possible number
						for(int sat = 0; sat < numSatellitesLoaded; sat++)
						{
							seatchingSat = &satelliteDataNative[sat];

							pass = seatchingSat->predictions.listDisplayPassSearchStartIndex;

							if(seatchingSat->predictions.passes[pass].valid == PREDICTION_RESULT_OK)
							{
								AOS = seatchingSat->predictions.passes[pass].satelliteAOS;
								if (AOS < passTime)
								{
									passTime = AOS;
									foundSat = seatchingSat;
									foundSatelliteIndex = sat;
									foundPassNumber = pass;
								}
							}
						}
						if ((foundPassNumber >= 0) && (foundSat != NULL))
						{
							foundSat->predictions.listDisplayPassSearchStartIndex = (foundPassNumber + 1);// last foundPass must be for the foundSatellite

							if (numSatellitesFound >= currentlyDisplayedListPosition)
							{
								char passTimeBuffer[SCREEN_LINE_BUFFER_SIZE];

								AOS = foundSat->predictions.passes[foundPassNumber].satelliteAOS +  ((nonVolatileSettings.timezone & 0x80) ? ((nonVolatileSettings.timezone & 0x7F) - 64) * (15 * 60) : 0);
								struct tm passDateTime;
								gmtime_r_Custom(&AOS, &passDateTime);

								displayPrintCentered(6 + (20 * predictionsListNumSatellitePassesDisplayed), foundSat->name, FONT_SIZE_2);

								snprintf(passTimeBuffer, SCREEN_LINE_BUFFER_SIZE, "%02u:%02u:%02u%s", passDateTime.tm_hour, passDateTime.tm_min, passDateTime.tm_sec, ((nonVolatileSettings.timezone & 0x80) ? "" : "UTC"));
								snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s %02d%c", passTimeBuffer, satelliteGetMaximumElevation(foundSat, foundPassNumber), 176);
								displayPrintCentered(6 + (20 * predictionsListNumSatellitePassesDisplayed) + 8, buffer, FONT_SIZE_2);

								if ((foundPassNumber == 0) && foundSat->predictions.isVisible)
								{
									uint32_t yPos = 6 + (20 * predictionsListNumSatellitePassesDisplayed);
									displayThemeApply(THEME_ITEM_FG_SATELLITE_COLOUR, THEME_ITEM_BG);
									displayFillRect((DISPLAY_SIZE_X - (DISPLAY_X_POS_MENU_OFFSET * 2) - 6), yPos, 4, 16, false);
									displayThemeResetToDefault();
								}

								if (predictionsListNumSatellitePassesDisplayed == 0)
								{
									displayDrawRect(2 + DISPLAY_X_POS_MENU_OFFSET, 4, (DISPLAY_SIZE_X - (DISPLAY_X_POS_MENU_OFFSET * 2) - 4), 20, true);

									if (nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
									{
										voicePromptsInit();
										voicePromptsAppendString(foundSat->name);
										voicePromptsAppendLanguageString(currentLanguage->time);
										voicePromptsAppendString(passTimeBuffer);
										voicePromptsAppendLanguageString(currentLanguage->maximum);
										snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%2d%c", satelliteGetMaximumElevation(foundSat, foundPassNumber), 176);
										voicePromptsAppendString(buffer);
										if (announceVP)
										{
											voicePromptsPlay();
										}
									}

									foundSat->predictions.selectedPassNumber = foundPassNumber;
									predictionsListSelectedSatellite = foundSatelliteIndex;
									//selectSatellite(predictionsListSelectedSatellite);
								}

								predictionsListNumSatellitePassesDisplayed++;
							}
							numSatellitesFound++;
						}
					}
					menuSatelliteScreenNextUpdateTime = 0;// Don't do time based update
					displayRender();
				}
				break;

			default:
				break;
		}

		hasRecalculated = false;
	}
}

static void handleEvent(uiEvent_t *ev)
{
	if ((ev->events & FUNCTION_EVENT) && (ev->function == FUNC_REDRAW))
	{
		hasRecalculated = true; // Force full redraw
		updateScreen(ev, false, false);
		return;
	}

	if (displayMode == SATELLITE_SCREEN_SELECTED_SATELLITE)
	{
		bool needsUpdate = false;

		if (KEYCHECK_SHORTUP(ev->keys, KEY_1))
		{
			currentSatelliteFreqIndex = SATELLITE_VOICE_FREQ;
			needsUpdate = true;
		}
		else if (KEYCHECK_SHORTUP(ev->keys, KEY_2))
		{
			currentSatelliteFreqIndex = SATELLITE_APRS_FREQ;
			needsUpdate = true;
		}
		else if (KEYCHECK_SHORTUP(ev->keys, KEY_3))
		{
			currentSatelliteFreqIndex = SATELLITE_OTHER_FREQ;
			needsUpdate = true;
		}

		if (needsUpdate)
		{
			hasRecalculated = true; // Need to set this otherwise the SATELLITE_SCREEN_SELECTED_SATELLITE does not immediately redraw
			updateScreen(ev, false, true);
			return;
		}
	}

	if (KEYCHECK_SHORTUP(ev->keys, KEY_RED))
	{
		if (displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST)
		{
			menuSystemPopPreviousMenu();
		}
		else
		{
			displayMode = SATELLITE_SCREEN_ALL_PREDICTIONS_LIST;
			voicePromptsInit();
			updateScreen(ev, false, true);
		}
	}

	if (!hasSatelliteKeps)
	{
		return;
	}

	if (ev->keys.event & KEY_MOD_PRESS)
	{
		// Cancels currently beeping alarm
		if ((uiDataGlobal.SatelliteAndAlarmData.alarmType == ALARM_TYPE_SATELLITE) &&
				(uiDataGlobal.dateTimeSecs >= uiDataGlobal.SatelliteAndAlarmData.alarmTime))
		{
			uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_NONE;
		}
	}

	if (ev->keys.event & KEY_MOD_PRESS)
	{
		switch(ev->keys.key)
		{
			case KEY_GREEN:
			{
				uint32_t alarmTime;

				// Cancels ongoing alarm
				if ((uiDataGlobal.SatelliteAndAlarmData.alarmType == ALARM_TYPE_SATELLITE) &&
						(uiDataGlobal.dateTimeSecs < uiDataGlobal.SatelliteAndAlarmData.alarmTime))
				{
					uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_NONE;
					nextKeyBeepMelody = (int16_t *)MELODY_QUICKKEYS_CLEAR_ACK_BEEP;
					return;
				}

				if (displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST)
				{
					selectSatellite(predictionsListSelectedSatellite);

					alarmTime = currentActiveSatellite->predictions.passes[currentActiveSatellite->predictions.selectedPassNumber].satelliteAOS - ALARM_OFFSET_SECS;

					if (!(BUTTONCHECK_DOWN(ev, BUTTON_SK2) && (currentActiveSatellite->predictions.isVisible || uiDataGlobal.dateTimeSecs >= alarmTime)))
					{
						displayMode = SATELLITE_SCREEN_SELECTED_SATELLITE_POLAR;//SATELLITE_SCREEN_SELECTED_SATELLITE;
						updateScreen(ev, true, true);
					}
				}

				if (BUTTONCHECK_DOWN(ev, BUTTON_SK2))
				{
					alarmTime = currentActiveSatellite->predictions.passes[currentActiveSatellite->predictions.selectedPassNumber].satelliteAOS - ALARM_OFFSET_SECS;

					if (!(currentActiveSatellite->predictions.isVisible || uiDataGlobal.dateTimeSecs >= alarmTime))
					{
						uiDataGlobal.SatelliteAndAlarmData.alarmTime = currentActiveSatellite->predictions.passes[currentActiveSatellite->predictions.selectedPassNumber].satelliteAOS - ALARM_OFFSET_SECS;
						uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_SATELLITE;
#if defined(PLATFORM_MD9600)
						nextKeyBeepMelody = (int16_t *)MELODY_ACK_BEEP;
#else
						powerOffFinalStage(true, false);
#endif
					}
					else
					{
						nextKeyBeepMelody = (int16_t *)MELODY_ERROR_BEEP;
					}
				}
				break;
			}

			case KEY_RIGHT:
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			case KEY_ROTARY_INCREMENT:
#endif
				if (BUTTONCHECK_DOWN(ev, BUTTON_SK2))
				{
					if (increasePowerLevel(false))
					{
						headerRowIsDirty = true;
					}
					uiNotificationShow(NOTIFICATION_TYPE_POWER, NOTIFICATION_ID_POWER, 1000, NULL, true);
				}
				else
				{
					switch(displayMode)
					{
						case SATELLITE_SCREEN_SELECTED_SATELLITE_PREDICTION:
							if ((currentActiveSatellite->predictions.numPasses > 0) &&
									(currentActiveSatellite->predictions.selectedPassNumber < (currentActiveSatellite->predictions.numPasses - 1)))
							{
								currentActiveSatellite->predictions.selectedPassNumber++;
								updateScreen(ev, true, true);
							}
							break;
						default:
							// more squelch
							if(currentChannelData->sql == 0) //If we were using default squelch level
							{
								currentChannelData->sql = nonVolatileSettings.squelchDefaults[currentRadioDevice->trxCurrentBand[TRX_RX_FREQ_BAND]];//start the adjustment from that point.
							}

							if (currentChannelData->sql < CODEPLUG_MAX_VARIABLE_SQUELCH)
							{
								currentChannelData->sql++;
							}

							announceItem(PROMPT_SQUENCE_SQUELCH, PROMPT_THRESHOLD_3);

							uiNotificationShow(NOTIFICATION_TYPE_SQUELCH, NOTIFICATION_ID_SQUELCH, 1000, NULL, true);
						break;
					}
				}
				break;

			case KEY_LEFT:
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			case KEY_ROTARY_DECREMENT:
#endif
				if (BUTTONCHECK_DOWN(ev, BUTTON_SK2))
				{
					if (decreasePowerLevel())
					{
						headerRowIsDirty = true;
					}
					uiNotificationShow(NOTIFICATION_TYPE_POWER, NOTIFICATION_ID_POWER, 1000, NULL, true);
				}
				else
				{
					switch(displayMode)
					{
					case SATELLITE_SCREEN_SELECTED_SATELLITE_PREDICTION:
						if (currentActiveSatellite->predictions.selectedPassNumber > 0)
						{
							currentActiveSatellite->predictions.selectedPassNumber--;
							updateScreen(ev, false, true);
						}
						break;
					default:
						// less squelch
						if(currentChannelData->sql == 0) //If we were using default squelch level
						{
							currentChannelData->sql = nonVolatileSettings.squelchDefaults[currentRadioDevice->trxCurrentBand[TRX_RX_FREQ_BAND]];//start the adjustment from that point.
						}

						if (currentChannelData->sql > CODEPLUG_MIN_VARIABLE_SQUELCH)
						{
							currentChannelData->sql--;
						}

						announceItem(PROMPT_SQUENCE_SQUELCH,PROMPT_THRESHOLD_3);

						uiNotificationShow(NOTIFICATION_TYPE_SQUELCH, NOTIFICATION_ID_SQUELCH, 1000, NULL, true);
						break;
					}
				}
				break;

			case KEY_UP:
				if (displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST)
				{
					if (currentlyDisplayedListPosition > 0)
					{
						currentlyDisplayedListPosition--;
						updateScreen(ev, true, true);
					}
				}
				else
				{
					if (!BUTTONCHECK_DOWN(ev, BUTTON_SK2))
					{
						displayMode--;
						if (displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST)
						{
							displayMode = NUM_SATELLITE_SCREEN_ITEMS - 1;
						}
						voicePromptsInit();
					}
					else
					{
						uint32_t newSatIndex = uiDataGlobal.SatelliteAndAlarmData.currentSatellite - 1;
						newSatIndex %= numSatellitesLoaded;
						selectSatellite(newSatIndex);
					}

					nextCalculationTime = (ev->time - 1);// force recalculation
					updateScreen(ev, false, true);
				}
				break;

			case KEY_DOWN:
				if (displayMode ==SATELLITE_SCREEN_ALL_PREDICTIONS_LIST)
				{
					if (predictionsListNumSatellitePassesDisplayed > 1)
					{
						currentlyDisplayedListPosition++;
						updateScreen(ev, true, true);
					}
				}
				else
				{
					if (!BUTTONCHECK_DOWN(ev, BUTTON_SK2))
					{
						displayMode++;
						if (displayMode == NUM_SATELLITE_SCREEN_ITEMS)
						{
							displayMode = SATELLITE_SCREEN_SELECTED_SATELLITE;
						}
						voicePromptsInit();
					}
					else
					{
						uint32_t newSatIndex = uiDataGlobal.SatelliteAndAlarmData.currentSatellite + 1;
						newSatIndex %= numSatellitesLoaded;
						selectSatellite(newSatIndex);
					}
					nextCalculationTime = (ev->time - 1);// force recalculation
					updateScreen(ev, false, true);
				}

				break;

			case KEY_HASH:
			{
				satellitePass_t *pass;
				uint32_t newSatIndex = uiDataGlobal.SatelliteAndAlarmData.currentSatellite;

				for(int i = 0; i < (numSatellitesLoaded - 1); i++)
				{
					newSatIndex++;
					newSatIndex %= numSatellitesLoaded;

					pass = &satelliteDataNative[newSatIndex].predictions.passes[0];
					if ((pass->valid == PREDICTION_RESULT_OK) &&
						(pass->satelliteAOS  <= uiDataGlobal.dateTimeSecs) &&
						(pass->satelliteLOS  >= uiDataGlobal.dateTimeSecs))
					{
						selectSatellite(newSatIndex);
						updateScreen(ev, true, true);
						break;
					}
				}
			}
				break;
				/*
				 * Testing only. Move time forward 30 minutes
			case KEY_STAR:
				uiDataGlobal.dateTimeSecs += 1800;
				break;
				*/
		}
	}
#if ! (defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380))
	// set as KEY_RIGHT on some platforms (but longdown is not possible on MD380/MD-UV380)
	else if (KEYCHECK_LONGDOWN(ev->keys, KEY_INCREASE)
			&& BUTTONCHECK_DOWN(ev, BUTTON_SK2))
	{
		// Long press allows the 5W+ power setting to be selected immediately
		if (increasePowerLevel(true))
		{
			headerRowIsDirty = true;
		}
	}
#endif

	if (ev->events & FUNCTION_EVENT)
	{
		if (QUICKKEY_TYPE(ev->function) == QUICKKEY_MENU)
		{
			displayMode = QUICKKEY_ENTRYID(ev->function);

			updateScreen(ev, true, true);
			return;
		}
		else if (ev->function == FUNC_REDRAW)
		{
			updateScreen(ev, false, false);
		}
	}

	if (ev->events & BUTTON_EVENT)
	{
		if (BUTTONCHECK_DOWN(ev, BUTTON_SK1))
		{
			if (voicePromptsIsPlaying())
			{
				voicePromptsTerminate();
			}
			else
			{
				if ((displayMode == SATELLITE_SCREEN_SELECTED_SATELLITE) || (displayMode ==  SATELLITE_SCREEN_SELECTED_SATELLITE_POLAR))
				{
					voicePromptsInit();
					updateScreen(ev, true, true);
					return;
				}
				else
				{
					voicePromptsPlay();
				}
			}
		}
	}

	if (KEYCHECK_SHORTUP_NUMBER(ev->keys) && (BUTTONCHECK_DOWN(ev, BUTTON_SK2)))
	{
		saveQuickkeyMenuIndex(ev->keys.key, menuSystemGetCurrentMenuNumber(), displayMode, 0);
	}
}

static bool findSelectedPass = false;
static uint32_t selectedPassAOS = 0;
static bool calculatePredictionsForSatelliteIndex(int satelliteIndex)
{
	satelliteResults_t results;
	satelliteData_t * satellite = &satelliteDataNative[satelliteIndex];

	if ((uiDataGlobal.dateTimeSecs >= satellite->predictions.passes[0].satelliteAOS) && (uiDataGlobal.dateTimeSecs <= satellite->predictions.passes[0].satelliteLOS))
	{
		if ((displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST) && (satellite->predictions.isVisible == false))
		{
			menuSatelliteScreenNextUpdateTime = 1U;
		}
		satellite->predictions.isVisible = true;
	}
	else
	{
		if ((displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST) && (satellite->predictions.isVisible == true))
		{
			menuSatelliteScreenNextUpdateTime = 1U;
		}
		satellite->predictions.isVisible = false;
	}

	// Force rebuilding of the prediction if the the time is after the LOS of the first predicted pass
	if ((satellite->predictions.numPasses > 0) && (uiDataGlobal.dateTimeSecs > satellite->predictions.passes[0].satelliteLOS) && !satellite->predictions.isPredicting)
	{
		findSelectedPass = false;
		if (satellite->predictions.selectedPassNumber != 0)
		{
			selectedPassAOS = satellite->predictions.passes[satellite->predictions.selectedPassNumber].satelliteAOS;
			if (selectedPassAOS >= uiDataGlobal.dateTimeSecs)
			{
				findSelectedPass = true;
			}
		}

		// find passes which are in the past
		int passNum;
		for(passNum=0; passNum<satellite->predictions.numPasses; passNum++)
		{
			if (!(uiDataGlobal.dateTimeSecs > satellite->predictions.passes[passNum].satelliteLOS))
			{
				break;
			}
		}
		int numPassesToMove = satellite->predictions.numPasses - passNum;
		memcpy(&satellite->predictions.passes[0], &satellite->predictions.passes[passNum],  numPassesToMove * sizeof(satellitePass_t));

		int numPassesToClear = NUM_SATELLITE_PREDICTIONS - numPassesToMove;
		memset(&satellite->predictions.passes[numPassesToMove], 0x00, (numPassesToClear) * sizeof(satellitePass_t));// clear all predictions for this satellite

		satellite->predictions.numPasses = numPassesToMove;
		satellite->predictions.numPassBeingPredicted = numPassesToMove;
		satellite->predictions.listDisplayPassSearchStartIndex = 0;
		satellite->predictions.selectedPassNumber = 0;
		satellite->predictions.isPredicting = true;
		satellite->predictions.isVisible = false;


		//memset(&satellite->predictions, 0x00, sizeof(satellitePredictions_t));// clear all predictions for this satellite
		//satellite->predictions.numPassBeingPredicted = 0;
		satellite->predictions.isPredicting = true;
		numTotalSatellitesPredicted--;
		currentlyDisplayedListPosition = 0;// Reset the list position, as there may be less satellites after the passes for this satellite are re-calculated
	}

	if (satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].valid == PREDICTION_RESULT_NONE)
	{
		time_t_custom predictionStartTime;

		if ((satellite->predictions.numPassBeingPredicted == 0) && (currentPrediction.state == PREDICTION_STATE_NONE))
		{
			satelliteCalculateForDateTimeSecs(satellite, uiDataGlobal.dateTimeSecs, &results, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

			if (results.elevation < 0)
			{
				predictionStartTime = uiDataGlobal.dateTimeSecs;
			}
			else
			{
				predictionStartTime = uiDataGlobal.dateTimeSecs - (30 * 60);// If satellite is currently visible. Change prediction start back  30 mins
			}
		}
		else
		{
			predictionStartTime = satellite->predictions.passes[satellite->predictions.numPassBeingPredicted - 1].satelliteAOS +
				satellite->predictions.passes[satellite->predictions.numPassBeingPredicted - 1].satellitePassDuration + 30 * 60;// 30 minutes after the last pass
		}

		if ((currentPrediction.state == PREDICTION_STATE_NONE))
		{
			currentPrediction.state = PREDICTION_STATE_INIT_AOS;// Start the prediction
		}

		if (!satellitePredictNextPassFromDateTimeSecs(&currentPrediction, satellite, predictionStartTime, (uiDataGlobal.dateTimeSecs + (24 * 60 * 60)), 500,  &satellite->predictions.passes[satellite->predictions.numPassBeingPredicted]))
		{
			satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].valid = PREDICTION_RESULT_NONE;
		}

		switch(currentPrediction.state)
		{
			case PREDICTION_STATE_COMPLETE:
				if (satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].satelliteAOS != 0)
				{

					satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].valid = PREDICTION_RESULT_OK;
					satellite->predictions.numPasses++;
					if (satellite->predictions.numPasses < (NUM_SATELLITE_PREDICTIONS - 1))
					{
						if (findSelectedPass && (selectedPassAOS == satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].satelliteAOS))
						{
							satellite->predictions.selectedPassNumber = satellite->predictions.numPassBeingPredicted;
							findSelectedPass = false;
						}
						satellite->predictions.numPassBeingPredicted++;
					}
					else
					{
						satellite->predictions.numPasses--;// hack
						satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].valid = PREDICTION_RESULT_LIMIT;
						currentPrediction.state = PREDICTION_STATE_NONE;// Start the prediction
						satellite->predictions.isPredicting = false;
					}
				}
				currentPrediction.state = PREDICTION_STATE_NONE;// Start the prediction
				break;

			case PREDICTION_STATE_ITERATION_LIMIT:
				// Do something. There has been a problem while computing the predictions
			case PREDICTION_STATE_LIMIT:
				satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].valid = PREDICTION_RESULT_LIMIT;
				currentPrediction.state = PREDICTION_STATE_NONE;// Start the prediction
				satellite->predictions.isPredicting = false;
				//clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_SPEED_RUN);
				findSelectedPass = false;
				return true;
				break;
			case PREDICTION_STATE_NONE:
			case PREDICTION_STATE_INIT_AOS:
			case PREDICTION_STATE_FIND_AOS:
			case PREDICTION_STATE_INIT_LOS:
			case PREDICTION_STATE_FIND_LOS:
				satellite->predictions.passes[satellite->predictions.numPassBeingPredicted].valid = PREDICTION_RESULT_NONE;// move on to next pass
				return false;
				break;
		}
	}
	return true;
}

static void loadKeps(void)
{
	codeplugSatelliteCuctsomDataUnion_t codeplugKepsData;

	//volatile int s = sizeof(codeplugSatelliteData_t);

	hasSatelliteKeps = codeplugGetOpenGD77CustomData(CODEPLUG_CUSTOM_DATA_TYPE_SATELLITE_TLE, (uint8_t *)&codeplugKepsData.data);
	if (hasSatelliteKeps)
	{
		for(numSatellitesLoaded = 0; numSatellitesLoaded < NUM_SATELLITES; numSatellitesLoaded++)
		{
			if (codeplugKepsData.data[numSatellitesLoaded].TLE_Name[0] != 0)
			{
				satelliteTLE2Native(
						codeplugKepsData.data[numSatellitesLoaded].TLE_Name,
						codeplugKepsData.data[numSatellitesLoaded].TLE_Line1,
						codeplugKepsData.data[numSatellitesLoaded].TLE_Line2, &satelliteDataNative[numSatellitesLoaded]) ;

						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_VOICE_FREQ].rxFreq = codeplugKepsData.data[numSatellitesLoaded].rxFreq1;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_VOICE_FREQ].txFreq = codeplugKepsData.data[numSatellitesLoaded].txFreq1;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_VOICE_FREQ].txCTCSS = codeplugKepsData.data[numSatellitesLoaded].txCTCSS1;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_VOICE_FREQ].armCTCSS = codeplugKepsData.data[numSatellitesLoaded].armCTCSS1;

						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_APRS_FREQ].rxFreq = codeplugKepsData.data[numSatellitesLoaded].rxFreq2;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_APRS_FREQ].txFreq = codeplugKepsData.data[numSatellitesLoaded].txFreq2;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_APRS_FREQ].txCTCSS = 0;//codeplugKepsData.data[numSatellitesLoaded].txCTCSS1;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_APRS_FREQ].armCTCSS = 0;//codeplugKepsData.data[numSatellitesLoaded].armCTCSS1;

						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_OTHER_FREQ].rxFreq = codeplugKepsData.data[numSatellitesLoaded].rxFreq3;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_OTHER_FREQ].txFreq = codeplugKepsData.data[numSatellitesLoaded].txFreq3;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_OTHER_FREQ].txCTCSS = 0;//codeplugKepsData.data[numSatellitesLoaded].txCTCSS1;
						satelliteDataNative[numSatellitesLoaded].freqs[SATELLITE_OTHER_FREQ].armCTCSS = 0;//codeplugKepsData.data[numSatellitesLoaded].armCTCSS1;
						memcpy(satelliteDataNative[numSatellitesLoaded].AdditionalData, codeplugKepsData.data[numSatellitesLoaded].AdditionalData, ADDITION_DATA_SIZE);

						memset(&satelliteDataNative[numSatellitesLoaded].predictions, 0x00, sizeof(satellitePredictions_t));
			}
			else
			{
				break;
			}
		}
	}
}

void menuSatelliteScreenClearPredictions(bool reloadKeps)
{
	numTotalSatellitesPredicted = 0;
	currentlyDisplayedListPosition = 0; //reset the list display position if the predictions have been cleared.

	for(int s = 0; s < NUM_SATELLITES; s++)
	{
		memset(&satelliteDataNative[s].predictions, 0x00, sizeof(satellitePredictions_t));
	}

	if (reloadKeps)
	{
		loadKeps();
	}

	currentActiveSatellite = &satelliteDataNative[0];

	uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_NONE;
}

static int menuSatelliteFindNextSatellite(void)
{
	uint32_t AOS;
	int foundSatelliteIndex =-1;
	uint32_t passTime = 0xFFFFFFFF;// highest possible number
	satelliteData_t *seatchingSat;

	for(int sat = 0; sat < numSatellitesLoaded; sat++)
	{
		seatchingSat = &satelliteDataNative[sat];

		if(seatchingSat->predictions.passes[0].valid == PREDICTION_RESULT_OK)
		{
			AOS = seatchingSat->predictions.passes[0].satelliteAOS;
			if (AOS < passTime)
			{
				passTime = AOS;
				foundSatelliteIndex = sat;
			}
		}
	}
	return foundSatelliteIndex;
}

static void selectSatellite(uint32_t selectedSatellite)
{
		uiDataGlobal.SatelliteAndAlarmData.currentSatellite = selectedSatellite;
		currentActiveSatellite = &satelliteDataNative[uiDataGlobal.SatelliteAndAlarmData.currentSatellite];
		satelliteVisible = false;// presume initially the new satellite has not been acquired

		calculateActiveSatelliteData(true);

		currentChannelData->txTone = currentActiveSatellite->freqs[currentSatelliteFreqIndex].txCTCSS;

		if (currentChannelData->chMode == RADIO_MODE_NONE)
		{
			currentChannelData->chMode = RADIO_MODE_ANALOG;
			trxSetModeAndBandwidth(currentChannelData->chMode, true);
		}
		hasSelectedSatellite = true;

		aprsBeaconingPrepareSatelliteConfig();

		voicePromptsInit();
}

bool menuSatelliteIsDisplayingHeader(void)
{
	return (displayMode == SATELLITE_SCREEN_SELECTED_SATELLITE);
}

void menuSatelliteTxScreen(uint32_t txTimeSecs)
{
	char buffer[SCREEN_LINE_BUFFER_SIZE];

	displayClearBuf();
	uiUtilityRenderHeader(false, false);

	snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, " %d ", txTimeSecs);
	uiUtilityDisplayInformation(buffer, DISPLAY_INFO_TX_TIMER, -1);

	satelliteCalculateForDateTimeSecs(currentActiveSatellite, uiDataGlobal.dateTimeSecs, &currentSatelliteResults, SATELLITE_PREDICTION_LEVEL_FULL);

	snprintf(buffer, SCREEN_LINE_BUFFER_SIZE, "%s:%3d%c %s:%3d%c", currentLanguage->azimuth, currentSatelliteResults.azimuthAsInteger, 176, currentLanguage->elevation, currentSatelliteResults.elevationAsInteger, 176);
	displayPrintCentered(DISPLAY_Y_POS_RX_FREQ + 1, buffer, FONT_SIZE_3);

	int val_before_dp = currentChannelData->txFreq / 100000;
	int val_after_dp = currentChannelData->txFreq - val_before_dp * 100000;
	snprintf(buffer, SCREEN_LINE_BUFFER_SIZE,  "%d.%05d MHz", val_before_dp, val_after_dp);
	displayPrintCentered(DISPLAY_Y_POS_TX_FREQ, buffer, FONT_SIZE_3);

	displayRender();
}

void menuSatelliteSetFullReload(void)
{
	displayMode = SATELLITE_SCREEN_ALL_PREDICTIONS_LIST;
	hasSelectedSatellite = false;
	hasSatelliteKeps = false;
}

static void exitCallback(void *data)
{
	if (displayMode == SATELLITE_SCREEN_ALL_PREDICTIONS_LIST)
	{
		clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_SPEED_RUN);
	}

	if (menuSystemGetRootMenuNumber() == UI_CHANNEL_MODE)
	{
		currentChannelData = &channelScreenChannelData;
		uiChannelModeLoadChannelData(true, false);
	}
	else
	{
		currentChannelData = &settingsVFOChannel[nonVolatileSettings.currentVFONumber];
		uiVFOModeLoadChannelData(false);
	}

	aprsBeaconingResetTimers();
}

static void calculateActiveSatelliteData(bool forceFrequencyUpdate)
{
	gmtime_r_Custom(&uiDataGlobal.dateTimeSecs, &timeAndDate);

	satelliteCalculateForDateTimeSecs(currentActiveSatellite, uiDataGlobal.dateTimeSecs, &currentSatelliteResults, SATELLITE_PREDICTION_LEVEL_FULL);

	uint32_t rxF = currentSatelliteResults.freqs[currentSatelliteFreqIndex].rxFreq / 10;
	uint32_t txF = currentSatelliteResults.freqs[currentSatelliteFreqIndex].txFreq / 10;
	currentChannelData->txFreq = txF;// Tx freq can be updated immediately
	// Only update the Rx freq is its off by more than 500Hz,
	// to prevent constant dropout in the Rx caused by the process of changing the frequency of the AT1846S
	// Note value is stored in the channel data a 10Hz resolution hence the / 10
	if (forceFrequencyUpdate || (abs(currentChannelData->rxFreq - rxF) > (500 / 10)))
	{
		currentChannelData->rxFreq = rxF;

		trxSetFrequency(currentChannelData->rxFreq, currentChannelData->txFreq, DMR_MODE_AUTO);

		trxPostponeReadRSSIAndNoise(100);
	}

	// Handle VP for AOS and LOS
	if (currentSatelliteResults.elevation > 0)
	{
		if (!satelliteVisible)
		{
			satelliteVisible = true;// satellite acquired

			voicePromptsInitWithOverride();
			voicePromptsAppendLanguageString(currentLanguage->satellite);
			voicePromptsAppendString(currentActiveSatellite->name);
			voicePromptsAppendLanguageString(currentLanguage->azimuth);

			char buf[16];
			snprintf(buf, 16, "%03d%c", currentSatelliteResults.azimuthAsInteger, 176);
			voicePromptsAppendString(buf);
			voicePromptsPlay();
		}
	}
	else
	{
		if (satelliteVisible)
		{
			satelliteVisible = false;// satellite lost
			voicePromptsInitWithOverride();
			voicePromptsAppendLanguageString(currentLanguage->satellite);
			voicePromptsAppendLanguageString(currentLanguage->off);
			voicePromptsPlay();
		}
	}
	hasRecalculated = true;
}
