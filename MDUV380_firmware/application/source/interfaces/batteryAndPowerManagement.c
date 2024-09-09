/*
 * Copyright (C) 2021-2024 Roger Clark, VK3KYY / G4KYF
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

#include "main.h"
#include "usb_device.h"
#include "interfaces/batteryAndPowerManagement.h"
#include "user_interface/uiGlobals.h"
#include "user_interface/uiLocalisation.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiUtilities.h"
#include "hardware/HR-C6000.h"
#include "usb/usb_com.h"
#include "functions/ticks.h"
#include "interfaces/clockManager.h"
#include "functions/codeplug.h"
#include "hardware/radioHardwareInterface.h"
#include "interfaces/gps.h"
#include "interfaces/settingsStorage.h"

//#define DEBUG_HARDWARE_SCREEN 1

static uint32_t lowBatteryCount = 0;
#define LOW_BATTERY_INTERVAL                       ((1000 * 60) * 5) // 5 minute;
#define LOW_BATTERY_WARNING_VOLTAGE_DIFFERENTIAL   6	// Offset between the minimum voltage and when the battery warning audio starts. 6 = 0.6V
#define LOW_BATTERY_VOLTAGE_RECOVERY_TIME          30000 // 30 seconds
#define SUSPEND_LOW_BATTERY_RATE                   1000 // 1 second

#define BATTERY_VOLTAGE_TICK_RELOAD                100
#define BATTERY_VOLTAGE_CALLBACK_TICK_RELOAD       20

static int batteryVoltageCallbackTick = 0;
static int batteryVoltageTick = BATTERY_VOLTAGE_TICK_RELOAD;

volatile float averageBatteryVoltage = 0;
static volatile float previousAverageBatteryVoltage;
volatile int batteryVoltage = 0;
volatile uint16_t micLevel = 0;
volatile uint16_t potLevel = 0;
volatile uint16_t temperatureLevel = 0;
volatile int lastValidBatteryVoltage = 64;
volatile uint32_t resumeTicks = 0;


#if !defined(PLATFORM_GD77S)
ticksTimer_t apoTimer;
#endif

#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
bool powerRotarySwitchIsOn(void)
{
	return (batteryVoltage > POWEROFF_VOLTAGE_THRESHOLD);
}
#endif

void showLowBattery(void)
{
	showErrorMessage(currentLanguage->low_battery);
}

bool batteryIsLowWarning(void)
{
	return (lowBatteryCount > LOW_BATTERY_VOLTAGE_RECOVERY_TIME);
}

bool batteryIsLowVoltageWarning(void)
{
	return (powerRotarySwitchIsOn() ? (batteryVoltage < (CUTOFF_VOLTAGE_LOWER_HYST + LOW_BATTERY_WARNING_VOLTAGE_DIFFERENTIAL)) : false);
}

bool batteryIsLowCriticalVoltage(void)
{
	return (powerRotarySwitchIsOn() ? (batteryVoltage < CUTOFF_VOLTAGE_LOWER_HYST) : false);
}

bool batteryLastReadingIsCritical(void)
{
	if (powerRotarySwitchIsOn() == false)
	{
		return (lastValidBatteryVoltage < CUTOFF_VOLTAGE_UPPER_HYST);
	}

	lastValidBatteryVoltage = adcGetBatteryVoltage();

	return (lastValidBatteryVoltage < CUTOFF_VOLTAGE_UPPER_HYST);
}

void batteryChecking(uiEvent_t *ev)
{
	static ticksTimer_t lowBatteryBeepTimer = { 0, 0 };
	static ticksTimer_t lowBatteryHeaderRedrawTimer = { 0, 0 };
	static uint32_t lowBatteryCriticalCount = 0;
	bool lowBatteryWarning = batteryIsLowVoltageWarning();
	bool batIsLow = false;

	if (powerRotarySwitchIsOn() && (batteryVoltage < 20))
	{
		if (menuSystemGetCurrentMenuNumber() != UI_POWER_OFF)
		{
			menuSystemPushNewMenu(UI_POWER_OFF);
		}
		return;
	}

	// Low battery threshold is reached after 30 seconds, in total, of lowBatteryWarning.
	// Once reached, another 30 seconds is added to the counter to avoid retriggering on voltage fluctuations.
	lowBatteryCount += (lowBatteryWarning
			? ((lowBatteryCount <= (LOW_BATTERY_VOLTAGE_RECOVERY_TIME * 2)) ? ((lowBatteryCount == LOW_BATTERY_VOLTAGE_RECOVERY_TIME) ? LOW_BATTERY_VOLTAGE_RECOVERY_TIME : 1) : 0)
			: (lowBatteryCount ? -1 : 0));

	// Do we need to redraw the header row now ?
	batIsLow = batteryIsLowWarning();
	if (batIsLow && ticksTimerHasExpired(&lowBatteryHeaderRedrawTimer))
	{
		headerRowIsDirty = true;
		ticksTimerStart(&lowBatteryHeaderRedrawTimer, 500);
	}

	if ((settingsUsbMode != USB_MODE_HOTSPOT) &&
#if defined(PLATFORM_GD77S)
			(trxTransmissionEnabled == false) &&
#else
			(menuSystemGetCurrentMenuNumber() != UI_TX_SCREEN) &&
#endif
			batIsLow &&
			ticksTimerHasExpired(&lowBatteryBeepTimer))
	{

		if (melody_play == NULL)
		{
			if (nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
			{
				soundSetMelody(MELODY_LOW_BATTERY);
			}
			else
			{
				voicePromptsInit();
				voicePromptsAppendLanguageString(currentLanguage->low_battery);
				voicePromptsPlay();
			}

			// Let the beep sound, or the VP, to finish to play before resuming the scan (otherwise is could be silent).
			if (uiDataGlobal.Scan.active)
			{
				uiDataGlobal.Scan.active = false;
				watchdogRun(false);

				while ((melody_play != NULL) || voicePromptsIsPlaying())
				{
					voicePromptsTick();
					soundTickMelody();

					osDelay(1);
				}

				watchdogRun(true);
				uiDataGlobal.Scan.active = true;
			}

			ticksTimerStart(&lowBatteryBeepTimer, LOW_BATTERY_INTERVAL);
		}
	}

	// Check if the battery has reached critical voltage (power off)
	bool lowBatteryCritical = powerRotarySwitchIsOn() ? batteryIsLowCriticalVoltage() : false;

	// Critical battery threshold is reached after 30 seconds, in total, of lowBatteryCritical.
	lowBatteryCriticalCount += (lowBatteryCritical ? 1 : (lowBatteryCriticalCount ? -1 : 0));

	// Low battery or poweroff (non RD-5R)
	bool powerSwitchIsOff =
#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			(powerRotarySwitchIsOn() == false);
#else
			false;
#endif

	if ((powerSwitchIsOff || lowBatteryCritical) && (menuSystemGetCurrentMenuNumber() != UI_POWER_OFF))
	{
		// is considered as flat (stable value), now stop the firmware: make it silent
		if ((lowBatteryCritical && (lowBatteryCriticalCount > LOW_BATTERY_VOLTAGE_RECOVERY_TIME)) || powerSwitchIsOff)
		{
			radioAudioAmp(false);
			soundSetMelody(NULL);
		}

		// Avoids delayed power off (on non RD-5R) if the power switch is turned off while in low battery condition
		if (lowBatteryCritical && (powerSwitchIsOff == false))
		{
			// Now, the battery is considered as flat (stable value), powering off.
			if (lowBatteryCriticalCount > LOW_BATTERY_VOLTAGE_RECOVERY_TIME)
			{
				showLowBattery();
				powerOffFinalStage(false, false);
			}
		}
#if ! defined(PLATFORM_RD5R)
		else
		{
			bool suspend = false;

#if !(defined(PLATFORM_GD77S) || defined(STM32F405xx))
			suspend = settingsIsOptionBitSet(BIT_POWEROFF_SUSPEND);

			// Suspend bit is set, but user pressed the SK2, asking for a real poweroff
			if (suspend && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
			{
				suspend = false;
			} // Suspend bit is NOT set, but user pressed the SK2, asking for a suspend
			else if ((suspend == false) && BUTTONCHECK_DOWN(ev, BUTTON_SK2))
			{
				suspend = true;
			}
#endif
			if (suspend)
			{
				powerOffFinalStage(true, false);
			}
			else
			{
				menuSystemPushNewMenu(UI_POWER_OFF);
			}
		}
#endif // ! PLATFORM_RD5R
	}
}

void batteryUpdate(void)
{
	batteryVoltageTick++;
	if (batteryVoltageTick >= BATTERY_VOLTAGE_TICK_RELOAD)
	{
		if (powerRotarySwitchIsOn())
		{
			if (previousAverageBatteryVoltage != averageBatteryVoltage)
			{
				previousAverageBatteryVoltage = averageBatteryVoltage;
				headerRowIsDirty = true;
			}

			batteryVoltageCallbackTick++;
			if (batteryVoltageCallbackTick >= BATTERY_VOLTAGE_CALLBACK_TICK_RELOAD)
			{
				menuRadioInfosPushBackVoltage(averageBatteryVoltage);
				batteryVoltageCallbackTick = 0;
			}
		}

		batteryVoltageTick = 0;
	}
}

void powerDown(bool doNotSavePowerOffState)
{
	uint32_t m = ticksGetMillis();

	if (!doNotSavePowerOffState)
	{
#if defined(PLATFORM_MD9600)
		uint16_t radioIsInStandby = RADIO_IN_STANDBY_FLAG_PATTERN;
		batteryRAM_Write(0,(uint8_t *)&radioIsInStandby,2);
#endif
	}

	settingsSaveSettings(true);
	codeplugSaveLastUsedChannelInZone();

#if defined(HAS_GPS)
#if defined(LOG_GPS_DATA)
	gpsLoggingStop();
#endif
	gpsOff();
#endif

	// Give it a bit of time to finish to write the flash (avoiding corruptions).
	while (true)
	{
		if ((ticksGetMillis() - m) > 100)
		{
			break;
		}

		osDelay(1);
	}

	gpioSetDisplayBacklightIntensityPercentage(0);
	displaySetDisplayPowerMode(false);
	radioPowerOff(true, true);

	//Reset the display, saves about 1mA
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	osDelay(20);

	MX_USB_DEVICE_DeInit(); // Deinit USB

	HAL_SuspendTick();
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

	HAL_PWR_EnterSTANDBYMode();

	HAL_GPIO_WritePin(PWR_SW_GPIO_Port, PWR_SW_Pin, GPIO_PIN_RESET);
}

void die(bool usbMonitoring, bool maintainRTC, bool forceSuspend, bool safeBoot)
{
	int8_t batteryCriticalCount = 0;
#if !defined(PLATFORM_RD5R) && !defined(PLATFORM_GD77S)
	uint32_t lowBatteryCriticalCount = 0;
	ticksTimer_t nextPITCounterRunTimer = { .start = ticksGetMillis(), .timeout = SUSPEND_LOW_BATTERY_RATE };
#if defined(DEBUG_HARDWARE_SCREEN)
	uint16_t y = 8;
#else

	UNUSED_PARAMETER(safeBoot);
#endif

	if (!maintainRTC)
	{
		HAL_GPIO_WritePin(PWR_SW_GPIO_Port, PWR_SW_Pin, GPIO_PIN_RESET); // This is normally already done before this function is called.
		// But do it again, just in case, as its important that the radio will turn off when the power control is turned to off
	}
#endif

	disableAudioAmp(AUDIO_AMP_MODE_RF);
	disableAudioAmp(AUDIO_AMP_MODE_BEEP);
	disableAudioAmp(AUDIO_AMP_MODE_PROMPT);
	LedWrite(LED_GREEN, 0);
	LedWrite(LED_RED, 0);
	trxResetSquelchesState(RADIO_DEVICE_PRIMARY); // Could be put in sleep state and awaken with a signal, so this will re-enable the audio AMP

	trxPowerUpDownRxAndC6000(false, true, true);

	if (usbMonitoring)
	{
		ticksTimer_t checkBatteryTimer = { .start = ticksGetMillis(), .timeout = (safeBoot ? 500U : 1000U) };

#if defined(DEBUG_HARDWARE_SCREEN)
#warning SAFEBOOT HARDWARE DEBUG SCREEN ENABLED
		if (safeBoot)
		{
#if defined(STM32F405xx) && ! defined(PLATFORM_MD9600)
			char cpuTypeBuf[SCREEN_LINE_BUFFER_SIZE] = {0};
			uint32_t vpHeader[2];

			displayClearBuf();

			sprintf(cpuTypeBuf, "CPU Type   :%s (%u)", ((NumInterruptPriorityBits == 4) ? "STM" : "TYT"), NumInterruptPriorityBits);
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "CPU Sig    :0x%04X", cpuGetSignature());
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "CPU Rev    :0x%04X", cpuGetRevision());
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "CPU Pack   :0x%04X", cpuGetPackage());
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "CPU Flash  :%dkb", cpuGetFlashSize());
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "Flash Type :0x%X", flashChipPartNumber);
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "NVRAM :0x%X %u", nonVolatileSettings.magicNumber, settingsIsOptionBitSet(BIT_SETTINGS_UPDATED));
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			sprintf(cpuTypeBuf, "Codec :%s", (codecIsAvailable() ? "OK" : "KO"));
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			SPI_Flash_read(VOICE_PROMPTS_FLASH_HEADER_ADDRESS, (uint8_t *)&vpHeader, sizeof(vpHeader));
			sprintf(cpuTypeBuf, "VP :0x%X 0x%X", vpHeader[0], vpHeader[1]);
			displayPrintAt(0, y, cpuTypeBuf , FONT_SIZE_2);
			y += FONT_SIZE_2_HEIGHT;

			displayRender();
#endif
		}
#endif

		while(true)
		{
			tick_com_request();
			vTaskDelay((0 / portTICK_PERIOD_MS));

#if defined(DEBUG_HARDWARE_SCREEN)
			if (safeBoot)
			{
				if (ticksTimerHasExpired(&checkBatteryTimer)) // reuse this timer
				{
					char buf[SCREEN_LINE_BUFFER_SIZE];
					uint16_t ypos = y;
					uint32_t buttons = 0;
					int button_event = EVENT_BUTTON_NONE;
					uint32_t key = keyboardRead();
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701)
					uint8_t rotDir = ' ';
#elif defined(PLATFORM_MD2017)
					uint8_t trackDir = ' ';
#endif

					buttonsCheckButtonsEvent(&buttons, &button_event, false);

					displayFillRect(0, ypos, DISPLAY_SIZE_X, DISPLAY_SIZE_Y - y, true);

					sprintf(buf, "Ps :%u, Lvl :%u %d", powerRotarySwitchIsOn(), potLevel, getVolumeControl());
					displayPrintAt(0, ypos, buf , FONT_SIZE_2);
					ypos += FONT_SIZE_2_HEIGHT;

					sprintf(buf, "V :%d, ADC :%d", lastValidBatteryVoltage, adcGetBatteryVoltage());
					displayPrintAt(0, ypos, buf , FONT_SIZE_2);
					ypos += FONT_SIZE_2_HEIGHT;

					sprintf(buf, "Btn :0x%X, Evt :%d", buttons, button_event);
					displayPrintAt(0, ypos, buf , FONT_SIZE_2);
					ypos += FONT_SIZE_2_HEIGHT;

					sprintf(buf, "Key :%3d [%c]", key, ((key != 0) ? key : ' '));
					displayPrintAt(0, ypos, buf , FONT_SIZE_2);
					ypos += FONT_SIZE_2_HEIGHT;

#if defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701)
					if (rotaryData.Direction != 0)
					{
						rotDir = ((rotaryData.Direction == 1) ? '+' : '-');

						nonVolatileSettings.displayBacklightPercentage[DAY] += ((rotaryData.Direction > 0) ?
								(nonVolatileSettings.displayBacklightPercentage[DAY] >= 10 ? 10 : 1)
								:
								(nonVolatileSettings.displayBacklightPercentage[DAY] >= 20 ? -10 : -1));
						nonVolatileSettings.displayBacklightPercentage[DAY] = CLAMP(nonVolatileSettings.displayBacklightPercentage[DAY], 0, 100);
						gpioSetDisplayBacklightIntensityPercentage(nonVolatileSettings.displayBacklightPercentage[DAY]);
						rotaryData.Direction = 0;
					}
					sprintf(buf, "Rot :[%c], Bcl :%d%%", rotDir, nonVolatileSettings.displayBacklightPercentage[DAY]);
#elif defined(PLATFORM_MD2017)
					if (trackballData.Count > (2 + 1)) // TRACKBALL_FAST_MOTION + 1
					{
						trackDir = trackballData.Direction;
						trackballData.Count -= 2; // TRACKBALL_FAST_MOTION
					}
					sprintf(buf, "TBall :[%c], Cnt :%d", trackDir, trackballData.Count);
#endif
					displayPrintAt(0, ypos, buf , FONT_SIZE_2);
					//ypos += FONT_SIZE_2_HEIGHT;

					displayRenderRows(y / 8, ((ypos / 8) + (FONT_SIZE_2_HEIGHT / 8)));
					ticksTimerStart(&checkBatteryTimer, 200U);
				}

				batteryUpdate();
				getVolumeControl(); // continue to average value..
			}
			else
#endif
			{
				if (ticksTimerHasExpired(&checkBatteryTimer))
				{
					batteryCriticalCount += (batteryLastReadingIsCritical() ? 1 : (batteryCriticalCount ? -1 : 0));

					if (batteryCriticalCount > (LOW_BATTERY_VOLTAGE_RECOVERY_TIME / 1000))
					{
						powerDown(true);
						while(true); // won't reach this.
					}

					ticksTimerStart(&checkBatteryTimer, 1000U);
				}
			}
		}
	}
	else
	{
		uint8_t batteryLowRetries = 50;
#if !defined(PLATFORM_GD77S)
		uint32_t prevPowerSwitchState =
#if !defined(PLATFORM_RD5R)
				(powerRotarySwitchIsOn() ? 0 : 1)
#else
				1
#endif
				;
#endif

#if defined(HAS_GPS)
#if defined(LOG_GPS_DATA)
		gpsLoggingStop();
#endif
		gpsOff();
#endif

		while(batteryLowRetries-- > 0)
		{
			batteryCriticalCount += (batteryLastReadingIsCritical() ? 1 : (batteryCriticalCount ? -1 : 0));
			osDelay(1);
		}
		bool batteryIsCritical = batteryCriticalCount > 25;

		clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_RUN_SUSPEND_MODE);
		isSuspended = true;
/* VK3KYY Need to port 
		watchdogDeinit();
*/
		if (batteryIsCritical)
		{
			powerDown(true);
			while(true); // won't reach this.
		}
		else
		{
			displaySetDisplayPowerMode(false);
		}

		MX_USB_DEVICE_DeInit(); // Deinit USB

		while(true)
		{
			batteryUpdate();

#if !defined(PLATFORM_GD77S)
			if (uiDataGlobal.SatelliteAndAlarmData.alarmType == ALARM_TYPE_NONE)
			{
				uint32_t powerSwitchState =
#if !defined(PLATFORM_RD5R)
						(powerRotarySwitchIsOn() ? 0 : 1)
#else
						1
#endif
						;

				// Safe Power On option is ON, user didn't press SK1 on power ON, so
				// forceSuspend is true.
				// Now, user just turned OFF the power switch, clear the forceSuspend flag to
				// be able to handle the power ON event.
				if ((powerSwitchState != 0) && forceSuspend)
				{
					forceSuspend = false;
				}

				if ((powerSwitchState == 0) && (forceSuspend == false) &&
						((settingsIsOptionBitSet(BIT_SAFE_POWER_ON) ?
								(((buttonsRead() & BUTTON_SK1) != 0) && (powerSwitchState != prevPowerSwitchState))
								: true)))
				{
					// User wants to go in bootloader mode
					if (buttonsRead() == (BUTTON_SK1 | BUTTON_PTT))
					{
						NVIC_SystemReset();
					}

					wakeFromSleep();
					return;
				}

				prevPowerSwitchState = powerSwitchState;
			}
#endif

#if !defined(PLATFORM_RD5R) && !defined(PLATFORM_GD77S)
			if (ticksTimerHasExpired(&nextPITCounterRunTimer))
			{
				// Check if the battery has reached critical voltage (power off)
				bool powerSwitchIsOn = powerRotarySwitchIsOn();
				bool lowBatteryCritical = powerSwitchIsOn ? batteryIsLowCriticalVoltage() : false;

				// Critical battery threshold is reached after 30 seconds, in total, of lowBatteryCritical.
				lowBatteryCriticalCount += (lowBatteryCritical ? 1 : (lowBatteryCriticalCount ? -1 : 0));

				if ((powerSwitchIsOn == false) ||
						(lowBatteryCritical && (lowBatteryCriticalCount > (LOW_BATTERY_VOLTAGE_RECOVERY_TIME / 1000) /* 30s in total */)))
				{
					HAL_GPIO_WritePin(PWR_SW_GPIO_Port, PWR_SW_Pin, GPIO_PIN_RESET);
					while(true);
				}

				ticksTimerStart(&nextPITCounterRunTimer, SUSPEND_LOW_BATTERY_RATE);
			}

			if (uiDataGlobal.SatelliteAndAlarmData.alarmType != ALARM_TYPE_NONE)
			{
				bool powerSwitchIsOff =	(powerRotarySwitchIsOn() == false);
				if (powerSwitchIsOff)
				{
					uiDataGlobal.SatelliteAndAlarmData.alarmType = ALARM_TYPE_NONE;
				}
			}

			if (uiDataGlobal.SatelliteAndAlarmData.alarmType != ALARM_TYPE_NONE)
			{
				bool wakingUp =
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
						(buttonsRead() & BUTTON_ORANGE);
#else // MD-UV3x0 | RT3S
						((buttonsRead() & (BUTTON_SK2 | BUTTON_PTT)) == (BUTTON_SK2 | BUTTON_PTT));

#endif
				if (wakingUp)
				{
					// Wait for button(s) release
					while (
#if (defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017))
							(buttonsRead() & BUTTON_ORANGE)
#else
							(buttonsRead() & (BUTTON_SK2 | BUTTON_PTT))
#endif
					)
					{
						osDelay(10);
					}

					wakeFromSleep();
					return;
				}
			}

			if (uiDataGlobal.SatelliteAndAlarmData.alarmType == ALARM_TYPE_SATELLITE ||
					uiDataGlobal.SatelliteAndAlarmData.alarmType == ALARM_TYPE_CLOCK)
			{
				if (uiDataGlobal.dateTimeSecs >= uiDataGlobal.SatelliteAndAlarmData.alarmTime)
				{
					wakeFromSleep();
					return;
				}
			}
#endif
			osDelay(100);
		}

	}
}

