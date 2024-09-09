/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
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

#ifndef _OPENGD77_SOUND_H_
#define _OPENGD77_SOUND_H_

#include <FreeRTOS.h>
#include <task.h>
#include "main.h"
#include "interfaces/wdog.h"

extern Task_t beepTask;

extern int16_t melody_generic[512];
extern const int16_t MELODY_POWER_ON[];
extern const int16_t MELODY_PRIVATE_CALL[];
extern const int16_t MELODY_KEY_BEEP[];
extern const int16_t MELODY_KEY_LONG_BEEP[];
extern const int16_t MELODY_ACK_BEEP[];
extern const int16_t MELODY_NACK_BEEP[];
extern const int16_t MELODY_ERROR_BEEP[];
extern const int16_t MELODY_TX_TIMEOUT_BEEP[];
extern const int16_t MELODY_DMR_TX_START_BEEP[];
extern const int16_t MELODY_DMR_TX_STOP_BEEP[];
extern const int16_t MELODY_KEY_BEEP_FIRST_ITEM[];
extern const int16_t MELODY_LOW_BATTERY[];
extern const int16_t MELODY_APO_TRIGGERED[];
extern const int16_t MELODY_QUICKKEYS_CLEAR_ACK_BEEP[];
extern const int16_t MELODY_RX_TGTSCC_WARNING_BEEP[];
extern const int16_t MELODY_RX_BEEP_BEGIN_BEEP[];
extern const int16_t MELODY_RX_BEEP_END_BEEP[];
extern const int16_t MELODY_RX_BEEP_CALLER_BEGIN_BEEP[];
extern const int16_t MELODY_RX_BEEP_CALLER_END_BEEP[];
extern const int16_t MELODY_1750[];
extern const int16_t MELODY_DTMF[];
extern const int16_t MELODY_SK2_BEEP[];
extern const int16_t MELODY_NSK2_BEEP[];

extern volatile int16_t *melody_play;
extern volatile int16_t melody_idx;
extern volatile int micAudioSamplesTotal;
extern int soundBeepVolumeDivider;
extern volatile float dmrRxAGCrxPeakAverage;

#define WAV_BUFFER_SIZE                          160
#define WAV_BUFFER_COUNT                          30 // 5 DMR frames, was 24
#define WAV_BUFFER_AMBE_PREBUFFERING_COUNT        12 // 2 DMR frames, 6 buffers each
#define HOTSPOT_BUFFER_SIZE                      50U
#define HOTSPOT_BUFFER_COUNT                     48U

#define DMR_RX_AGC_DEFAULT_PEAK_SAMPLES			2000.0f

extern union sharedDataBuffer
{
	volatile uint8_t wavbuffer[WAV_BUFFER_COUNT][WAV_BUFFER_SIZE]; // 3840
	volatile uint8_t hotspotBuffer[HOTSPOT_BUFFER_COUNT][HOTSPOT_BUFFER_SIZE]; // 2400
	volatile uint8_t rawBuffer[HOTSPOT_BUFFER_COUNT * HOTSPOT_BUFFER_SIZE]; // 2400
} audioAndHotspotDataBuffer;

extern volatile int16_t wavbuffer_read_idx;
extern volatile int16_t wavbuffer_write_idx;
extern volatile int16_t wavbuffer_count;
extern volatile uint8_t *currentWaveBuffer;


void soundInit(void);
void soundTerminateSound(void);
void soundSetMelody(const int16_t *melody);
void soundCreateSong(const uint8_t *melody);
void soundInitBeepTask(void);

void soundSendData(void);

void soundStoreBuffer(void);
void soundRetrieveBuffer(void);
void soundTickRXBuffer(void);
void soundSetupBuffer(void);
void soundStopMelody(void);

void soundTickMelody(void);

bool soundRefillData(uint32_t bufNum);
bool soundReceiveData(void);
void soundReceiveRefillData(uint32_t bufNum);
bool soundMelodyIsPlaying(void);
void soundStartDMA(void);

//bit masks to track amp usage
#define AUDIO_AMP_MODE_NONE 	0
#define AUDIO_AMP_MODE_BEEP 	(1 << 0)
#define AUDIO_AMP_MODE_RF 		(1 << 1)
#define AUDIO_AMP_MODE_PROMPT 	(1 << 2)


uint8_t getAudioAmpStatus(void);
void enableAudioAmp(uint8_t mode);
void disableAudioAmp(uint8_t mode);

#endif /* _OPENGD77_SOUND_H_ */
