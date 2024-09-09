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
#include "dmr_codec/codec.h"
#include "functions/sound.h"
#include "functions/voicePrompts.h"
#include "functions/settings.h"
#include "user_interface/uiLocalisation.h"
#include "functions/rxPowerSaving.h"
#include "hardware/radioHardwareInterface.h"

const uint32_t VOICE_PROMPTS_DATA_MAGIC = 0x5056;//'VP'
const uint32_t VOICE_PROMPTS_DATA_VERSION =
#if defined(HAS_COLOURS)
											0x0009; // Version 9 TOC increased to 368 (10 slots free)
													// Version 7 TOC increased to 350.
#define VOICE_PROMPTS_TOC_SIZE 				368
#else
											0x0008; // Version 8 TOC increased to 331 (10 slots free)
													// Version 6 TOC increased to 320. Added PROMOT_VOX and PROMPT_UNUSED_1 to PROMPT_UNUSED_10
#define VOICE_PROMPTS_TOC_SIZE 				331
#endif
													// Version 5 TOC increased to 300
													// Version 4 does not have unused items
                                                    // Version 3 does not have the PROMPT_TBD items in it

typedef struct
{
	uint32_t magic;
	uint32_t version;
} VoicePromptsDataHeader_t;

const uint32_t VOICE_PROMPTS_FLASH_HEADER_ADDRESS     = 0x8F400 + FLASH_ADDRESS_OFFSET;
const uint32_t VOICE_PROMPTS_FLASH_OLD_HEADER_ADDRESS = 0xE0000 + FLASH_ADDRESS_OFFSET;
static uint32_t voicePromptsFlashDataAddress;// = VOICE_PROMPTS_FLASH_HEADER_ADDRESS + sizeof(VoicePromptsDataHeader_t) + sizeof(uint32_t)*VOICE_PROMPTS_TOC_SIZE ;
// 76 x 27 byte ambe frames
#define AMBE_DATA_BUFFER_SIZE  2052
bool voicePromptDataIsLoaded = false;
static volatile bool voicePromptIsActive = false; // used within ISR
static int promptDataPosition = -1;
static int currentPromptLength = -1;

#define PROMPT_TAIL  30
static volatile uint32_t promptTail = 0; // used within ISR

static __attribute__((section(".ccmram"))) uint8_t ambeData[AMBE_DATA_BUFFER_SIZE];

#define VOICE_PROMPTS_SEQUENCE_BUFFER_SIZE 128

typedef struct
{
	uint16_t  Buffer[VOICE_PROMPTS_SEQUENCE_BUFFER_SIZE];
	uint32_t  Pos;
	uint32_t  Length;
} VoicePromptsSequence_t;

static __attribute__((section(".ccmram"))) VoicePromptsSequence_t voicePromptsCurrentSequence =
{
	.Pos = 0,
	.Length = 0
};

static __attribute__((section(".ccmram"))) uint32_t tableOfContents[VOICE_PROMPTS_TOC_SIZE];

static bool temporaryOverride = false;

