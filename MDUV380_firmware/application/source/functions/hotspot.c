/*
 * Copyright (C) 2019-2024 Roger Clark, VK3KYY / G4KYF
 *                         Daniel Caujolle-Bert, F1RMB
 *
 * Low level DMR stream implementation informed by code written by
 *                         DSD Author (anonymous)
 *                         MBELib Author (anonymous)
 *                         Ian Wraith G7GHH
 *                         Jonathan Naylor G4KLX
 *
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

#include "functions/hotspot.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "functions/calibration.h"
#include "functions/hotspot.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiUtilities.h"
#include "user_interface/uiLocalisation.h"
#include "hardware/HR-C6000.h"
#include "functions/settings.h"
#include "functions/sound.h"
#include "functions/ticks.h"
#include "functions/trx.h"
#include "usb/usb_com.h"
#include "functions/rxPowerSaving.h"
#include "user_interface/uiHotspot.h"
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#include "hardware/radioHardwareInterface.h"
#endif

#define MMDVM_HEADER_LENGTH 4
#define concat(a, b) a " GitID #" b ""
#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])


static void ReedSolomonDMREncode(const uint8_t *inputData, uint8_t *outputData);
static uint8_t LUT_Mult(uint8_t a, uint8_t b);
static void BPTCglobalsInit(void);
static void BPTCdecode(const uint8_t *inputData, uint8_t *outputData);
static void BPTCencode(const uint8_t *inputData, uint8_t *outputData);
static void DMRLC2Bytes(const DMRLC_t *LC_DataInput, uint8_t *outputBytes);
static uint8_t hammingGetBits(bool *inputOutputBooleanBitsArray, bool is16114);
static void hammingEncode(bool *inputOutputBooleanBitsArray,bool is16114);
static bool hammingDecodeType1(bool *inputOutputBooleanBitsArray);
static bool hammingDecodeType2(bool *inputOutputBooleanBitsArray);
static void embeddedDataDecodeEmbeddedData(void);
static void embeddedDataEncodeEmbeddedData(void);
static uint32_t CRC_encodeFiveBit(const bool *in);
static void byteToBooleanBitsArray(uint8_t byteIn, bool *bitsOut);
static uint8_t BooleanBitsArrayToByte(const bool *bitsIn);
static uint8_t setFreq(const uint8_t *data, uint8_t length);
static void sendNAK(uint8_t cmd, uint8_t err);
static void sendACK(uint8_t cmd);
static uint8_t hotspotModeReceiveNetFrame(const uint8_t *comBuffer, uint8_t timeSlot);
static bool voiceLCHeaderDecode(const uint8_t *data, uint8_t type, DMRLC_t *lc);
static bool DMRFullLC_encode(DMRLC_t *lc, uint8_t *data, uint8_t type);
static void embeddedDataBuffersInt(void);
static bool embeddedDataAddData(const uint8_t *data, uint8_t lcss);
static void embeddedDataGetData(uint8_t sequenceNumber, uint8_t *outputData);
static bool embeddedDataGetRawData(uint8_t *outputData);
static void embeddedDataSetLC(const DMRLC_t *lc);
static bool hasTXOverflow(void);
static bool hasRXOverflow(void);


extern LinkItem_t *LinkHead;

static const uint8_t PROTOCOL_VERSION = 1;
#if defined(PLATFORM_MD9600) || defined(PLATFORM_MDUV380) || defined(PLATFORM_MD380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
static const char HARDWARE[] = concat(HOTSPOT_VERSION_STRING, XSTRINGIFY(GITVERSION));
#else
static const char HARDWARE[] = concat(HOTSPOT_VERSION_STRING, GITVERSION);
#endif
static const uint8_t MS_SOURCED_AUDIO_SYNC[]   = { 0x07, 0xF7, 0xD5, 0xDD, 0x57, 0xDF, 0xD0 };
static const uint8_t SYNC_MASK[]               = { 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
static const uint8_t MMDVM_VOICE_SYNC_PATTERN = 0x20;
static const int EMBEDDED_DATA_OFFSET = 13;
static const int TX_BUFFER_MIN_BEFORE_TRANSMISSION = 4;
static const uint8_t START_FRAME_PATTERN[]  = { 0xFF,0x57,0xD7,0x5D,0xF5,0xD9 };
static const uint8_t END_FRAME_PATTERN[]    = { 0x5D,0x7F,0x77,0xFD,0x75,0x79 };
static const uint8_t VOICE_LC_SYNC_FULL[]       = { 0x04, 0x6D, 0x5D, 0x7F, 0x77, 0xFD, 0x75, 0x7E, 0x30 };
static const uint8_t TERMINATOR_LC_SYNC_FULL[]  = { 0x04, 0xAD, 0x5D, 0x7F, 0x77, 0xFD, 0x75, 0x79, 0x60 };
static const uint8_t LC_SYNC_MASK_FULL[]        = { 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };

static const uint8_t DMR_AUDIO_SEQ_SYNC[6][7]   = {
		{ 0x07, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0xD0 },  // seq 0 NOT USED AS THIS IS THE SYNC
		{ 0x01, 0x30, 0x00, 0x00, 0x00, 0x09, 0x10 },  // seq 1
		{ 0x01, 0x70, 0x00, 0x00, 0x00, 0x07, 0x40 },  // seq 2
		{ 0x01, 0x70, 0x00, 0x00, 0x00, 0x07, 0x40 },  // seq 3
		{ 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x70 },  // seq 4
		{ 0x01, 0x10, 0x00, 0x00, 0x00, 0x0E, 0x20 }   // seq 5
};

static const uint8_t DMR_AUDIO_SEQ_MASK[]       = { 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0xF0 };
static const uint8_t DMR_EMBED_SEQ_MASK[]       = { 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xF0, 0x00 };
static const uint8_t BIT_MASK_TABLE[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static const struct
{
	uint8_t  c;
	uint32_t pattern;
	uint8_t  length;
} CW_SYMBOL_LIST[] = {
		{'A', 0xB8000000, 8},
		{'B', 0xEA800000, 12},
		{'C', 0xEBA00000, 14},
		{'D', 0xEA000000, 10},
		{'E', 0x80000000, 4},
		{'F', 0xAE800000, 12},
		{'G', 0xEE800000, 12},
		{'H', 0xAA000000, 10},
		{'I', 0xA0000000, 6},
		{'J', 0xBBB80000, 16},
		{'K', 0xEB800000, 12},
		{'L', 0xBA800000, 12},
		{'M', 0xEE000000, 10},
		{'N', 0xE8000000, 8},
		{'O', 0xEEE00000, 14},
		{'P', 0xBBA00000, 14},
		{'Q', 0xEEB80000, 16},
		{'R', 0xBA000000, 10},
		{'S', 0xA8000000, 8},
		{'T', 0xE0000000, 6},
		{'U', 0xAE000000, 10},
		{'V', 0xAB800000, 12},
		{'W', 0xBB800000, 12},
		{'X', 0xEAE00000, 14},
		{'Y', 0xEBB80000, 16},
		{'Z', 0xEEA00000, 14},
		{'1', 0xBBBB8000, 20},
		{'2', 0xAEEE0000, 18},
		{'3', 0xABB80000, 16},
		{'4', 0xAAE00000, 14},
		{'5', 0xAA800000, 12},
		{'6', 0xEAA00000, 14},
		{'7', 0xEEA80000, 16},
		{'8', 0xEEEA0000, 18},
		{'9', 0xEEEE8000, 20},
		{'0', 0xEEEEE000, 22},
		{'/', 0xEAE80000, 16},
		{'?', 0xAEEA0000, 18},
		{',', 0xEEAEE000, 22},
		{'-', 0xEAAE0000, 18},
		{'=', 0xEAB80000, 16},
		{'.', 0xBAEB8000, 20},
		{' ', 0x00000000, 4},
		{0,   0x00000000, 0}
};

static const uint8_t VOICE_LC_HEADER_CRC_MASK[]    = {0x96, 0x96, 0x96};
static const uint8_t TERMINATOR_WITH_LC_CRC_MASK[] = {0x99, 0x99, 0x99};

static const int BPTC19696CopyRanges[][2] = {{4,11},{16,26},{31,41},{46,56},{61,71},{76,86},{91,101},{106,116},{121,131}};
static const int embedddataCopyRanges[][2] = {{0,10},{16,26},{32,41},{48,57},{64,73},{80,89},{96,105}};

static uint8_t hotspotTxLC[9];
static bool startedEmbeddedSearch = false;

// USB TX read/write positions and count
static volatile uint16_t usbComSendBufWritePosition = 0;
static volatile uint16_t usbComSendBufReadPosition = 0;
static volatile uint16_t usbComSendBufCount = 0;

// RF data read/write positions and count
static volatile uint32_t rfFrameBufReadIdx = 0;
static volatile uint32_t rfFrameBufWriteIdx = 0;
static volatile uint32_t rfFrameBufCount = 0;

static uint8_t lastRxState = HOTSPOT_RX_IDLE;
static const int TX_BUFFERING_TIMEOUT = 360;
static const int RX_NET_FRAME_TIMEOUT = 360;
static int timeoutCounter;
static uint32_t mmdvmHostLastActiveTime = 0; // store last activity time (ms)
static const uint32_t MMDVMHOST_TIMEOUT = 20000; // 20s timeout (MMDVMHost mode only, there is no timeout for BlueDV)
static volatile HOTSPOT_STATE hotspotState = HOTSPOT_STATE_NOT_CONNECTED;
static uint8_t rf_power = 255;
static int txStopDelay = 0;
static int netRXDataTimer = 0;
static bool rxLCFrameSent = false;


typedef enum
{
	LCS_0,
	LCS_1,
	LCS_2,
	LCS_3
} LC_STATE_t;


static uint8_t colorCode = 1;
static char overriddenLCTA[2 * 9] = {0}; // 2 LC frame only (enough to store callsign)
static bool overriddenLCAvailable = false;
static uint32_t hotspotTxDelay = 0;
static uint8_t overriddenBlocksTA = 0x00;
static LC_STATE_t embeddedDataSequenceState;
static bool	embeddedDataRaw[128];
static bool	embeddedDataProcessed[72];
static int	embeddedDataFLCO;
static bool	embeddedDataIsValid;

#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
#define BPTRAMLOCATION ".data.$RAM2"
#else
#define BPTRAMLOCATION ".ccmram"
#endif

__attribute__((section(BPTRAMLOCATION))) static bool BPTCRaw[196];
__attribute__((section(BPTRAMLOCATION))) static bool BPTCDeInterleaved[196];

static const uint32_t cwDOTDuration = 60; // 60ms per DOT
static ticksTimer_t cwNextPeriodTimer = { 0, 0 };
static uint8_t cwBuffer[64];
static uint16_t cwpoPtr;

#if defined(PLATFORM_MD9600)
static HRC6000_Tone1Config_t tone1Config;
#endif

bool hotspotCwKeying = false;
uint16_t hotspotCwpoLen;
uint8_t hotspotCurrentRxCommandState;
uint32_t hotspotFreqRx = 0;
uint32_t hotspotFreqTx = 0;
char hotspotMmdvmQSOInfoIP[22] = {0}; // use 6x8 font; 21 char long
DMRLC_t hotspotRxedDMR_LC; // used to stored LC info from RXed frames
uint8_t hotspotSavedPowerLevel = POWER_UNSET;// no power level saved yet
bool hotspotMmdvmHostIsConnected = false;
uint8_t hotspotPowerLevel = 0;// no power level saved yet
volatile MMDVM_STATE hotspotModemState = STATE_IDLE;

static volatile MMDVMHOST_RX_STATE MMDVMHostRxState;

static bool voiceLCHeaderDecode(const uint8_t *data, uint8_t type, DMRLC_t *lc)
{
	uint8_t parityCheckArray[4];

	BPTCglobalsInit();
	BPTCdecode(data, lc->rawData);

	lc->rawData[9]  ^= VOICE_LC_HEADER_CRC_MASK[0];
	lc->rawData[10] ^= VOICE_LC_HEADER_CRC_MASK[1];
	lc->rawData[11] ^= VOICE_LC_HEADER_CRC_MASK[2];

	ReedSolomonDMREncode(lc->rawData, parityCheckArray);

	if (!((lc->rawData[9] == parityCheckArray[2]) && (lc->rawData[10] == parityCheckArray[1]) && (lc->rawData[11] == parityCheckArray[0])))
	{
		return false;
	}

	lc->PF = (lc->rawData[0] & 0x80) == 0x80;
	lc->R  = (lc->rawData[0] & 0x40) == 0x40;
	lc->FLCO = lc->rawData[0] & 0x3F;
	lc->FID = lc->rawData[1];
	lc->options = lc->rawData[2];
	lc->dstId = (((uint32_t)lc->rawData[3]) << 16) + (((uint32_t)lc->rawData[4]) << 8) + ((uint32_t)lc->rawData[5]);
	lc->srcId = (((uint32_t)lc->rawData[6]) << 16) + (((uint32_t)lc->rawData[7]) << 8) + ((uint32_t)lc->rawData[8]);

	return true;
}

bool DMRFullLC_encode(DMRLC_t *lc, uint8_t *data, uint8_t type)
{
	uint8_t lcData[LC_DATA_LENGTH];

	DMRLC2Bytes(lc, lcData);

	uint8_t parity[4];
	ReedSolomonDMREncode(lcData, parity);

	if (type == DT_VOICE_LC_HEADER)
	{
		lcData[9]  = parity[2] ^ VOICE_LC_HEADER_CRC_MASK[0];
		lcData[10] = parity[1] ^ VOICE_LC_HEADER_CRC_MASK[1];
		lcData[11] = parity[0] ^ VOICE_LC_HEADER_CRC_MASK[2];
	}
	else
	{
		// must be DT_TERMINATOR_WITH_LC:
		lcData[9]  = parity[2] ^ TERMINATOR_WITH_LC_CRC_MASK[0];
		lcData[10] = parity[1] ^ TERMINATOR_WITH_LC_CRC_MASK[1];
		lcData[11] = parity[0] ^ TERMINATOR_WITH_LC_CRC_MASK[2];
	}

	BPTCglobalsInit();
	BPTCencode(lcData, data);

	return true;
}

static void embeddedDataBuffersInt(void)
{
	memset(embeddedDataRaw, 0, sizeof(embeddedDataRaw));
	memset(embeddedDataProcessed, 0, sizeof(embeddedDataProcessed));
	embeddedDataFLCO = 0;
	embeddedDataIsValid = false;
}

static bool embeddedDataAddData(const uint8_t *data, uint8_t lcss)
{
	bool rawData[36];

	for (int i = 0; i < 5; i++)
	{
		byteToBooleanBitsArray(data[i + 14], rawData + (i << 3));
	}

	switch (lcss)
	{
		case 1:
			for (int i = 0; i < 32; i++)
			{
				embeddedDataRaw[i] = rawData[i + 4];
			}
			embeddedDataSequenceState = LCS_1;
			embeddedDataIsValid = false;

			return false;
			break;
		case 2:
			if (embeddedDataSequenceState == LCS_3)
			{
				for (int i = 0; i < 32; i++)
				{
					embeddedDataRaw[i + 96] = rawData[i + 4];
				}

				embeddedDataSequenceState = LCS_0;

				embeddedDataDecodeEmbeddedData();
				if (embeddedDataIsValid)
				{
					embeddedDataEncodeEmbeddedData();
				}
				return embeddedDataIsValid;
			}
			break;
		case 3:
			switch (embeddedDataSequenceState)
			{
				case LCS_1:
					for (int i = 4; i < 36; i++)
					{
						embeddedDataRaw[i + 28] = rawData[i];
					}

					embeddedDataSequenceState = LCS_2;

					return false;
					break;
				case LCS_2:
					for (int i = 0; i < 32; i++)
					{
						embeddedDataRaw[i + 64] = rawData[i + 4];
					}

					embeddedDataSequenceState = LCS_3;

					return false;
					break;
				default:
					break;
			}
			break;
	}

	return false;
}

static void embeddedDataGetData(uint8_t sequenceNumber, uint8_t *outputData)
{
	memset(outputData, 0, DMR_FRAME_LENGTH_BYTES);//clear

	if ((sequenceNumber >= 1) && (sequenceNumber < 5))
	{
		bool bits[40];
		uint8_t bytes[5];

		sequenceNumber--;

		memset(bits, 0, 40 * sizeof(bool));
		memcpy(bits + 4, embeddedDataRaw + (sequenceNumber * 32), 32 * sizeof(bool));

		for (int i = 0; i < 5; i++)
		{
			bytes[i] = BooleanBitsArrayToByte(bits + (i << 3));
		}

		outputData[14] = (outputData[14] & 0xF0) | (bytes[0] & 0x0F);
		outputData[15] = bytes[1];
		outputData[16] = bytes[2];
		outputData[17] = bytes[3];
		outputData[18] = (outputData[18] & 0x0F) | (bytes[4] & 0xF0);

		return;
	}

	outputData[14] &= 0xF0;
	outputData[15]  = 0x00;
	outputData[16]  = 0x00;
	outputData[17]  = 0x00;
	outputData[18] &= 0x0F;
}

static bool embeddedDataGetRawData(uint8_t *outputData)
{
	if (!embeddedDataIsValid)
	{
		return false;
	}

	for (int i = 0; i < 9; i++)
	{
		outputData[i] = BooleanBitsArrayToByte(embeddedDataProcessed + (i << 3));
	}

	return true;
}

static void embeddedDataSetLC(const DMRLC_t *lc)
{
	uint8_t bytes[9];

	DMRLC2Bytes(lc, bytes);

	for (int i = 0; i < 9; i++)
	{
		byteToBooleanBitsArray(bytes[i], embeddedDataProcessed + (i << 3));
	}

	embeddedDataFLCO  = lc->FLCO;
	embeddedDataIsValid = true;
	embeddedDataEncodeEmbeddedData();
}

static uint8_t LUT_Mult(uint8_t a, uint8_t b)
{
	/* LUTs from
	 * ETSI TS 102 361-1 V2.2.1 (2013-02)
	 * Page 138
	 */
	const uint8_t EXP_LUT[] =
	{
		   1,    2,    4,    8, 0x10, 0x20, 0x40, 0x80, 0x1D, 0x3A, 0x74, 0xE8, 0xCD, 0x87, 0x13, 0x26,
		0x4C, 0x98, 0x2D, 0x5A, 0xB4, 0x75, 0xEA, 0xC9, 0x8F, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0,
		0x9D, 0x27, 0x4E, 0x9C, 0x25, 0x4A, 0x94, 0x35, 0x6A, 0xD4, 0xB5, 0x77, 0xEE, 0xC1, 0x9F, 0x23,
		0x46, 0x8C, 0x05, 0x0A, 0x14, 0x28, 0x50, 0xA0, 0x5D, 0xBA, 0x69, 0xD2, 0xB9, 0x6F, 0xDE, 0xA1,
		0x5F, 0xBE, 0x61, 0xC2, 0x99, 0x2F, 0x5E, 0xBC, 0x65, 0xCA, 0x89, 0x0F, 0x1E, 0x3C, 0x78, 0xF0,
		0xFD, 0xE7, 0xD3, 0xBB, 0x6B, 0xD6, 0xB1, 0x7F, 0xFE, 0xE1, 0xDF, 0xA3, 0x5B, 0xB6, 0x71, 0xE2,
		0xD9, 0xAF, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0D, 0x1A, 0x34, 0x68, 0xD0, 0xBD, 0x67, 0xCE,
		0x81, 0x1F, 0x3E, 0x7C, 0xF8, 0xED, 0xC7, 0x93, 0x3B, 0x76, 0xEC, 0xC5, 0x97, 0x33, 0x66, 0xCC,
		0x85, 0x17, 0x2E, 0x5C, 0xB8, 0x6D, 0xDA, 0xA9, 0x4F, 0x9E, 0x21, 0x42, 0x84, 0x15, 0x2A, 0x54,
		0xA8, 0x4D, 0x9A, 0x29, 0x52, 0xA4, 0x55, 0xAA, 0x49, 0x92, 0x39, 0x72, 0xE4, 0xD5, 0xB7, 0x73,
		0xE6, 0xD1, 0xBF, 0x63, 0xC6, 0x91, 0x3F, 0x7E, 0xFC, 0xE5, 0xD7, 0xB3, 0x7B, 0xF6, 0xF1, 0xFF,
		0xE3, 0xDB, 0xAB, 0x4B, 0x96, 0x31, 0x62, 0xC4, 0x95, 0x37, 0x6E, 0xDC, 0xA5, 0x57, 0xAE, 0x41,
		0x82, 0x19, 0x32, 0x64, 0xC8, 0x8D, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xDD, 0xA7, 0x53, 0xA6,
		0x51, 0xA2, 0x59, 0xB2, 0x79, 0xF2, 0xF9, 0xEF, 0xC3, 0x9B, 0x2B, 0x56, 0xAC, 0x45, 0x8A, 0x09,
		0x12, 0x24, 0x48, 0x90, 0x3D, 0x7A, 0xF4, 0xF5, 0xF7, 0xF3, 0xFB, 0xEB, 0xCB, 0x8B, 0x0B, 0x16,
		0x2C, 0x58, 0xB0, 0x7D, 0xFA, 0xE9, 0xCF, 0x83, 0x1B, 0x36, 0x6C, 0xD8, 0xAD, 0x47, 0x8E
	};

	const uint8_t LOG_LUT[] =
	{
		  0,   0,   1,  25,   2,  50,  26, 198,   3, 223,  51, 238,  27, 104, 199,  75,
		  4, 100, 224,  14,  52, 141, 239, 129,  28, 193, 105, 248, 200,   8,  76, 113,
		  5, 138, 101,  47, 225,  36,  15,  33,  53, 147, 142, 218, 240,  18, 130,  69,
		 29, 181, 194, 125, 106,  39, 249, 185, 201, 154,   9, 120,  77, 228, 114, 166,
		  6, 191, 139,  98, 102, 221,  48, 253, 226, 152,  37, 179,  16, 145,  34, 136,
		 54, 208, 148, 206, 143, 150, 219, 189, 241, 210,  19,  92, 131,  56,  70,  64,
		 30,  66, 182, 163, 195,  72, 126, 110, 107,  58,  40,  84, 250, 133, 186,  61,
		202,  94, 155, 159,  10,  21, 121,  43,  78, 212, 229, 172, 115, 243, 167,  87,
		  7, 112, 192, 247, 140, 128,  99,  13, 103,  74, 222, 237,  49, 197, 254,  24,
		227, 165, 153, 119,  38, 184, 180, 124,  17,  68, 146, 217,  35,  32, 137,  46,
		 55,  63, 209,  91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190,  97,
		242,  86, 211, 171,  20,  42,  93, 158, 132,  60,  57,  83,  71, 109,  65, 162,
		 31,  45,  67, 216, 183, 123, 164, 118, 196,  23,  73, 236, 127,  12, 111, 246,
		108, 161,  59,  82,  41, 157,  85, 170, 251,  96, 134, 177, 187, 204,  62,  90,
		203,  89,  95, 176, 156, 169, 160,  81,  11, 245,  22, 235, 122, 117,  44, 215,
		 79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168,  80,  88, 175
	};

	if ((a == 0) || (b == 0))
	{
		return 0;
	}

	int sum = LOG_LUT[a] + LOG_LUT[b];

	if (sum != 511)
	{
		return EXP_LUT[sum % 255];
	}

	return 0;
}

