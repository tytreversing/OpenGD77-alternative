/*
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
#ifndef _OPENGD77_HOTSPOT_H_
#define _OPENGD77_HOTSPOT_H_

#include <stdint.h>
#include <stdbool.h>

#define DMR_FRAME_LENGTH_BYTES  33U
#define DT_VOICE_LC_HEADER  	0x01U
#define DT_TERMINATOR_WITH_LC 	0x02U
#define DMR_SYNC_DATA           0x40U

#define HOTSPOT_VERSION_STRING "OpenGD77_HS v0.1.18"

typedef struct
{
	bool        PF;
	bool        R;
	int         FLCO;
	uint8_t 	FID;
	uint8_t 	options;
	uint32_t  	srcId;
	uint32_t  	dstId;
	uint8_t		rawData[12];
} DMRLC_t;



enum HOTSPOT_RX_STATE
{
	HOTSPOT_RX_IDLE = 0U,
	HOTSPOT_RX_START,
	HOTSPOT_RX_START_LATE,
	HOTSPOT_RX_AUDIO_FRAME,
	HOTSPOT_RX_STOP,
	HOTSPOT_RX_UNKNOWN
};


typedef enum
{
	STATE_IDLE      = 0,
	STATE_DSTAR     = 1,
	STATE_DMR       = 2,
	STATE_YSF       = 3,
	STATE_P25       = 4,
	STATE_NXDN      = 5,
	STATE_POCSAG    = 6,

	// Dummy states start at 90
	STATE_DMRDMO1K  = 92,
	STATE_RSSICAL   = 96,
	STATE_CWID      = 97,
	STATE_DMRCAL    = 98,
	STATE_DSTARCAL  = 99,
	STATE_INTCAL    = 100,
	STATE_POCSAGCAL = 101
} MMDVM_STATE;

enum HOTSPOT_COMMAND
{
	MMDVM_GET_VERSION   = 0x00U,
	MMDVM_GET_STATUS    = 0x01U,
	MMDVM_SET_CONFIG    = 0x02U,
	MMDVM_SET_MODE      = 0x03U,
	MMDVM_SET_FREQ      = 0x04U,
	MMDVM_CAL_DATA      = 0x08U,
	MMDVM_RSSI_DATA     = 0x09U,
	MMDVM_SEND_CWID     = 0x0AU,
	MMDVM_DSTAR_HEADER  = 0x10U,
	MMDVM_DSTAR_DATA    = 0x11U,
	MMDVM_DSTAR_LOST    = 0x12U,
	MMDVM_DSTAR_EOT     = 0x13U,
	MMDVM_DMR_DATA1     = 0x18U,
	MMDVM_DMR_LOST1     = 0x19U,
	MMDVM_DMR_DATA2     = 0x1AU,
	MMDVM_DMR_LOST2     = 0x1BU,
	MMDVM_DMR_SHORTLC   = 0x1CU,
	MMDVM_DMR_START     = 0x1DU,
	MMDVM_DMR_ABORT     = 0x1EU,
	MMDVM_YSF_DATA      = 0x20U,
	MMDVM_P25_HDR       = 0x30U,
	MMDVM_P25_LDU       = 0x31U,
	MMDVM_NXDN_DATA     = 0x40U,
	MMDVM_POCSAG_DATA   = 0x50U,
	MMDVM_ACK           = 0x70U,
	MMDVM_NAK           = 0x7FU,
	MMDVM_TRANSPARENT   = 0x90U,
	MMDVM_QSO_INFO      = 0x91U,
	MMDVM_FRAME_START   = 0xE0U
};

typedef enum
{
	HOTSPOT_STATE_NOT_CONNECTED,
	HOTSPOT_STATE_INITIALISE,
	HOTSPOT_STATE_RX_START,
	HOTSPOT_STATE_RX_PROCESS,
	HOTSPOT_STATE_RX_END,
	HOTSPOT_STATE_TX_START_BUFFERING,
	HOTSPOT_STATE_TRANSMITTING,
	HOTSPOT_STATE_TX_SHUTDOWN
} HOTSPOT_STATE;

typedef enum
{
	MMDVMHOST_RX_READY,
	MMDVMHOST_RX_BUSY,
	MMDVMHOST_RX_ERROR
} MMDVMHOST_RX_STATE;


// Uncomment this to enable all mmdvmSendDebug*() functions. You will see the results in the MMDVMHost log file.
//#define MMDVM_SEND_DEBUG

#if defined(MMDVM_SEND_DEBUG)
#define MMDVM_DEBUG1        0xF1
#define MMDVM_DEBUG2        0xF2
#define MMDVM_DEBUG3        0xF3
#define MMDVM_DEBUG4        0xF4
#define MMDVM_DEBUG5        0xF5
void mmdvmSendDebug1(const char *text);
void mmdvmSendDebug2(const char *text, int16_t n1);
void mmdvmSendDebug3(const char *text, int16_t n1, int16_t n2);
void mmdvmSendDebug4(const char *text, int16_t n1, int16_t n2, int16_t n3);
void mmdvmSendDebug5(const char *text, int16_t n1, int16_t n2, int16_t n3, int16_t n4);
#endif

void hotspotRxFrameHandler(uint8_t *frameBuf);

void cwProcess(void);
void cwReset(void);

void handleHotspotRequest(void);
void processUSBDataQueue(void);
void enqueueUSBData(uint8_t *data, uint8_t length);
void hotspotStateMachine(void);
void hotspotInit(void);

extern bool hotspotCwKeying;
extern uint16_t hotspotCwpoLen;
extern uint8_t hotspotCurrentRxCommandState;
extern char hotspotMmdvmQSOInfoIP[22];
extern DMRLC_t hotspotRxedDMR_LC; // used to stored LC info from RXed frames
extern uint8_t hotspotSavedPowerLevel;
extern uint32_t hotspotFreqRx;
extern uint32_t hotspotFreqTx;
extern volatile MMDVM_STATE hotspotModemState;
extern uint8_t hotspotPowerLevel;
extern bool hotspotMmdvmHostIsConnected;
#endif
