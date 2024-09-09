/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <FreeRTOS.h>
#include <task.h>

#include "applicationMain.h"
#include "utils.h"
#include "io/buttons.h"
#include "io/LEDs.h"
#include "io/keyboard.h"
#include "io/rotary_switch.h"
#include "io/display.h"
#include "functions/vox.h"
#include "hardware/HX8353E.h"
#include "hardware/HR-C6000.h"
#include "interfaces/i2c.h"
#include "interfaces/hr-c6000_spi.h"
#include "interfaces/i2s.h"
#include "interfaces/wdog.h"
#include "interfaces/adc.h"
#include "interfaces/dac.h"
#include "interfaces/pit.h"
#include "functions/sound.h"
#include "functions/trx.h"
#include "hardware/SPI_Flash.h"
#include "hardware/EEPROM.h"

#include "cmsis_os.h"

extern uint32_t NumInterruptPriorityBits;

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern ADC_HandleTypeDef hadc1;
extern I2S_HandleTypeDef hi2s3;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern DAC_HandleTypeDef hdac;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c3;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_tim1_ch1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern volatile bool aprsIsrFlag;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SystemClock_Config(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DMR_SPI_CS_Pin GPIO_PIN_2
#define DMR_SPI_CS_GPIO_Port GPIOE
#define DMR_SPI_CLK_Pin GPIO_PIN_3
#define DMR_SPI_CLK_GPIO_Port GPIOE
#define DMR_SPI_MOSI_Pin GPIO_PIN_4
#define DMR_SPI_MOSI_GPIO_Port GPIOE
#define DMR_SPI_MISO_Pin GPIO_PIN_5
#define DMR_SPI_MISO_GPIO_Port GPIOE
#define C6000_PWD_Pin GPIO_PIN_6
#define C6000_PWD_GPIO_Port GPIOE
#define TIME_SLOT_INTER_Pin GPIO_PIN_0
#define TIME_SLOT_INTER_GPIO_Port GPIOC
#define TIME_SLOT_INTER_EXTI_IRQn EXTI0_IRQn
#define SYS_INTER_Pin GPIO_PIN_1
#define SYS_INTER_GPIO_Port GPIOC
#define SYS_INTER_EXTI_IRQn EXTI1_IRQn
#define RF_TX_INTER_Pin GPIO_PIN_2
#define RF_TX_INTER_GPIO_Port GPIOC
#define RF_TX_INTER_EXTI_IRQn EXTI2_IRQn
#define ADC_VOLUME_Pin GPIO_PIN_0
#define ADC_VOLUME_GPIO_Port GPIOA
#define Battery_voltage_Pin GPIO_PIN_1
#define Battery_voltage_GPIO_Port GPIOA
#define R5_U_SW_Pin GPIO_PIN_2
#define R5_U_SW_GPIO_Port GPIOA
#define VOX_Pin GPIO_PIN_3
#define VOX_GPIO_Port GPIOA
#define APC_REF_Pin GPIO_PIN_4
#define APC_REF_GPIO_Port GPIOA
#define R5_V_SW_Pin GPIO_PIN_5
#define R5_V_SW_GPIO_Port GPIOA
#define KEYPAD_ROW0_Pin GPIO_PIN_6
#define KEYPAD_ROW0_GPIO_Port GPIOA
#define PWR_SW_Pin GPIO_PIN_7
#define PWR_SW_GPIO_Port GPIOA
#define PA_EN_2_Pin GPIO_PIN_4
#define PA_EN_2_GPIO_Port GPIOC
#define PA_EN_1_Pin GPIO_PIN_5
#define PA_EN_1_GPIO_Port GPIOC
#define LCD_D4_Pin GPIO_PIN_7
#define LCD_D4_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_8
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D6_Pin GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D7_Pin GPIO_PIN_10
#define LCD_D7_GPIO_Port GPIOE
#define PTT_Pin GPIO_PIN_11
#define PTT_GPIO_Port GPIOE
#define PTT_EXTERNAL_Pin GPIO_PIN_12
#define PTT_EXTERNAL_GPIO_Port GPIOE
#define ROTARY_SW_A_Pin GPIO_PIN_14
#define ROTARY_SW_A_GPIO_Port GPIOE
#define ROTARY_SW_B_Pin GPIO_PIN_11
#define ROTARY_SW_B_GPIO_Port GPIOB
#define ROTARY_SW_B_EXTI_IRQn EXTI15_10_IRQn
#define V_SPI_CS_Pin GPIO_PIN_12
#define V_SPI_CS_GPIO_Port GPIOB
#define SPI2_SCK_Pin GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define LCD_BKLIGHT_Pin GPIO_PIN_8
#define LCD_BKLIGHT_GPIO_Port GPIOD
#define RX_AUDIO_MUX_Pin GPIO_PIN_9
#define RX_AUDIO_MUX_GPIO_Port GPIOD
#define LCD_RS_Pin GPIO_PIN_12
#define LCD_RS_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOD
#define LCD_D0_Pin GPIO_PIN_14
#define LCD_D0_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_15
#define LCD_D1_GPIO_Port GPIOD
#define PA_SEL_SW_Pin GPIO_PIN_6
#define PA_SEL_SW_GPIO_Port GPIOC
#define CTC_DCS_PWM_Pin GPIO_PIN_7
#define CTC_DCS_PWM_GPIO_Port GPIOC
#define BEEP_PWM_Pin GPIO_PIN_8
#define BEEP_PWM_GPIO_Port GPIOC
#define GPS_EN_Pin GPIO_PIN_9
#define GPS_EN_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_10
#define GPS_RX_GPIO_Port GPIOA
#define MICPWR_SW_Pin GPIO_PIN_13
#define MICPWR_SW_GPIO_Port GPIOA
#define LCD_D2_Pin GPIO_PIN_0
#define LCD_D2_GPIO_Port GPIOD
#define LCD_D3_Pin GPIO_PIN_1
#define LCD_D3_GPIO_Port GPIOD
#define KEYPAD_ROW1_Pin GPIO_PIN_2
#define KEYPAD_ROW1_GPIO_Port GPIOD
#define KEYPAD_ROW2_Pin GPIO_PIN_3
#define KEYPAD_ROW2_GPIO_Port GPIOD
#define LCD_RD_Pin GPIO_PIN_4
#define LCD_RD_GPIO_Port GPIOD
#define LCD_WR_Pin GPIO_PIN_5
#define LCD_WR_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_6
#define LCD_CS_GPIO_Port GPIOD
#define SPI_Flash_CS_Pin GPIO_PIN_7
#define SPI_Flash_CS_GPIO_Port GPIOD
#define SPI1_SCK_Pin GPIO_PIN_3
#define SPI1_SCK_GPIO_Port GPIOB
#define SPI1_SDO_Pin GPIO_PIN_4
#define SPI1_SDO_GPIO_Port GPIOB
#define SPI1_SDI_Pin GPIO_PIN_5
#define SPI1_SDI_GPIO_Port GPIOB
#define SPK_MUTE_Pin GPIO_PIN_8
#define SPK_MUTE_GPIO_Port GPIOB
#define AUDIO_AMP_EN_Pin GPIO_PIN_9
#define AUDIO_AMP_EN_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOE
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

// PA_SEL_SW and LCB_BKLIGHT pins are swapped on the MD-UV390Plus 10W radios
#if defined(PLATFORM_VARIANT_UV380_PLUS_10W)
#undef PA_SEL_SW_Pin
#undef PA_SEL_SW_GPIO_Port
#undef LCD_BKLIGHT_Pin
#undef LCD_BKLIGHT_GPIO_Port

#define PA_SEL_SW_Pin GPIO_PIN_8
#define PA_SEL_SW_GPIO_Port GPIOD
#define LCD_BKLIGHT_Pin GPIO_PIN_6
#define LCD_BKLIGHT_GPIO_Port GPIOC
#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
