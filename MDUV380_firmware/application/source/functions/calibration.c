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

#include "utils.h"
#include "functions/calibration.h"
#include "functions/trx.h"

const int MAX_PA_DAC_VALUE = 4095;

typedef struct
{
//0x00
	uint8_t VoxLevel1;				//calibration for Vox Setting 1
	uint8_t VoxLevel10;				//calibration for Vox Setting 10
	uint8_t RxLowVoltage;			//Exact use unknown
	uint8_t RxHighVoltage;			//Exact use unknown
	uint8_t RSSI120;			    //RSSI Calibration for -120dBm
	uint8_t RSSI70;					//RSSI Calibration for -70dBm
	uint8_t UnknownBlock1[2];		//Unknown
//0x08
	uint8_t Unknown1;				//Unknown
	uint8_t UHFOscRefTune;			//UHF reference tuning
	uint8_t UnknownBlock2[2];		//Unknown
	uint8_t VHFOscRefTune;			//UHF reference tuning
	uint8_t UnknownBlock3[3];		//Unknown
//0x10
	uint8_t UHFHighPowerCal[9];	//UHF High Power Calibration 9 frequencies
//0x19
	uint8_t VHFHighPowerCal[5];	//VHF High Power Calibration 5 frequencies
//0x1E
	uint8_t UnknownBlock4[2];		//Unknown
//0x20
	uint8_t UHFLowPowerCal[9];		//UHF Low Power Calibration 9 frequencies
//0x29
	uint8_t VHFLowPowerCal[5];		//VHF Low Power Calibration 5 frequencies
//0x2E
	uint8_t UnknownBlock5[2];		//Unknown
//0x30
	uint8_t UHFRxTuning[9];		//UHF Rx Front End Tuning 9 frequencies
//0x39
	uint8_t VHFRxTuning[5];		//VHF Rx Front End Tuning 5 frequencies
//0x3E
	uint8_t UnknownBlock6[2];		//Unknown
//0x40
	uint8_t UHFOpenSquelch9[9];   //UHF Squelch Level 9 Opening  9 frequencies
//0x49
	uint8_t UnknownBlock7[7];		//Unknown
//0x50
	uint8_t UHFCloseSquelch9[9];   //UHF Squelch Level 9 Closing 9 frequencies
//0x59
	uint8_t UnknownBlock8[7];		//Unknown
//0x60
	uint8_t UHFOpenSquelch1[9];   //UHF Squelch Level 1 Opening  9 frequencies
//0x69
	uint8_t UnknownBlock9[7];		//Unknown
//0x70
	uint8_t UHFCloseSquelch1[9];   //UHF Squelch Level 1 Closing 9 frequencies
//0x79
	uint8_t UnknownBlock10[7];		//Unknown
//0x80
	uint8_t UnknownBlock11[16];		//Unknown
//0x90
	uint8_t UHFCTC67[9];			//UHF CTCSS Deviation for 67Hz Tone 9 frequencies
//0x99
	uint8_t UnknownBlock12[2];		//Unknown
//0x9B
	uint8_t VHFCTC67[5];			//VHF CTCSS Deviation for 67Hz Tone 5 frequencies
//0xA0
	uint8_t UHFCTC151[9];			//UHF CTCSS Deviation for 151.4Hz Tone 9 frequencies
//0xA9
	uint8_t UnknownBlock13[2];		//Unknown
//0x9B
	uint8_t VHFCTC151[5];			//VHF CTCSS Deviation for 151.4Hz Tone 5 frequencies
//0xB0
	uint8_t UHFCTC254[9];			//UHF CTCSS Deviation for 254.1Hz Tone 9 frequencies
//0xB9
	uint8_t UnknownBlock14[2];		//Unknown
//0xBB
	uint8_t VHFCTC254[5];			//VHF CTCSS Deviation for 254.1Hz Tone 5 frequencies
//0xC0
	uint8_t UnknownBlock15[16];		//Unknown
//0xD0
	uint8_t UHFDCS[9];				//UHF DCS Deviation 9 frequencies
//0xD9
	uint8_t UnknownBlock16[2];		//Unknown
//0xDB
	uint8_t VHFDCS[5];				//VHF DCS Deviation 5 frequencies
//0xE0
	uint8_t VHFOpenSquelch9[5];     //VHF Squelch Level 9 Opening  5 frequencies
	uint8_t VHFCloseSquelch9[5];     //VHF Squelch Level 9 Closing  5 frequencies
	uint8_t VHFOpenSquelch1[5];     //VHF Squelch Level 1 Opening  5 frequencies
	uint8_t VHFCloseSquelch1[5];     //VHF Squelch Level 1 Closing  5 frequencies
//0xF4
	uint8_t UnknownBlock17[12];		//Unknown
//0x100
	uint8_t VHFCalFreqs[10][4];		// VHF Calibration Frequencies 4 BCD bytes per freq, 5 pairs of freqs Rx and Tx
//0x128
	uint8_t UnknownBlock18[8];		//Unknown
//0x130
	uint8_t UHFDMRIGain[9];			//UHF I Gain for DMR	9 Frequencies
//0x139
	uint8_t VHFDMRIGain[5];			//VHF I Gain for DMR	5 Frequencies
	uint8_t UnknownBlock19[2];		//Unknown
//0x140
	uint8_t UHFDMRQGain[9];			//UHF Q Gain for DMR	9 Frequencies
//0x149
	uint8_t VHFDMRQGain[5];			//VHF Q Gain for DMR	5 Frequencies
	uint8_t UnknownBlock20[2];		//Unknown
//0x150
	uint8_t UnknownBlock21[32];		//Unknown
//0x170
	uint8_t UHFFMIGain[9];			//UHF I Gain for FM	9 Frequencies
//0x179
	uint8_t VHFFMIGain[5];			//VHF I Gain for FM	5 Frequencies
	uint8_t UnknownBlock22[2];		//Unknown
//0x180
	uint8_t UHFFMQGain[9];			//UHF Q Gain for FM	9 Frequencies
//0x189
	uint8_t VHFFMQGain[5];			//VHF Q Gain for FM	5 Frequencies
	uint8_t UnknownBlock23[2];		//Unknown
//0x190
	uint8_t UHFMidPowerCal[9];		//UHF Mid Power Calibration 9 frequencies
//0x199
	uint8_t VHFMidPowerCal[5];		//VHF Mid Power Calibration 5 frequencies
//0x19E
	uint8_t UnknownBlock24[2];		//Unknown
//0x1A0
	uint8_t UHFVeryLowPowerCal[9];		//UHF VeryLow Power Calibration 9 frequencies (not used or initialised by official firmware)
//0x1A9
	uint8_t VHFVeryLowPowerCal[5];		//VHF VeryLow Power Calibration 5 frequencies (not used or initialised by official firmware)
//0x1AE
	uint8_t UnknownBlock25[2];		//Unknown
//0x1B0
	uint8_t UHFCalFreqs[18][4];		// UHF Calibration Frequencies 4 BCD bytes per freq, 9 pairs of freqs Rx and Tx
	uint8_t UnknownBlock26[8];		//Unknown
//0x200
} CalibrationData_t;

