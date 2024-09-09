/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
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

#include <FreeRTOS.h>
#include "hardware/HX8353E.h"
#include "io/display.h"
#include "functions/settings.h"
#include "interfaces/gpio.h"
#include "main.h"
#include "user_interface/uiGlobals.h"

uint8_t displayLCD_Type = 1;
static bool displayIsInverseVideo = false;
#if defined(PLATFORM_VARIANT_DM1701)
static void displayClearScreenBlankLines(bool isInverted);
#endif

void displayWriteCmd(uint8_t cmd)
{
    *((volatile uint8_t*) LCD_FSMC_ADDR_COMMAND) = cmd;
}
void displayWriteData(uint8_t val)
{
    *((volatile uint8_t*) LCD_FSMC_ADDR_DATA) = val;
}

void displayWriteCmds(uint8_t cmd, size_t len, uint8_t opts[])
{
	*((volatile uint8_t*) LCD_FSMC_ADDR_COMMAND) = cmd;

	for(volatile int x = 0; x < 1; x++);//Display seems to need a tiny delay to respond to the data

	if (len)
	{
		for (size_t i = 0; i < len; i++)
		{
			*((volatile uint8_t*) LCD_FSMC_ADDR_DATA) = opts[i];
			for(volatile int x = 0; x < 1; x++);//Display seems to need a tiny delay to respond to the data
		}
	}
}

void displaySetInvertedState(bool isInverted)
{
	if (displayIsInverseVideo != isInverted)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		displayIsInverseVideo = isInverted;

		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

		GPIO_InitStruct.Pin = LCD_D0_Pin | LCD_D1_Pin | LCD_D2_Pin | LCD_D3_Pin;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
		osDelay(10);
		displayWriteCmd(displayIsInverseVideo ? HX8583_CMD_INVON : HX8583_CMD_INVOFF);
		osDelay(10);
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

		*((volatile uint8_t*) LCD_FSMC_ADDR_DATA) = 0;// write 0 to the display pins , to pull them all low, so keyboard reads don't need to
#if defined(PLATFORM_VARIANT_DM1701)
		displayClearScreenBlankLines(displayIsInverseVideo);
#endif
		displaySetInverseVideo(displayIsInverseVideo);
	}
}

