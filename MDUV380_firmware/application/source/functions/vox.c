/*
 * Copyright (C) 2020-2024 Roger Clark, VK3KYY / G4KYF
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

#include "functions/vox.h"
#include "interfaces/adc.h"
#include "functions/sound.h"
#include "functions/settings.h"
#include "functions/ticks.h"
#include "interfaces/batteryAndPowerManagement.h" // for micLevel decl
#include "utils.h"

#define PIT_COUNTS_PER_MS  1U
#define VOX_UPDATE_MS 4U


static const uint32_t VOX_TAIL_TIME_UNIT = (PIT_COUNTS_PER_MS * 500U); // 500ms tail unit
static const uint16_t VOX_SETTLE_TIME = (6000U / VOX_UPDATE_MS); // Countdown before doing first real sampling;

typedef struct
{
	uint16_t     sampled;
	uint16_t     averaged;
	uint16_t     noiseFloor;
	uint16_t     sampledNoise;
	ticksTimer_t nextTimeSamplingTimer;
	ticksTimer_t preTriggeringTimer;
	ticksTimer_t tailTimer;
	uint16_t     settleCount;
	uint8_t      threshold; // threshold is a super low value, 8 bits are enough
	uint8_t      tailUnits;
	bool         triggered;
} voxData_t;

static voxData_t vox;

void voxInit(void)
{
	voxReset();
	vox.threshold = 0U;
	vox.noiseFloor = 0U;
	vox.tailUnits = 1U;
	vox.settleCount = VOX_SETTLE_TIME;
}

void voxSetParameters(uint8_t threshold, uint8_t tailHalfSecond)
{
	vox.triggered = false;
	vox.threshold = threshold;

	voxReset();
	vox.tailUnits = tailHalfSecond;
	vox.settleCount = VOX_SETTLE_TIME;
}

bool voxIsEnabled(void)
{
	return ((codeplugChannelGetFlag(currentChannelData, CHANNEL_FLAG_VOX) != 0) &&
			(codeplugChannelGetFlag(currentChannelData, CHANNEL_FLAG_RX_ONLY) == 0) && ((nonVolatileSettings.txFreqLimited == BAND_LIMITS_NONE) || trxCheckFrequencyInAmateurBand(currentChannelData->txFreq)
#if defined(PLATFORM_MD9600)
					|| (codeplugChannelGetFlag(currentChannelData, CHANNEL_FLAG_OUT_OF_BAND) != 0)
#endif
			) && (settingsUsbMode != USB_MODE_HOTSPOT));
}

bool voxIsTriggered(void)
{
	return vox.triggered;
}

void voxReset(void)
{
	vox.triggered = false;
	vox.sampled = 0U;
	vox.averaged = 0U;
	ticksTimerStart(&vox.nextTimeSamplingTimer, VOX_UPDATE_MS);
	ticksTimerReset(&vox.tailTimer);
	ticksTimerReset(&vox.preTriggeringTimer);
	vox.settleCount = (VOX_SETTLE_TIME >> 1);
	vox.sampledNoise = 0U;
}

void voxTick(void)
{
	if (voxIsEnabled())
	{
		if (ticksTimerHasExpired(&vox.nextTimeSamplingTimer))
		{
			if ((getAudioAmpStatus() & (AUDIO_AMP_MODE_RF | AUDIO_AMP_MODE_BEEP | AUDIO_AMP_MODE_PROMPT)))
			{
				voxReset();
			}
			else
			{
				uint16_t sample = micLevel; // NOTE: not using adcGetVOX(), but direct (and valid) ADC value.

				if (vox.settleCount > 0)
				{
					vox.settleCount--;
					return;
				}

				vox.sampled += sample;
				vox.averaged = (vox.sampled + (1 << (2 - 1))) >> 2;
				vox.sampled -= vox.averaged;

				if ((vox.averaged > 0) && (vox.noiseFloor > 0) && (vox.averaged >= (vox.noiseFloor + vox.threshold)))
				{
					if (ticksTimerIsEnabled(&vox.preTriggeringTimer) == false)
					{
						ticksTimerStart(&vox.preTriggeringTimer, 100U);
					}

					// We need at least 100ms of level above the noise to trigger the VOX
					if (ticksTimerHasExpired(&vox.preTriggeringTimer))
					{
						vox.triggered = true;
						ticksTimerStart(&vox.tailTimer, ((vox.tailUnits * VOX_TAIL_TIME_UNIT) + VOX_UPDATE_MS));
					}
				}
				else
				{
					// it was pre-triggered, but not long enough to trigger the vox, reset the pre-trigger timer
					if (ticksTimerIsEnabled(&vox.preTriggeringTimer) && ticksTimerHasExpired(&vox.preTriggeringTimer) && (vox.triggered == false))
					{
						ticksTimerReset(&vox.preTriggeringTimer);
					}

					// Noise is sampled all the time, when the transceiver is silent, and not XMitting
					// Except when it's pre-trigged
					if (ticksTimerIsEnabled(&vox.preTriggeringTimer) == false)
					{
						// Noise floor averaging
						vox.sampledNoise += sample;
						vox.noiseFloor = (vox.sampledNoise + (1 << (2 - 1))) >> 2;
						vox.sampledNoise -= vox.noiseFloor;
					}
				}
			}

			ticksTimerStart(&vox.nextTimeSamplingTimer, VOX_UPDATE_MS);
		}

		if (ticksTimerIsEnabled(&vox.tailTimer) && ticksTimerHasExpired(&vox.tailTimer))
		{
			vox.triggered = false;
			ticksTimerReset(&vox.tailTimer);
			ticksTimerReset(&vox.preTriggeringTimer);
		}
	}

	if (vox.triggered && (settingsUsbMode == USB_MODE_HOTSPOT))
	{
		voxReset();
	}
}
