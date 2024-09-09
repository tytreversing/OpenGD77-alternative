/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
 * Copyright (C) 2019-2020 Alex, DL4LEX
 * Copyright (C) 2019-2024 Roger Clark, VK3KYY / G4KYF
 *                         Daniel Caujolle-Bert, F1RMB
 *                         Colin Durbridge, G4EML
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
#ifndef _OPENGD77_KEYBOARD_H_
#define _OPENGD77_KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>
#include "interfaces/gpio.h"
#include "main.h"


#define KEY_GREENSTAR   '+'    // GREEN + STAR

#define KEY_NONE         0
#define KEY_UP           1
#define KEY_DOWN         2
#define KEY_LEFT         3
#define KEY_RIGHT        4

#if defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
#define KEY_VFO_MR       5
#define KEY_A_B          6
#endif

// Radios which have a non-absolute rotary encoder
#define KEY_ROTARY_DECREMENT        7
#define KEY_ROTARY_INCREMENT        8
#define KEY_FRONT_UP				9
#define KEY_FRONT_DOWN              10

#define KEY_GREEN       13
#define KEY_RED         27
#define KEY_POWER       26
#define KEY_ORANGE      28
#define KEY_0           '0'
#define KEY_1           '1'
#define KEY_2           '2'
#define KEY_3           '3'
#define KEY_4           '4'
#define KEY_5           '5'
#define KEY_6           '6'
#define KEY_7           '7'
#define KEY_8           '8'
#define KEY_9           '9'
#define KEY_A           'A'
#define KEY_B           'B'
#define KEY_C           'C'
#define KEY_D           'D'
#define KEY_STAR        '*'
#define KEY_HASH        '#'


#define KEY_MOD_DOWN    0x01
#define KEY_MOD_UP      0x02
#define KEY_MOD_LONG    0x04
#define KEY_MOD_PRESS   0x08
#define KEY_MOD_PREVIEW 0x10

#define EVENT_KEY_NONE   0
#define EVENT_KEY_CHANGE 1

#define KEY_DEBOUNCE_COUNTER   20

#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#if defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#define KEY_INCREASE KEY_RIGHT
#define KEY_DECREASE KEY_LEFT
#else
#define KEY_INCREASE KEY_FRONT_UP
#define KEY_DECREASE KEY_FRONT_DOWN
#endif
#elif defined(PLATFORM_MD9600)
#define KEY_INCREASE KEY_RIGHT
#define KEY_DECREASE KEY_LEFT
#else
#define KEY_INCREASE KEY_RIGHT
#define KEY_DECREASE KEY_LEFT
#endif

//#define KEYCHECK(keys,k) (((keys) & 0xffffff) == (k))
//#define KEYCHECK_KEYMOD(keys, k, mask, mod) (((((keys) & 0xffffff) == (k)) && ((keys) & (mask)) == (mod)))
//#define KEYCHECK_MOD(keys, mask, mod) (((keys) & (mask)) == (mod))

#define KEYCHECK_UP(keys, k)              ((keys.key == k) && (keys.event & KEY_MOD_UP))
#define KEYCHECK_SHORTUP(keys, k)         ((keys.key == k) && ((keys.event & (KEY_MOD_UP | KEY_MOD_LONG)) == KEY_MOD_UP))
#define KEYCHECK_DOWN(keys, k)            ((keys.key == k) && (keys.event & KEY_MOD_DOWN))
#define KEYCHECK_PRESS(keys, k)           ((keys.key == k) && (keys.event & KEY_MOD_PRESS))
#define KEYCHECK_LONGDOWN(keys, k)        ((keys.key == k) && ((keys.event & (KEY_MOD_DOWN | KEY_MOD_LONG)) == (KEY_MOD_DOWN | KEY_MOD_LONG)))
#define KEYCHECK_LONGDOWN_REPEAT(keys, k) ((keys.key == k) && ((keys.event & (KEY_MOD_PRESS | KEY_MOD_LONG)) == (KEY_MOD_PRESS | KEY_MOD_LONG)))

#define KEYCHECK_SHORTUP_NUMBER(keys)      ((keys.key >='0' && keys.key <='9') && ((keys.event & (KEY_MOD_UP | KEY_MOD_LONG)) == KEY_MOD_UP))
#define KEYCHECK_PRESS_NUMBER(keys)        ((keys.key >='0' && keys.key <='9') && (keys.event & KEY_MOD_PRESS))
#define KEYCHECK_LONGDOWN_NUMBER(keys)     ((keys.key >='0' && keys.key <='9') && ((keys.event & (KEY_MOD_DOWN | KEY_MOD_LONG)) == (KEY_MOD_DOWN | KEY_MOD_LONG)))

#define EVENTCHECK_SHORTUP(keys)		   ((keys.event & (KEY_MOD_UP | KEY_MOD_LONG)) == KEY_MOD_UP)

extern volatile bool keypadLocked;
extern volatile bool keypadAlphaEnable;

typedef struct keyboardCode
{
		uint8_t event;
		char key;
} keyboardCode_t;

#define NO_KEYCODE  { .event = 0, .key = 0 }


typedef struct
{
	GPIO_PinState lastB;
	int32_t       Count;
	int8_t        Direction;
} rotaryData_t;
extern volatile rotaryData_t rotaryData;


void keyboardInit(void);
void keyboardReset(void);
uint32_t keyboardRead(void);
bool keyboardKeyIsDTMFKey(char key);
void keyboardCheckKeyEvent(keyboardCode_t *keys, int *event);
bool keyboardScanKey(uint32_t scancode, char *keycode);

void rotaryEncoderISR(void);




#endif /* _OPENGD77_KEYBOARD_H_ */