#define CALIBRATION_TABLE_LENGTH 0x200 // Calibration table is 512 bytes long
static __attribute__((section(".ccmram"))) CalibrationData_t calibrationData;
#define CALIBRATION_TABLE_LOCAL_COPY_ADDRESS  0x10000        //Flash address for local calibration copy.

const int MARKER_BYTES_LENGTH = 8;													//	we will use the 8 bytes of the first UHF calibration frequency as a marker
const uint8_t MARKER_BYTES[] = {0x00, 0x25, 0x00, 0x40, 0x00, 0x45, 0x01, 0x40};	//  400.02500   400.145

void calibrationInit(void)
{
	calibrationReadLocal();					//first try to read the local copy of the calibration table

	if(memcmp(MARKER_BYTES, calibrationData.UHFCalFreqs[0], MARKER_BYTES_LENGTH) != 0)			//do we have a good local copy?
	{
	    calibrationReadFactory(true);																//no so copy the factory values
        calibrationSaveLocal();																    //to the local copy
	}
}

void calibrationReadLocal(void)
{
	(void)SPI_Flash_read(CALIBRATION_TABLE_LOCAL_COPY_ADDRESS , (uint8_t *)&calibrationData, CALIBRATION_TABLE_LENGTH);
}

void calibrationSaveLocal(void)
{
	(void)SPI_Flash_write(CALIBRATION_TABLE_LOCAL_COPY_ADDRESS , (uint8_t *)&calibrationData, CALIBRATION_TABLE_LENGTH);
}

