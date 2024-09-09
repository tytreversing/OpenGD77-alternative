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

#ifndef _OPENGD77_BUTTONS_H_
#define _OPENGD77_BUTTONS_H_

#include "interfaces/gpio.h"
#if defined(PLATFORM_MD9600)
extern volatile bool   PTTLocked;
#endif

#define BUTTON_NONE                    0x00000000
#define BUTTON_PTT                     0x00000001
#define BUTTON_SK1                     0x00000002
#define BUTTON_SK1_SHORT_UP            0x00000004
#define BUTTON_SK1_LONG_DOWN           0x00000008
#define BUTTON_SK1_EXTRA_LONG_DOWN     0x00000010
#define BUTTON_SK2                     0x00000020
#define BUTTON_SK2_SHORT_UP            0x00000040
#define BUTTON_SK2_LONG_DOWN           0x00000080
#define BUTTON_SK2_EXTRA_LONG_DOWN     0x00000100
#if ! defined(PLATFORM_RD5R)
#define BUTTON_ORANGE                  0x00000200
#define BUTTON_ORANGE_SHORT_UP         0x00000400
#define BUTTON_ORANGE_LONG_DOWN        0x00000800
#define BUTTON_ORANGE_EXTRA_LONG_DOWN  0x00001000
#endif // ! PLATFORM_RD5R
#define BUTTON_WAIT_NEW_STATE          0x00002000

#define EVENT_BUTTON_NONE   0
#define EVENT_BUTTON_CHANGE 1

extern volatile bool PTTLocked;

void buttonsInit(void);
#if defined(PLATFORM_MD9600)
void buttonsCheckButtonsEvent(uint32_t *buttons, int *event, bool keyIsDown, bool sk2IsLatched, uint16_t *frontPanelButtons);
#else
uint32_t buttonsRead(void);
void buttonsCheckButtonsEvent(uint32_t *buttons, int *event, bool keyIsDown);
#endif

#endif /* _OPENGD77_BUTTONS_H_ */