static void ReedSolomonDMREncode(const uint8_t *inputData, uint8_t *outputData)
{
	const uint8_t POLYNOMIAL_FACTORS[3] = {64, 56, 14};

	memset(outputData, 0, 4 * sizeof(uint8_t));

	for (int i = 0; i < 9; i++)
	{
		uint8_t tmp = inputData[i] ^ outputData[2];

		for (int j = 2; j > 0; j--)
		{
			outputData[j] = outputData[j - 1] ^ LUT_Mult(POLYNOMIAL_FACTORS[j], tmp);
		}

		outputData[0] = LUT_Mult(POLYNOMIAL_FACTORS[0], tmp);
	}
}

static void BPTCglobalsInit(void)
{
	memset(BPTCRaw, 0, sizeof(BPTCRaw));
	memset(BPTCDeInterleaved, 0, sizeof(BPTCDeInterleaved));
}

static void BPTCdecode(const uint8_t *inputData, uint8_t *outputData)
{
	// 0xFF means don't use this value
	const uint8_t BITS_LOOKUP[16] = {0xFF, 9, 10, 6, 11, 3, 7, 1, 12, 0xFF, 4, 0xFF, 8, 5, 2, 0};
	bool bitData[96];
	bool tmpArray[13];
	uint32_t bitDataIndex = 0;
	bool stillProcessing;
	uint8_t n;

	for (int i = 0; i < 13; i++)
	{
		byteToBooleanBitsArray(inputData[i], BPTCRaw + (i << 3));
	}

	byteToBooleanBitsArray(inputData[20], tmpArray);
	BPTCRaw[98] = tmpArray[6];
	BPTCRaw[99] = tmpArray[7];

	for (int i = 0; i < 13; i++)
	{
		byteToBooleanBitsArray(inputData[i + 21], BPTCRaw + (100 + (i << 3)));
	}

	for (int i = 0; i < 196; i++)
	{
		BPTCDeInterleaved[i] = BPTCRaw[(i * 181) % 196];// interleave
	}

	stillProcessing = true;// Need to initially set this to true to start the for loop

	for (int i = 0; ((i < 5) && stillProcessing); i++)
	{
		stillProcessing = false;

		for (int j = 0; j < 15; j++)
		{
			int pos = j + 1;
			for (int k = 0; k < 13; k++)
			{
				tmpArray[k] = BPTCDeInterleaved[pos];
				pos += 15;
			}

			bool hammingOK = false;

			n  = ((tmpArray[0] ^ tmpArray[1] ^ tmpArray[3] ^ tmpArray[5] ^ tmpArray[6]) != tmpArray[9])  ? 0x01 : 0x00;
			n |= ((tmpArray[0] ^ tmpArray[1] ^ tmpArray[2] ^ tmpArray[4] ^ tmpArray[6] ^ tmpArray[7]) != tmpArray[10]) ? 0x02 : 0x00;
			n |= ((tmpArray[0] ^ tmpArray[1] ^ tmpArray[2] ^ tmpArray[3] ^ tmpArray[5] ^ tmpArray[7] ^ tmpArray[8]) != tmpArray[11]) ? 0x04 : 0x00;
			n |= ((tmpArray[0] ^ tmpArray[2] ^ tmpArray[4] ^ tmpArray[5] ^ tmpArray[8]) != tmpArray[12]) ? 0x08 : 0x00;

			if (n < 16)
			{
				uint8_t bitLocation = BITS_LOOKUP[n];
				if (bitLocation != 0xFF)
				{
					tmpArray[bitLocation] = !tmpArray[bitLocation];
					hammingOK = true;
				}
			}

			if (hammingOK)
			{
				pos = j + 1;
				for (int k = 0; k < 13; k++)
				{
					BPTCDeInterleaved[pos] = tmpArray[k];
					pos += 15;
				}
				stillProcessing = true;
			}
		}

		for (int j = 0; j < 9; j++)
		{
			uint32_t pos = (j * 15) + 1;
			if (hammingDecodeType2(BPTCDeInterleaved + pos))
			{
				stillProcessing = true;
			}
		}
	}

	for (int range = 0; range < 9; range++)
	{
		for (uint32_t a = BPTC19696CopyRanges[range][0]; a <= BPTC19696CopyRanges[range][1]; a++, bitDataIndex++)
		{
			bitData[bitDataIndex] = BPTCDeInterleaved[a];
		}
	}

	for (int i = 0; i < LC_DATA_LENGTH; i++)
	{
		outputData[i] = BooleanBitsArrayToByte(bitData + (i << 3));
	}
}

