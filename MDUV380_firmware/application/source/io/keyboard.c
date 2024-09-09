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
#include "io/keyboard.h"
#include "interfaces/pit.h"
#include "functions/settings.h"
#include "interfaces/gpio.h"
#include "interfaces/adc.h"
#include "io/buttons.h"

// Keyboard Keys
typedef struct
{
	GPIO_TypeDef  *GPIOPort;
	uint16_t       GPIOPin;
	uint16_t       Key;
} KeyboardKeySetting_t;

static char oldKeyboardCode;
static uint32_t keyDebounceScancode;
static int keyDebounceCounter;
static uint8_t keyState;
static char keypadAlphaKey;
static int keypadAlphaIndex;
volatile bool keypadAlphaEnable;
volatile bool keypadLocked = false;


/*
static const uint32_t keyMap[] = {
		KEY_1, KEY_2, KEY_3, KEY_GREEN,
		KEY_4, KEY_5, KEY_6, KEY_UP,
		KEY_7, KEY_8, KEY_9, KEY_DOWN,
		KEY_STAR, KEY_0, KEY_HASH, KEY_RED,
};
 */
volatile rotaryData_t rotaryData =
{
		.lastB = (GPIO_PinState)(GPIO_PIN_SET + 1), // To be sure it will be different on the first run (Hackish ? Yeah !)
		.Count = 0,
		.Direction = 0
};

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DEBOUNCE,
	KEY_PRESS,
	KEY_WAITLONG,
	KEY_REPEAT,
	KEY_WAIT_RELEASED
};


static const char keypadAlphaMap[11][31] = {
		"0 ",
		"1.!,@-:?()~/[]#<>=*+$%'`&|_^{}",
		"abc2ABC",
		"def3DEF",
		"ghi4GHI",
		"jkl5JKL",
		"mno6MNO",
		"pqrs7PQRS",
		"tuv8TUV",
		"wxyz9WXYZ",
		"*"
};

#define KEYBOARD_KEYS_PER_ROW  8U

static const struct
{
	GPIO_TypeDef         *GPIOCtrlPort;
	uint16_t              GPIOCtrlPin;
	KeyboardKeySetting_t  Rows[KEYBOARD_KEYS_PER_ROW];
} KeyboardMatrix[] =
#if defined(PLATFORM_MDUV380)
{
		{
				KEYPAD_ROW0_GPIO_Port,
				KEYPAD_ROW0_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_1    },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_2    },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_3    },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_4    },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_5    },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_6    },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_0    },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_STAR }
				}
		},
		{
				KEYPAD_ROW1_GPIO_Port,
				KEYPAD_ROW1_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_GREEN      },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_FRONT_UP   },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_FRONT_DOWN },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_7          },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_8          },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_9          },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_HASH       },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_RED        }
				}
		},
		{
				KEYPAD_ROW2_GPIO_Port,
				KEYPAD_ROW2_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_NONE },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_NONE },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_NONE },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_NONE },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_NONE },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_NONE },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_NONE },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_NONE }
				}
		}
};
#elif defined(PLATFORM_RT84_DM1701)
{
		{
				KEYPAD_ROW0_GPIO_Port,
				KEYPAD_ROW0_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_1        },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_4        },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_7        },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_STAR     },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_FRONT_UP },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_RIGHT    },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_LEFT     },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_NONE     }
				}
		},
		{
				KEYPAD_ROW1_GPIO_Port,
				KEYPAD_ROW1_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_2          },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_5          },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_8          },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_0          },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_FRONT_DOWN },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_RED        },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_GREEN      },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_NONE       }
				}
		},
		{
				KEYPAD_ROW2_GPIO_Port,
				KEYPAD_ROW2_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_3    },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_6    },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_9    },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_HASH },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_NONE },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_NONE },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_NONE },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_NONE }
				}
		}
};
#elif defined(PLATFORM_MD2017)
{
		{
				KEYPAD_ROW0_GPIO_Port,
				KEYPAD_ROW0_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_1    },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_2    },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_3    },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_4    },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_5    },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_6    },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_0    },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_STAR }
				}
		},
		{
				KEYPAD_ROW1_GPIO_Port,
				KEYPAD_ROW1_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_LEFT  },//P1
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_GREEN },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_RED   },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_7     },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_8     },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_9     },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_HASH  },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_RIGHT }//P2
				}
		},
		{
				KEYPAD_ROW2_GPIO_Port,
				KEYPAD_ROW2_Pin,
				{
						{ LCD_D0_GPIO_Port, LCD_D0_Pin, KEY_NONE       },
						{ LCD_D1_GPIO_Port, LCD_D1_Pin, KEY_NONE       },
						{ LCD_D2_GPIO_Port, LCD_D2_Pin, KEY_NONE       },
						{ LCD_D3_GPIO_Port, LCD_D3_Pin, KEY_NONE       },
						{ LCD_D4_GPIO_Port, LCD_D4_Pin, KEY_FRONT_DOWN },
						{ LCD_D5_GPIO_Port, LCD_D5_Pin, KEY_NONE       },
						{ LCD_D6_GPIO_Port, LCD_D6_Pin, KEY_FRONT_UP   },
						{ LCD_D7_GPIO_Port, LCD_D7_Pin, KEY_NONE       }
				}
		}
};
#endif


