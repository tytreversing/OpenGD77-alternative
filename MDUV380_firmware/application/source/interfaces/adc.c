/*
 * Copyright (C) 2019      Kai Ludwig, DG4KLU
 * Copyright (C) 2019-2024 Roger Clark, VK3KYY / G4KYF
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

#include "interfaces/adc.h"
#include "functions/settings.h"
#include "interfaces/batteryAndPowerManagement.h"
#include "functions/ticks.h"


const int TEMPERATURE_DECIMAL_RESOLUTION = 1000000;
const int CUTOFF_VOLTAGE_UPPER_HYST = 64;
const int CUTOFF_VOLTAGE_LOWER_HYST = 62;
const int BATTERY_MAX_VOLTAGE = 82;
const int POWEROFF_VOLTAGE_THRESHOLD = 55;

#if defined(PLATFORM_MDUV380)
#define BATTERY_ADC_COEFF 40.3f
#else
#define BATTERY_ADC_COEFF 37.5f
#endif


//lookup table to convert linear Voltage values between 1 and 255 to dBs relative to 1 with a resolution of 0.2dB.  Values in tabble are dbs * 5 to use all of the available byte range.
//calculated using dBs= (20 * Log10(index)) * 5
const uint8_t PointTwodBs[] =
{
		0,   0,   30,  48,  60,  70,  78,  85,  90,  95,  100, 104, 108, 111, 115, 118,
		120, 123, 126, 128, 130, 132, 134, 136, 138, 140, 141, 143, 145, 146, 148, 149,
		151, 152, 153, 154, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
		168, 169, 170, 171, 172, 172, 173, 174, 175, 176, 176, 177, 178, 179, 179, 180,
		181, 181, 182, 183, 183, 184, 185, 185, 186, 186, 187, 188, 188, 189, 189, 190,
		190, 191, 191, 192, 192, 193, 193, 194, 194, 195, 195, 196, 196, 197, 197, 198,
		198, 199, 199, 200, 200, 200, 201, 201, 202, 202, 203, 203, 203, 204, 204, 205,
		205, 205, 206, 206, 206, 207, 207, 208, 208, 208, 209, 209, 209, 210, 210, 210,
		211, 211, 211, 212, 212, 212, 213, 213, 213, 214, 214, 214, 215, 215, 215, 216,
		216, 216, 216, 217, 217, 217, 218, 218, 218, 218, 219, 219, 219, 220, 220, 220,
		220, 221, 221, 221, 221, 222, 222, 222, 223, 223, 223, 223, 224, 224, 224, 224,
		225, 225, 225, 225, 226, 226, 226, 226, 226, 227, 227, 227, 227, 228, 228, 228,
		228, 229, 229, 229, 229, 229, 230, 230, 230, 230, 231, 231, 231, 231, 231, 232,
		232, 232, 232, 232, 233, 233, 233, 233, 233, 234, 234, 234, 234, 234, 235, 235,
		235, 235, 235, 236, 236, 236, 236, 236, 237, 237, 237, 237, 237, 237, 238, 238,
		238, 238, 238, 239, 239, 239, 239, 239, 239, 240, 240, 240, 240, 240, 240, 241
};

volatile uint16_t adcVal[NUM_ADC_CHANNELS];
static const int AVERAGE_BATTERY_VOLTAGE_SAMPLE_WINDOW = 1000.0f;
static int16_t averagedVolume = 0; // value is (* 16) the real value.

void adcStartDMA(void)
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adcVal, NUM_ADC_CHANNELS);
	HAL_TIM_Base_Start_IT(&htim3);
}

//void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){} // Don't need to use the half full callback at the moment

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	potLevel = adcVal[0];
	batteryVoltage = ((int)adcVal[1] / BATTERY_ADC_COEFF) + ((nonVolatileSettings.batteryCalibration & 0x0F) - 5);
	micLevel = adcVal[2];
	temperatureLevel = adcVal[3];

	// Handle switching the power off, using the rotary control.
	if (batteryVoltage > POWEROFF_VOLTAGE_THRESHOLD)
	{
		lastValidBatteryVoltage = batteryVoltage;
	}

	if (ticksGetMillis() < (BATTERY_VOLTAGE_STABILISATION_TIME + resumeTicks))
	{
		averageBatteryVoltage = batteryVoltage;
	}
	else
	{
		averageBatteryVoltage = (averageBatteryVoltage * (AVERAGE_BATTERY_VOLTAGE_SAMPLE_WINDOW - 1) + batteryVoltage) / AVERAGE_BATTERY_VOLTAGE_SAMPLE_WINDOW;
	}
}

int adcGetBatteryVoltage(void)
{
	return batteryVoltage;			//doesn't need averaging so just use the most recent conversion
}

int adcGetVOX(void)
{
	return ((micLevel > 255) ? 255 : PointTwodBs[micLevel]);
}

int getTemperature(void)
{
	const int tV25 = 943;					// ADC Value at 25 degrees from data sheet (0.76V with 12 Bit ADC ref=3V3)
    const int tslope = 31;	                //Slope from datasheet ADC Counts *10

    return (((temperatureLevel - tV25) * 100) / tslope) + 250 + (nonVolatileSettings.temperatureCalibration * 5);
}

#if defined(PLATFORM_RT84_DM1701)
#define MIN_VOL_ADC_LOW 34 // 29 + 5
#define MAX_VOL_ADC_VAL 2070 // 2075 - 5
#define VOL_POT_ADC_MID_POINT 318
#define MIN_VOL_LOW_TO_HIGH_OFFSET 5
#elif defined(PLATFORM_VARIANT_UV380_PLUS_10W)
#define MIN_VOL_ADC_LOW 24 // 19 + 5
#define MAX_VOL_ADC_VAL 2049 // 2054 - 5
#define VOL_POT_ADC_MID_POINT 308
#define MIN_VOL_LOW_TO_HIGH_OFFSET 5
#elif defined(PLATFORM_MD2017)
#define MIN_VOL_ADC_LOW 37 // 32 + 5
#define MAX_VOL_ADC_VAL 2043 // 2048 - 5
#define VOL_POT_ADC_MID_POINT 321
#define MIN_VOL_LOW_TO_HIGH_OFFSET 5
#else
// COMMENTED: MD-UV380 values
//#define MIN_VOL_ADC_LOW 25 // 22 + 3
//#define MAX_VOL_ADC_VAL 2065 // 2070 - 5
//#define VOL_POT_ADC_MID_POINT 309
//#define MIN_VOL_LOW_TO_HIGH_OFFSET 5
// RT3S
#define MIN_VOL_ADC_LOW 30 // 23 + 7 // RT3S ADC reading is not really stable at lowest level
#define MAX_VOL_ADC_VAL 2015 // 2020 - 5
#define VOL_POT_ADC_MID_POINT 317
#define MIN_VOL_LOW_TO_HIGH_OFFSET 8
#endif

#define MIN_VOL_ADC_HIGH (MIN_VOL_ADC_LOW + MIN_VOL_LOW_TO_HIGH_OFFSET)
#define VOL_LOW_SIDE_DIV ((VOL_POT_ADC_MID_POINT - MIN_VOL_ADC_HIGH) / 31)
#define VOL_HIGH_SIDE_DIV ((MAX_VOL_ADC_VAL - VOL_POT_ADC_MID_POINT) / 31)

static int8_t getVolumeControlRaw(void)
{
	// volume control is adc[0]. Max value seems to be:
	//   - RT3S:             2020, min value seems to be 23
	//   - MD-UV380:         2070, min value seems to be 22
	//   - MD-UV380Plus 10W: 2054, min value seems to be 19
	//   - DM-1701:          2075, min value seems to be 29
	//   - MD-2017:          2048, min value seems to be 32
	// Pot is Log law which needs converting back to linear law
	// Log Law is approximated by two straight lines with breakpoint at 300
	int vol = potLevel;
	int pos;
	static bool minvol;

	if (vol < VOL_POT_ADC_MID_POINT)
	{
		pos = (vol - MIN_VOL_ADC_HIGH) / VOL_LOW_SIDE_DIV; // convert to 0-31
	}
	else
	{
		pos = 31 + ((vol - VOL_POT_ADC_MID_POINT) / VOL_HIGH_SIDE_DIV); // convert to 31-62
	}

	if (minvol ? (vol < MIN_VOL_ADC_HIGH) : (vol < MIN_VOL_ADC_LOW))
	{
		minvol = true;
		return -99; // -99 = volume min (muted)
	}

	minvol = false;
	return (int8_t)CLAMP(pos - 31, -31, 31);
}

int8_t getVolumeControl(void)
{
	int8_t v = getVolumeControlRaw();

	if ((v == -99) || ((v != -99) && (averagedVolume == (-99 << 4))))
	{
		averagedVolume = (v << 4);
		return v;
	}

	averagedVolume = (v + averagedVolume - ((averagedVolume - 8) >> 4));

	return (int8_t)CLAMP((averagedVolume >> 4), -31, 31);
}
