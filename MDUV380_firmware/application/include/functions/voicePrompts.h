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
#ifndef _OPENGD77_VOICEPROMPTS_H_
#define _OPENGD77_VOICEPROMPTS_H_

typedef enum
{
	PROMPT_SILENCE = 0,
	PROMPT_POINT,
	PROMPT_0, PROMPT_1,	PROMPT_2, PROMPT_3, PROMPT_4, PROMPT_5, PROMPT_6, PROMPT_7, PROMPT_8, PROMPT_9,
	PROMPT_A, PROMPT_B, PROMPT_C, PROMPT_D, PROMPT_E, PROMPT_F, PROMPT_G, PROMPT_H, PROMPT_I, PROMPT_J,
	PROMPT_K, PROMPT_L, PROMPT_M, PROMPT_N, PROMPT_O, PROMPT_P, PROMPT_Q, PROMPT_R, PROMPT_S, PROMPT_T,
	PROMPT_U, PROMPT_V, PROMPT_W, PROMPT_X, PROMPT_Y, PROMPT_Z, PROMPT_Z2,
	PROMPT_CHANNEL,
	PROMPT_CONTACT,
	PROMPT_MILLISECONDS,
	PROMPT_MEGAHERTZ,
	PROMPT_KILOHERTZ,
	PROMPT_TALKGROUP,
	PROMPT_DEGREES,
	PROMPT_VFO,
	PROMPT_SECONDS,
	PROMPT_MINUTES,
	PROMPT_VOLTS,
	PROMPT_WATT,
	PROMPT_WATTS,
	PROMPT_MILLIWATTS,
	PROMPT_PERCENT,
	PROMPT_RECEIVE,
	PROMPT_TRANSMIT,
	PROMPT_MODE,
	PROMPT_DMR,
	PROMPT_FM,
	PROMPT_PLUS,
	PROMPT_MINUS,
	PROMPT_VFO_EXCHANGE_TX_RX,
	PROMPT_VFO_COPY_RX_TO_TX,
	PROMPT_VFO_COPY_TX_TO_RX,
	PROMPT_POWER,
	PROMPT_CHANNEL_MODE,
	PROMPT_SCAN_MODE,
	PROMPT_TIMESLOT_MODE,
	PROMPT_FILTER_MODE,
	PROMPT_ZONE_MODE,
	PROMPT_POWER_MODE,
	PROMPT_COLORCODE_MODE,
	PROMPT_HERTZ,
	PROMPT_SETTINGS_UPDATE,
	PROMPT_STAR,
	PROMPT_HASH,
	PROMPT_VOX,
	PROMPT_SWEEP_SCAN_MODE,
	PROMPT_ECO_MODE,
	PROMPT_DURATION,
	PROMPT_REVERSE_REPEATER,
	PROMPT_DISTANCE,
	PROMPT_KMPH,
	PROMPT_KILOMETERS,
	PROMPT_UNUSED_8,
	PROMPT_UNUSED_9,
	PROMPT_UNUSED_10,
	NUM_VOICE_PROMPTS,
	__MAKE_ENUM_16BITS = INT16_MAX
} voicePrompt_t;


extern bool voicePromptDataIsLoaded;
extern const uint32_t VOICE_PROMPTS_FLASH_HEADER_ADDRESS;
extern const uint32_t VOICE_PROMPTS_FLASH_OLD_HEADER_ADDRESS;

void voicePromptsCacheInit(void);
void voicePromptsTick(void);// Called from HR-C6000.c

void voicePromptsInit(void);// Call before building the prompt sequence
void voicePromptsAppendPrompt(voicePrompt_t prompt);// Append an individual prompt item. This can be a single letter number or a phrase
void voicePromptsAppendString(const char *);// Append a text string e.g. "VK3KYY"
void voicePromptsAppendInteger(int32_t value); // Append a signed integer
void voicePromptsAppendLanguageString(const char *);//Append a text from the current language e.g. &currentLanguage->battery
void voicePromptsPlay(void);// Starts prompt playback
extern bool voicePromptsIsPlaying(void);
bool voicePromptsHasDataToPlay(void);
bool voicePromptsDoesItContainPrompt(voicePrompt_t prompt);
void voicePromptsTerminate(void);
void voicePromptsTerminateNoTail(void);
bool voicePromptsCheckMagicAndVersion(uint32_t *bufferAddress);

void voicePromptsInitWithOverride(void);

#endif