static void BPTCencode(const uint8_t *inputData, uint8_t *outputData)
{
	uint8_t byteData;
	uint32_t bitDataPosition = 0;
	bool bitData[96];
	bool hammingBits[13];

	for (int i = 0; i < LC_DATA_LENGTH; i++)
	{
		byteToBooleanBitsArray(inputData[i], bitData + (i << 3));
	}

	memset(BPTCDeInterleaved, 0, 196 * sizeof(bool));

	for (int range = 0; range < 9; range++)
	{
		for (uint32_t a = BPTC19696CopyRanges[range][0]; a <= BPTC19696CopyRanges[range][1]; a++, bitDataPosition++)
		{
			BPTCDeInterleaved[a] = bitData[bitDataPosition];
		}
	}

	for (int i = 0; i < 9; i++)
	{
		hammingEncode(BPTCDeInterleaved + ((i * 15) + 1), false);
	}

	for (int i = 0; i < 15; i++)
	{
		int pos = i + 1;
		for (int j = 0; j < 13; j++)
		{
			hammingBits[j] = BPTCDeInterleaved[pos];
			pos += 15;
		}

		hammingBits[9]  = hammingBits[0] ^ hammingBits[1] ^ hammingBits[3] ^ hammingBits[5] ^ hammingBits[6];
		hammingBits[10] = hammingBits[0] ^ hammingBits[1] ^ hammingBits[2] ^ hammingBits[4] ^ hammingBits[6] ^ hammingBits[7];
		hammingBits[11] = hammingBits[0] ^ hammingBits[1] ^ hammingBits[2] ^ hammingBits[3] ^ hammingBits[5] ^ hammingBits[7] ^ hammingBits[8];
		hammingBits[12] = hammingBits[0] ^ hammingBits[2] ^ hammingBits[4] ^ hammingBits[5] ^ hammingBits[8];

		pos = i + 1;
		for (int j = 0; j < 13; j++)
		{
			BPTCDeInterleaved[pos] = hammingBits[j];
			pos += 15;
		}
	}

	for (int i = 0; i < 196; i++)
	{
		BPTCRaw[(i * 181) % 196] = BPTCDeInterleaved[i];// interleave
	}

	for (int i = 0; i < LC_DATA_LENGTH; i++)
	{
		outputData[i] = BooleanBitsArrayToByte(BPTCRaw + (i << 3));
	}

	byteData = BooleanBitsArrayToByte(BPTCRaw + 96);
	outputData[12] = (outputData[12] & 0x3F) | ((byteData >> 0) & 0xC0);
	outputData[20] = (outputData[20] & 0xFC) | ((byteData >> 4) & 0x03);

	for (int i = 0; i < 12; i++)
	{
		outputData[i + 21] = BooleanBitsArrayToByte(BPTCRaw + 100 + (i << 3));
	}
}

static void DMRLC2Bytes(const DMRLC_t *LC_DataInput, uint8_t *outputBytes)
{
	outputBytes[0] = (uint8_t)LC_DataInput->FLCO;

	if (LC_DataInput->PF)
	{
		outputBytes[0] |= 0x80;
	}
	if (LC_DataInput->R)
	{
		outputBytes[0] |= 0x40;
	}

	outputBytes[1] = LC_DataInput->FID;

	outputBytes[2] = LC_DataInput->options;

	outputBytes[3] = (LC_DataInput->dstId >> 16) & 0xFF;
	outputBytes[4] = (LC_DataInput->dstId >> 8) & 0xFF;
	outputBytes[5] = (LC_DataInput->dstId & 0xFF);

	outputBytes[6] = (LC_DataInput->srcId >> 16) & 0xFF;
	outputBytes[7] = (LC_DataInput->srcId >> 8) & 0xFF;
	outputBytes[8] = (LC_DataInput->srcId  & 0xFF);
}

static bool hammingDecodeType2(bool *inputOutputBooleanBitsArray)
{
	const uint8_t BITS_LOOKUP[16] = {0xFF, 11, 12, 8, 13, 5, 9, 3, 14, 0, 6, 1, 10, 7, 4, 2};
	uint8_t numBits = hammingGetBits(inputOutputBooleanBitsArray, false);

	if (numBits < 16)
	{
		uint8_t bitLocation = BITS_LOOKUP[numBits];
		if (bitLocation != 0xFF)
		{
			inputOutputBooleanBitsArray[bitLocation] = !inputOutputBooleanBitsArray[bitLocation];
			return true;
		}
	}

	return false;
}

static bool hammingDecodeType1(bool *inputOutputBooleanBitsArray)
{
	// 0xFF means don't use this value. Also Index 0 is never used, its only here to reduce the number of if's
	const uint8_t BITS_LOOKUP[32] = { 0xFF, 11, 12, 0xFF, 13, 0xFF, 0xFF, 3, 14, 0xFF, 0xFF, 1, 0xFF, 7, 4, 0xFF, 15, 0xFF, 0xFF, 8, 0xFF, 5, 9, 0xFF, 0xFF, 0, 6, 0xFF, 10, 0xFF ,0xFF, 2};

	uint8_t c = hammingGetBits(inputOutputBooleanBitsArray, true);
	if (c == 0)
	{
		return true;
	}

	if (c < 32)
	{
		uint8_t bitLocation = BITS_LOOKUP[c];
		if (bitLocation != 0xFF)
		{
			inputOutputBooleanBitsArray[bitLocation] = !inputOutputBooleanBitsArray[bitLocation];
			return true;
		}
	}

	return false;
}

static void hammingEncode(bool *inputOutputBooleanBitsArray,bool is16114)
{
	inputOutputBooleanBitsArray[11] = inputOutputBooleanBitsArray[0] ^ inputOutputBooleanBitsArray[1] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[3] ^ inputOutputBooleanBitsArray[5] ^ inputOutputBooleanBitsArray[7] ^ inputOutputBooleanBitsArray[8];
	inputOutputBooleanBitsArray[12] = inputOutputBooleanBitsArray[1] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[3] ^ inputOutputBooleanBitsArray[4] ^ inputOutputBooleanBitsArray[6] ^ inputOutputBooleanBitsArray[8] ^ inputOutputBooleanBitsArray[9];
	inputOutputBooleanBitsArray[13] = inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[3] ^ inputOutputBooleanBitsArray[4] ^ inputOutputBooleanBitsArray[5] ^ inputOutputBooleanBitsArray[7] ^ inputOutputBooleanBitsArray[9] ^ inputOutputBooleanBitsArray[10];
	inputOutputBooleanBitsArray[14] = inputOutputBooleanBitsArray[0] ^ inputOutputBooleanBitsArray[1] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[4] ^ inputOutputBooleanBitsArray[6] ^ inputOutputBooleanBitsArray[7] ^ inputOutputBooleanBitsArray[10];

	if (is16114)
	{
		inputOutputBooleanBitsArray[15] = inputOutputBooleanBitsArray[0] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[5] ^ inputOutputBooleanBitsArray[6] ^ inputOutputBooleanBitsArray[8] ^ inputOutputBooleanBitsArray[9] ^ inputOutputBooleanBitsArray[10];
	}
}

static uint8_t hammingGetBits(bool *inputOutputBooleanBitsArray, bool is16114)
{
	uint8_t n;

	n  = ((inputOutputBooleanBitsArray[0] ^ inputOutputBooleanBitsArray[1] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[3] ^ inputOutputBooleanBitsArray[5] ^ inputOutputBooleanBitsArray[7] ^ inputOutputBooleanBitsArray[8]) != inputOutputBooleanBitsArray[11]) ? 0x01 : 0x00;
	n |= ((inputOutputBooleanBitsArray[1] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[3] ^ inputOutputBooleanBitsArray[4] ^ inputOutputBooleanBitsArray[6] ^ inputOutputBooleanBitsArray[8] ^ inputOutputBooleanBitsArray[9]) != inputOutputBooleanBitsArray[12]) ? 0x02 : 0x00;
	n |= ((inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[3] ^ inputOutputBooleanBitsArray[4] ^ inputOutputBooleanBitsArray[5] ^ inputOutputBooleanBitsArray[7] ^ inputOutputBooleanBitsArray[9] ^ inputOutputBooleanBitsArray[10]) != inputOutputBooleanBitsArray[13]) ? 0x04 : 0x00;
	n |= ((inputOutputBooleanBitsArray[0] ^ inputOutputBooleanBitsArray[1] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[4] ^ inputOutputBooleanBitsArray[6] ^ inputOutputBooleanBitsArray[7] ^ inputOutputBooleanBitsArray[10]) != inputOutputBooleanBitsArray[14]) ? 0x08 : 0x00;

	if (is16114)
	{
		n |= ((inputOutputBooleanBitsArray[0] ^ inputOutputBooleanBitsArray[2] ^ inputOutputBooleanBitsArray[5] ^ inputOutputBooleanBitsArray[6] ^ inputOutputBooleanBitsArray[8] ^ inputOutputBooleanBitsArray[9] ^ inputOutputBooleanBitsArray[10]) != inputOutputBooleanBitsArray[15]) ? 0x10 : 0x00;
	}

	return n;
}

