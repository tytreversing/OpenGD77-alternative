/*
 * Copyright (C) 2020-2024 Roger Clark, VK3KYY / G4KYF
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
#ifndef _OPENGD77_GPIO_H_
#define _OPENGD77_GPIO_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define GPIO_PinWrite(port,pin,value)  HAL_GPIO_WritePin(port,pin,value)

/* --------------  Buttons  ---------------- */
#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S)
#define Port_PTT		PORTA
#define GPIO_PTT		GPIOA
#define Pin_PTT			1

#define Port_SK1		PORTB
#define GPIO_SK1		GPIOB
#define Pin_SK1			1

#define Port_SK2		PORTB
#define GPIO_SK2		GPIOB
#define Pin_SK2			9

#define Port_Orange		PORTA
#define GPIO_Orange		GPIOA
#define Pin_Orange		2

#elif defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A)

#define Port_PTT		PORTA
#define GPIO_PTT		GPIOA
#define Pin_PTT			1

#define Port_SK1		PORTB
#define GPIO_SK1		GPIOB
#define Pin_SK1			1

#define Port_SK2		PORTB
#define GPIO_SK2		GPIOB
#define Pin_SK2			9

#define Port_Orange		PORTA
#define GPIO_Orange		GPIOA
#define Pin_Orange		2

#elif defined(PLATFORM_RD5R)

#define Port_PTT		PORTA
#define GPIO_PTT		GPIOA
#define Pin_PTT			1
#define Port_SK1		PORTB	// 'CALL' side button
#define GPIO_SK1		GPIOB
#define Pin_SK1			1
#define Port_SK2		PORTB	// 'MONI' side button
#define GPIO_SK2		GPIOB
#define Pin_SK2			9

#endif


#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S)

/* --------------  Battery power control  ---------------- */


// Power On/Off logic
#define Port_Keep_Power_On  PORTE
#define GPIO_Keep_Power_On  GPIOE
#define Pin_Keep_Power_On	26

#define Port_Power_Switch   PORTA
#define GPIO_Power_Switch 	GPIOA
#define Pin_Power_Switch	13


/* --------------  Audio amp and audio path  ---------------- */

#define Port_audio_amp_enable     PORTB
#define GPIO_audio_amp_enable     GPIOB
#define Pin_audio_amp_enable      0

#define Port_RX_audio_mux     PORTC
#define GPIO_RX_audio_mux     GPIOC
#define Pin_RX_audio_mux      5

#define Port_TX_audio_mux     PORTC
#define GPIO_TX_audio_mux     GPIOC
#define Pin_TX_audio_mux      6

/* --------------  Tx and Rx RF amplifiers  ---------------- */

#define Port_VHF_RX_amp_power PORTC
#define GPIO_VHF_RX_amp_power GPIOC
#define Pin_VHF_RX_amp_power  13
#define Port_UHF_RX_amp_power PORTC
#define GPIO_UHF_RX_amp_power GPIOC
#define Pin_UHF_RX_amp_power  15
#define Port_UHF_TX_amp_power PORTE
#define GPIO_UHF_TX_amp_power GPIOE
#define Pin_UHF_TX_amp_power  2
#define Port_VHF_TX_amp_power PORTE
#define GPIO_VHF_TX_amp_power GPIOE
#define Pin_VHF_TX_amp_power  3

#endif

/* --------------  Display  ---------------- */
#define DISPLAY_LED_PWM
#if ! defined(PLATFORM_GD77S)
#ifdef DISPLAY_LED_PWM

#endif
#endif // ! PLATFORM_GD77S

#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S)

#define Port_Display_Light	PORTC
#define GPIO_Display_Light	GPIOC
#define Pin_Display_Light	4
#define Port_Display_CS		PORTC
#define GPIO_Display_CS		GPIOC
#define Pin_Display_CS		8
#define Port_Display_RST	PORTC
#define GPIO_Display_RST	GPIOC
#define Pin_Display_RST		9
#define Port_Display_RS		PORTC
#define GPIO_Display_RS		GPIOC
#define Pin_Display_RS		10
#define Port_Display_SCK	PORTC
#define GPIO_Display_SCK 	GPIOC
#define Pin_Display_SCK		11
#define Port_Display_SDA    PORTC
#define GPIO_Display_SDA 	GPIOC
#define Pin_Display_SDA		12