void calibrationReadFactory(bool applyConversion)
{
	static const float fractionalPowers[2][4] = {
#if defined(PLATFORM_RT84_DM1701)
		// DM1701 or RT84 which have same RF hardware
			{0.61f, 1.00f, 0.85f, 1.30f},// VHF
			{0.65f, 1.00f, 0.86f, 1.70f},// UHF
#else
	#if defined(PLATFORM_VARIANT_UV380_PLUS_10W)
		// 10W UV380
		{0.35f, 0.65f, 1.25f, 1.40f},// VHF
		{0.40f, 0.70f, 1.20f, 1.35f},// UHF
	#else
		//  5W UV380
		{0.63f, 1.00f, 1.00f, 1.10f},// VHF
		{0.72f, 1.00f, 1.05f, 1.25f},// UHF
	#endif
#endif
	};//fractionalPowers

	(void)SPI_Flash_readSecurityRegisters(0, (uint8_t *)&calibrationData, CALIBRATION_TABLE_LENGTH);

    memcpy(calibrationData.UHFCalFreqs[0] , MARKER_BYTES , MARKER_BYTES_LENGTH);
    //add the marker bytes just in case they are different in this radio.

    for(uint8_t freqRangeIndex = 0; freqRangeIndex < 5 ; freqRangeIndex++)
    {
		calibrationData.VHFVeryLowPowerCal[freqRangeIndex]	 = calibrationData.VHFLowPowerCal[freqRangeIndex] * fractionalPowers[0][0];
		if (applyConversion)
		{
			calibrationData.VHFLowPowerCal[freqRangeIndex]		*= fractionalPowers[0][1];
			calibrationData.VHFMidPowerCal[freqRangeIndex]		*= fractionalPowers[0][2];
			calibrationData.VHFHighPowerCal[freqRangeIndex]		*= fractionalPowers[0][3];
		}
    }

    for(uint8_t freqRangeIndex = 0; freqRangeIndex < 9; freqRangeIndex++)
    {
		calibrationData.UHFVeryLowPowerCal[freqRangeIndex]	 = calibrationData.UHFLowPowerCal[freqRangeIndex] * fractionalPowers[1][0];

    	if (applyConversion)
    	{
    		calibrationData.UHFLowPowerCal[freqRangeIndex]		*= fractionalPowers[1][1];
    		calibrationData.UHFMidPowerCal[freqRangeIndex]		*= fractionalPowers[1][2];
    		calibrationData.UHFHighPowerCal[freqRangeIndex]		*= fractionalPowers[1][3];
    	}
    }
}

//look up the tuning voltage and interpolate between points (not used on MDuV380 but retained for MD-9600)
uint16_t calibrationGetRxTuneForFrequency(int freq)
{
	int index;
	int offset;
	int limit;
	int upper;
	int lower;

	if (freq > 30000000)
	{
		index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) / 1000000;
		offset= (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) % 1000000;
		limit = 8;
		index = CLAMP(index, 0, limit);
		lower = calibrationData.UHFRxTuning[index] << 4;					//get the lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFRxTuning[index + 1] << 4;				//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFRxTuning[index - 1] << 4));       //extrapolate outside top point using the same slope
		}

		return CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4095);
	}

	index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) / 950000;
	offset= (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) % 950000;
	limit = 4;
	index = CLAMP(index, 0, limit);
	lower = calibrationData.VHFRxTuning[index] << 4;					//get the lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFRxTuning[index + 1] << 4;				//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFRxTuning[index - 1] << 4));       //extrapolate outside top point using the same slope
	}

	return CLAMP(interpolate(lower, upper, offset, 950000), 0, 4095);
}

uint8_t calibrationGetAnalogIGainForFrequency(int freq)
{
	int index;
	int offset;
	int limit;
	int upper;
	int lower;

	if (freq > 30000000)
	{
		index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) / 1000000;
		offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) % 1000000;
		limit = 8;
		index = CLAMP(index, 0, limit);
		lower = calibrationData.UHFFMIGain[index];					//get the lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFFMIGain[index + 1];				//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFFMIGain[index - 1]));       //extrapolate outside top point using the same slope
		}

		return CLAMP(interpolate(lower, upper, offset, 1000000), 0, 255);
	}

	index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) / 950000;
	offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) % 950000;
	limit = 4;
	index = CLAMP(index, 0, limit);
	lower = calibrationData.VHFFMIGain[index];					//get the lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFFMIGain[index + 1];				//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFFMIGain[index - 1]));       //extrapolate outside top point using the same slope
	}

	return CLAMP(interpolate(lower, upper, offset, 950000), 0, 255);
}