static void embeddedDataEncodeEmbeddedData(void)
{
	bool data[128];
	uint32_t arrayIndex = 0;

	uint32_t crc = CRC_encodeFiveBit(embeddedDataProcessed);

	memset(data, 0, 128 * sizeof(bool));

	data[106] = (crc & 0x01) == 0x01;
	data[90]  = (crc & 0x02) == 0x02;
	data[74]  = (crc & 0x04) == 0x04;
	data[58]  = (crc & 0x08) == 0x08;
	data[42]  = (crc & 0x10) == 0x10;

	for (int range = 0; range < 7; range++)
	{
		for (uint32_t i = embedddataCopyRanges[range][0]; i <= embedddataCopyRanges[range][1]; i++, arrayIndex++)
		{
			data[i] = embeddedDataProcessed[arrayIndex];
		}
	}

	for (int i = 0; i < 112; i += 16)
	{
		hammingEncode(data + i, true);
	}

	for (int i = 0; i < 16; i++)
	{
		data[i + 112] = data[i + 0] ^ data[i + 16] ^ data[i + 32] ^ data[i + 48] ^ data[i + 64] ^ data[i + 80] ^ data[i + 96];
	}

	arrayIndex = 0;
	for (int i = 0; i < 128; i++)
	{
		embeddedDataRaw[i] = data[arrayIndex];
		arrayIndex += 16;
		if (arrayIndex > 127)
		{
			arrayIndex -= 127;
		}
	}
}

static void embeddedDataDecodeEmbeddedData(void)
{
	uint32_t crc = 0;
	bool tmpBooleanBitsArray[128];
	int bitArrayIndex = 0;

	memset(tmpBooleanBitsArray, 0, 128 * sizeof(bool));

	for (int i = 0; i < 128; i++)
	{
		tmpBooleanBitsArray[bitArrayIndex] = embeddedDataRaw[i];
		bitArrayIndex += 16;
		if (bitArrayIndex > 127)
		{
			bitArrayIndex -= 127;
		}
	}

	for (int i = 0; i < 112; i += 16)
	{
		if (!hammingDecodeType1(tmpBooleanBitsArray + i))
		{
			return;
		}
	}

	// Check parity
	for (int i = 0; i < 16; i++)
	{
		bool parity = tmpBooleanBitsArray[i + 0] ^ tmpBooleanBitsArray[i + 16] ^ tmpBooleanBitsArray[i + 32] ^ tmpBooleanBitsArray[i + 48] ^ tmpBooleanBitsArray[i + 64] ^ tmpBooleanBitsArray[i + 80] ^ tmpBooleanBitsArray[i + 96] ^ tmpBooleanBitsArray[i + 112];
		if (parity)
		{
			return;
		}
	}

	bitArrayIndex = 0;

	for (int range = 0; range < 7; range++)
	{
		for (uint32_t i = embedddataCopyRanges[range][0]; i <= embedddataCopyRanges[range][1]; i++, bitArrayIndex++)
		{
			embeddedDataProcessed[bitArrayIndex] = tmpBooleanBitsArray[i];
		}
	}

	if (tmpBooleanBitsArray[42])
	{
		crc += 16;
	}

	if (tmpBooleanBitsArray[58])
	{
		crc += 8;
	}

	if (tmpBooleanBitsArray[74])
	{
		crc += 4;
	}

	if (tmpBooleanBitsArray[90])
	{
		crc += 2;
	}

	if (tmpBooleanBitsArray[106])
	{
		crc += 1;
	}

	if (crc != CRC_encodeFiveBit(embeddedDataProcessed))
	{
		return;
	}

	embeddedDataIsValid = true;

	uint8_t flco = BooleanBitsArrayToByte(embeddedDataProcessed + 0);
	embeddedDataFLCO = (int)(flco & 0x3F);
}

static uint32_t CRC_encodeFiveBit(const bool *in)
{
	uint32_t total = 0;

	for (int i = 0; i < 72; i += 8)
	{
		total += BooleanBitsArrayToByte(in + i);
	}

	total %= 31;

	return total;
}

static void byteToBooleanBitsArray(uint8_t byteIn, bool *bitsOut)
{
	for (int i = 0, shift = 7; i < 8; i++, shift--)
	{
		bitsOut[i] = (byteIn >> shift) & 0x01;
	}
}

static uint8_t BooleanBitsArrayToByte(const bool *bitsIn)
{
	uint8_t out = 0;
	for (int i = 0, shift = 7; i < 8; i++, shift--)
	{
		out  |= bitsIn[i] << shift;
	}
	return out;
}

void cwProcess(void)
{
	if (hotspotCwpoLen == 0)
	{
		return;
	}

	if (ticksTimerHasExpired(&cwNextPeriodTimer))
	{
		ticksTimerStart(&cwNextPeriodTimer, cwDOTDuration);

		bool b = READ_BIT1(cwBuffer, cwpoPtr);
		static bool lastValue = true;

		if (lastValue != b)
		{
			lastValue = b;

			if (b)
			{
				trxSetTone1(880);
			}
			else
			{
				trxSetTone1(0);
			}
		}

		cwpoPtr++;

		if (cwpoPtr >= hotspotCwpoLen)
		{
			cwpoPtr = 0;
			hotspotCwpoLen = 0;
			return;
		}
	}
}

void cwReset(void)
{
	hotspotCwpoLen = 0;
	cwpoPtr = 0;
}

static uint8_t handleCWID(const uint8_t *data, uint8_t length)
{
	cwReset();
	memset(cwBuffer, 0x00, sizeof(cwBuffer));

	hotspotCwpoLen = 6; // Silence at the beginning
	cwpoPtr = 0;

	for (uint8_t i = 0; i < length; i++)
	{
		for (uint8_t j = 0; CW_SYMBOL_LIST[j].c != 0; j++)
		{
			if (CW_SYMBOL_LIST[j].c == data[i])
			{
				uint32_t MASK = 0x80000000;

				for (uint8_t k = 0; k < CW_SYMBOL_LIST[j].length; k++, hotspotCwpoLen++, MASK >>= 1)
				{
					bool b = (CW_SYMBOL_LIST[j].pattern & MASK) == MASK;

					WRITE_BIT1(cwBuffer, hotspotCwpoLen, b);

					if (hotspotCwpoLen >= ((sizeof(cwBuffer) * 8) - 3)) // Will overflow otherwise
					{
						hotspotCwpoLen = 0;
						return 4;
					}
				}

				break;
			}
		}
	}

	// An empty message
	if (hotspotCwpoLen == 6)
	{
		hotspotCwpoLen = 0;
		return 4;
	}

	// Silence at the end
	hotspotCwpoLen += 3;

	return 0;
}




static void getStatus(void)
{
	uint8_t buf[16];

	// Send all sorts of interesting internal values
	buf[0]  = MMDVM_FRAME_START;
	buf[1]  = 13;
	buf[2]  = MMDVM_GET_STATUS;
	buf[3]  = (0x02 | 0x20); // DMR and POCSAG enabled
	buf[4]  = hotspotModemState;
	buf[5]  = ( ((hotspotState == HOTSPOT_STATE_TX_START_BUFFERING) ||
				(hotspotState == HOTSPOT_STATE_TRANSMITTING) ||
				(hotspotState == HOTSPOT_STATE_TX_SHUTDOWN)) ||
				hotspotCwKeying ) ? 0x01 : 0x00;

	if (hasRXOverflow())
	{
		buf[5] |= 0x04;
	}

	if (hasTXOverflow())
	{
		buf[5] |= 0x08;
	}

	buf[6]  = 0; // No DSTAR space

	buf[7]  = 10; // DMR Simplex
	buf[8]  = (HOTSPOT_BUFFER_COUNT - wavbuffer_count); // DMR space

	buf[9]  = 0; // No YSF space
	buf[10] = 0; // No P25 space
	buf[11] = 0; // no NXDN space
	buf[12] = 1; // virtual space for POCSAG

	if (!hotspotMmdvmHostIsConnected)
	{
		hotspotState = HOTSPOT_STATE_INITIALISE;
		hotspotMmdvmHostIsConnected = true;
		uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
	}

	enqueueUSBData(buf, buf[1]);
}

static uint8_t setConfig(const uint8_t *data, uint8_t length)
{
	if (length < 13)
	{
		return 4;
	}

	uint32_t tempTXDelay = (data[2] * 10); // MMDVMHost send in 10ms units, we use 1ms PIT counter unit

	if (tempTXDelay > 1000) // 1s limit
	{
		return 4;
	}

	hotspotTxDelay = tempTXDelay;

	// Only supported mode are DMR, CWID, POCSAG and IDLE
	switch (data[3])
	{
		case STATE_IDLE:
		case STATE_DMR:
		case STATE_CWID:
		case STATE_POCSAG:
			break;

		default:
			return 4;
			break;
	}

	hotspotModemState = (MMDVM_STATE)data[3];

	uint8_t tmpColorCode = data[6];

	if (tmpColorCode > 15)
	{
		return 4;
	}

	colorCode = tmpColorCode;
	trxSetDMRColourCode(colorCode);

	/* To Do
	 m_cwIdTXLevel = data[5]>>2;
     uint8_t dmrTXLevel    = data[10];
     io.setDeviations(dstarTXLevel, dmrTXLevel, ysfTXLevel, p25TXLevel, nxdnTXLevel, pocsagTXLevel, ysfLoDev);
     dmrDMOTX.setTXDelay(txDelay);
     */

	if ((hotspotModemState == STATE_DMR) && (hotspotState == HOTSPOT_STATE_NOT_CONNECTED))
	{
		hotspotState = HOTSPOT_STATE_INITIALISE;

		if (!hotspotMmdvmHostIsConnected)
		{
			hotspotMmdvmHostIsConnected = true;
			uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
		}
	}

	return 0;
}

static uint8_t setMode(const uint8_t *data, uint8_t length)
{
	if (length < 1)
	{
		return 4;
	}

	if (hotspotModemState == data[0])
	{
		return 0;
	}

	// Only supported mode are DMR, CWID, POCSAG and IDLE
	switch (data[0])
	{
		case STATE_IDLE:
		case STATE_DMR:
		case STATE_CWID:
		case STATE_POCSAG:
			break;

		default:
			return 4;
			break;
	}

	hotspotModemState = data[0];
#if 0
	// MMDVMHost seems to send setMode commands longer than 1 byte. This seems wrong according to the spec, so we ignore those.
	if (data[0] == STATE_IDLE || (length == 1 && data[0] == STATE_DMR) || data[0] != STATE_POCSAG)
	{
		hotspotModemState = data[0];
	}
#endif

#if 0
	// MMDVHost on the PC seems to send mode DMR when the transmitter should be turned on and IDLE when it should be turned off.
	switch(hotspotModemState)
	{
		case STATE_IDLE:
			//enableTransmission(false);
			break;
		case STATE_DMR:
			//enableTransmission(true);
			break;
		default:
			break;
	}
#endif

	if (!hotspotMmdvmHostIsConnected)
	{
		hotspotState = HOTSPOT_STATE_INITIALISE;
		hotspotMmdvmHostIsConnected = true;
		uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
	}

	return 0;
}