#define BOARD_FTM_BASEADDR FTM0
#define BOARD_FTM_CHANNEL kFTM_Chnl_3

#elif defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A)

#define Port_Display_Light	PORTC
#define GPIO_Display_Light	GPIOC
#define Pin_Display_Light	4
#define Port_Display_CS		PORTC
#define GPIO_Display_CS		GPIOC
#define Pin_Display_CS		8
#define Port_Display_RST	PORTC
#define GPIO_Display_RST	GPIOC
#define Pin_Display_RST		9
#define Port_Display_RS		PORTC
#define GPIO_Display_RS		GPIOC
#define Pin_Display_RS		10
#define Port_Display_SCK	PORTC
#define GPIO_Display_SCK 	GPIOC
#define Pin_Display_SCK		11
#define Port_Display_SDA    PORTC
#define GPIO_Display_SDA 	GPIOC
#define Pin_Display_SDA		12

#define BOARD_FTM_BASEADDR FTM0
#define BOARD_FTM_CHANNEL kFTM_Chnl_3

#elif defined(PLATFORM_RD5R)

#define Port_Display_Light	PORTC
#define GPIO_Display_Light	GPIOC
#define Pin_Display_Light	5
#define Port_Display_CS		PORTC
#define GPIO_Display_CS		GPIOC
#define Pin_Display_CS		11
#define Port_Display_RST	PORTC
#define GPIO_Display_RST	GPIOC
#define Pin_Display_RST		9
#define Port_Display_RS		PORTC
#define GPIO_Display_RS		GPIOC
#define Pin_Display_RS		12
#define Port_Display_SCK	PORTC
#define GPIO_Display_SCK 	GPIOC
#define Pin_Display_SCK		8
#define Port_Display_SDA    PORTC
#define GPIO_Display_SDA 	GPIOC
#define Pin_Display_SDA		10

#define BOARD_FTM_BASEADDR FTM0
#define BOARD_FTM_CHANNEL kFTM_Chnl_2

#endif



#if defined(PLATFORM_GD77S)

#define Port_RotarySW_Line0   PORTD
#define GPIO_RotarySW_Line0   GPIOD
#define Pin_RotarySW_Line0    4
#define Port_RotarySW_Line1   PORTD
#define GPIO_RotarySW_Line1   GPIOD
#define Pin_RotarySW_Line1    5
#define Port_RotarySW_Line2   PORTD
#define GPIO_RotarySW_Line2   GPIOD
#define Pin_RotarySW_Line2    6
#define Port_RotarySW_Line3   PORTD
#define GPIO_RotarySW_Line3   GPIOD
#define Pin_RotarySW_Line3    7

#endif // PLATFORM_GD77S

/* --------------  SPI Flash (GPIO Bit bang on NXP MK22) ---------------- */

#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S)

#define Port_SPI_FLASH_CS_U  	PORTA
#define GPIO_SPI_FLASH_CS_U  	GPIOA
#define Pin_SPI_FLASH_CS_U   	19

#define Port_SPI_FLASH_CLK_U 	PORTE
#define GPIO_SPI_FLASH_CLK_U 	GPIOE
#define Pin_SPI_FLASH_CLK_U  	5

#define Port_SPI_FLASH_DI_U  	PORTE
#define GPIO_SPI_FLASH_DI_U  	GPIOE
#define Pin_SPI_FLASH_DI_U   	6

#define Port_SPI_FLASH_DO_U  	PORTE
#define GPIO_SPI_FLASH_DO_U  	GPIOE
#define Pin_SPI_FLASH_DO_U   	4

#elif defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A)

#define Port_SPI_FLASH_CS_U PORTE
#define GPIO_SPI_FLASH_CS_U GPIOE
#define Pin_SPI_FLASH_CS_U 6

#define Port_SPI_FLASH_CLK_U PORTE
#define GPIO_SPI_FLASH_CLK_U GPIOE
#define Pin_SPI_FLASH_CLK_U 5

#define Port_SPI_FLASH_DI_U PORTA
#define GPIO_SPI_FLASH_DI_U GPIOA
#define Pin_SPI_FLASH_DI_U 19

#define Port_SPI_FLASH_DO_U PORTE
#define GPIO_SPI_FLASH_DO_U GPIOE
#define Pin_SPI_FLASH_DO_U 4