uint8_t calibrationGetAnalogQGainForFrequency(int freq)
{
	int index;
	int offset;
	int limit;
	int upper;
	int lower;

	if (freq > 30000000)
	{
		index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) / 1000000;
		offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) % 1000000;
		limit = 8;
		index = CLAMP(index, 0, limit);
		lower = calibrationData.UHFFMQGain[index];					//get the lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFFMQGain[index + 1];				//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFFMQGain[index - 1]));       //extrapolate outside top point using the same slope
		}

		return CLAMP(interpolate(lower, upper, offset, 1000000), 0, 255);
	}

	index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) / 950000;
	offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) % 950000;
	limit = 4;
	index = CLAMP(index, 0, limit);
	lower = calibrationData.VHFFMQGain[index];					//get the lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFFMQGain[index + 1];				//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFFMQGain[index - 1]));       //extrapolate outside top point using the same slope
	}

	return CLAMP(interpolate(lower, upper, offset, 950000), 0, 255);
}

uint8_t calibrationGetDigitalIGainForFrequency(int freq)
{
	int index;
	int offset;
	int limit;
	int upper;
	int lower;

	if (freq > 30000000)
	{
		index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) / 1000000;
		offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) % 1000000;
		limit = 8;
		index = CLAMP(index, 0, limit);
		lower = calibrationData.UHFDMRIGain[index];					//get the lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFDMRIGain[index + 1];				//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFDMRIGain[index - 1]));       //extrapolate outside top point using the same slope
		}

		return CLAMP(interpolate(lower, upper, offset, 1000000), 0, 255);
	}

	index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) / 950000;
	offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) % 950000;
	limit = 4;
	index = CLAMP(index, 0, limit);
	lower = calibrationData.VHFDMRIGain[index];					//get the lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFDMRIGain[index + 1];				//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFDMRIGain[index - 1]));       //extrapolate outside top point using the same slope
	}

	return CLAMP(interpolate(lower, upper, offset, 950000), 0, 255);
}

uint8_t calibrationGetDigitalQGainForFrequency(int freq)
{
	int index;
	int offset;
	int limit;
	int upper;
	int lower;

	if (freq > 30000000)
	{
		index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) / 1000000;
		offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calIQTableMinFreq) % 1000000;
		limit = 8;
		index = CLAMP(index, 0, limit);
		lower = calibrationData.UHFDMRQGain[index];					//get the lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFDMRQGain[index + 1];				//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFDMRQGain[index - 1]));       //extrapolate outside top point using the same slope
		}

		return CLAMP(interpolate(lower, upper, offset, 1000000), 0, 255);
	}

	index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) / 950000;
	offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calIQTableMinFreq) % 950000;
	limit = 4;
	index = CLAMP(index, 0, limit);
	lower = calibrationData.VHFDMRQGain[index];					//get the lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFDMRQGain[index + 1];				//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFDMRQGain[index - 1]));       //extrapolate outside top point using the same slope
	}

	return CLAMP(interpolate(lower, upper, offset, 950000), 0, 255);
}

int interpolate(int lowerpoint, int upperpoint, int numerator, int denominator)
{
	return lowerpoint + (((upperpoint - lowerpoint) * numerator) / denominator);
}