static void getVersion(void)
{
	char    buffer[80];
	uint8_t buf[128];
	uint8_t count = 0;

	buf[0]  = MMDVM_FRAME_START;
	buf[1]  = 0;
	buf[2]  = MMDVM_GET_VERSION;
	buf[3]  = PROTOCOL_VERSION;

	count = 4;

	snprintf(buffer, sizeof(buffer), "%s (Radio:%s, Mode:%s)", HARDWARE,
#if defined(PLATFORM_GD77)
			"GD-77"
#elif defined(PLATFORM_GD77S)
			"GD-77S"
#elif defined(PLATFORM_DM1801)
			"DM-1801"
#elif defined(PLATFORM_DM1801A)
			"DM-1801A"
#elif defined(PLATFORM_RD5R)
			"RD-5R"
#elif defined(PLATFORM_MD9600)
			"MD-9600"
#elif defined(PLATFORM_MDUV380)
			"MD-UV380"
#elif defined(PLATFORM_MD380)
			"MD-380"
#elif defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
			"DM-1701"
#else
			"Unknown"
#endif
			,(nonVolatileSettings.hotspotType == HOTSPOT_TYPE_MMDVM ? "MMDVM" : "BlueDV"));

	for (uint8_t i = 0; buffer[i] != 0x00; i++, count++)
	{
		buf[count] = buffer[i];
	}

	buf[1] = count;

	if (!hotspotMmdvmHostIsConnected)
	{
		hotspotState = HOTSPOT_STATE_INITIALISE;
		hotspotMmdvmHostIsConnected = true;
		uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
	}

	enqueueUSBData(buf, buf[1]);
}

static uint8_t handleDMRShortLC(const uint8_t *data, uint8_t length)
{
	////	uint8_t LCBuf[5];
	////	DMRShortLC_decode((uint8_t *) com_requestbuffer + 3,LCBuf);

	if (!hotspotMmdvmHostIsConnected)
	{
		hotspotState = HOTSPOT_STATE_INITIALISE;
		hotspotMmdvmHostIsConnected = true;
		uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
	}

	return 0;
}

static uint8_t setQSOInfo(const uint8_t *data, uint8_t length)
{
	if (length < (MMDVM_HEADER_LENGTH + 1))
	{
		return 4;
	}

	if (data[3] == 250) // IP info from MMDVMHost's CAST display driver
	{
		char buf[26]; // MMDVMHost use an array of 25
		char *pBuf = buf;
		char *pIface;

		memcpy(&buf, (char *)(data + MMDVM_HEADER_LENGTH), (length - MMDVM_HEADER_LENGTH));
		buf[(length - MMDVM_HEADER_LENGTH)] = 0;

		// Get rid of the interface name as it could be too large to fit in the screen.
		if ((pIface = strchr(pBuf, ':')) != NULL)
		{
			pBuf = pIface + 1;
		}

		snprintf(hotspotMmdvmQSOInfoIP, 22, "%s", pBuf);

		uiHotspotUpdateScreen(hotspotCurrentRxCommandState);

		return 0;
	}
	else if ((data[3] != STATE_DMR) && (data[3] != STATE_POCSAG)) // We just want DMR and POCSAG QSO info
	{
		return 4;
	}

	if (data[3] == STATE_DMR)
	{
		if (length != 47) // Check for QSO Info frame length
		{
			return 5;
		}

		// Source and destination are both fitted in 20 arrays, in MMDVMHost
		// We just store use and store source info
		char QSOInfo[21]; // QSO infos are array[20]
		char source[21];
		char *p;
		uint8_t len;

		memcpy(&source, (char *)(data + 5), 20);
		source[20] = 0;

		memset(&QSOInfo, 0, sizeof(QSOInfo));
		sprintf(QSOInfo, "%s", chomp(source));

		len = strlen(QSOInfo);

		// Keep the callsign only.
		if ((p = strchr(QSOInfo, ' ')) != NULL)
		{
			*p = 0;

			// zeroing
			p++;
			for (uint8_t i = 0; i < (len - (p - QSOInfo)); i++)
			{
				*(p + i) = 0;
			}

			len = strlen(QSOInfo);
		}

		// Non empty string, check if it's not numerical (TG/PC), as it will be ignored
		if (len > 0)
		{
			bool onlyDigits = true;

			for (uint8_t i = 0; i < len; i++)
			{
				if (isalpha((int)QSOInfo[i]))
				{
					onlyDigits = false;
					break;
				}
			}

			if (onlyDigits)
			{
				overriddenLCAvailable = false;
				return 0;
			}

			// Build fake TA (2 blocks max)
			memset(&overriddenLCTA, 0, sizeof(overriddenLCTA));

			overriddenLCTA[0]  = 0x04;
			overriddenLCTA[2]  = (0x01 << 6) | (len << 1);
			overriddenLCTA[9]  = 0x05;

			p = QSOInfo;
			for (uint8_t i = 0; i < 2; i++) // 2 blocks only, that enough for store a callsign
			{
				memcpy(&overriddenLCTA[(i * 9) + ((i == 0) ? 3 : 2)], p, ((i == 0) ? 6 : 7));

				p += ((i == 0) ? 6 : 7);
			}

			overriddenBlocksTA = 0x03; // 2 blocks are now available
		}

		overriddenLCAvailable = (len > 0);
	}
	else if (data[3] == STATE_POCSAG)
	{
		if ((length - 11) > (17 + 4))
		{
			for (int8_t i = 0; i < (((length - 11) / (17 + 4)) - 1); i++)
			{
				sendACK(data[2]);
			}
		}
	}

	return 0;
}