#elif defined(PLATFORM_RD5R)

#define Port_SPI_FLASH_CS_U  	PORTE
#define GPIO_SPI_FLASH_CS_U  	GPIOE
#define Pin_SPI_FLASH_CS_U   	2

#define Port_SPI_FLASH_CLK_U 	PORTE
#define GPIO_SPI_FLASH_CLK_U 	GPIOE
#define Pin_SPI_FLASH_CLK_U  	5

#define Port_SPI_FLASH_DI_U  	PORTE
#define GPIO_SPI_FLASH_DI_U  	GPIOE
#define Pin_SPI_FLASH_DI_U   	3

#define Port_SPI_FLASH_DO_U  	PORTE
#define GPIO_SPI_FLASH_DO_U  	GPIOE
#define Pin_SPI_FLASH_DO_U   	4

#endif



/* --------------  Keypad  ---------------- */

#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S)

// column lines
#define Port_Key_Col0   PORTC
#define GPIO_Key_Col0 	GPIOC
#define Pin_Key_Col0	0
#define Port_Key_Col1   PORTC
#define GPIO_Key_Col1 	GPIOC
#define Pin_Key_Col1 	1
#define Port_Key_Col2   PORTC
#define GPIO_Key_Col2 	GPIOC
#define Pin_Key_Col2 	2
#define Port_Key_Col3   PORTC
#define GPIO_Key_Col3 	GPIOC
#define Pin_Key_Col3 	3

// row lines
#define Port_Key_Row0   PORTB
#define GPIO_Key_Row0 	GPIOB
#define Pin_Key_Row0	19
#define Port_Key_Row1   PORTB
#define GPIO_Key_Row1 	GPIOB
#define Pin_Key_Row1	20
#define Port_Key_Row2   PORTB
#define GPIO_Key_Row2 	GPIOB
#define Pin_Key_Row2	21
#define Port_Key_Row3   PORTB
#define GPIO_Key_Row3 	GPIOB
#define Pin_Key_Row3	22
#define Port_Key_Row4   PORTB
#define GPIO_Key_Row4 	GPIOB
#define Pin_Key_Row4	23

#elif defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A)

// column lines
#define Port_Key_Col0   PORTC
#define GPIO_Key_Col0 	GPIOC
#define Pin_Key_Col0	0
#define Port_Key_Col1   PORTC
#define GPIO_Key_Col1 	GPIOC
#define Pin_Key_Col1 	1
#define Port_Key_Col2   PORTC
#define GPIO_Key_Col2 	GPIOC
#define Pin_Key_Col2 	2
#define Port_Key_Col3   PORTC
#define GPIO_Key_Col3 	GPIOC
#define Pin_Key_Col3 	3

// row lines
#define Port_Key_Row0   PORTB
#define GPIO_Key_Row0 	GPIOB
#define Pin_Key_Row0	19
#define Port_Key_Row1   PORTB
#define GPIO_Key_Row1 	GPIOB
#define Pin_Key_Row1	20
#define Port_Key_Row2   PORTB
#define GPIO_Key_Row2 	GPIOB
#define Pin_Key_Row2	21
#define Port_Key_Row3   PORTB
#define GPIO_Key_Row3 	GPIOB
#define Pin_Key_Row3	22
#define Port_Key_Row4   PORTB
#define GPIO_Key_Row4 	GPIOB
#define Pin_Key_Row4	23

#elif defined(PLATFORM_RD5R)

// column lines
#define Port_Key_Col0   PORTC
#define GPIO_Key_Col0 	GPIOC
#define Pin_Key_Col0	0
#define Port_Key_Col1   PORTC
#define GPIO_Key_Col1 	GPIOC
#define Pin_Key_Col1 	1
#define Port_Key_Col2   PORTC
#define GPIO_Key_Col2 	GPIOC
#define Pin_Key_Col2 	2
#define Port_Key_Col3   PORTC
#define GPIO_Key_Col3 	GPIOC
#define Pin_Key_Col3 	3

// row lines
#define Port_Key_Row0   PORTB
#define GPIO_Key_Row0 	GPIOB
#define Pin_Key_Row0	19
#define Port_Key_Row1   PORTB
#define GPIO_Key_Row1 	GPIOB
#define Pin_Key_Row1	20
#define Port_Key_Row2   PORTB
#define GPIO_Key_Row2 	GPIOB
#define Pin_Key_Row2	21
#define Port_Key_Row3   PORTB
#define GPIO_Key_Row3 	GPIOB
#define Pin_Key_Row3	22
#define Port_Key_Row4   PORTB
#define GPIO_Key_Row4 	GPIOB
#define Pin_Key_Row4	23

