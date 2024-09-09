/*
 * Copyright (C) 2021-2024 Roger Clark, VK3KYY / G4KYF
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

#include "functions/settings.h"
#include "functions/trx.h"
#include "functions/rxPowerSaving.h"
#include "hardware/HR-C6000.h"
#include "hardware/AT1846S.h"
#if defined(USING_EXTERNAL_DEBUGGER)
#include "SeggerRTT/RTT/SEGGER_RTT.h"
#endif
#include "main.h"

#if defined(PLATFORM_MD9600)
//list of CTCSS tones supported by the HRC6000. These are used to find the index for the tone. The order is critical.
const uint16_t HRC_CTCSSTones[] =
{
		6700,  7190,  7440,  7700,  7970,  8250,  8540,
		8850,  9150,  9480,  9740,  10000, 10350, 10720,
		11090, 11480, 11880, 12300, 12730, 13180, 13650,
		14130, 14620, 15140, 15670, 16220, 16790, 17380,
		17990, 18620, 19280, 20350, 21070, 21810, 22570,
		23360, 24180, 25030, 6930,  6250,  15980, 16550,
		17130, 17730, 18350, 18990, 19660, 19950, 20650,
		22910, 25410
};
#endif

// AT-1846 native values for Rx
static uint8_t fl_l;
static uint8_t fl_h;
static uint8_t fh_l;
static uint8_t fh_h;

RadioDevice_t currentRadioDeviceId = RADIO_DEVICE_PRIMARY;
TRXDevice_t radioDevices[RADIO_DEVICE_MAX] = {
		{
				.currentRxFrequency = FREQUENCY_UNSET,
				.currentTxFrequency = FREQUENCY_UNSET,
				.lastSetTxFrequency = FREQUENCY_UNSET,
				.trxCurrentBand = { RADIO_BAND_VHF, RADIO_BAND_VHF },
				.trxDMRModeRx = DMR_MODE_DMO,
				.trxDMRModeTx = DMR_MODE_DMO,
				.currentMode = RADIO_MODE_NONE,
				.txPowerLevel = POWER_UNSET,
				.lastSetTxPowerLevel = POWER_UNSET,
				.trxRxSignal = 0,
				.trxRxNoise = 255,
				.currentBandWidthIs25kHz = BANDWIDTH_12P5KHZ,
				.analogSignalReceived = false,
				.analogTriggeredAudio = false,
				.digitalSignalReceived = false
		}
};

TRXDevice_t *currentRadioDevice = &radioDevices[RADIO_DEVICE_PRIMARY];

void radioPowerOn(void)
{
	//Turn off Tx Voltages to prevent transmission.
	HAL_GPIO_WritePin(PA_EN_1_GPIO_Port, PA_EN_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PA_EN_2_GPIO_Port, PA_EN_2_Pin, GPIO_PIN_RESET);

	//Turn on all receiver voltages. (thats what the TYT firmware does, both receivers are on together)

	radioSetRxLNAForDevice(currentRadioDeviceId);

	HAL_GPIO_WritePin(C6000_PWD_GPIO_Port, C6000_PWD_Pin, GPIO_PIN_RESET);// Power On the C6000

	//Turn on the Microphone power
	HAL_GPIO_WritePin(MICPWR_SW_GPIO_Port, MICPWR_SW_Pin, GPIO_PIN_SET);
}

void radioSetRxLNAForDevice(RadioDevice_t deviceId)
{
	if (radioDevices[deviceId].trxCurrentBand[TRX_RX_FREQ_BAND] == RADIO_BAND_VHF)
	{
		HAL_GPIO_WritePin(R5_V_SW_GPIO_Port, R5_V_SW_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(R5_U_SW_GPIO_Port, R5_U_SW_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(R5_V_SW_GPIO_Port, R5_V_SW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(R5_U_SW_GPIO_Port, R5_U_SW_Pin, GPIO_PIN_SET);
	}
}

void radioPowerOff(bool invalidateFrequency, bool includeMic)
{
	//turn off all of the transmitter and receiver voltages
	HAL_GPIO_WritePin(PA_EN_1_GPIO_Port, PA_EN_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PA_EN_2_GPIO_Port, PA_EN_2_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(R5_V_SW_GPIO_Port, R5_V_SW_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(R5_U_SW_GPIO_Port, R5_U_SW_Pin, GPIO_PIN_RESET);

	// Some external speaker mic are problematic when the
	// Mic pin is powered down, triggering some cycling PTT down events.
	// So, we're keeping that pin high all the time when Rx Power Saving is running
	if (includeMic)
	{
		HAL_GPIO_WritePin(MICPWR_SW_GPIO_Port, MICPWR_SW_Pin, GPIO_PIN_RESET);
	}

	HAL_GPIO_WritePin(C6000_PWD_GPIO_Port, C6000_PWD_Pin, GPIO_PIN_SET);// Power Down the C6000

	// Turn off Rx in AT1846S
	if (currentRadioDevice->currentBandWidthIs25kHz)
	{
		// 25 kHz settings
		radioWriteReg2byte(0x30, 0x70, 0x06); // RX off
	}
	else
	{
		// 12.5 kHz settings
		radioWriteReg2byte(0x30, 0x40, 0x06); // RX off
	}

	if (invalidateFrequency)
	{
		trxInvalidateCurrentFrequency();
	}
}

void radioInit(void)
{
	radioSetTRxDevice(RADIO_DEVICE_PRIMARY);
	AT1846sInit();
}

void radioPostinit(void)
{
	AT1846sPostInit();
}

RadioDevice_t radioSetTRxDevice(RadioDevice_t deviceId)
{
	RadioDevice_t previousDeviceId = currentRadioDeviceId;

	currentRadioDeviceId = deviceId;

	currentRadioDevice = &radioDevices[currentRadioDeviceId];

	return previousDeviceId;
}

RadioDevice_t radioGetTRxDeviceId(void)
{
	return currentRadioDeviceId;
}

void radioSetBandwidth(bool Is25K)
{
	AT1846sSetBandWidth(Is25K);
}

void radioSetMode(int mode) // Called withing trx.c: in task critical sections
{
	if (mode == RADIO_MODE_ANALOG)
	{
		HRC6000SetFMRx();
	}
	else
	{
		HRC6000SetDMR();
	}
	AT1846sSetMode(mode);
}

void radioReadVoxAndMicStrength(void)
{
	trxTxVox = adcGetVOX();					//MDUV380 Mic is not connected to AT1846 but has dedicated ADC channel like MD9600
	trxTxMic = trxTxVox;					//MDUV380 doesn't have separate Signals so use Vox for both.
}

static void trxUpdateAT1846SCalibration(void)
{
	//Nothing needed on the MDUV380
}

void radioSetFrequency(uint32_t f_in, bool Tx)
{
	// G4EML.  Hack to fix the -3KHz offset on transmit on the MDUV380
	// Believed to be caused by something in the AT1846S configuration.
	// If we ever fix what is causing it then this hack can be removed.
	if (Tx)
	{
		if (currentRadioDevice->currentBandWidthIs25kHz)
		{
#warning Hack for FM 25khz bandwidth, somehow needed after changes to activate both AT1846S. Needs more investigation, as it may be masking another problem
			f_in = f_in + 615; //add 6.15Khz to Tx frequency
		}
		else
		{
			f_in = f_in + 300; //add 3Khz to Tx frequency
		}
	}
	//

	if (currentChannelData->libreDMR_Power != 0x00)
	{
		currentRadioDevice->txPowerLevel = currentChannelData->libreDMR_Power - 1;
	}
	else
	{
		currentRadioDevice->txPowerLevel = nonVolatileSettings.txPowerLevel;
	}

	if (Tx)
	{
		currentRadioDevice->trxCurrentBand[TRX_TX_FREQ_BAND] = trxGetBandFromFrequency(f_in);
	}
	else
	{
		currentRadioDevice->trxCurrentBand[TRX_RX_FREQ_BAND] = trxGetBandFromFrequency(f_in);
		radioSetRxLNAForDevice(currentRadioDeviceId);
	}

	uint32_t f = f_in * 0.16f;
	fl_l = (f & 0x000000ff) >> 0;
	fl_h = (f & 0x0000ff00) >> 8;
	fh_l = (f & 0x00ff0000) >> 16;
	fh_h = (f & 0xff000000) >> 24;

	if (currentRadioDevice->currentBandWidthIs25kHz)
	{
		// 25 kHz settings
		radioWriteReg2byte(0x30, 0x70, 0x06); // RX off
	}
	else
	{
		// 12.5 kHz settings
		radioWriteReg2byte(0x30, 0x60, 0x06); // RX off
	}
	radioWriteReg2byte(0x05, 0x87, 0x63); // select 'normal' frequency mode

	radioWriteReg2byte(0x29, fh_h, fh_l);
	radioWriteReg2byte(0x2a, fl_h, fl_l);
	radioWriteReg2byte(0x49, 0x0C, 0x15); // setting SQ open and shut threshold

	if (currentRadioDevice->currentBandWidthIs25kHz)
	{
		// 25 kHz settings
		radioWriteReg2byte(0x30, 0x70, 0x26); // RX on
	}
	else
	{
		// 12.5 kHz settings
		radioWriteReg2byte(0x30, 0x60, 0x26); // RX on
	}

	trxUpdateAT1846SCalibration();


	if (Tx) 						//We have just set the Tx frequency so we are about to transmit. Set the Tx Power Control DAC (Shared with VHF Receive Tuning)
	{
		dacOut(1, txDACDrivePower);
	}
	else
	{
		dacOut(1, 0);
	}
}

void radioSetIF(int band, bool wide)
{
	UNUSED_PARAMETER(band);

	radioSetBandwidth(wide);
}

void radioSetTx(uint8_t band)
{
	//Turn off receiver voltages. (thats what the TYT firmware does, both receivers are on or off together)
	HAL_GPIO_WritePin(R5_V_SW_GPIO_Port, R5_V_SW_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(R5_U_SW_GPIO_Port, R5_U_SW_Pin, GPIO_PIN_RESET);

	//Configure HRC-6000 for transmit
	if (trxGetMode() == RADIO_MODE_ANALOG)
	{
		HRC6000SetFMTx();

		if (currentRadioDevice->currentBandWidthIs25kHz)
		{
			radioWriteReg2byte(0x30, 0x70, 0x46); // 25 kHz settings // RX on
		}
		else
		{
			radioWriteReg2byte(0x30, 0x60, 0x46); // 12.5 kHz settings // RX on
		}

		trxSelectVoiceChannel(AT1846_VOICE_CHANNEL_MIC);// For 1750 tone burst
		//trxSelectVoiceChannel(AT1846_VOICE_CHANNEL_NONE);//  the MDUV380 does not use the AT1846S for modulation.
		trxSetMicGainFM(nonVolatileSettings.micGainFM);
	}
	else
	{
		HRC6000SetDMR();

		radioWriteReg2byte(0x30, 0x60, 0xC6); // Digital Tx
	}

	//Turn on Tx Voltage for the current band.
	HAL_GPIO_WritePin(PA_EN_1_GPIO_Port, PA_EN_1_Pin, GPIO_PIN_SET);

	if (band == RADIO_BAND_VHF)
	{
		HAL_GPIO_WritePin(PA_SEL_SW_GPIO_Port, PA_SEL_SW_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(PA_SEL_SW_GPIO_Port, PA_SEL_SW_Pin, GPIO_PIN_SET);
	}

	//Turn on the power control circuit

	HAL_GPIO_WritePin(PA_EN_2_GPIO_Port, PA_EN_2_Pin, GPIO_PIN_SET);
	trxIsTransmitting = true;
}

//just enable or disable the RF output . doesn't change back to receive
void radioFastTx(bool tx)
{
	if (tx)
	{
		radioWriteReg2byte(0x30, 0x60, 0xC6); // Digital Tx

		HAL_GPIO_WritePin(PA_EN_1_GPIO_Port, PA_EN_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PA_EN_2_GPIO_Port, PA_EN_2_Pin, GPIO_PIN_SET);
	}
	else
	{
		//Turn off Tx Voltages to prevent transmission.

		HAL_GPIO_WritePin(PA_EN_1_GPIO_Port, PA_EN_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PA_EN_2_GPIO_Port, PA_EN_2_Pin, GPIO_PIN_RESET);

		if (currentRadioDevice->currentBandWidthIs25kHz)
		{
			// 25 kHz settings
			radioWriteReg2byte(0x30, 0x70, 0x26); // RX on
		}
		else
		{
			// 12.5 kHz settings
			radioWriteReg2byte(0x30, 0x60, 0x26); // RX on
		}
	}
}

void radioSetRx(uint8_t band)
{
	//Turn off the power control circuit
	//HAL_GPIO_WritePin(RF_APC_SW_GPIO_Port, RF_APC_SW_Pin, GPIO_PIN_RESET);

	//Turn off Tx Voltages to prevent transmission.

	HAL_GPIO_WritePin(PA_EN_1_GPIO_Port, PA_EN_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PA_EN_2_GPIO_Port, PA_EN_2_Pin, GPIO_PIN_RESET);

	//Turn on receiver voltages. (thats what the TYT firmware does, both receivers are on together)
	radioSetRxLNAForDevice(currentRadioDeviceId);

	trxIsTransmitting = false;

	if (currentRadioDevice->currentBandWidthIs25kHz)
	{
		// 25 kHz settings
		radioWriteReg2byte(0x30, 0x70, 0x26); // RX on
	}
	else
	{
		// 12.5 kHz settings
		radioWriteReg2byte(0x30, 0x60, 0x26); // RX on
	}

	if (trxGetMode() == RADIO_MODE_ANALOG)
	{
		HRC6000SetFMRx();
	}
	else
	{
		HRC6000SetDMR();
	}
}

void radioReadRSSIAndNoiseForBand(uint8_t band)
{
	uint8_t val1, val2;

	UNUSED_PARAMETER(band);// not band specific on the MDUV380

	if (rxPowerSavingIsRxOn())
	{
		taskENTER_CRITICAL();
		if (radioReadReg2byte(0x1b, &val1, &val2))
		{
			currentRadioDevice->trxRxSignal = val1;
			currentRadioDevice->trxRxNoise = val2;
		}
		taskEXIT_CRITICAL();
		trxDMRSynchronisedRSSIReadPending = false;
	}
}

#if defined(PLATFORM_MD9600)
static int getCSSToneIndex(uint16_t tone)
{
	//the TYT firmware uses a PWM output to directly generate the CTCSS Tone.
	//however the HR-C6000 can also generate tones so we will use that instead
	//this uses a table so we need to convert the CTCSS tone to the Table Index.
	uint8_t HRCToneIndex = 1;

	for (int i = 0; i < ARRAY_SIZE(HRC_CTCSSTones); i++)
	{
		if (HRC_CTCSSTones[i] == tone)
		{
			HRCToneIndex = i + 1;
			break;
		}
	}

	return HRCToneIndex;
}
#endif

void radioTxCSSOff(void)
{
	AT1846sSetTxCTCSS(0);
}

void radioTxCTCSOn(uint16_t tone)
{
	AT1846sSetTxCTCSS(tone);
}

void radioTxDCSOn(uint16_t code, bool inverted)
{
#if defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
	AT1846sSetTxDCS(code, inverted);
#elif defined(PLATFORM_MD9600)
	HRC6000SetTxDCS(code, inverted);
#else
#error UNSUPPORTED PLATFORM
#endif
}

void radioRxCSSOff(RadioDevice_t deviceId)
{
	AT1846sSetRxCSSOff(deviceId);
}

void radioRxCTCSOn(RadioDevice_t deviceId, uint16_t tone)
{
	AT1846sSetRxCTCSS(deviceId, tone);
}

void radioRxDCSOn(RadioDevice_t deviceId, uint16_t code, bool inverted)
{
	AT1846sSetRxDCS(deviceId, code, inverted);
}

bool radioCheckCSS(uint16_t tone, CodeplugCSSTypes_t type)
{
	return AT1846sCheckCSS(tone, type);
}

void radioSetTone1(int tonefreq)
{
	if(tonefreq > 0)
	{
		trxSelectVoiceChannel(AT1846_VOICE_CHANNEL_TONE1);	//configure AT1846S for tone
		HRC6000SetMic(false);								//mute the microphone while sending tone.
	}
	else
	{
		trxSelectVoiceChannel(AT1846_VOICE_CHANNEL_MIC);	//reset At1846S
		HRC6000SetMic(true);								//enable the mic
	}
	AT1846sWriteTone1Reg(tonefreq * 10);
}

void radioSetMicGainFM(uint8_t gain)
{
	HRC6000SetMicGainFM(gain);
}

void radioAudioAmp(bool on)
{
	HAL_GPIO_WritePin(AUDIO_AMP_EN_GPIO_Port, AUDIO_AMP_EN_Pin, (on ? GPIO_PIN_SET : GPIO_PIN_RESET));				//Turn on the audio amp.
	HAL_GPIO_WritePin(SPK_MUTE_GPIO_Port, SPK_MUTE_Pin, (on ? GPIO_PIN_RESET : GPIO_PIN_SET));						//Unmute the speaker
}

void radioSetAudioPath(bool fromFM)
{
	HRC6000SetFmAudio(fromFM);
}

void radioSelectVoiceChannel(uint8_t channel, uint8_t *voiceGainTx, uint16_t *deviation)
{
	AT1846sSelectVoiceChannel(channel, voiceGainTx, deviation);
}