#if 0
static uint8_t handlePOCSAG(const uint8_t *data, uint8_t length)
{
	return 0;
}
#endif
void handleHotspotRequest(void)
{
	mmdvmHostLastActiveTime = ticksGetMillis(); // MMDVMHost sign of life.

	// Rebuild the MMDVMHost frame stored in the USB (circular) buffer
	uint8_t currentFrame[256]; // Absolute max frame length that MMDVMHost can send (+1)
	uint8_t frameLength = (com_requestbuffer[comRecvMMDVMIndexOut++] - 1); // Remove our data block length header

	// Make sure the index stays within limits.
	if (comRecvMMDVMIndexOut >= COM_REQUESTBUFFER_SIZE)
	{
		comRecvMMDVMIndexOut = 0;
	}

	// Extract the MMDVMHost frame
	for (uint8_t i = 0; i < frameLength; i++)
	{
		currentFrame[i] = com_requestbuffer[comRecvMMDVMIndexOut++];

		// Make sure the index stays within limits.
		if (comRecvMMDVMIndexOut >= COM_REQUESTBUFFER_SIZE)
		{
			comRecvMMDVMIndexOut = 0;
		}
	}

	// Make sure the index stays within limits.
	if (comRecvMMDVMIndexOut >= COM_REQUESTBUFFER_SIZE)
	{
		comRecvMMDVMIndexOut = 0;
	}

	// Decrement the frame counter
	comRecvMMDVMFrameCount--;

	// Something went wrong, resync to the RX buffer
	if (comRecvMMDVMFrameCount < 0)
	{
		comRecvMMDVMIndexIn = comRecvMMDVMIndexOut = 0; // Resync, it may skip few frames.
		comRecvMMDVMFrameCount = 0;
	}

	// Handle the frame, if valid.
	if (currentFrame[0] == MMDVM_FRAME_START)
	{
		uint8_t err = 2;

		switch(currentFrame[2])
		{
			case MMDVM_GET_STATUS:
				getStatus();
				break;

			case MMDVM_GET_VERSION:
				getVersion();
				break;

			case MMDVM_SET_CONFIG:
				err = setConfig(currentFrame + 3, frameLength - 3);
				if (err == 0)
				{
					sendACK(currentFrame[2]);
					uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;

			case MMDVM_SET_MODE:
			{
				MMDVM_STATE prevState = hotspotModemState;

				err = setMode(currentFrame + 3, frameLength - 3);

				if (((prevState == STATE_POCSAG) && (hotspotModemState != STATE_POCSAG)) ||
						((prevState != STATE_POCSAG) && (hotspotModemState == STATE_POCSAG)))
				{
					uiHotspotUpdateScreen(HOTSPOT_RX_IDLE); // refresh screen
				}

				if (err == 0)
				{
					sendACK(currentFrame[2]);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
			}
			break;

			case MMDVM_SET_FREQ:
				err = setFreq(currentFrame + 3, frameLength - 3);
				if (err == 0)
				{
					sendACK(currentFrame[2]);
					uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;


			case MMDVM_SEND_CWID:
				err = 5;
				if (hotspotModemState == STATE_IDLE)
				{
					err = handleCWID(currentFrame + 3, frameLength - 3);
				}

				if (err == 0)
				{
					hotspotCwKeying = true;
					ticksTimerStart(&cwNextPeriodTimer, hotspotTxDelay);
					sendACK(currentFrame[2]);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;

			case MMDVM_DMR_DATA2:
				// It seems BlueDV under Windows forget to set correct mode
				// when it connect a TG with an already running QSO. So we force
				// the modemState and init the HS.
				if ((nonVolatileSettings.hotspotType == HOTSPOT_TYPE_BLUEDV) && (hotspotModemState == STATE_IDLE))
				{
					hotspotModemState = STATE_DMR;
				}

				err = hotspotModeReceiveNetFrame(currentFrame, 2);
				if (err == 0)
				{
					sendACK(currentFrame[2]);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;

			case MMDVM_DMR_START: // Only for duplex
				sendACK(currentFrame[2]);
				break;

			case MMDVM_DMR_SHORTLC:
				err = handleDMRShortLC(currentFrame, frameLength);
				if (err == 0)
				{
					sendACK(currentFrame[2]);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;

			case MMDVM_DMR_ABORT: // Only for duplex
				sendACK(currentFrame[2]);
				break;

			case MMDVM_DSTAR_HEADER:
			case MMDVM_DSTAR_DATA:
			case MMDVM_DSTAR_EOT:
			case MMDVM_YSF_DATA:
			case MMDVM_P25_HDR:
			case MMDVM_P25_LDU:
			case MMDVM_NXDN_DATA:
			case MMDVM_DMR_DATA1:
			case MMDVM_CAL_DATA:
				sendNAK(currentFrame[2], err);
				break;

			case MMDVM_POCSAG_DATA:
				if ((hotspotModemState == STATE_IDLE) || (hotspotModemState == STATE_POCSAG))
				{
					//err = handlePOCSAG(currentFrame + 3, frameLength - 3);
					err = 0;
				}

				if (err == 0)
				{
					sendACK(currentFrame[2]); // We don't send pages, but POCSAG can be enabled in Pi-Star
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;

			case MMDVM_TRANSPARENT: // Do nothing, stay silent
				break;

			case MMDVM_QSO_INFO:
				err = setQSOInfo(currentFrame, frameLength);
				if ((err == 0) || (err == 4) /* non DMR mode, ignored */)
				{
					sendACK(currentFrame[2]);
				}
				else
				{
					sendNAK(currentFrame[2], err);
				}
				break;

			default:
				sendNAK(currentFrame[2], 1);
				break;
		}
	}
	else
	{
		// Invalid MMDVM header byte: resync (it may skip few frames).
		comRecvMMDVMIndexIn = comRecvMMDVMIndexOut = 0;
		comRecvMMDVMFrameCount = 0;
	}

	if ((uiDataGlobal.displayQSOState == QSO_DISPLAY_CALLER_DATA) || (uiDataGlobal.displayQSOState == QSO_DISPLAY_CALLER_DATA_UPDATE))
	{
		uiHotspotUpdateScreen(hotspotCurrentRxCommandState);
		uiDataGlobal.displayQSOState = QSO_DISPLAY_IDLE;
	}

	if (hotspotCwKeying)
	{
		if (!trxTransmissionEnabled)
		{
			// Start TX CWID, prepare for ANALOG
			if (trxGetMode() != RADIO_MODE_ANALOG)
			{
				trxSetModeAndBandwidth(RADIO_MODE_ANALOG, false);
				trxSetTxCSS(CODEPLUG_CSS_TONE_NONE);
			}

			HRC6000ClearIsWakingState();
			trxSetTone1(0);
			trxEnableTransmission();

#if defined(CPU_MK22FN512VLL12)
			trxSelectVoiceChannel(AT1846_VOICE_CHANNEL_TONE1);
			enableAudioAmp(AUDIO_AMP_MODE_RF);
			GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 1);
#elif defined(PLATFORM_MD9600)
			// As the HRC6000 tone system is made, we can't set more than
			// one trxSetTone1() call without a trxDTMFoff() in the middle, as some
			// configuration values need to be restored each time.
			// So, grabbing the original values (after the first trxSetTone1() call permits
			// us to restore these settings after the CW ID beaconing is done.
			//
			// Get the previous Tone1 config (collected by HRC6000SendTone().
			HRC6000GetTone1Config(&tone1Config);
#endif

			uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
		}

		// CWID has been TXed, restore DIGITAL
		if (hotspotCwpoLen == 0)
		{
			trxDisableTransmission();

			if (trxTransmissionEnabled)
			{
				trxTransmissionEnabled = false;
				trxIsTransmitting = false;
				PTTToggledDown = false;

				if (trxGetMode() == RADIO_MODE_ANALOG)
				{
					trxSetModeAndBandwidth(RADIO_MODE_DIGITAL, false);

#if defined(CPU_MK22FN512VLL12)
					trxSelectVoiceChannel(AT1846_VOICE_CHANNEL_MIC);
					disableAudioAmp(AUDIO_AMP_MODE_RF);
#elif defined(PLATFORM_MD9600)
					// Restore original Tone1 config, otherwise DIGITAL won't work until power-cycling.
					HRC6000SetTone1Config(&tone1Config);
					trxDTMFoff(true); // Apply restored Tone1 config.
#endif
				}
			}

			hotspotCwKeying = false;
			uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
		}
	}
}


// Queue system is a single byte header containing the length of the item, followed by the data
// if the block won't fit in the space between the current write location and the end of the buffer,
// a zero is written to the length for that block and the data and its length byte is put at the beginning of the buffer
void enqueueUSBData(uint8_t *data, uint8_t length)
{
	if ((usbComSendBufWritePosition + (length + 1)) > (COM_BUFFER_SIZE - 1))
	{
		usbComSendBuf[usbComSendBufWritePosition] = 0xFF; // flag that the data block won't fit and will be put at the start of the buffer
		usbComSendBufWritePosition = 0;
	}

	usbComSendBuf[usbComSendBufWritePosition] = length;
	memcpy((uint8_t *)&usbComSendBuf[usbComSendBufWritePosition + 1], data, length);
	usbComSendBufWritePosition += (length + 1);
	usbComSendBufCount++;

	if (length <= 1)
	{
		// Blah
	}

}

void processUSBDataQueue(void)
{
	if (usbComSendBufCount > 0)
	{
		if (usbComSendBuf[usbComSendBufReadPosition] == 0xFF) // End marker
		{
			usbComSendBuf[usbComSendBufReadPosition] = 0;
			usbComSendBufReadPosition = 0;
		}

		uint8_t len = usbComSendBuf[usbComSendBufReadPosition] + 1;

		if (len < (3 + 1)) // the shortest MMDVM frame length (3 = DMRLost)
		{
			usbComSendBufCount--;
		}
		else
		{
#if defined(STM32F405xx)
			uint8_t status = CDC_Transmit_FS((uint8_t *)&usbComSendBuf[usbComSendBufReadPosition + 1], usbComSendBuf[usbComSendBufReadPosition]);

			if (status == USBD_OK)
#else
			usb_status_t status = USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, &usbComSendBuf[usbComSendBufReadPosition + 1], usbComSendBuf[usbComSendBufReadPosition]);

			if (status == kStatus_USB_Success)
#endif
			{
				usbComSendBufReadPosition += len;

				if (usbComSendBufReadPosition >= (COM_BUFFER_SIZE - 1)) // reaching the end of the buffer
				{
					usbComSendBufReadPosition = 0;
				}

				usbComSendBufCount--;
			}
			else
			{
				// USB Send Fail
			}
		}
	}
}

static void swapWithFakeTA(uint8_t *lc)
{
	if ((lc[0] >= DMR_EMBEDDED_DATA_TALKER_ALIAS_HEADER) && (lc[0] < DMR_EMBEDDED_DATA_TALKER_ALIAS_BLOCK2))
	{
		uint8_t blockID = lc[0] - 4;

		if ((overriddenBlocksTA & (1 << blockID)) != 0)
		{
			// Clear the bit as it was consumed
			overriddenBlocksTA &= ~(1 << blockID);

			memcpy(lc, overriddenLCTA + (blockID * 9), 9);
		}
	}
}

static void setRSSIToFrame(uint8_t *frameData)
{
	frameData[DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH]     = 0;
	frameData[DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 1] = radioDevices[RADIO_DEVICE_PRIMARY].trxRxSignal;
}

static bool hotspotSendVoiceFrame(volatile const uint8_t *receivedDMRDataAndAudio)
{
	if ((hotspotRxedDMR_LC.srcId == 0) || (hotspotRxedDMR_LC.dstId == 0))
	{
		return false;
	}

	uint8_t frameData[DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 2] = {MMDVM_FRAME_START, (DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 2), MMDVM_DMR_DATA2};
	uint8_t embData[DMR_FRAME_LENGTH_BYTES];
	uint8_t sequenceNumber = receivedDMRDataAndAudio[AMBE_AUDIO_LENGTH + LC_DATA_LENGTH + 1] - 1;

	// copy the audio sections
	memcpy(frameData + MMDVM_HEADER_LENGTH, (uint8_t *)receivedDMRDataAndAudio + LC_DATA_LENGTH, 14);
	memcpy(frameData + MMDVM_HEADER_LENGTH + EMBEDDED_DATA_OFFSET + 6, (uint8_t *)receivedDMRDataAndAudio + LC_DATA_LENGTH + EMBEDDED_DATA_OFFSET, 14);

	if (sequenceNumber == 0)
	{
		frameData[3] = MMDVM_VOICE_SYNC_PATTERN;// sequence 0
		for (uint8_t i = 0; i < 7; i++)
		{
			frameData[i + EMBEDDED_DATA_OFFSET + MMDVM_HEADER_LENGTH] = (frameData[i + EMBEDDED_DATA_OFFSET + MMDVM_HEADER_LENGTH] & ~SYNC_MASK[i]) | MS_SOURCED_AUDIO_SYNC[i];
		}
	}
	else
	{
		frameData[3] = sequenceNumber;
		embeddedDataGetData(sequenceNumber, embData);
		for (uint8_t i = 0; i < 7; i++)
		{
			frameData[i + EMBEDDED_DATA_OFFSET + MMDVM_HEADER_LENGTH] = (frameData[i + EMBEDDED_DATA_OFFSET + MMDVM_HEADER_LENGTH] & ~DMR_AUDIO_SEQ_MASK[i]) | DMR_AUDIO_SEQ_SYNC[sequenceNumber][i];
			frameData[i + EMBEDDED_DATA_OFFSET + MMDVM_HEADER_LENGTH] = (frameData[i + EMBEDDED_DATA_OFFSET + MMDVM_HEADER_LENGTH] & ~DMR_EMBED_SEQ_MASK[i]) | embData[i + EMBEDDED_DATA_OFFSET];
		}
	}

	// Add RSSI into frame
	setRSSIToFrame(frameData);

	enqueueUSBData(frameData, frameData[1]);

	return true;
}

static bool sendVoiceHeaderLC_Frame(volatile const uint8_t *receivedDMRDataAndAudio)
{
	uint8_t frameData[DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 2] = {MMDVM_FRAME_START, (DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 2), MMDVM_DMR_DATA2, DMR_SYNC_DATA | DT_VOICE_LC_HEADER};
	DMRLC_t lc;

	hotspotRxedDMR_LC.srcId = 0;
	hotspotRxedDMR_LC.dstId = 0;
	memset(&lc, 0, sizeof(DMRLC_t));// clear automatic variable
	lc.srcId = (receivedDMRDataAndAudio[6] << 16) + (receivedDMRDataAndAudio[7] << 8) + (receivedDMRDataAndAudio[8] << 0);
	lc.dstId = (receivedDMRDataAndAudio[3] << 16) + (receivedDMRDataAndAudio[4] << 8) + (receivedDMRDataAndAudio[5] << 0);
	lc.FLCO = receivedDMRDataAndAudio[0];// Private or group call

	if ((lc.srcId == 0) || (lc.dstId == 0))
	{
		return false;
	}

	// Encode the src and dst Ids etc
	if (!DMRFullLC_encode(&lc, frameData + MMDVM_HEADER_LENGTH, DT_VOICE_LC_HEADER)) // Encode the src and dst Ids etc
	{
		return false;
	}

	memcpy(&hotspotRxedDMR_LC, &lc, sizeof(DMRLC_t));

	embeddedDataSetLC(&lc);
	for (uint8_t i = 0; i < 8; i++)
	{
		frameData[i + LC_DATA_LENGTH + MMDVM_HEADER_LENGTH] = (frameData[i + LC_DATA_LENGTH + MMDVM_HEADER_LENGTH] & ~LC_SYNC_MASK_FULL[i]) | VOICE_LC_SYNC_FULL[i];
	}

	// Add RSSI into frame
	setRSSIToFrame(frameData);

	enqueueUSBData(frameData, frameData[1]);

	return true;
}

static void sendTerminator_LC_Frame(volatile const uint8_t *receivedDMRDataAndAudio)
{
	uint8_t frameData[DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 2] = {MMDVM_FRAME_START, (DMR_FRAME_LENGTH_BYTES + MMDVM_HEADER_LENGTH + 2), MMDVM_DMR_DATA2, DMR_SYNC_DATA | DT_TERMINATOR_WITH_LC};
	DMRLC_t lc;

	memset(&lc, 0, sizeof(DMRLC_t));
	lc.srcId = hotspotRxedDMR_LC.srcId;
	lc.dstId = hotspotRxedDMR_LC.dstId;

	// Encode the src and dst Ids etc
	if (!DMRFullLC_encode(&lc, frameData + MMDVM_HEADER_LENGTH, DT_TERMINATOR_WITH_LC))
	{
		return;
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		frameData[i + LC_DATA_LENGTH + MMDVM_HEADER_LENGTH] = (frameData[i + LC_DATA_LENGTH + MMDVM_HEADER_LENGTH] & ~LC_SYNC_MASK_FULL[i]) | TERMINATOR_LC_SYNC_FULL[i];
	}

	// Add RSSI into frame
	setRSSIToFrame(frameData);

	enqueueUSBData(frameData, frameData[1]);
}

void hotspotRxFrameHandler(uint8_t* frameBuf) // It's called by and ISR in HRC-6000 code.
{
	memcpy((uint8_t *)&audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufWriteIdx], frameBuf, AMBE_AUDIO_LENGTH + LC_DATA_LENGTH + 2);// 27 audio + 0x0c header + 2 hotspot signalling bytes
	rfFrameBufCount++;
	rfFrameBufWriteIdx = ((rfFrameBufWriteIdx + 1) % HOTSPOT_BUFFER_COUNT);
}

static bool getEmbeddedData(volatile const uint8_t *comBuffer)
{
	uint8_t         lcss;
	uint8_t         DMREMB[2];
	// the following is used for fake TA
	static uint32_t oldTrxDMRID = 0;
	static uint8_t  rawDataCount = 0;
	static uint8_t  fakeTABlockID = DMR_EMBEDDED_DATA_TALKER_ALIAS_HEADER;
	static bool     sendFakeTA = true;

	DMREMB[0]  = (comBuffer[MMDVM_HEADER_LENGTH + 13] << 4) & 0xF0;
	DMREMB[0] |= (comBuffer[MMDVM_HEADER_LENGTH + 14] >> 4) & 0x0F;
	DMREMB[1]  = (comBuffer[MMDVM_HEADER_LENGTH + 18] << 4) & 0xF0;
	DMREMB[1] |= (comBuffer[MMDVM_HEADER_LENGTH + 19] >> 4) & 0x0F;

//	m_colorCode = (DMREMB[0] >> 4) & 0x0F;
//	m_PI        = (DMREMB[0] & 0x08) == 0x08;

	lcss = (DMREMB[0] >> 1) & 0x03;

	if (startedEmbeddedSearch == false)
	{
		embeddedDataBuffersInt();
		startedEmbeddedSearch = true;
	}

	if (embeddedDataAddData((uint8_t *)comBuffer + MMDVM_HEADER_LENGTH, lcss))
	{
		bool res = embeddedDataGetRawData(hotspotTxLC);

		if (res)
		{
			if (overriddenLCAvailable) // We can send fake talker aliases.
			{
				if (trxDMRID != oldTrxDMRID)
				{
					// reset counters
					rawDataCount = 0;
					sendFakeTA = true;
					fakeTABlockID = DMR_EMBEDDED_DATA_TALKER_ALIAS_HEADER;
					oldTrxDMRID = trxDMRID;
				}

				if (sendFakeTA)
				{
					if ((hotspotTxLC[0] >= DMR_EMBEDDED_DATA_TALKER_ALIAS_HEADER) && (hotspotTxLC[0] <= DMR_EMBEDDED_DATA_TALKER_ALIAS_BLOCK3))
					{
						sendFakeTA = false;
						rawDataCount = 0;
						fakeTABlockID = DMR_EMBEDDED_DATA_TALKER_ALIAS_HEADER;
						overriddenLCAvailable = false;
					}
					else
					{
						rawDataCount++;

						if (rawDataCount > 4)
						{
							hotspotTxLC[0] = fakeTABlockID;

							swapWithFakeTA(&hotspotTxLC[0]);

							// Update LH with fake TA
							lastHeardListUpdate(hotspotTxLC, true);

							fakeTABlockID++;

							// We just send 2 fake blocks, as it only contains callsign
							if (fakeTABlockID == DMR_EMBEDDED_DATA_TALKER_ALIAS_BLOCK2)
							{
								rawDataCount = 0;
								sendFakeTA = false;
								fakeTABlockID = DMR_EMBEDDED_DATA_TALKER_ALIAS_HEADER;
								oldTrxDMRID = trxDMRID;
								overriddenLCAvailable = false;
							}
						}
					}
				}
			}
			else
			{
				lastHeardListUpdate(hotspotTxLC, true);
			}
		}
		startedEmbeddedSearch = false;
	}

	return false;
}

static void storeNetFrame(volatile const uint8_t *comBuffer)
{
	bool foundEmbedded;

	if (memcmp((uint8_t *)&comBuffer[18], END_FRAME_PATTERN, 6) == 0)
	{
		return;
	}

	if (memcmp((uint8_t *)&comBuffer[18], START_FRAME_PATTERN, 6) == 0)
	{
		return;
	}

	foundEmbedded = getEmbeddedData(comBuffer);

	if ((foundEmbedded || (nonVolatileSettings.hotspotType == HOTSPOT_TYPE_BLUEDV)) &&
			(hotspotTxLC[0] == TG_CALL_FLAG || hotspotTxLC[0] == PC_CALL_FLAG) &&
			(hotspotState != HOTSPOT_STATE_TX_START_BUFFERING && hotspotState != HOTSPOT_STATE_TRANSMITTING))
	{
		timeoutCounter = TX_BUFFERING_TIMEOUT;// set buffering timeout
		hotspotState = HOTSPOT_STATE_TX_START_BUFFERING;
	}

	if (hotspotState == HOTSPOT_STATE_TRANSMITTING ||
		hotspotState == HOTSPOT_STATE_TX_SHUTDOWN  ||
		hotspotState == HOTSPOT_STATE_TX_START_BUFFERING)
	{
		if (wavbuffer_count >= HOTSPOT_BUFFER_COUNT)
		{
			// Buffer overflow
		}

		taskENTER_CRITICAL();
		memcpy((uint8_t *)&audioAndHotspotDataBuffer.hotspotBuffer[wavbuffer_write_idx][LC_DATA_LENGTH], (uint8_t *)comBuffer + 4, 13);//copy the first 13, whole bytes of audio
		audioAndHotspotDataBuffer.hotspotBuffer[wavbuffer_write_idx][LC_DATA_LENGTH + 13] = (comBuffer[17] & 0xF0) | (comBuffer[23] & 0x0F);
		memcpy((uint8_t *)&audioAndHotspotDataBuffer.hotspotBuffer[wavbuffer_write_idx][LC_DATA_LENGTH + 14], (uint8_t *)&comBuffer[24], 13);//copy the last 13, whole bytes of audio

		memcpy((uint8_t *)&audioAndHotspotDataBuffer.hotspotBuffer[wavbuffer_write_idx], hotspotTxLC, 9);// copy the current LC into the data (mainly for use with the embedded data);
		wavbuffer_count++;
		wavbuffer_write_idx = ((wavbuffer_write_idx + 1) % HOTSPOT_BUFFER_COUNT);
		taskEXIT_CRITICAL();
	}
}

static uint8_t hotspotModeReceiveNetFrame(const uint8_t *comBuffer, uint8_t timeSlot)
{
	DMRLC_t lc;

	if (!hotspotMmdvmHostIsConnected)
	{
		hotspotState = HOTSPOT_STATE_INITIALISE;
		hotspotMmdvmHostIsConnected = true;
		uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
	}

	netRXDataTimer = RX_NET_FRAME_TIMEOUT;

	lc.srcId = 0;// zero these values as they are checked later in the function, but only updated if the data type is DT_VOICE_LC_HEADER
	lc.dstId = 0;

	voiceLCHeaderDecode((uint8_t *)comBuffer + MMDVM_HEADER_LENGTH, DT_VOICE_LC_HEADER, &lc);// Need to decode the frame to get the source and destination

	// update the src and destination ID's if valid
	if 	((lc.srcId != 0) && (lc.dstId != 0))
	{
		trxTalkGroupOrPcId = lc.dstId | (lc.FLCO << 24);
		trxDMRID = lc.srcId;

		if (hotspotState != HOTSPOT_STATE_TX_START_BUFFERING)
		{
			memcpy(hotspotTxLC, lc.rawData, 9);//Hotspot uses LC Data bytes rather than the src and dst ID's for the embed data

			lastHeardListUpdate(hotspotTxLC, true);

			// the Src and Dst Id's have been sent, and we are in RX mode then an incoming Net normally arrives next
			timeoutCounter = TX_BUFFERING_TIMEOUT;
			hotspotState = HOTSPOT_STATE_TX_START_BUFFERING;
		}
	}
	else
	{
		storeNetFrame(comBuffer);
	}

	return 0;
}

#if defined(MMDVM_SEND_DEBUG)
#warning MMDVM_SEND_DEBUG is defined
void mmdvmSendDebug1(const char *text)
{
	uint8_t buf[130];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 0;
	buf[2] = MMDVM_DEBUG1;

	uint8_t count = 3;
	for (uint8_t i = 0; text[i] != '\0'; i++, count++)
		buf[count] = text[i];

	buf[1] = count;

	enqueueUSBData(buf, buf[1]);
}

void mmdvmSendDebug2(const char *text, int16_t n1)
{
	uint8_t buf[130];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 0;
	buf[2] = MMDVM_DEBUG2;

	uint8_t count = 3;
	for (uint8_t i = 0; text[i] != '\0'; i++, count++)
		buf[count] = text[i];

	buf[count++] = (n1 >> 8) & 0xFF;
	buf[count++] = (n1 >> 0) & 0xFF;

	buf[1] = count;

	enqueueUSBData(buf, buf[1]);
}

void mmdvmSendDebug3(const char *text, int16_t n1, int16_t n2)
{
	uint8_t buf[130];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 0;
	buf[2] = MMDVM_DEBUG3;

	uint8_t count = 3;
	for (uint8_t i = 0; text[i] != '\0'; i++, count++)
		buf[count] = text[i];

	buf[count++] = (n1 >> 8) & 0xFF;
	buf[count++] = (n1 >> 0) & 0xFF;

	buf[count++] = (n2 >> 8) & 0xFF;
	buf[count++] = (n2 >> 0) & 0xFF;

	buf[1] = count;

	enqueueUSBData(buf, buf[1]);
}

void mmdvmSendDebug4(const char *text, int16_t n1, int16_t n2, int16_t n3)
{
	uint8_t buf[130];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 0;
	buf[2] = MMDVM_DEBUG4;

	uint8_t count = 3;
	for (uint8_t i = 0; text[i] != '\0'; i++, count++)
		buf[count] = text[i];

	buf[count++] = (n1 >> 8) & 0xFF;
	buf[count++] = (n1 >> 0) & 0xFF;

	buf[count++] = (n2 >> 8) & 0xFF;
	buf[count++] = (n2 >> 0) & 0xFF;

	buf[count++] = (n3 >> 8) & 0xFF;
	buf[count++] = (n3 >> 0) & 0xFF;

	buf[1] = count;

	enqueueUSBData(buf, buf[1]);
}

void mmdvmSendDebug5(const char *text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
	uint8_t buf[130];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 0;
	buf[2] = MMDVM_DEBUG5;

	uint8_t count = 3;
	for (uint8_t i = 0; text[i] != '\0'; i++, count++)
		buf[count] = text[i];

	buf[count++] = (n1 >> 8) & 0xFF;
	buf[count++] = (n1 >> 0) & 0xFF;

	buf[count++] = (n2 >> 8) & 0xFF;
	buf[count++] = (n2 >> 0) & 0xFF;

	buf[count++] = (n3 >> 8) & 0xFF;
	buf[count++] = (n3 >> 0) & 0xFF;

	buf[count++] = (n4 >> 8) & 0xFF;
	buf[count++] = (n4 >> 0) & 0xFF;

	buf[1] = count;

	enqueueUSBData(buf, buf[1]);
}
#endif

static void sendDMRLost(void)
{
	uint8_t buf[3];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 3;
	buf[2] = MMDVM_DMR_LOST2;

	enqueueUSBData(buf, buf[1]);
}

static void sendACK(uint8_t cmd)
{
	uint8_t buf[4];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 4;
	buf[2] = MMDVM_ACK;
	buf[3] = cmd;

	enqueueUSBData(buf, buf[1]);
}

static void sendNAK(uint8_t cmd, uint8_t err)
{
	uint8_t buf[5];

	buf[0] = MMDVM_FRAME_START;
	buf[1] = 5;
	buf[2] = MMDVM_NAK;
	buf[3] = cmd;
	buf[4] = err;

	enqueueUSBData(buf, buf[1]);
}

void hotspotStateMachine(void)
{
	static uint32_t rxFrameTime = 0;

	switch(hotspotState)
	{
		case HOTSPOT_STATE_NOT_CONNECTED:
			// do nothing
			lastRxState = HOTSPOT_RX_UNKNOWN;

			// force immediate shutdown of Tx if we get here and the tx is on for some reason.
			if (trxTransmissionEnabled)
			{
				trxTransmissionEnabled = false;
				trxDisableTransmission();
			}

			rfFrameBufCount = 0;
			if (hotspotMmdvmHostIsConnected)
			{
				hotspotState = HOTSPOT_STATE_INITIALISE;
			}
			else
			{
				if ((nonVolatileSettings.hotspotType == HOTSPOT_TYPE_MMDVM) &&
						((ticksGetMillis() - mmdvmHostLastActiveTime) > MMDVMHOST_TIMEOUT))
				{
					wavbuffer_count = 0;

					hotspotExit();
					break;
				}
			}
			break;

		case HOTSPOT_STATE_INITIALISE:
			wavbuffer_read_idx = 0;
			wavbuffer_write_idx = 0;
			wavbuffer_count = 0;
			rfFrameBufCount = 0;

			overriddenLCAvailable = false;

			hotspotState = HOTSPOT_STATE_RX_START;
			break;

		case HOTSPOT_STATE_RX_START:
			// force immediate shutdown of Tx if we get here and the tx is on for some reason.
			if (trxTransmissionEnabled)
			{
				trxTransmissionEnabled = false;
				trxDisableTransmission();
				uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
			}

			rxLCFrameSent = false;
			wavbuffer_read_idx = 0;
			wavbuffer_write_idx = 0;
			wavbuffer_count = 0;
			rxFrameTime = ticksGetMillis();

			hotspotState = HOTSPOT_STATE_RX_PROCESS;
			break;

		case HOTSPOT_STATE_RX_PROCESS:
			if (hotspotMmdvmHostIsConnected)
			{
				// No activity from MMDVMHost
				if ((nonVolatileSettings.hotspotType == HOTSPOT_TYPE_MMDVM) &&
						((ticksGetMillis() - mmdvmHostLastActiveTime) > MMDVMHOST_TIMEOUT))
				{
					hotspotMmdvmHostIsConnected = false;
					hotspotState = HOTSPOT_STATE_NOT_CONNECTED;
					rfFrameBufCount = 0;
					wavbuffer_count = 0;

					hotspotExit();
					break;
				}
			}
			else
			{
				hotspotState = HOTSPOT_STATE_NOT_CONNECTED;
				rfFrameBufCount = 0;
				wavbuffer_count = 0;

				if (trxTransmissionEnabled)
				{
					trxTransmissionEnabled = false;
					trxDisableTransmission();
				}

				uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
				break;
			}

			if (rfFrameBufCount > 0)
			{
				// We have pending data in RF side, but don't process it when MMDVMHost
				// set the hotspot in POCSAG mode. Just trash it.
				if (hotspotModemState == STATE_POCSAG)
				{
					memset((void *)&audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx], 0, HOTSPOT_BUFFER_SIZE);
				}

				if (MMDVMHostRxState == MMDVMHOST_RX_READY)
				{
					uint8_t rx_command = audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx][AMBE_AUDIO_LENGTH + LC_DATA_LENGTH];

					switch(rx_command)
					{
						case HOTSPOT_RX_IDLE:
							break;

						case HOTSPOT_RX_START:
							if (sendVoiceHeaderLC_Frame(audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx]))
							{
								rxLCFrameSent = true;
								uiHotspotUpdateScreen(rx_command);
								lastRxState = HOTSPOT_RX_START;
								rxFrameTime = ticksGetMillis();
							}
							break;

						case HOTSPOT_RX_START_LATE:
							if (sendVoiceHeaderLC_Frame(audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx]))
							{
								rxLCFrameSent = true;
								uiHotspotUpdateScreen(rx_command);
								lastRxState = HOTSPOT_RX_START_LATE;
								rxFrameTime = ticksGetMillis();
							}
							break;

						case HOTSPOT_RX_AUDIO_FRAME:
							if (rxLCFrameSent)
							{
								if (hotspotSendVoiceFrame(audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx]))
								{
									lastRxState = HOTSPOT_RX_AUDIO_FRAME;
									rxFrameTime = ticksGetMillis();
								}
							}
							else
							{
								// Under some conditions, starting frames were missed, probably due to frequency instabilities.
								// This will pick the LC data from this voice frame, and send a voice frame header to MMDVMHost.
								if (sendVoiceHeaderLC_Frame(audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx]))
								{
									rxLCFrameSent = true;
									uiHotspotUpdateScreen(HOTSPOT_RX_START_LATE);
									lastRxState = HOTSPOT_RX_START_LATE;
									rxFrameTime = ticksGetMillis();
								}
							}
							break;

						case HOTSPOT_RX_STOP:
							uiHotspotUpdateScreen(rx_command);
							if (rxLCFrameSent)
							{
								sendTerminator_LC_Frame(audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx]);
							}
							lastRxState = HOTSPOT_RX_STOP;
							hotspotState = HOTSPOT_STATE_RX_END;
							break;

						default:
							lastRxState = HOTSPOT_RX_UNKNOWN;
							break;
					}

					memset((void *)&audioAndHotspotDataBuffer.hotspotBuffer[rfFrameBufReadIdx], 0, HOTSPOT_BUFFER_SIZE);
					rfFrameBufReadIdx = ((rfFrameBufReadIdx + 1) % HOTSPOT_BUFFER_COUNT);

					if (rfFrameBufCount > 0)
					{
						rfFrameBufCount--;
					}
				}
				else
				{
					// Rx Error NAK
					hotspotState = HOTSPOT_STATE_RX_END;
				}
			}
			else
			{
				// Timeout: no RF data for too long
				if (((lastRxState == HOTSPOT_RX_AUDIO_FRAME) || (lastRxState == HOTSPOT_RX_START) || (lastRxState == HOTSPOT_RX_START_LATE)) &&
						((ticksGetMillis() - rxFrameTime) > 300)) // 300ms
				{
					sendDMRLost();
					uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
					lastRxState = HOTSPOT_RX_STOP;
					hotspotState = HOTSPOT_STATE_RX_END;
					rfFrameBufCount = 0;
					return;
				}
			}
			break;

		case HOTSPOT_STATE_RX_END:
			hotspotState = HOTSPOT_STATE_RX_START;
			rxLCFrameSent = false;
			break;

		case HOTSPOT_STATE_TX_START_BUFFERING:
			// If MMDVMHost tells us to go back to idle. (receiving)
			if (hotspotModemState == STATE_IDLE)
			{
				//modemState = STATE_DMR;
				//wavbuffer_read_idx = 0;
				//wavbuffer_write_idx = 0;
				//wavbuffer_count = 0;
				rfFrameBufCount = 0;
				lastRxState = HOTSPOT_RX_IDLE;
				hotspotState = HOTSPOT_STATE_TX_SHUTDOWN;
				hotspotMmdvmHostIsConnected = false;
				trxTransmissionEnabled = false;
				trxDisableTransmission();
				uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
			}
			else
			{
				if (wavbuffer_count > TX_BUFFER_MIN_BEFORE_TRANSMISSION)
				{
					if (hotspotCwKeying == false)
					{
						HRC6000ClearIsWakingState();
						hotspotState = HOTSPOT_STATE_TRANSMITTING;
						trxEnableTransmission();
						uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
					}
				}
				else
				{
					// Buffering time has expired, put the modem in RX.
					if (--netRXDataTimer <= 0)
					{
						if (--timeoutCounter <= 0)
						{
							hotspotState = HOTSPOT_STATE_INITIALISE;
						}
					}
				}
			}
			break;

		case HOTSPOT_STATE_TRANSMITTING:
			// Stop transmitting when there is no data in the buffer or if MMDVMHost sends the idle command
			if (((wavbuffer_count == 0) && (--netRXDataTimer <= 0)) || (hotspotModemState == STATE_IDLE))
			{
				hotspotState = HOTSPOT_STATE_TX_SHUTDOWN;
				txStopDelay = ((hotspotModemState == STATE_IDLE) ? TX_BUFFERING_TIMEOUT : (TX_BUFFERING_TIMEOUT * 2));
			}
			break;

		case HOTSPOT_STATE_TX_SHUTDOWN:
			overriddenLCAvailable = false;
			if ((hotspotModemState != STATE_IDLE) && (txStopDelay > 0))
			{
				txStopDelay--;

				// Some data appeared in the buffer while shutting down, restart buffering.
				if (wavbuffer_count > 0)
				{
					// restart
					timeoutCounter = TX_BUFFERING_TIMEOUT;
					hotspotState = HOTSPOT_STATE_TX_START_BUFFERING;
				}
			}
			else
			{
				txStopDelay = 0; // ensure its value is 0;
				if (trxIsTransmitting ||
						((hotspotModemState == STATE_IDLE) && trxTransmissionEnabled)) // MMDVMHost asked to go back to IDLE (mostly on shutdown)
				{
					trxTransmissionEnabled = false;
					trxDisableTransmission();
					hotspotState = HOTSPOT_STATE_RX_START;
					uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
				}
			}
			break;
	}
}