void wakeFromSleep(void)
{
	MX_USB_DEVICE_Init();

#if !defined(PLATFORM_RD5R)
	if (menuSystemGetPreviousMenuNumber() == MENU_SATELLITE)
	{
//		clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_SPEED_HS_RUN);
		clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_SPEED_RUN);
	}
	else
	{
		clockManagerSetRunMode(kAPP_PowerModeRun, CLOCK_MANAGER_SPEED_RUN);
	}

/*
	GPIO_PinWrite(GPIO_Keep_Power_On, Pin_Keep_Power_On, 1);// This is normally already done before this function is called.
	// But do it again, just in case, as its important that the radio will turn off when the power control is turned to off
*/

	trxPowerUpDownRxAndC6000(true, true, true);

	// Reset counters before enabling watchdog
	hrc6000Task.AliveCount = TASK_FLAGGED_ALIVE;
	beepTask.AliveCount = TASK_FLAGGED_ALIVE;

/* VK3KYY Need to port to MD9600
	watchdogInit();
*/
	displaySetDisplayPowerMode(true);

	if (trxGetMode() == RADIO_MODE_DIGITAL)
	{
		HRC6000ResetTimeSlotDetection();
		HRC6000InitDigitalDmrRx();
	}
#endif

#if defined(HAS_GPS)
	if (nonVolatileSettings.gps >= GPS_MODE_OFF)
	{
		gpsOn();
#if defined(LOG_GPS_DATA)
		gpsLoggingStart();
#endif
	}
