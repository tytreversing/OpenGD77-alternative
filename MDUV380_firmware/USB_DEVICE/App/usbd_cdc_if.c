/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "usb/usb_com.h"
#include "functions/hotspot.h"
#if defined(HAS_GPS)
#include "user_interface/uiGlobals.h"
#include "interfaces/gps.h"
#endif
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
volatile static uint32_t s_receivingBufferOffset = 0;
volatile static int32_t s_recvCount = 0;

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);

  memset((uint8_t *)com_requestbuffer, 0, sizeof(com_requestbuffer));
  s_receivingBufferOffset = 0;
  com_request = 0;
  s_recvCount = 0;
  s_receivingBufferOffset = 0;
  comRecvMMDVMIndexIn = comRecvMMDVMIndexOut = 0;

  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
	int32_t recvSize = *Len;

	if (recvSize > 0)
	{
		if (settingsUsbMode == USB_MODE_HOTSPOT)
		{
			if (Buf[0] == MMDVM_FRAME_START)
			{
				if (recvSize >= 3) // The shortest MMDVMHost frame length is 3U
				{
					uint8_t frameLength = (uint8_t)recvSize;

					s_recvCount = recvSize;

					// We can't rely on the MMDVMHost's frame size, so we're managing
					// the block size ourselves.
					com_requestbuffer[comRecvMMDVMIndexIn++] = frameLength + 1;

					// Make sure the index stays within limits.
					if (comRecvMMDVMIndexIn >= COM_REQUESTBUFFER_SIZE)
					{
						comRecvMMDVMIndexIn = 0;
					}

					// Copy the MMDVMHost data
					for (uint8_t i = 0; i < frameLength; i++)
					{
						com_requestbuffer[comRecvMMDVMIndexIn++] = Buf[i];

						// Make sure the index stays within limits.
						if (comRecvMMDVMIndexIn >= COM_REQUESTBUFFER_SIZE)
						{
							comRecvMMDVMIndexIn = 0;
						}
					}

					// Make sure the index stays within limits.
					if (comRecvMMDVMIndexIn >= COM_REQUESTBUFFER_SIZE)
					{
						comRecvMMDVMIndexIn = 0;
					}

					// Increment the frame counter
					comRecvMMDVMFrameCount++;
				}
			}
		}
		else
		{
			if (com_request == 0)
			{
				if ((s_receivingBufferOffset + recvSize) > sizeof(com_requestbuffer))
				{
					com_request = 1;
					s_receivingBufferOffset = 0;
					s_recvCount = 0;

					// Send back failure status
					uint8_t tmpBuf[2] = { '-' };
					CDC_Transmit_FS((uint8_t *)tmpBuf, 1);
				}
				else
				{
					bool mmdvmStarts = false;
					uint32_t mmdvmLen = 0;

					// Clear the buffer when the first bulk is handled
					if (s_receivingBufferOffset == 0)
					{
						memset((uint8_t *)com_requestbuffer, 0, sizeof(com_requestbuffer));
						s_recvCount = 0;

						// Extract the total expected length, if needed.
						if (recvSize == hUsbDeviceFS.ep_out[0].maxpacket)
						{
							switch (Buf[0])
							{
								case 'R': // Read
									if (recvSize >= 8)
									{
										s_recvCount = 8;
									}
									else
									{
										// Problem
									}
									break;

								case 'W': // HTs write
								case 'X': // MD9600 / MDUV380 / MD380 write
									if (recvSize >= 2)
									{
										switch (Buf[1])
										{
											case 1: // Flash, prepare sector
												s_recvCount = 5;
												break;

											case 3: // Flash, write sector
												s_recvCount = 2;
												break;

											case 2: // Flash send data
											case 4: // EEPROM write
											case 7: // Write WAV
												if (recvSize >= 8)
												{
													s_recvCount = 8 + ((Buf[6] << 8) + (Buf[7] << 0));
												}
												else
												{
													// Problem
												}
												break;

											default:
												break;
										}
									}
									break;

								case 'C':
									// Clamp commands, it may not exceed 5 + 16
									s_recvCount = recvSize;
									break;

								default:
									s_recvCount = recvSize;
									break;
							}
						}
						else
						{
							if ((Buf[0] == MMDVM_FRAME_START) &&
									(nonVolatileSettings.hotspotType != HOTSPOT_TYPE_OFF))
							{
								if (recvSize >= 3) // The shortest MMDVMHost frame length is 3U
								{
#if defined(HAS_GPS)
									// Turn NMEA output off as soon as possible.
									// The UI code will do more
									if (nonVolatileSettings.gps >= GPS_MODE_ON_NMEA)
									{
#if defined(LOG_GPS_DATA)
										gpsLoggingStop();
#endif
										gpsDataInputStartStop(false);
										(void)USBD_LL_FlushEP(&hUsbDeviceFS, CDC_OUT_EP);
									}
#endif

									// We can't rely on the MMDVMHost's frame size, so we're managing
									// the block size ourselves.
									mmdvmLen = recvSize + 1;
									com_requestbuffer[0] = mmdvmLen;
									s_receivingBufferOffset = 1;
									comRecvMMDVMIndexIn = comRecvMMDVMIndexOut = 0;
									mmdvmStarts = true;
									settingsUsbMode = USB_MODE_HOTSPOT;
								}
							}

                            s_recvCount = recvSize; // just for the fun
						}
					}

					memcpy((uint8_t *)com_requestbuffer + s_receivingBufferOffset, Buf, recvSize);

					s_recvCount -= recvSize;

					if ((recvSize < hUsbDeviceFS.ep_out[0].maxpacket) || (s_recvCount <= 0))
					{
						// MMDVMHost send a valid request, time to switch USB mode.
						if (mmdvmStarts)
						{
							comRecvMMDVMIndexIn = mmdvmLen;
							comRecvMMDVMFrameCount = 1;
						}

						s_receivingBufferOffset = 0;
						s_recvCount = 0;
						com_request = 1;
					}
					else
					{
						// More data will be received for this frame
						s_receivingBufferOffset += recvSize;
					}
				}

			}
			else
			{
				uint8_t tmpBuf[2];
				tmpBuf[0] = '-';
				CDC_Transmit_FS((uint8_t *)tmpBuf, 1);
			}
		}
	}

	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS); // Reset the RX buffer.
	USBD_CDC_ReceivePacket(&hUsbDeviceFS); // Prepare for the next reception.

	return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;

  uint32_t maxLen = SAFE_MIN(Len, sizeof(UserTxBufferFS));

  if (hcdc->TxState != 0){
	  return USBD_BUSY;
  }

  memcpy(UserTxBufferFS, Buf, maxLen);
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, maxLen);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