static uint8_t setFreq(const uint8_t *data, uint8_t length)
{
	if (length < 9)
	{
		return 4;
	}

	// satellite frequencies banned frequency ranges
	const int BAN1_MIN  = 14580000;
	const int BAN1_MAX  = 14600000;
	const int BAN2_MIN  = 43500000;
	const int BAN2_MAX  = 43800000;
	uint32_t fRx, fTx;

	hotspotState = HOTSPOT_STATE_INITIALISE;

	if (!hotspotMmdvmHostIsConnected)
	{
		hotspotMmdvmHostIsConnected = true;
		uiHotspotUpdateScreen(HOTSPOT_RX_IDLE);
	}

	// Very old MMDVMHost, set full power
	if (length == 9)
	{
		rf_power = 255;
	}
	// Current MMDVMHost, set power from MMDVM.ini
	if (length >= 10)
	{
		rf_power = data[9];// 255 = max power
	}

	fRx = (data[1] << 0 | data[2] << 8  | data[3] << 16 | data[4] << 24) / 10;
	fTx = (data[5] << 0 | data[6] << 8  | data[7] << 16 | data[8] << 24) / 10;

	if ((fTx >= BAN1_MIN && fTx <= BAN1_MAX) || (fTx >= BAN2_MIN && fTx <= BAN2_MAX))
	{
		return 4;// invalid frequency
	}

	if (trxCheckFrequencyInAmateurBand(fRx) && trxCheckFrequencyInAmateurBand(fTx))
	{
		hotspotFreqRx = fRx;
		hotspotFreqTx = fTx;
		trxSetFrequency(hotspotFreqRx, hotspotFreqTx, DMR_MODE_DMO);// Override the default assumptions about DMR mode based on frequency
	}
	else
	{
		return 4;// invalid frequency
	}

	hotspotPowerLevel = nonVolatileSettings.txPowerLevel;
	// If the power level sent by MMDVMHost is 255 it means the user has left the setting at 100% and potentially does not realise that there is even a setting for this
	// As the GD-77 can't be run at full power, this power level will be ignored and instead the level specified for normal operation will be used.
	if (rf_power != 255)
	{
		hotspotSavedPowerLevel = nonVolatileSettings.txPowerLevel;

		if (rf_power < 50)
		{
			hotspotPowerLevel = rf_power / 12;
		}
		else
		{
			hotspotPowerLevel = (rf_power / 50) + 3;
		}
		trxSetPowerFromLevel(hotspotPowerLevel);
	}

  return 0;
}