#endif

#if !defined(PLATFORM_GD77S)
	if (nonVolatileSettings.apo > 0)
	{
		ticksTimerStart(&apoTimer, ((nonVolatileSettings.apo * 30) * 60000U));
	}

	if ((nonVolatileSettings.autolockTimer > 0) && ticksTimerIsEnabled(&autolockTimer))
	{
		ticksTimerStart(&autolockTimer, (nonVolatileSettings.autolockTimer * 30000U));
	}
#endif

	// Trick to compensate the fact that VBat is disconnected from the battery when
	// the power switch is set to OFF. Hence, we need to display the non averaged
	// voltage value, to avoid the long ramp up.
	resumeTicks = (ticksGetMillis() + BATTERY_VOLTAGE_STABILISATION_TIME);

	voicePromptsInit(); // Flush the VPs

	isSuspended = false;
}


void powerOffFinalStage(bool maintainRTC, bool forceSuspend)
{
	uint32_t m;

	// If TXing, get back to RX (this function can be called on low battery event).
	if (trxTransmissionEnabled)
	{
		trxTransmissionEnabled = false;
		trxActivateRx(true);
		trxIsTransmitting = false;

		LedWrite(LED_RED, 0);//LEDs_PinWrite(GPIO_LEDred, Pin_LEDred, 0);
	}

	// Restore DMR filter settings if the radio is turned off whilst in monitor mode
	if (monitorModeData.isEnabled)
	{
		nonVolatileSettings.dmrCcTsFilter = monitorModeData.savedDMRCcTsFilter;
		nonVolatileSettings.dmrDestinationFilter = monitorModeData.savedDMRDestinationFilter;
	}

	// If user was in a private call when they turned the radio off we need to restore the last Tg prior to stating the Private call.
	// to the nonVolatile Setting overrideTG, otherwise when the radio is turned on again it be in PC mode to that station.
	if ((trxTalkGroupOrPcId >> 24) == PC_CALL_FLAG)
	{
		settingsSet(nonVolatileSettings.overrideTG, uiDataGlobal.tgBeforePcMode);
	}

	menuHotspotRestoreSettings();

	codeplugSaveLastUsedChannelInZone();

#if defined(LOG_GPS_DATA)
	gpsLoggingStop();
#endif

	m = ticksGetMillis();
	settingsSaveSettings(true);

	// Give it a bit of time before pulling the plug as DM-1801 EEPROM looks slower
	// than GD-77 to write, then quickly power cycling triggers settings reset.
	while (true)
	{
		if ((ticksGetMillis() - m) > 50)
		{
			break;
		}

		osDelay(1);
	}

	displayEnableBacklight(false, 0);

#if ! (defined(PLATFORM_RD5R))// || defined(PLATFORM_MD9600) || defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017))
	// This turns the power off to the CPU.
	if (!maintainRTC)
	{
		HAL_GPIO_WritePin(PWR_SW_GPIO_Port, PWR_SW_Pin, GPIO_PIN_RESET);
	}
#endif

	die(false, maintainRTC, forceSuspend, false);
}