void calibrationGetPowerForFrequency(int freq, calibrationPowerValues_t *powerSettings)
{
	int index;
	int offset;
	int limit;
	int upper;
	int lower;

	if (freq > 30000000)
	{
		index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calPowerTableMinFreq) / 1000000;
		offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_UHF].calPowerTableMinFreq) % 1000000;
		limit = 8;
		index = CLAMP(index, 0, limit);
		lower = calibrationData.UHFLowPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFLowPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFLowPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
		}

		powerSettings->lowPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
		lower = calibrationData.UHFVeryLowPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFVeryLowPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFVeryLowPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
		}

		powerSettings->veryLowPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
		lower = calibrationData.UHFMidPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFMidPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFMidPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
		}

		powerSettings->midPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
		lower = calibrationData.UHFHighPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

		if (index < limit)
		{
			upper = calibrationData.UHFHighPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
		}
		else
		{
			upper = lower + (lower - (calibrationData.UHFHighPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
		}

		powerSettings->highPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);

		return;
	}

	index = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calPowerTableMinFreq) / 1000000;
	offset = (freq - RADIO_HARDWARE_FREQUENCY_BANDS[RADIO_BAND_VHF].calPowerTableMinFreq) % 1000000;
	limit = 4;
	index = CLAMP(index, 0, limit);
	lower = calibrationData.VHFLowPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFLowPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFLowPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
	}

	powerSettings->lowPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
	lower = calibrationData.VHFVeryLowPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFVeryLowPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFVeryLowPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
	}

	powerSettings->veryLowPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
	lower = calibrationData.VHFMidPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFMidPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFMidPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
	}

	powerSettings->midPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
	lower = calibrationData.VHFHighPowerCal[index] << 4;				// get the Lower lookup point and scale it to 12 bits

	if (index < limit)
	{
		upper = calibrationData.VHFHighPowerCal[index + 1] << 4;			//get the higher lookup point and scale it to 12 bits
	}
	else
	{
		upper = lower + (lower - (calibrationData.VHFHighPowerCal[index - 1] << 4));       //extrapolate outside top point using the same slope
	}

	powerSettings->highPower = CLAMP(interpolate(lower, upper, offset, 1000000), 0, 4096);
}

int8_t calibrationGetMod2Offset(int band)
{
	return (int8_t)(((band == RADIO_BAND_VHF) ? calibrationData.VHFOscRefTune : calibrationData.UHFOscRefTune) - 128);
}

void calibrationSetMod2Offset(int band, int8_t value)
{
	if (band == RADIO_BAND_VHF)
	{
		calibrationData.VHFOscRefTune = (int)value + 128;
	}
	else
	{
		calibrationData.UHFOscRefTune = (int)value + 128;
	}
}

//(Not Used)
bool calibrationGetRSSIMeterParams(calibrationRSSIMeter_t *rssiMeterValues)
{
	rssiMeterValues->minVal = calibrationData.RSSI120;
	rssiMeterValues->rangeVal = calibrationData.RSSI70;

    return true;
}

uint8_t calibrationGetVHFOscTune(void)
{
	return calibrationData.VHFOscRefTune;
}

void calibrationPutVHFOscTune(uint8_t val)
{
	calibrationData.VHFOscRefTune = val;
}

uint8_t calibrationGetPower(int freqindex, int powerindex)
{

	if (freqindex < 5)			//VHF calibration values
	{
		switch(powerindex)
		{
			case 0:
				return calibrationData.VHFVeryLowPowerCal[freqindex];
				break;
			case 1:
				return calibrationData.VHFLowPowerCal[freqindex];
				break;
			case 2:
				return calibrationData.VHFMidPowerCal[freqindex];
				break;
			case 3:
				return calibrationData.VHFHighPowerCal[freqindex];
				break;
		}
	}
	else                        //UHF calibration values
	{
		switch(powerindex)
		{
			case 0:
				return calibrationData.UHFVeryLowPowerCal[freqindex - 5];
				break;
			case 1:
				return calibrationData.UHFLowPowerCal[freqindex - 5];
				break;
			case 2:
				return calibrationData.UHFMidPowerCal[freqindex - 5];
				break;
			case 3:
				return calibrationData.UHFHighPowerCal[freqindex - 5];
				break;
		}
	}

	return 0;
}

void calibrationPutPower(int freqindex, int powerindex, uint8_t val)
{
	if (freqindex < 5)			//VHF calibration values
	{
		switch(powerindex)
		{
			case 0:
				calibrationData.VHFVeryLowPowerCal[freqindex] = val;
				break;
			case 1:
				calibrationData.VHFLowPowerCal[freqindex] = val;
				break;
			case 2:
				calibrationData.VHFMidPowerCal[freqindex] = val;
				break;
			case 3:
				calibrationData.VHFHighPowerCal[freqindex] = val;
				break;
		}
	}
	else                        //UHF calibration values
	{
		switch(powerindex)
		{
			case 0:
				calibrationData.UHFVeryLowPowerCal[freqindex - 5] = val;
				break;
			case 1:
				calibrationData.UHFLowPowerCal[freqindex - 5] = val;
				break;
			case 2:
				calibrationData.UHFMidPowerCal[freqindex - 5] = val;
				break;
			case 3:
				calibrationData.UHFHighPowerCal[freqindex - 5] = val;
				break;
		}
	}
}

uint8_t *calibrationGetLocalDataPointer(void)
{
	return (uint8_t *)&calibrationData;
}