void displayInit(bool isInverted, bool SPIFlashAvailable)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Enable the FMC interface clock
	__HAL_RCC_FSMC_CLK_ENABLE();

	// was 0x10D9;
	FSMC_Bank1->BTCR[0] =
			FSMC_BCR1_MBKEN |  // bit 0
			FSMC_BCR1_MTYP_1 | // bit 3
			FSMC_BCR1_MWID_0 | // bit 4
			FSMC_BCR1_FACCEN | // bit 6
			//((0x2UL << FSMC_BCR1_FACCEN_Pos)) | // bit 7: UNDOCUMENTED
			FSMC_BCR1_WREN;    // bit 12

	// was 0x00100517;
	FSMC_Bank1->BTCR[1] =
			FSMC_BCR1_MBKEN |   // bit 0
			FSMC_BCR1_MUXEN |   // bit 1
			FSMC_BCR1_MTYP_0 |  // bit 2
			FSMC_BCR1_MWID_0 |  // bit 4
			FSMC_BCR1_BURSTEN | // bit 8
			FSMC_BCR1_WRAPMOD | // bit 10
			FSMC_BCR1_CBURSTRW; // bit 19, as bit 20 isn't documented

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

	GPIO_InitStruct.Pin = LCD_D0_Pin | LCD_D1_Pin | LCD_D2_Pin | LCD_D3_Pin | LCD_RS_Pin | LCD_WR_Pin | LCD_RD_Pin;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = 0x00;
	GPIO_InitStruct.Pin = LCD_CS_Pin | LCD_RST_Pin;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// Reset the screen
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	osDelay(20);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

	// Init
	if((DISPLAYLCD_GET_TYPE(displayLCD_Type) == 2) || (DISPLAYLCD_GET_TYPE(displayLCD_Type) == 3))
	{
		displayWriteCmd(0xfe);
		displayWriteCmd(0xef);

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xb4, 1, opts);
		}

		{
			uint8_t opts[] = { 0x16 };
			displayWriteCmds(0xff, 1, opts);
		}


		{
			uint8_t opts[] = { ((DISPLAYLCD_GET_TYPE(displayLCD_Type) == 3) ? 0x40 : 0x4f) };
			displayWriteCmds(0xfd, 1, opts);
		}

		{
			uint8_t opts[] = { 0x70 };
			displayWriteCmds(0xa4, 1, opts);
		}

		{
			uint8_t opts[] = { 0x94, 0x88 };
			displayWriteCmds(0xe7, 2, opts);
		}

		{
			uint8_t opts[] = { 0x3a };
			displayWriteCmds(0xea, 1, opts);
		}

		{
			uint8_t opts[] = { 0x11 };
			displayWriteCmds(0xed, 1, opts);
		}

		{
			uint8_t opts[] = { 0xc5 };
			displayWriteCmds(0xe4, 1, opts);
		}

		{
			uint8_t opts[] = { 0x80 };
			displayWriteCmds(0xe2, 1, opts);
		}

		{
			uint8_t opts[] = { 0x12 };
			displayWriteCmds(0xa3, 1, opts);
		}

		{
			uint8_t opts[] = { 0x07 };
			displayWriteCmds(0xe3, 1, opts);
		}

		{
			uint8_t opts[] = { 0x10 };
			displayWriteCmds(0xe5, 1, opts);
		}

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xf0, 1, opts);
		}

		{
			uint8_t opts[] = { 0x55 };
			displayWriteCmds(0xf1, 1, opts);
		}

		{
			uint8_t opts[] = { 0x05 };
			displayWriteCmds(0xf2, 1, opts);
		}

		{
			uint8_t opts[] = { 0x53 };
			displayWriteCmds(0xf3, 1, opts);
		}

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xf4, 1, opts);
		}

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xf5, 1, opts);
		}

		{
			uint8_t opts[] = { 0x27 };
			displayWriteCmds(0xf7, 1 , opts);
		}

		{
			uint8_t opts[] = { 0x22 };
			displayWriteCmds(0xf8, 1, opts);
		}

		{
			uint8_t opts[] = { 0x77 };
			displayWriteCmds(0xf9, 1, opts);
		}

		{
			uint8_t opts[] = { 0x35 };
			displayWriteCmds(0xfa, 1, opts);
		}

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xfb, 1, opts);
		}

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xfc, 1, opts);
		}

		displayWriteCmd(0xfe);

		displayWriteCmd(0xef);

		{
			uint8_t opts[] = { 0x00 };
			displayWriteCmds(0xe9, 1, opts);
		}

		osDelay(20);
	}
	else
	{
		displayWriteCmd(0x11);
		osDelay(120);

		{
			uint8_t opts[] = {0x05, 0x3c, 0x3c };
			displayWriteCmds(0xb1, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x05, 0x3c, 0x3c };
			displayWriteCmds(0xb2, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x05, 0x3c, 0x3c, 0x05, 0x3c, 0x3c };
			displayWriteCmds(0xb3, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x03 };
			displayWriteCmds(0xb4, 1, opts);
		}

		{
			uint8_t opts[] = { 0x28, 0x08, 0x04 };
			displayWriteCmds(0xc0, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0xc0 };
			displayWriteCmds(0xc1, 1, opts);
		}

		{
			uint8_t opts[] = { 0xd, 0x00};
			displayWriteCmds(0xc2, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x8d, 0x2a };
			displayWriteCmds(0xc3, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x8d, 0xee };
			displayWriteCmds(0xc4, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x1a };
			displayWriteCmds(0xc5, 1 , opts);
		}

		{
			uint8_t opts[] = { 0x08 };
			displayWriteCmds(0x36, 1, opts);
		}

		{
			uint8_t opts[] = { 0x04, 0x0c, 0x07, 0x0a, 0x2e, 0x30, 0x25, 0x2a, 0x28, 0x26, 0x2e, 0x3a, 0x00, 0x01, 0x03, 0x13 };
			displayWriteCmds(0xe0, sizeof(opts), opts);
		}

		{
			uint8_t opts[] = { 0x04, 0x16, 0x06, 0x0d, 0x2d, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2d, 0x3b, 0x00, 0x01, 0x04, 0x13 };
			displayWriteCmds(0xE1, sizeof(opts), opts);
		}
	}

	{
		uint8_t opts[] = {
				(DISPLAYLCD_GET_TYPE(displayLCD_Type) == 1) ? 0x60 :
						(DISPLAYLCD_GET_TYPE(displayLCD_Type) == 2) ? 0xE0 :
								0xA0
		};
		displayWriteCmds(HX8583_CMD_MADCTL, 1, opts);
	}

	{
		uint8_t opts[] = { 0x00, 0x00, 0x00, DISPLAY_SIZE_X };
		displayWriteCmds(HX8583_CMD_CASET, sizeof(opts), opts);
	}

	{
#if defined(PLATFORM_VARIANT_DM1701)		
		uint8_t opts[] = { 0x00, 0x00, 0x00, DISPLAY_SIZE_Y + DISPLAY_Y_OFFSET};
#else
		uint8_t opts[] = { 0x00, 0x00, 0x00, DISPLAY_SIZE_Y};
#endif
		displayWriteCmds(HX8583_CMD_RASET, sizeof(opts), opts);
	}

	{
		uint8_t opts[] = { 0x05 }; // RGB565 16 bits per pixel
		displayWriteCmds(HX8583_CMD_COLMOD, 1, opts);
	}

	//osDelay(10);// does not seem to be needed
	displayWriteCmd(HX8583_CMD_SLPOUT); // Activate the display
	//osDelay(120);// does not seem to be needed

	displayWriteCmd(HX8583_CMD_DISPON);

	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

	displayBegin(isInverted, SPIFlashAvailable);

	displayClearBuf();