static bool hasRXOverflow(void)
{
	return ((HOTSPOT_BUFFER_SIZE - rfFrameBufCount) <= 0);
}

static bool hasTXOverflow(void)
{
	return ((HOTSPOT_BUFFER_COUNT - wavbuffer_count) <= 0);
}

void hotspotInit(void)
{
	hotspotMmdvmHostIsConnected = false;
	trxTalkGroupOrPcId = 0;
	hotspotCurrentRxCommandState = HOTSPOT_RX_UNKNOWN;
	hotspotState = HOTSPOT_STATE_NOT_CONNECTED;

	overriddenLCTA[0] = 0;
	overriddenBlocksTA = 0x0;
	overriddenLCAvailable = false;
	hotspotCwKeying = false;
	cwReset();
	hotspotTxDelay = 0;
	memset(&hotspotRxedDMR_LC, 0, sizeof(DMRLC_t));// clear automatic variable

	rxLCFrameSent = false;

	// Clear RF buffers
	rfFrameBufCount = 0;
	rfFrameBufReadIdx = 0;
	rfFrameBufWriteIdx = 0;
	for (uint8_t i = 0; i < HOTSPOT_BUFFER_COUNT; i++)
	{
		memset((void *)&audioAndHotspotDataBuffer.hotspotBuffer[i], 0, HOTSPOT_BUFFER_SIZE);
	}

	// Clear USB TX buffers
	usbComSendBufWritePosition = 0;
	usbComSendBufReadPosition = 0;
	usbComSendBufCount = 0;
	memset((uint8_t *)&usbComSendBuf, 0, sizeof(usbComSendBuf));

	trxSetModeAndBandwidth(RADIO_MODE_DIGITAL, false);// hotspot mode is for DMR i.e Digital mode

	if (hotspotFreqTx == 0)
	{
		hotspotFreqTx = 43000000;
	}

	if (hotspotFreqRx == 0)
	{
		hotspotFreqRx = 43000000;
	}

	MMDVMHostRxState = MMDVMHOST_RX_READY; // We have not sent anything to MMDVMHost, so it can't be busy yet.

	// Set CC, QRG and power, in case hotspot menu has left then re-enter.
	trxSetDMRColourCode(colorCode);

	if (trxCheckFrequencyInAmateurBand(hotspotFreqRx) && trxCheckFrequencyInAmateurBand(hotspotFreqTx))
	{
		trxSetFrequency(hotspotFreqRx, hotspotFreqTx, DMR_MODE_DMO);
	}

	if (rf_power != 255)
	{
		if (rf_power < 50)
		{
			hotspotPowerLevel = rf_power / 12;
		}
		else
		{
			hotspotPowerLevel = (rf_power / 50) + 3;
		}

		trxSetPowerFromLevel(hotspotPowerLevel);
	}

	HRC6000ResetTimeSlotDetection();
	mmdvmHostLastActiveTime = ticksGetMillis();
}