#endif


/* --------------  HR-C6000 interface  ---------------- */

#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S)

// C6000 interrupts

// Rx interrupt
#define Port_INT_C6000_RF_RX PORTC
#define GPIO_INT_C6000_RF_RX GPIOC
#define Pin_INT_C6000_RF_RX  7

// Tx interrupt
#define Port_INT_C6000_RF_TX PORTC
#define GPIO_INT_C6000_RF_TX GPIOC
#define Pin_INT_C6000_RF_TX  16

// Sys interrupt
#define Port_INT_C6000_SYS   PORTC
#define GPIO_INT_C6000_SYS   GPIOC
#define Pin_INT_C6000_SYS    17

// Timeslot interrupt
#define Port_INT_C6000_TS    PORTC
#define GPIO_INT_C6000_TS    GPIOC
#define Pin_INT_C6000_TS     18

// Connections with C6000

// Reset
#define Port_C6000_RESET PORTE
#define GPIO_C6000_RESET GPIOE
#define Pin_C6000_RESET  0

// Power down
#define Port_C6000_PWD   PORTE
#define GPIO_C6000_PWD   GPIOE
#define Pin_C6000_PWD    1

#elif defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A)

// C6000 interrupts

// Rx interrupt
#define Port_INT_C6000_RF_RX PORTC
#define GPIO_INT_C6000_RF_RX GPIOC
#define Pin_INT_C6000_RF_RX  16

// Tx Interrupt
#define Port_INT_C6000_RF_TX PORTC
#define GPIO_INT_C6000_RF_TX GPIOC
#define Pin_INT_C6000_RF_TX  7

// Sys interrupt
#define Port_INT_C6000_SYS   PORTC
#define GPIO_INT_C6000_SYS   GPIOC
#define Pin_INT_C6000_SYS    17

// Timeslot interrupt
#define Port_INT_C6000_TS    PORTC
#define GPIO_INT_C6000_TS    GPIOC
#define Pin_INT_C6000_TS     18

// Connections with C6000

// Reset
#define Port_C6000_RESET PORTE
#define GPIO_C6000_RESET GPIOE
#define Pin_C6000_RESET  2

// Power down
#define Port_C6000_PWD   PORTE
#define GPIO_C6000_PWD   GPIOE
#define Pin_C6000_PWD    3

#elif defined(PLATFORM_RD5R)

// C6000 interrupts

// Rx interrupt
#define Port_INT_C6000_RF_RX PORTC
#define GPIO_INT_C6000_RF_RX GPIOC
#define Pin_INT_C6000_RF_RX  7

// Tx Interrupt
#define Port_INT_C6000_RF_TX PORTC
#define GPIO_INT_C6000_RF_TX GPIOC
#define Pin_INT_C6000_RF_TX  16

// Sys interrupt
#define Port_INT_C6000_SYS   PORTC
#define GPIO_INT_C6000_SYS   GPIOC
#define Pin_INT_C6000_SYS    17

// Timeslot interrupt
#define Port_INT_C6000_TS    PORTC
#define GPIO_INT_C6000_TS    GPIOC
#define Pin_INT_C6000_TS     18

// Connections with C6000

// Reset
#define Port_C6000_RESET PORTE
#define GPIO_C6000_RESET GPIOE
#define Pin_C6000_RESET  0

// Power down
#define Port_C6000_PWD   PORTE
#define GPIO_C6000_PWD   GPIOE
#define Pin_C6000_PWD    1

#endif

#if 0
void gpioInitButtons(void);
void gpioInitCommon(void);
#endif
void gpioInitDisplay(void);
void gpioSetDisplayBacklightIntensityPercentage(uint8_t intensityPercentage);
uint8_t gpioGetDisplayBacklightIntensityPercentage(void);
#if 0
void gpioInitFlash(void);
#endif
void gpioInitKeyboard(void);
void gpioInitLEDs(void);
void gpioInitRotarySwitch(void);

#if 0
//void gpioInitC6000Interface(void);
#endif


#endif
