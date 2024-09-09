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
#ifndef _OPENGD77_CALIBRATION_H_
#define _OPENGD77_CALIBRATION_H_

#include "hardware/SPI_Flash.h"

typedef enum
{
	CalibrationBandUHF,
	CalibrationBandVHF,
	CalibrationBandMAX
} CalibrationBand_t;

typedef enum
{
	CalibrationSection_DACDATA_SHIFT,
	CalibrationSection_TWOPOINT_MOD,
	CalibrationSection_Q_MOD2_OFFSET,
	CalibrationSection_PHASE_REDUCE,
} CalibrationSection_t;

typedef struct
{
	uint16_t offset;
	uint16_t mod;

	union
	{
		uint16_t value;
		uint8_t  bytes[2];
	};
} CalibrationDataResult_t;


typedef struct calibrationPowerValues
{
	uint32_t veryLowPower;
	uint32_t lowPower;
	uint32_t midPower;
	uint32_t highPower;
} calibrationPowerValues_t;

typedef struct calibrationRSSIMeter
{
	uint8_t minVal;
	uint8_t rangeVal;
} calibrationRSSIMeter_t;


extern const int MAX_PA_DAC_VALUE;


void calibrationInit(void);
void calibrationReadFactory(bool applyConversion);
void calibrationSaveLocal(void);
void calibrationReadLocal(void);
void calibrationGetPowerForFrequency(int freq, calibrationPowerValues_t *powerSettings);
uint16_t calibrationGetRxTuneForFrequency(int freq);
uint8_t calibrationGetAnalogIGainForFrequency(int freq);
uint8_t calibrationGetAnalogQGainForFrequency(int freq);
uint8_t calibrationGetDigitalIGainForFrequency(int freq);
uint8_t calibrationGetDigitalQGainForFrequency(int freq);
int8_t calibrationGetMod2Offset(int band);
void calibrationSetMod2Offset(int band, int8_t value);
bool calibrationGetRSSIMeterParams(calibrationRSSIMeter_t *rssiMeterValues);
int interpolate(int lowerpoint, int upperpoint, int numerator, int denominator);
uint8_t calibrationGetPower(int freqindex, int powerindex);
void calibrationPutPower(int freqindex, int powerindex, uint8_t val);
uint8_t *calibrationGetLocalDataPointer(void);

#endif
