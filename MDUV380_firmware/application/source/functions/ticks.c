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

#include <string.h>
#include "functions/ticks.h"
#include "user_interface/menuSystem.h"

#define PIT_COUNTS_PER_MS  1U

#if !(defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017))
extern volatile uint32_t PITCounter; // 1ms granularity
#endif

typedef struct
{
	timerCallback_t  funPtr;
	int              menuDestination;
	ticksTimer_t     PIT_TriggerTimer;
} timerCallbackbackStruct_t;

#define MAX_NUM_TIMER_CALLBACKS 8
static timerCallbackbackStruct_t callbacksArray[MAX_NUM_TIMER_CALLBACKS];// As a global this will get cleared by the compiler

inline uint32_t ticksGetMillis(void)
{
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
	return uwTick;
#else
	return PITCounter;
#endif
}

void handleTimerCallbacks(void)
{
	int i = 0;

	while((callbacksArray[i].funPtr != NULL) && (i < MAX_NUM_TIMER_CALLBACKS))
	{
		timerCallback_t cbFunction = NULL;

		if (ticksTimerHasExpired(&callbacksArray[i].PIT_TriggerTimer))
		{
			// Does the current menu matches the desired destination menu
			if ((callbacksArray[i].menuDestination == MENU_ANY) || (callbacksArray[i].menuDestination == menuSystemGetCurrentMenuNumber()))
			{
				// Postpone the call to callback function, as it could add/delete/update a TimerCallback in its code.
				cbFunction = callbacksArray[i].funPtr;
			}

			if (i != (MAX_NUM_TIMER_CALLBACKS - 1))
			{
				memmove(&callbacksArray[i], &callbacksArray[i + 1], ((MAX_NUM_TIMER_CALLBACKS - 1) - i) * sizeof(timerCallbackbackStruct_t));
			}

			callbacksArray[MAX_NUM_TIMER_CALLBACKS - 1].funPtr = NULL;
		}
		else
		{
			i++;
		}

		if (cbFunction != NULL)
		{
			cbFunction();
			i = 0; // Restart from the beginning of the array
		}
	}
}

bool addTimerCallback(timerCallback_t funPtr, uint32_t delayIn_mS, int menuDest, bool updateExistingCallbackTime)
{
	uint32_t callBackTime =
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			delayIn_mS;
#else
			(delayIn_mS * PIT_COUNTS_PER_MS);
#endif

	for(int i = 0; i < MAX_NUM_TIMER_CALLBACKS; i++)
	{
		if (callbacksArray[i].funPtr == NULL)
		{
			callbacksArray[i].funPtr = funPtr;
			callbacksArray[i].menuDestination = menuDest;
			ticksTimerStart(&callbacksArray[i].PIT_TriggerTimer, callBackTime);
			return true;
		}

		if ((callbacksArray[i].funPtr == funPtr) && updateExistingCallbackTime)
		{
			callbacksArray[i].menuDestination = menuDest;
			ticksTimerStart(&callbacksArray[i].PIT_TriggerTimer, callBackTime);
			return true;
		}

		// callbacksArray[i] must be non-null pointer
		if (ticksTimerHasExpired(&callbacksArray[i].PIT_TriggerTimer))
		{
			if (i != (MAX_NUM_TIMER_CALLBACKS - 1))
			{
				// shuffle all other callbacks down in the list if there is space
				memmove(&callbacksArray[i+1], &callbacksArray[i], ((MAX_NUM_TIMER_CALLBACKS - 1) - i) * sizeof(timerCallbackbackStruct_t));
			}
			callbacksArray[i].funPtr = funPtr;
			callbacksArray[i].menuDestination = menuDest;
			ticksTimerStart(&callbacksArray[i].PIT_TriggerTimer, callBackTime);
			return true;
		}
	}
	return false;
}

bool cancelTimerCallback(timerCallback_t funPtr, int menuDest)
{
	for(int i = 0; i < MAX_NUM_TIMER_CALLBACKS; i++)
	{
		if ((callbacksArray[i].funPtr == funPtr) && (callbacksArray[i].menuDestination == menuDest))
		{
			if (i != (MAX_NUM_TIMER_CALLBACKS - 1))
			{
				memmove(&callbacksArray[i], &callbacksArray[i + 1], ((MAX_NUM_TIMER_CALLBACKS - 1) - i) * sizeof(timerCallbackbackStruct_t));
			}
			callbacksArray[MAX_NUM_TIMER_CALLBACKS - 1].funPtr = NULL;
			return true;
		}
	}
	return false;
}

void ticksTimerReset(ticksTimer_t *timer)
{
	timer->start = 0;
	timer->timeout = 0;
}

void ticksTimerStart(ticksTimer_t *timer, uint32_t timeout)
{
	timer->start = ticksGetMillis();
	timer->timeout = timeout;
}

bool ticksTimerHasExpired(ticksTimer_t *timer)
{
	return ((ticksGetMillis() - timer->start) >= timer->timeout);
}

uint32_t ticksTimerRemaining(ticksTimer_t *timer)
{
	if (timer->timeout > 0)
	{
		uint32_t elapsed = (ticksGetMillis() - timer->start);
		return ((elapsed >= timer->timeout) ? 0 : timer->timeout - elapsed);
	}

	return 0;
}

bool ticksTimerIsEnabled(ticksTimer_t *timer)
{
	return (timer->start && timer->timeout);
}