void keyboardInit(void)
{
	//gpioInitKeyboard();

	oldKeyboardCode = 0;
	keyDebounceScancode = 0;
	keyDebounceCounter = 0;
	keypadAlphaEnable = false;
	keypadAlphaIndex = 0;
	keypadAlphaKey = 0;
	keyState = KEY_IDLE;
	keypadLocked = false;
}

void keyboardReset(void)
{
	oldKeyboardCode = 0;
	keypadAlphaEnable = false;
	keypadAlphaIndex = 0;
	keypadAlphaKey = 0;
	keyState = KEY_WAIT_RELEASED;
}

uint32_t keyboardRead(void)
{
	uint32_t result = KEY_NONE;
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;

	GPIO_InitStruct.Pin = LCD_D0_Pin | LCD_D1_Pin | LCD_D2_Pin | LCD_D3_Pin;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	for (size_t i = 0; i < (sizeof(KeyboardMatrix) / sizeof(KeyboardMatrix[0])); i++)
	{
		//Set the Row Pin as Output
		GPIO_InitStruct.Pin = KeyboardMatrix[i].GPIOCtrlPin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(KeyboardMatrix[i].GPIOCtrlPort, &GPIO_InitStruct);

		//Set the row pin high to select that row of keys
		HAL_GPIO_WritePin(KeyboardMatrix[i].GPIOCtrlPort, KeyboardMatrix[i].GPIOCtrlPin, GPIO_PIN_SET);

		for(volatile int xx = 0; xx < 100; xx++); // arbitrary settling delay

		for (size_t k = 0; k < KEYBOARD_KEYS_PER_ROW; k++)
		{
			if((KeyboardMatrix[i].Rows[k].Key != KEY_NONE) &&
					(HAL_GPIO_ReadPin(KeyboardMatrix[i].Rows[k].GPIOPort, KeyboardMatrix[i].Rows[k].GPIOPin) == GPIO_PIN_SET))
			{
				result = KeyboardMatrix[i].Rows[k].Key;
				break;
			}
		}

		//set the row pin back to floating. This prevents conflicts between multiple key presses.
		GPIO_InitStruct.Pin = KeyboardMatrix[i].GPIOCtrlPin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(KeyboardMatrix[i].GPIOCtrlPort, &GPIO_InitStruct);

		// Stop on first down key (we don't support multiple key presses).
		if (result != KEY_NONE)
		{
			break;
		}
	}

	return result;
}

bool keyboardKeyIsDTMFKey(char key)
{
	switch (key)
	{
		case KEY_0 ... KEY_9:
		case KEY_STAR:
		case KEY_HASH:
		case KEY_A:
		case KEY_B:
		case KEY_C:
		case KEY_D:
			return true;
	}
	return false;
}

void rotaryEncoderISR(void)
{
	GPIO_PinState pinA = HAL_GPIO_ReadPin(ROTARY_SW_A_GPIO_Port, ROTARY_SW_A_Pin);
	GPIO_PinState pinB = HAL_GPIO_ReadPin(ROTARY_SW_B_GPIO_Port, ROTARY_SW_B_Pin);

	if (pinB != rotaryData.lastB)
	{
		rotaryData.lastB = pinB;
		rotaryData.Direction = ((pinA == pinB) ? -1 : 1);
		rotaryData.Count += rotaryData.Direction;
	}
}

