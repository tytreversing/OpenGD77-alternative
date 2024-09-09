/*
 * Copyright (C) 2021-2024 Roger Clark, VK3KYY / G4KYF
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

#ifndef _RADIO_HARDWARE_INTERFACE_H_
#define _RADIO_HARDWARE_INTERFACE_H_
#include "main.h"


typedef enum
{
	RADIO_DEVICE_PRIMARY   = 0U,
#if defined(PLATFORM_MD2017)
	RADIO_DEVICE_SECONDARY = 1U,
#endif
	RADIO_DEVICE_MAX
} RadioDevice_t;

#define FREQUENCY_UNSET        UINT32_MAX
#define FREQUENCY_OUT_OF_BAND  UINT32_MAX
#define POWER_UNSET            UINT8_MAX

void radioPowerOn(void);
void radioPowerOff(bool invalidateFrequency, bool includeMic);
void radioInit(void);
void radioPostinit(void);
RadioDevice_t radioSetTRxDevice(RadioDevice_t deviceId);
RadioDevice_t radioGetTRxDeviceId(void);
void radioSetBandwidth(bool Is25K);
void radioSetCalibration(void);
void radioSetIF(int band, bool wide);
void radioSetMode(int mode);
void radioSetFrequency(uint32_t freq, bool Tx);
void radioSetTx(uint8_t band);
void radioSetRx(uint8_t band);
void radioReadVoxAndMicStrength(void);
void radioReadRSSIAndNoiseForBand(uint8_t band);
void radioRxCSSOff(RadioDevice_t deviceId);
void radioRxCTCSOn(RadioDevice_t deviceId, uint16_t tone);
void radioRxDCSOn(RadioDevice_t deviceId, uint16_t code, bool inverted);
void radioTxCSSOff(void);
void radioTxCTCSOn(uint16_t tone);
void radioTxDCSOn(uint16_t code, bool inverted);
bool radioCheckCSS(uint16_t tone, CodeplugCSSTypes_t type);
void radioSetTone1(int tonefreq);
void radioSetTone2(int tonefreq);
void radioSetMicGain(uint8_t gain_tx);
void radioSetMicGainFM(uint8_t gain);
void radioAudioAmp(bool on);
void radioSetAudioPath(bool fromFM);
void radioFastTx(bool tx);
void radioSetRxLNAForDevice(RadioDevice_t deviceId);
void radioSelectVoiceChannel(uint8_t channel, uint8_t *voiceGainTx, uint16_t *deviation);

extern RadioDevice_t currentRadioDeviceId;


typedef struct
{
	volatile uint32_t trxDMRModeRx;
	uint32_t currentRxFrequency;
	uint32_t currentTxFrequency;
	uint32_t lastSetTxFrequency;
	uint32_t trxCurrentBand[2];
	uint32_t trxDMRModeTx;
	uint32_t currentMode;
	volatile uint8_t trxRxSignal;
	volatile uint8_t trxRxNoise;
	uint8_t txPowerLevel;
	uint8_t lastSetTxPowerLevel;
	bool currentBandWidthIs25kHz;
	bool analogSignalReceived;
	bool analogTriggeredAudio;
	bool digitalSignalReceived;
} TRXDevice_t;

extern TRXDevice_t *currentRadioDevice;
extern TRXDevice_t radioDevices[RADIO_DEVICE_MAX];

#endif
