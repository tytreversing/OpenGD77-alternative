/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
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

#ifndef _OPENGD77_USB_COM_H_
#define _OPENGD77_USB_COM_H_

#include <FreeRTOS.h>
#include <task.h>
#include "main.h"
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380)  || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701)
#include "usbd_cdc_if.h"
#else
#include "virtual_com.h"
#define APP_RX_DATA_SIZE (512 * 3)
#endif

#define COM_BUFFER_SIZE APP_RX_DATA_SIZE
#define COM_REQUESTBUFFER_SIZE COM_BUFFER_SIZE

extern volatile uint8_t com_buffer[COM_BUFFER_SIZE];
extern int com_buffer_write_idx;
extern int com_buffer_read_idx;
extern volatile int com_buffer_cnt;

extern volatile int comRecvMMDVMIndexIn;
extern volatile int comRecvMMDVMIndexOut;
extern volatile int comRecvMMDVMFrameCount;
extern volatile int com_request;
extern volatile uint8_t com_requestbuffer[COM_REQUESTBUFFER_SIZE];

#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380)  || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701)
extern volatile uint8_t usbComSendBuf[COM_BUFFER_SIZE];
#else
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t usbComSendBuf[COM_BUFFER_SIZE];
#endif
extern bool isCompressingAMBE;

extern volatile bool usbIsResetting;

void tick_com_request(void);
void send_packet(uint8_t val_0x82, uint8_t val_0x86, int ram);
void send_packet_big(uint8_t val_0x82, uint8_t val_0x86, int ram1, int ram2);
void add_to_commbuffer(uint8_t value);
bool USB_DeviceIsResetting(void);

void USB_DEBUG_PRINT(char *str);
void USB_DEBUG_printf(const char *format, ...) __attribute__((format(__printf__, 1, 2)));

typedef struct __attribute__ ((__packed__))
{
	uint32_t 		magicNumber;
	uint8_t			timezone;// Lower 7 bits are the timezone. 64 = UTC, values < 64 are negative TZ values.  Bit 8 is a flag which indicates TZ/UTC. 0 = UTC
	uint8_t			beepOptions; // 2 pairs of bits + 1 (TX and RX beeps)
	uint32_t		bitfieldOptions; // see bitfieldOptions_t
	uint32_t		aprsBeaconingSettingsPart1;
	uint32_t		aprsBeaconingSettingsPart2;
	uint16_t		aprsBeaconingSettingsPart3;
	uint8_t			txPowerLevel;
	uint8_t			txTimeoutBeepX5Secs;
	uint8_t			beepVolumeDivider;
	uint8_t			micGainDMR;
	uint8_t			micGainFM;
	uint8_t			backlightMode; // see BACKLIGHT_MODE enum
	uint8_t			backLightTimeout; // 0 = never timeout. 1 - 255 time in seconds
	int8_t			displayContrast;
	int8_t			displayBacklightPercentageDay;
	int8_t			displayBacklightPercentageNight;
	int8_t			displayBacklightPercentageOff; // backlight level when "off"
	uint8_t			extendedInfosOnScreen;
	uint8_t			scanModePause;
	uint8_t			scanDelay;
	uint8_t			DMR_RxAGC;
	uint8_t			hotspotType;
	uint8_t			scanStepTime;
	uint8_t			dmrCaptureTimeout;
	uint8_t    		privateCalls;
	uint8_t			contactDisplayPriority;
	uint8_t			splitContact;
	uint8_t			voxThreshold; // 0: disabled
	uint8_t			voxTailUnits; // 500ms units
	uint8_t			audioPromptMode;
	uint8_t			batteryCalibration; // Units of 0.01V (NOTE: only the 4 lower bits are used)
	uint8_t			squelchDefaultVHF;
	uint8_t			squelchDefaultUHF;
	uint8_t			placeHolder;
	uint8_t			ecoLevel;// Power saving / economy level
	uint8_t			apo; // unit: 30 minutes (5 is skipped, as we want 0, 30, 60, 90, 120 and 180)
	uint8_t			keypadTimerLong;
	uint8_t			keypadTimerRepeat;
	uint8_t			autolockTimer; // in minutes
	uint8_t         buttonP3; //����� ������ ������ P3
	uint8_t         buttonP3Long;
	uint8_t         scanPriority; //��������� ������������� ������������, 2...10
	uint8_t         txFreqLimited;
	uint8_t         reserve1;
	uint16_t        reserve2;
	uint32_t        reserve3;
} settingsAlignedStruct_t;


#endif /* _OPENGD77_USB_COM_H_ */
