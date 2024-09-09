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

#ifndef _BATTERYANDPOWERMANAGEMENT_H_
#define _BATTERYANDPOWERMANAGEMENT_H_

#include "functions/ticks.h"
#include "user_interface/menuSystem.h"

#if defined(PLATFORM_MD9600)
#define RADIO_IN_STANDBY_FLAG_PATTERN  0x00FF
#endif

extern volatile int batteryVoltage;
extern volatile uint16_t micLevel;
extern volatile uint16_t potLevel;
extern volatile uint16_t temperatureLevel;
extern volatile int lastValidBatteryVoltage;
extern volatile float averageBatteryVoltage;
extern volatile uint32_t resumeTicks;

#if !defined(PLATFORM_GD77S)
extern ticksTimer_t apoTimer;
#endif


#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
bool powerRotarySwitchIsOn(void);
#endif
void showLowBattery(void);
bool batteryIsLowWarning(void);
bool batteryIsLowVoltageWarning(void);
bool batteryIsLowCriticalVoltage(void);
bool batteryLastReadingIsCritical(void);
void batteryChecking(uiEvent_t *ev);
void batteryUpdate(void);
void die(bool usbMonitoring, bool maintainRTC, bool forceSuspend, bool safeBoot);
void wakeFromSleep(void);
void powerOffFinalStage(bool maintainRTC, bool forceSuspend);
void powerDown(bool doNotSavePowerOffState);
void apoTick(bool eventFromOperator);

#endif
