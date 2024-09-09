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

#ifndef _OPENGD77_USB_COM_H_
#define _OPENGD77_USB_COM_H_

#include <FreeRTOS.h>
#include <task.h>
#include "main.h"
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380)  || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701)
#include "usbd_cdc_if.h"
#else
#include "virtual_com.h"
#define APP_RX_DATA_SIZE (512 * 3)
#endif

#define COM_BUFFER_SIZE APP_RX_DATA_SIZE
#define COM_REQUESTBUFFER_SIZE COM_BUFFER_SIZE

extern volatile uint8_t com_buffer[COM_BUFFER_SIZE];
extern int com_buffer_write_idx;
extern int com_buffer_read_idx;
extern volatile int com_buffer_cnt;

extern volatile int comRecvMMDVMIndexIn;
extern volatile int comRecvMMDVMIndexOut;
extern volatile int comRecvMMDVMFrameCount;
extern volatile int com_request;
extern volatile uint8_t com_requestbuffer[COM_REQUESTBUFFER_SIZE];

#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380)  || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701)
extern volatile uint8_t usbComSendBuf[COM_BUFFER_SIZE];
#else
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t usbComSendBuf[COM_BUFFER_SIZE];
#endif
extern bool isCompressingAMBE;

extern volatile bool usbIsResetting;

void tick_com_request(void);
void send_packet(uint8_t val_0x82, uint8_t val_0x86, int ram);
void send_packet_big(uint8_t val_0x82, uint8_t val_0x86, int ram1, int ram2);
void add_to_commbuffer(uint8_t value);
bool USB_DeviceIsResetting(void);

void USB_DEBUG_PRINT(char *str);
void USB_DEBUG_printf(const char *format, ...) __attribute__((format(__printf__, 1, 2)));

#endif /* _OPENGD77_USB_COM_H_ */