#if !defined(PLATFORM_GD77S)
void apoTick(bool eventFromOperator)
{
	if (nonVolatileSettings.apo > 0)
	{
		int currentMenu = menuSystemGetCurrentMenuNumber();

		// Reset APO timer:
		//   - on events
		//   - when scanning
		//   - when user has set a Satellite alarm
		//   - when transmissing, while in hotspot mode or while using the CPS
		//   - on RF activity
		if (eventFromOperator ||
				uiDataGlobal.Scan.active ||
				((currentMenu == UI_TX_SCREEN) || (currentMenu == UI_HOTSPOT_MODE) || (currentMenu == UI_CPS)) ||
				(uiDataGlobal.SatelliteAndAlarmData.alarmType != ALARM_TYPE_NONE))
		{
			if (uiNotificationIsVisible() && (uiNotificationGetId() == NOTIFICATION_ID_USER_APO))
			{
				uiNotificationHide(true);
			}

			ticksTimerStart(&apoTimer, ((nonVolatileSettings.apo * 30) * 60000U));
		}

		// No event in the last 'apo' time => Suspend
		if (ticksTimerHasExpired(&apoTimer))
		{
			powerDown(false);
		}
		else
		{
			 // 1 minute or less is remaining, it's time to show the APO notification
			if ((ticksTimerRemaining(&apoTimer) <= 60000U) &&
					((uiNotificationIsVisible() && (uiNotificationGetId() == NOTIFICATION_ID_USER_APO)) == false))
			{
				if (nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD)
				{
					soundSetMelody(MELODY_APO_TRIGGERED);
				}
				else
				{
					voicePromptsInit();
					voicePromptsAppendLanguageString(currentLanguage->auto_power_off);
					voicePromptsPlay();
				}

				uiNotificationShow(NOTIFICATION_TYPE_MESSAGE, NOTIFICATION_ID_USER_APO, 60000U, currentLanguage->auto_power_off, true);
			}
		}
	}
}
#endif