void voicePromptsCacheInit(void)
{
	VoicePromptsDataHeader_t header;

	SPI_Flash_read(VOICE_PROMPTS_FLASH_HEADER_ADDRESS, (uint8_t *)&header, sizeof(VoicePromptsDataHeader_t));

	if (voicePromptsCheckMagicAndVersion((uint32_t *)&header))
	{
		voicePromptDataIsLoaded = SPI_Flash_read(VOICE_PROMPTS_FLASH_HEADER_ADDRESS + sizeof(VoicePromptsDataHeader_t), (uint8_t *)&tableOfContents, sizeof(uint32_t) * VOICE_PROMPTS_TOC_SIZE);
		voicePromptsFlashDataAddress = VOICE_PROMPTS_FLASH_HEADER_ADDRESS + sizeof(VoicePromptsDataHeader_t) + (sizeof(uint32_t) * VOICE_PROMPTS_TOC_SIZE);
	}

	if (!voicePromptDataIsLoaded)
	{
		SPI_Flash_read(VOICE_PROMPTS_FLASH_OLD_HEADER_ADDRESS, (uint8_t *)&header, sizeof(VoicePromptsDataHeader_t));
		if (voicePromptsCheckMagicAndVersion((uint32_t *)&header))
		{
			voicePromptDataIsLoaded = SPI_Flash_read(VOICE_PROMPTS_FLASH_OLD_HEADER_ADDRESS + sizeof(VoicePromptsDataHeader_t), (uint8_t *)&tableOfContents, sizeof(uint32_t) * VOICE_PROMPTS_TOC_SIZE);
			voicePromptsFlashDataAddress = VOICE_PROMPTS_FLASH_OLD_HEADER_ADDRESS + sizeof(VoicePromptsDataHeader_t) + (sizeof(uint32_t) * VOICE_PROMPTS_TOC_SIZE);
		}
	}

	// DMR (digital) is disabled.
	if (uiDataGlobal.dmrDisabled)
	{
		voicePromptDataIsLoaded = false;
	}

	// is data is not loaded change prompt mode back to beep.
	if ((nonVolatileSettings.audioPromptMode >= AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && (voicePromptDataIsLoaded == false))
	{
		settingsSet(nonVolatileSettings.audioPromptMode, AUDIO_PROMPT_MODE_BEEP);
	}
}

bool voicePromptsCheckMagicAndVersion(uint32_t *bufferAddress)
{
	VoicePromptsDataHeader_t *header = (VoicePromptsDataHeader_t *)bufferAddress;

	return ((header->magic == VOICE_PROMPTS_DATA_MAGIC) && (header->version == VOICE_PROMPTS_DATA_VERSION));
}

static void getAmbeData(int offset, int length)
{
	if (length <= AMBE_DATA_BUFFER_SIZE)
	{
		SPI_Flash_read(voicePromptsFlashDataAddress + offset, (uint8_t *)&ambeData, length);
	}
}

static void voicePromptsTerminateOptionalTail(bool withTail)
{
	if (voicePromptIsActive)
	{
		disableAudioAmp(AUDIO_AMP_MODE_PROMPT);

		voicePromptsCurrentSequence.Pos = 0;
		promptTail = (withTail ? PROMPT_TAIL : 0);

		taskENTER_CRITICAL();
		soundTerminateSound();
		codecInit(true);
		voicePromptIsActive = false;
		temporaryOverride = false;
		taskEXIT_CRITICAL();
	}
}

void voicePromptsTick(void)
{
	if (voicePromptIsActive)
	{
		if (promptDataPosition < currentPromptLength)
		{
			taskENTER_CRITICAL();
			if (wavbuffer_count <= WAV_BUFFER_AMBE_PREBUFFERING_COUNT)
			{
				codecDecode((uint8_t *)&ambeData[promptDataPosition], 3);
				promptDataPosition += AMBE_AUDIO_LENGTH;
			}

			soundTickRXBuffer();
			taskEXIT_CRITICAL();
		}
		else
		{
			if (voicePromptsCurrentSequence.Pos < (voicePromptsCurrentSequence.Length - 1))
			{
				voicePromptsCurrentSequence.Pos++;
				promptDataPosition = 0;

				int promptNumber = voicePromptsCurrentSequence.Buffer[voicePromptsCurrentSequence.Pos];

				if ((tableOfContents[promptNumber + 1] == 0) || (tableOfContents[promptNumber] == 0))
				{
					promptNumber = PROMPT_SILENCE;
				}

				currentPromptLength = tableOfContents[promptNumber + 1] - tableOfContents[promptNumber];
				getAmbeData(tableOfContents[promptNumber], currentPromptLength);
			}
			else
			{
				// wait for wave buffer to empty when prompt has finished playing

				if (wavbuffer_count == 0)
				{
					voicePromptsTerminateOptionalTail(true);
				}
			}
		}
	}
	else
	{
		if (promptTail > 0)
		{
			promptTail--;

			if ((promptTail == 0) && trxCarrierDetected(RADIO_DEVICE_PRIMARY) && (trxGetMode() == RADIO_MODE_ANALOG))
			{
				radioSetAudioPath(true);					// Set the audio path to FM -> audio amp.
			}
		}
	}
}

void voicePromptsTerminate(void)
{
	voicePromptsTerminateOptionalTail(true);
}

void voicePromptsTerminateNoTail(void)
{
	voicePromptsTerminateOptionalTail(false);
}

void voicePromptsInit(void)
{
	if ((nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && !temporaryOverride)
	{
		return;
	}

	if (voicePromptIsActive)
	{
		voicePromptsTerminateOptionalTail(false);
	}

	voicePromptsCurrentSequence.Length = 0;
	voicePromptsCurrentSequence.Pos = 0;
}

void voicePromptsInitWithOverride(void)
{
	temporaryOverride = voicePromptDataIsLoaded;// only allow override if prompts are actually loaded
	voicePromptsInit();
}

void voicePromptsAppendPrompt(voicePrompt_t prompt)
{
	if ((nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && !temporaryOverride)
	{
		return;
	}

	if (voicePromptIsActive)
	{
		voicePromptsInit();
	}

	if (voicePromptsCurrentSequence.Length < VOICE_PROMPTS_SEQUENCE_BUFFER_SIZE)
	{
		voicePromptsCurrentSequence.Buffer[voicePromptsCurrentSequence.Length] = prompt;
		voicePromptsCurrentSequence.Length++;
	}
}

void voicePromptsAppendString(const char *promptString)
{
	if ((nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && !temporaryOverride)
	{
		return;
	}

	if (voicePromptIsActive)
	{
		voicePromptsInit();
	}

	while (*promptString != 0)
	{
		if ((*promptString >= '0') && (*promptString <= '9'))
		{
			voicePromptsAppendPrompt(*promptString - '0' + PROMPT_0);
		}
		else if ((*promptString >= 'A') && (*promptString <= 'Z'))
		{
			voicePromptsAppendPrompt(*promptString - 'A' + PROMPT_A);
		}
		else if ((*promptString >= 'a') && (*promptString <= 'z'))
		{
			voicePromptsAppendPrompt(*promptString - 'a' + PROMPT_A);
		}
		else if (*promptString == '.')
		{
			voicePromptsAppendPrompt(PROMPT_POINT);
		}
		else if (*promptString == '+')
		{
			voicePromptsAppendPrompt(PROMPT_PLUS);
		}
		else if (*promptString == '-')
		{
			voicePromptsAppendPrompt(PROMPT_MINUS);
		}
		else if (*promptString == '%')
		{
			voicePromptsAppendPrompt(PROMPT_PERCENT);
		}
		else if (*promptString == '*')
		{
			voicePromptsAppendPrompt(PROMPT_STAR);
		}
		else if (*promptString == '#')
		{
			voicePromptsAppendPrompt(PROMPT_HASH);
		}
		else if (*promptString == 176)
		{
			voicePromptsAppendPrompt(PROMPT_DEGREES);
		}
		else
		{
			// otherwise just add silence
			voicePromptsAppendPrompt(PROMPT_SILENCE);
		}

		promptString++;
	}
}

void voicePromptsAppendInteger(int32_t value)
{
	char buf[12] = {0}; // min: -2147483648, max: 2147483647
	itoa(value, buf, 10);
	voicePromptsAppendString(buf);
}

void voicePromptsAppendLanguageString(const char *languageStringAdd)
{
	if (((nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && !temporaryOverride) || (languageStringAdd == NULL))
	{
		return;
	}

	voicePromptsAppendPrompt(NUM_VOICE_PROMPTS +
			((languageStringAdd - currentLanguage->LANGUAGE_NAME)
#if ! defined(HAS_COLOURS)
					- ((languageStringAdd >= currentLanguage->theme_chooser) ?
							((currentLanguage->theme_colour_picker_blue - currentLanguage->theme_chooser) + LANGUAGE_TEXTS_LENGTH) : 0)
#endif
			) / LANGUAGE_TEXTS_LENGTH);
}

void voicePromptsPlay(void)
{
	if ((nonVolatileSettings.audioPromptMode < AUDIO_PROMPT_MODE_VOICE_THRESHOLD) && !temporaryOverride)
	{
		return;
	}

	if ((voicePromptIsActive == false) && (voicePromptsCurrentSequence.Length > 0))
	{
		rxPowerSavingSetState(ECOPHASE_POWERSAVE_INACTIVE);

		if (nonVolatileSettings.DMR_RxAGC != 0)
		{
			HRC6000SetDmrAGCGain(0);
		}

		taskENTER_CRITICAL();

		// Early state change to lock out HR-C6000.
		voicePromptIsActive = true;// Start the playback
		if (melody_play && (melody_play != MELODY_ERROR_BEEP) && (melody_play != MELODY_ACK_BEEP))
		{
			soundStopMelody();
		}

		int promptNumber = voicePromptsCurrentSequence.Buffer[0];

		voicePromptsCurrentSequence.Pos = 0;

		if ((tableOfContents[promptNumber + 1] == 0) || (tableOfContents[promptNumber] == 0))
		{
			promptNumber = PROMPT_SILENCE;
		}

		currentPromptLength = tableOfContents[promptNumber + 1] - tableOfContents[promptNumber];
		getAmbeData(tableOfContents[promptNumber], currentPromptLength);

		radioSetAudioPath(false);			// set the audio path to HR-C6000 -> audio amp (Actually this is always on, this call just disables the FM audio)
		enableAudioAmp(AUDIO_AMP_MODE_PROMPT);

		codecInit(true);
		promptDataPosition = 0;
		promptTail = 0;

		taskEXIT_CRITICAL();
	}
}

inline bool voicePromptsIsPlaying(void)
{
	return (voicePromptIsActive || (promptTail > 0));
}

bool voicePromptsHasDataToPlay(void)
{
	return (voicePromptsCurrentSequence.Length > 0);
}

bool voicePromptsDoesItContainPrompt(voicePrompt_t prompt)
{
	if (voicePromptsCurrentSequence.Length > 0)
	{
		return (memchr(&voicePromptsCurrentSequence.Buffer[0], (int)prompt, voicePromptsCurrentSequence.Length) != NULL);
	}
	return false;
}