#if defined(PLATFORM_VARIANT_DM1701)	
	displayClearScreenBlankLines(isInverted);
#endif
	displayRender();
}
#if defined(PLATFORM_VARIANT_DM1701)
static void displayClearScreenBlankLines(bool isInverted)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	memset(&GPIO_InitStruct, 0x00, sizeof(GPIO_InitTypeDef));

	// Display shares its pins with the keypad, so the pins need to be put into alternate mode to work with the FSMC
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

	GPIO_InitStruct.Pin = LCD_D0_Pin | LCD_D1_Pin | LCD_D2_Pin | LCD_D3_Pin;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

	uint8_t opts[] = { 0x00, 0, 0x00, DISPLAY_Y_OFFSET };
	displayWriteCmds(HX8583_CMD_RASET, sizeof(opts), opts);

	displayWriteCmd(HX8583_CMD_RAMWR);

	uint8_t fillData = isInverted ? 0xFF: 0x00;

	for(int y = 0; y < DISPLAY_Y_OFFSET * DISPLAY_SIZE_X * sizeof(uint16_t); y++)
	{
		*((volatile uint8_t*) LCD_FSMC_ADDR_DATA) = fillData;
	}

	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

	*((volatile uint8_t*) LCD_FSMC_ADDR_DATA) = 0;// write 0 to the display pins , to pull them all low, so keyboard reads don't need to

}
#endif

void displayEnableBacklight(bool enable, int displayBacklightPercentageOff)
{
	if (enable)
	{
		gpioSetDisplayBacklightIntensityPercentage(nonVolatileSettings.displayBacklightPercentage[DAYTIME_CURRENT]);
	}
	else
	{
		gpioSetDisplayBacklightIntensityPercentage(((nonVolatileSettings.backlightMode == BACKLIGHT_MODE_NONE) ? 0 : displayBacklightPercentageOff));
	}
}

bool displayIsBacklightLit(void)
{
	return (gpioGetDisplayBacklightIntensityPercentage() != nonVolatileSettings.displayBacklightPercentageOff);
}