void keyboardCheckKeyEvent(keyboardCode_t *keys, int *event)
{
	uint32_t scancode = 0;
	char keycode = 0;
	bool validKey;
	int newAlphaKey;
	uint32_t tmp_timer_keypad;
	uint32_t keypadTimerLong = nonVolatileSettings.keypadTimerLong * 100;
	uint32_t keypadTimerRepeat = nonVolatileSettings.keypadTimerRepeat * 100;

	*event = EVENT_KEY_NONE;
	keys->event = 0;
	keys->key = KEY_NONE;

	if (rotaryData.Direction != 0)
	{
		keys->key = (rotaryData.Direction == 1) ? KEY_ROTARY_INCREMENT : KEY_ROTARY_DECREMENT;
		keys->event = KEY_MOD_UP | KEY_MOD_PRESS; // Hack send both Up and Down events because the menus use KEY_MOD_PRESS but the VFO uses KEY_MOD_UP
		*event = EVENT_KEY_CHANGE;
		rotaryData.Direction = 0;
		return;
	}
	else
	{
		keycode = (char) keyboardRead();
		scancode = keycode;
	}

	validKey = true;

	if (keyState > KEY_DEBOUNCE && !validKey)
	{
		keyState = KEY_WAIT_RELEASED;
	}

	switch (keyState)
	{
		case KEY_IDLE:
			if (keycode != 0)
			{
				keyState = KEY_DEBOUNCE;
				keyDebounceCounter = 0;
				keyDebounceScancode = scancode;
				oldKeyboardCode = 0;
			}
			taskENTER_CRITICAL();
			tmp_timer_keypad = timer_keypad_timeout;
			taskEXIT_CRITICAL();

			if (tmp_timer_keypad == 0 && keypadAlphaKey != 0)
			{
				keys->key = keypadAlphaMap[keypadAlphaKey - 1][keypadAlphaIndex];
				keys->event = KEY_MOD_PRESS;
				*event = EVENT_KEY_CHANGE;
				keypadAlphaKey = 0;
			}
			break;
		case KEY_DEBOUNCE:
			keyDebounceCounter++;
			if (keyDebounceCounter > KEY_DEBOUNCE_COUNTER)
			{
				if (keyDebounceScancode == scancode)
				{
					oldKeyboardCode = keycode;
					keyState = KEY_PRESS;
				}
				else
				{
					keyState = KEY_WAIT_RELEASED;
				}
			}
			break;
		case KEY_PRESS:
			keys->key = keycode;
			keys->event = KEY_MOD_DOWN | KEY_MOD_PRESS;
			*event = EVENT_KEY_CHANGE;

			taskENTER_CRITICAL();
			timer_keypad = keypadTimerLong;
			timer_keypad_timeout = 1000;
			taskEXIT_CRITICAL();
			keyState = KEY_WAITLONG;

			if (keypadAlphaEnable == true)
			{
				newAlphaKey = 0;
				if ((keycode >= '0') && (keycode <= '9'))
				{
					newAlphaKey = (keycode - '0') + 1;
				}
				else if (keycode == KEY_STAR)
				{
					newAlphaKey = 11;
				}

				if (keypadAlphaKey == 0)
				{
					if (newAlphaKey != 0)
					{
						keypadAlphaKey = newAlphaKey;
						keypadAlphaIndex = 0;
					}
				}
				else
				{
					if (newAlphaKey == keypadAlphaKey)
					{
						keypadAlphaIndex++;
						if (keypadAlphaMap[keypadAlphaKey - 1][keypadAlphaIndex] == 0)
						{
							keypadAlphaIndex = 0;
						}
					}
				}

				if (keypadAlphaKey != 0)
				{
					if (newAlphaKey == keypadAlphaKey)
					{
						keys->key =	keypadAlphaMap[keypadAlphaKey - 1][keypadAlphaIndex];
						keys->event = KEY_MOD_PREVIEW;
					}
					else
					{
						keys->key = keypadAlphaMap[keypadAlphaKey - 1][keypadAlphaIndex];
						keys->event = KEY_MOD_PRESS;
						*event = EVENT_KEY_CHANGE;
						keypadAlphaKey = newAlphaKey;
						keypadAlphaIndex = -1;
						keyState = KEY_PRESS;
					}
				}
			}
			break;
		case KEY_WAITLONG:
			if (keycode == 0)
			{
				keys->key = oldKeyboardCode;
				keys->event = KEY_MOD_UP;
				*event = EVENT_KEY_CHANGE;
				keyState = KEY_IDLE;
			}
			else
			{
				taskENTER_CRITICAL();
				tmp_timer_keypad = timer_keypad;
				taskEXIT_CRITICAL();

				if (tmp_timer_keypad == 0)
				{
					taskENTER_CRITICAL();
					timer_keypad = keypadTimerRepeat;
					taskEXIT_CRITICAL();

					keys->key = keycode;
					keys->event = KEY_MOD_LONG | KEY_MOD_DOWN;
					*event = EVENT_KEY_CHANGE;
					keyState = KEY_REPEAT;
				}
			}
			break;
		case KEY_REPEAT:
			if (keycode == 0)
			{
				keys->key = oldKeyboardCode;
				keys->event = KEY_MOD_LONG | KEY_MOD_UP;
				*event = EVENT_KEY_CHANGE;

				keyState = KEY_IDLE;
			}
			else
			{
				taskENTER_CRITICAL();
				tmp_timer_keypad = timer_keypad;
				taskEXIT_CRITICAL();

				keys->key = keycode;
				keys->event = KEY_MOD_LONG;
				*event = EVENT_KEY_CHANGE;

				if (tmp_timer_keypad == 0)
				{
					taskENTER_CRITICAL();
					timer_keypad = keypadTimerRepeat;
					taskEXIT_CRITICAL();

					if ((keys->key == KEY_LEFT) || (keys->key == KEY_RIGHT)
							|| (keys->key == KEY_UP) || (keys->key == KEY_DOWN) || (keys->key == KEY_FRONT_DOWN) || (keys->key == KEY_FRONT_UP))
					{
						keys->event = (KEY_MOD_LONG | KEY_MOD_PRESS);
					}
				}
			}
			break;
		case KEY_WAIT_RELEASED:
			if (scancode == 0)
			{
				keyState = KEY_IDLE;
			}
			break;
	}
}

/*
void buttonsFrontPanelDump(void)
{
	uint16_t keys = buttonsFrontPanelRead();

	char buffer[17];
	snprintf(buffer, sizeof(buffer), "K: 0x%03X %ld", keys, rotaryData.Count);
	displayPrintCentered(40, buffer, FONT_SIZE_3);
}
 */
