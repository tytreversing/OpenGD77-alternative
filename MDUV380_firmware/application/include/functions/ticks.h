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
#ifndef _OPENGD77_TICKS_H_
#define _OPENGD77_TICKS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>

typedef void (*timerCallback_t)(void);

typedef struct
{
	uint32_t start;
	uint32_t timeout;
} ticksTimer_t;

extern uint32_t ticksGetMillis(void);
bool addTimerCallback(timerCallback_t funPtr, uint32_t delayIn_mS, int menuDest, bool updateExistingCallbackTime);
bool cancelTimerCallback(timerCallback_t funPtr, int menuDest);
void handleTimerCallbacks(void);

void ticksTimerReset(ticksTimer_t *timer);
void ticksTimerStart(ticksTimer_t *timer, uint32_t timeout);
bool ticksTimerHasExpired(ticksTimer_t *timer);
uint32_t ticksTimerRemaining(ticksTimer_t *timer);
bool ticksTimerIsEnabled(ticksTimer_t *timer);

#endif /* _OPENGD77_TICKS_H_ */
