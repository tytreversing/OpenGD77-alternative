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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "functions/satellite.h"
#include "interfaces/pit.h"
#include "user_interface/uiGlobals.h"
#include "user_interface/menuSystem.h"
#include "user_interface/uiUtilities.h"

#if defined(USING_EXTERNAL_DEBUGGER)
#include "SeggerRTT/RTT/SEGGER_RTT.h"
#endif

static int satelliteGetDoppler(float dopplerFactor, uint32_t freq);
static void satelliteSetElementsTLE2Native(float YE_in,  float TE_in,  float M2_in,  float IN_in,  float RA_in,  float EC_in,  float WP_in,  float MA_in,
                        float MM_in,  float RV_in,  float ALON_in,satelliteKeps_t *kepDataOut);

#define ONEPPM 1.0e-6

// Days in a year
#define SATELLITE_YM  365.25f

// Tropical year, days
#define SATELLITE_YT 365.2421970

const int SATELLITE_PREDICTION_INITIAL_TIME_STEP = 256;

#define FLOAT_ROUNDING_CONSTANT 0.4999999


// WGS-84 Earth Ellipsoid
#define 	satData_RE  6378.137f
#define 	satData_FL  (1.0 / 298.257224)
#define 	currentSatelliteData_RP (satData_RE * (1.0 - satData_FL))
#define		currentSatelliteData_XX  (satData_RE * satData_RE)
#define		currentSatelliteData_ZZ  (currentSatelliteData_RP * currentSatelliteData_RP)

// YM = 365.25;		Mean year, days
// YT = 365.2421970;	Tropical year, days


// Earth's rotation rate, rads/whole day
#define		currentSatelliteData_WW  (2 * M_PI / SATELLITE_YT)

// Earth's rotation rate, rads/day
#define		currentSatelliteData_WE  (2 * M_PI + currentSatelliteData_WW)

// Earth's rotation rate, rads/sec
#define		currentSatelliteData_W0  (currentSatelliteData_WE / 86400)

// Earth's gravitational constant km^3/s^2
#define currentSatelliteData_GM  3.986E5

// 2nd Zonal coeff, Earth's gravity Field
#define currentSatelliteData_J2  1.08263E-3



// Sideral and solar data. Never needs changing. Valid to year 2000+

// GHAA, Year YG, Jan 0.0
#define currentSatelliteData_YG  2010

#define currentSatelliteData_G0  99.5578
// MA Sun and rate, deg, deg/day
#define currentSatelliteData_MAS0  356.4485
#define currentSatelliteData_MASD  0.98560028

// Sun's equation of center terms
#define currentSatelliteData_EQC1  0.03341
#define currentSatelliteData_EQC2  0.00035


satelliteData_t satelliteDataNative[NUM_SATELLITES];// Store native format Keps for each satellite

satelliteObserver_t observerData;

satelliteData_t *currentActiveSatellite;
static const int MAX_TOTAL_ITERATIONS = 1000;


static float satelliteGetElement(const uint8_t *gstr,int gstart,int glength)
{
	char gestr[40];

	memcpy(gestr, &gstr[gstart], glength);
	gestr[glength] = 0;

	return atof(gestr);
}

static char *DECOMPRESSION_LOOKUP = "0123456789. +-*";// * not actually ever in the data, its just there so there are all possible indexes

static void decompressTleData(const uint8_t *inStr, uint8_t *outStr,int len)
{
	uint8_t *outBufPtr = outStr;

	for(int i = 0; i < len; i++)
	{
		*outBufPtr++ = DECOMPRESSION_LOOKUP[inStr[i] >> 4];
		*outBufPtr++ = DECOMPRESSION_LOOKUP[inStr[i] & 0x0F];
	}
}

void satelliteTLE2Native(const char *satelliteName,const uint8_t *kep1,const uint8_t *kep2,satelliteData_t *kepDataOut)
{
	uint8_t tle1DecompressBuffer[24];
	uint8_t tle2DecompressBuffer[56];

	decompressTleData(kep1,tle1DecompressBuffer,12);
	decompressTleData(kep2,tle2DecompressBuffer,28);

	memcpy(kepDataOut->name,satelliteName,8);//  satellite name is not always a string, it may not be null terminated.
	kepDataOut->name[8] = 0;


	satelliteSetElementsTLE2Native(
		satelliteGetElement(tle1DecompressBuffer,0,2) + 2000,		// Year
		satelliteGetElement(tle1DecompressBuffer,2,12),		// TE: Elapsed time (Epoch - YG)
        satelliteGetElement(tle1DecompressBuffer,14,10),		// M2: Decay rate (rev/d/d)

		satelliteGetElement(tle2DecompressBuffer,0,8), 		// IN: Inclination (deg)
        satelliteGetElement(tle2DecompressBuffer,8,8), 		// RA: R.A.A.N (deg)
		satelliteGetElement(tle2DecompressBuffer,16,7) * 1.0e-7,	// EC: Eccentricity
		satelliteGetElement(tle2DecompressBuffer,23,8),		// WP: Arg perifee (deg)
		satelliteGetElement(tle2DecompressBuffer,31,8),		// MA: Mean motion (rev/d)
		satelliteGetElement(tle2DecompressBuffer,39,11), 		// MM: Mean motion (rev/d)
		(satelliteGetElement(tle2DecompressBuffer,50,5) + ONEPPM),	// RV: Orbit number
		0,					// ALON: Sat attitude (deg)
		&kepDataOut->keps);

}

uint32_t satelliteDayFn(int year,int month,int day)
{
	if (month <= 2 )
    {
		year -= 1;
		month += 12;
	}

	return (uint32_t)(year * SATELLITE_YM) + (int)((month + 1) * 30.6) + (day - 428);
}

float satelliteAtnFn(float y,float x)
{
	float a;

	if (x != 0.0 )
    {
        a = atan(y / x);
    }
	else
	{
        a = M_PI / 2.0 * sin(y);
	}


	if (x < 0.0 )
    {
		a = a + M_PI;
	}

	if (a < 0.0 )
	{
		a = a + 2.0 * M_PI;
	}
	return a;
}

static int satelliteGetDoppler(float dopplerFactor, uint32_t freq)
{
	int digit;
	float tally = 0.0;
	float inBetween;
	long bare;
	long factor = dopplerFactor * 1E11;

	freq = (freq + 50000L) / 100000L;
	for (int x = 4; x > -1; x--)
	{
		digit = freq/pow(10,x);
		bare = digit * pow(10,x);
		freq = freq - bare;
		inBetween =  (factor * (float)bare) / 1E6;
		tally += inBetween;
	}
	return (int)(tally + 0.5);
}

void satelliteSetObserverLocation(float lat,float lon,int height)
{
	observerData.LatInRadians = deg2rad(lat);
	observerData.LonInRadians = deg2rad(lon);
	observerData.HeightInKilometers = ((float) height)/1000.0; // this needs to be in km

	float ObserverCosLat = cos(observerData.LatInRadians);
	float ObserverSinLat = sin(observerData.LatInRadians);
	float ObserverCosLon = cos(observerData.LonInRadians);
	float ObserverSineLon = sin(observerData.LonInRadians);

	float D = sqrt(currentSatelliteData_XX * ObserverCosLat * ObserverCosLat + currentSatelliteData_ZZ * ObserverSinLat * ObserverSinLat);
	float observerRx  = currentSatelliteData_XX / D + observerData.HeightInKilometers;
	float observerRz = currentSatelliteData_ZZ / D + observerData.HeightInKilometers;

	// Observer's unit vectors Up EAST and NORTH in geocentric coordinates
	observerData.Ux = ObserverCosLat * ObserverCosLon;
	observerData.Ex = -ObserverSineLon;
	observerData.Nx = -ObserverSinLat * ObserverCosLon;

	observerData.Uy = ObserverCosLat * ObserverSineLon;
	observerData.Ey = ObserverCosLon;
	observerData.Ny = -ObserverSinLat * ObserverSineLon;

	observerData.Uz = ObserverSinLat;
	//currentSatelliteData.observerEz = 0;
	observerData.Nz = ObserverCosLat;

	// Observer's XYZ coordinates at earth's surface
	observerData.Ox = observerRx * observerData.Ux;
	observerData.Oy = observerRx * observerData.Uy;
	observerData.Oz = observerRz * observerData.Uz;

	// Observer's velocity, geocentric coordinates
	observerData.VOx = -observerData.Oy * currentSatelliteData_W0;
	observerData.VOy = observerData.Ox * currentSatelliteData_W0;
}

static void satelliteSetElementsTLE2Native(float YE_in,  float TE_in,  float M2_in,  float IN_in,  float RA_in,  float EC_in,  float WP_in,  float MA_in,
                        float MM_in,  float RV_in,  float ALON_in,satelliteKeps_t *kepDataOut)
{
	kepDataOut->RA = deg2rad(RA_in);
	kepDataOut->EC = EC_in;
	kepDataOut->WP = deg2rad(WP_in);
	kepDataOut->MA = deg2rad(MA_in);
	kepDataOut->MM = MM_in * 2.0 * M_PI;
	kepDataOut->N0 = kepDataOut->MM / 86400.0;			// Mean motion rads/s
	kepDataOut->A0 = pow(currentSatelliteData_GM / kepDataOut->N0 / kepDataOut->N0, 1.0 / 3.0);	// Semi major axis km

	kepDataOut->M2 = M2_in * 2.0 * M_PI;
	kepDataOut->RV = RV_in;// ------------------------------- POSSIBLY NOT USED

	int TE_IntPart = (int)TE_in;
	kepDataOut->TE_FloatPart = TE_in - TE_IntPart;
	kepDataOut->DE = satelliteDayFn(YE_in, 1, 0) + TE_IntPart;

	float IN = deg2rad(IN_in);
	kepDataOut->SI = sin(IN);
	kepDataOut->CI = cos(IN);
	kepDataOut->b0 = kepDataOut->A0 * sqrt(1.0 - kepDataOut->EC * kepDataOut->EC);		// Semi minor axis km

	float PC = satData_RE * kepDataOut->A0 / (kepDataOut->b0 * kepDataOut->b0);
	PC = 1.5 * currentSatelliteData_J2 * PC * PC * kepDataOut->MM;		// Precession const, rad/day
	kepDataOut->QD = -PC * kepDataOut->CI;				// Node Precession rate, rad/day
	kepDataOut->WD = PC *(5.0 * kepDataOut->CI * kepDataOut->CI - 1.0) / 2.0;	// Perigee Precession rate, rad/day
	kepDataOut->DC = -2.0 * kepDataOut->M2 / kepDataOut->MM / 3.0;		// Drag coeff

	// Bring Sun data to satellite epoch
	float TEG = (kepDataOut->DE - satelliteDayFn(currentSatelliteData_YG, 1, 0)) + kepDataOut->TE_FloatPart;	// Elapsed Time: Epoch - YG
	kepDataOut->GHAE = deg2rad(currentSatelliteData_G0) + TEG * currentSatelliteData_WE;		// GHA Aries, epoch
}


void satelliteCalculateForDateTimeSecs(const satelliteData_t *satelliteData, time_t_custom dateTimeSecs, satelliteResults_t *currentSatelliteData, satellitePredictionLevel_t predictionLevel)
{
	struct tm timeAndDate;
	gmtime_r_Custom(&dateTimeSecs, &timeAndDate);

	uint32_t tmpDN = (uint32_t)satelliteDayFn((timeAndDate.tm_year + 1900),(timeAndDate.tm_mon + 1),timeAndDate.tm_mday);
	float tmpTN = ((float)timeAndDate.tm_hour + ((float)timeAndDate.tm_min + ((float)timeAndDate.tm_sec/60.0)) /60.0)/24.0;

	float tmpT = (tmpDN - satelliteData->keps.DE) + (tmpTN - satelliteData->keps.TE_FloatPart);//83.848;	// Elapsed T since epoch
	float tmpDT = satelliteData->keps.DC * tmpT / 2.0;			// Linear drag terms
	float tmpKD = 1.0 + 4.0 * tmpDT;
	float tmpKDP = 1.0 - 7.0 * tmpDT;
	float tmpM = satelliteData->keps.MA + satelliteData->keps.MM * tmpT * (1.0 - 3.0 * tmpDT); 	// Mean anomaly at YR,/ TN
	int tmpDR = (int)(tmpM / (2.0 * M_PI));		// Strip out whole no of revs
	tmpM = tmpM - tmpDR * 2.0 * M_PI;              	// M now in range 0 - 2PI
	//currentSatelliteData.RN = satelliteData->keps.RV + tmpDR + 1;                   	// VK3KYY We don't need to know the Current orbit number

	// Solve M = EA - EC * sin(EA) for EA given M, by Newton's method
	float tmpEA = tmpM;					// Initail solution
	float tmp;
	float tmpDNOM;
	float tmpC,tmpS;
	do	{
		tmpC = cos(tmpEA);
		tmpS = sin(tmpEA);
		tmpDNOM = 1.0 - satelliteData->keps.EC * tmpC;
		tmp = (tmpEA - satelliteData->keps.EC * tmpS - tmpM) / tmpDNOM;	// Change EA to better resolution
		tmpEA = tmpEA - tmp;			// by this amount until converged
	} while (fabs(tmp) > 1.0E-5 );

	// Distances
	float tmpA = satelliteData->keps.A0 * tmpKD;
	float tmpB = satelliteData->keps.b0 * tmpKD;
#if NEEDS_SATELLITE_LAT_LONG
	float tmpRS = tmpA * tmpDNOM;
#endif
	// Calculate satellite position and velocity in plane of ellipse
	float tmpSx = tmpA * (tmpC - satelliteData->keps.EC);
	float tmpVx = -tmpA * tmpS / tmpDNOM * satelliteData->keps.N0;
	float tmpSy = tmpB * tmpS;
	float tmpVy = tmpB * tmpC / tmpDNOM * satelliteData->keps.N0;

	float tmpAP = satelliteData->keps.WP + satelliteData->keps.WD * tmpT * tmpKDP;
	float tmpCWw = cos(tmpAP);
	float tmpSW = sin(tmpAP);
	float tmpRAAN =  satelliteData->keps.RA + satelliteData->keps.QD * tmpT * tmpKDP;
	float tmpCO = cos(tmpRAAN);
	float tmpSO = sin(tmpRAAN);

	// Plane -> celestial coordinate transformation, [C] = [RAAN]*[IN]*[AP]
	float tmpCXx = tmpCWw * tmpCO - tmpSW * satelliteData->keps.CI * tmpSO;
	float tmpCXy = -tmpSW * tmpCO - tmpCWw * satelliteData->keps.CI * tmpSO;

	float tmpCYx = tmpCWw * tmpSO + tmpSW * satelliteData->keps.CI * tmpCO;
	float tmpCYy = -tmpSW * tmpSO + tmpCWw * satelliteData->keps.CI * tmpCO;

	float tmpCZx = tmpSW * satelliteData->keps.SI;
	float tmpCZy = tmpCWw * satelliteData->keps.SI;

	// Compute satellite's position vector, ANTenna axis unit vector
	// and velocity  in celestial coordinates. (Note: Sz = 0, Vz = 0)
	float tmpSATx = tmpSx * tmpCXx + tmpSy * tmpCXy;
	float tmpVELx = tmpVx * tmpCXx + tmpVy * tmpCXy;
	float tmpSATy = tmpSx * tmpCYx + tmpSy * tmpCYy;
	float tmpVELy = tmpVx * tmpCYx + tmpVy * tmpCYy;
	float tmpSATz = tmpSx * tmpCZx + tmpSy * tmpCZy;
	float tmpVELz = tmpVx * tmpCZx + tmpVy * tmpCZy;

	// Also express SAT, ANT, and VEL in geocentric coordinates
	float tmpGHAA = satelliteData->keps.GHAE + currentSatelliteData_WE * tmpT;		// GHA Aries at elaprsed time T
	tmpC = cos(-tmpGHAA);
	tmpS = sin(-tmpGHAA);
	tmpSx = tmpSATx * tmpC - tmpSATy * tmpS;
	tmpVx = tmpVELx * tmpC - tmpVELy * tmpS;
	tmpSy = tmpSATx * tmpS + tmpSATy * tmpC;
	tmpVy = tmpVELx * tmpS + tmpVELy * tmpC;

	float tmpRx = tmpSx - observerData.Ox;
	float tmpRy = tmpSy - observerData.Oy;
	float tmpRz = tmpSATz - observerData.Oz;

	float tmpR = sqrt(tmpRx * tmpRx + tmpRy * tmpRy + tmpRz * tmpRz);    /* Range Magnitute */

	// Normalize range vector
	tmpRx = tmpRx / tmpR;
	tmpRy = tmpRy / tmpR;
	tmpRz = tmpRz / tmpR;

	float tmpU = tmpRx * observerData.Ux + tmpRy * observerData.Uy + tmpRz * observerData.Uz;
	currentSatelliteData->elevation = rad2deg(asin(tmpU));

	if (predictionLevel == SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY)
	{
		return;
	}

	float tmpE = tmpRx * observerData.Ex + tmpRy * observerData.Ey;
	float tmpN = tmpRx * observerData.Nx + tmpRy * observerData.Ny + tmpRz * observerData.Nz;

	currentSatelliteData->azimuth = rad2deg(satelliteAtnFn(tmpE, tmpN));
	currentSatelliteData->azimuthAsInteger = (int)(currentSatelliteData->azimuth + FLOAT_ROUNDING_CONSTANT);// round
	currentSatelliteData->elevationAsInteger =  (currentSatelliteData->elevation < 0.0)?((int)(currentSatelliteData->elevation - FLOAT_ROUNDING_CONSTANT)):((int)(currentSatelliteData->elevation + FLOAT_ROUNDING_CONSTANT));
	// Solve antenna vector along unit range vector, -r.a = cos(SQ)
	// SQ = deg(acos(-(Ax * Rx + Ay * Ry + Az * Rz)));

	if (predictionLevel == SATELLITE_PREDICTION_LEVEL_TIME_EL_AND_AZ)
	{
		return;
	}
	// else.. must be SATELLITE_PREDICTION_LEVEL_FULL

#if NEEDS_SATELLITE_LAT_LONG
	// Calculate sub-satellite Lat/Lon
    currentSatelliteData->longitude = rad2deg(satelliteAtnFn(tmpSy, tmpSx));		// Lon, + East
	currentSatelliteData->latitude = rad2deg(asin(tmpSATz / tmpRS));		// Lat, + North

	if (currentSatelliteData->longitude > 180.0 )
    {
   		currentSatelliteData->longitude -= 360.0;			// -ve is degrees West
    }
#endif

	// Resolve Sat-Obs velocity vector along unit range vector. (VOz = 0)
	float rangeRate = (tmpVx - observerData.VOx) * tmpRx + (tmpVy - observerData.VOy) * tmpRy + tmpVELz * tmpRz; // Range rate, km/sec
	float dopplerFactor = rangeRate / 299792.0;


	currentSatelliteData->freqs[SATELLITE_VOICE_FREQ].rxFreq = satelliteData->freqs[SATELLITE_VOICE_FREQ].rxFreq - satelliteGetDoppler(dopplerFactor, satelliteData->freqs[SATELLITE_VOICE_FREQ].rxFreq);
	currentSatelliteData->freqs[SATELLITE_VOICE_FREQ].txFreq = satelliteData->freqs[SATELLITE_VOICE_FREQ].txFreq + satelliteGetDoppler(dopplerFactor, satelliteData->freqs[SATELLITE_VOICE_FREQ].txFreq);

	currentSatelliteData->freqs[SATELLITE_APRS_FREQ].rxFreq = satelliteData->freqs[SATELLITE_APRS_FREQ].rxFreq - satelliteGetDoppler(dopplerFactor, satelliteData->freqs[SATELLITE_APRS_FREQ].rxFreq);
	currentSatelliteData->freqs[SATELLITE_APRS_FREQ].txFreq = satelliteData->freqs[SATELLITE_APRS_FREQ].txFreq + satelliteGetDoppler(dopplerFactor, satelliteData->freqs[SATELLITE_APRS_FREQ].txFreq);
	currentSatelliteData->freqs[SATELLITE_APRS_FREQ].txCTCSS = currentSatelliteData->freqs[SATELLITE_APRS_FREQ].armCTCSS = 0;

	currentSatelliteData->freqs[SATELLITE_OTHER_FREQ].rxFreq = satelliteData->freqs[SATELLITE_OTHER_FREQ].rxFreq - satelliteGetDoppler(dopplerFactor, satelliteData->freqs[SATELLITE_OTHER_FREQ].rxFreq);
	currentSatelliteData->freqs[SATELLITE_OTHER_FREQ].txFreq = satelliteData->freqs[SATELLITE_OTHER_FREQ].txFreq + satelliteGetDoppler(dopplerFactor, satelliteData->freqs[SATELLITE_OTHER_FREQ].txFreq);
	currentSatelliteData->freqs[SATELLITE_OTHER_FREQ].txCTCSS = currentSatelliteData->freqs[SATELLITE_OTHER_FREQ].armCTCSS = 0;

}

bool satellitePredictNextPassFromDateTimeSecs(predictionStateMachineData_t *stateData, const satelliteData_t *satelliteData, time_t_custom startDateTimeSecs, time_t_custom limitDateTimeSecs, int maxIterations, satellitePass_t *nextPass)
{
	satelliteResults_t currentSatelliteData;

    switch(stateData->state)
    {
    	case PREDICTION_STATE_INIT_AOS:
    		stateData->currentDateTimeSecs = startDateTimeSecs;
        	stateData->timeStep = SATELLITE_PREDICTION_INITIAL_TIME_STEP;
        	stateData->found = false;
    		stateData->totalIterations = 0;
    		stateData->foundStart = false;
    		stateData->direction = 1;
    		stateData->state = PREDICTION_STATE_FIND_AOS;
    		nextPass->valid = PREDICTION_RESULT_NONE;
			nextPass->satelliteMaxElevation = -1;// not yet calculated

    		//break;   deliberate drop through

    	case PREDICTION_STATE_FIND_AOS:
			stateData->iterations = 0;

    		do
    		{
    			stateData->currentDateTimeSecs += (stateData->timeStep * stateData->direction); // move  forward
    			satelliteCalculateForDateTimeSecs(satelliteData, stateData->currentDateTimeSecs, &currentSatelliteData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

    			if (!stateData->foundStart && currentSatelliteData.elevation >= 0)
    			{
    				stateData->foundStart = true;
    			}
    			if (stateData->foundStart)
    			{
    				if (stateData->timeStep == 1)
    				{
    					if (currentSatelliteData.elevation >= 0)
    					{
    						stateData->found = true;
    					}
    					else
    					{
    						stateData->direction = 1;
    					}
    				}
    				else
    				{
    					stateData->timeStep /= 2;
    					if (currentSatelliteData.elevation >= 0)
    					{
    						stateData->direction = -1;
    					}
    					else
    					{
    						stateData->direction = 1;
    					}
    				}
    			}
    			else
    			{
    				if (currentSatelliteData.elevation < -30)
					{
    					if (stateData->timeStep == SATELLITE_PREDICTION_INITIAL_TIME_STEP)
						{
    						stateData->timeStep = SATELLITE_PREDICTION_INITIAL_TIME_STEP * 4;
						}
					}
    				else
    				{
    					if (stateData->timeStep == (SATELLITE_PREDICTION_INITIAL_TIME_STEP * 4))
						{
							stateData->timeStep = SATELLITE_PREDICTION_INITIAL_TIME_STEP;
						}
    				}
    			}
    			stateData->iterations++;
    			stateData->totalIterations++;
    		} while (	!stateData->found &&
    					(stateData->iterations < maxIterations) &&
						(stateData->totalIterations < MAX_TOTAL_ITERATIONS) &&
						stateData->currentDateTimeSecs < limitDateTimeSecs);

    		if (stateData->currentDateTimeSecs >= limitDateTimeSecs)
    		{
    			stateData->state = PREDICTION_STATE_LIMIT;
    			return false;
    		}

    		if (!(stateData->iterations < maxIterations) || !(stateData->totalIterations < MAX_TOTAL_ITERATIONS))
    		{
    			stateData->state = PREDICTION_STATE_ITERATION_LIMIT;
    			return false;
    		}

    		if (stateData->found)
    		{
				nextPass->satelliteAOS = stateData->currentDateTimeSecs;

				stateData->state = PREDICTION_STATE_INIT_LOS;
    		}
    		break;

    	case PREDICTION_STATE_INIT_LOS:
    		stateData->found = false;
			stateData->foundStart = false;
			stateData->timeStep = SATELLITE_PREDICTION_INITIAL_TIME_STEP;
			stateData->direction = 1;

			// deliberate drop through

    	case PREDICTION_STATE_FIND_LOS:
			stateData->iterations = 0;

    		do
    		{
    			stateData->currentDateTimeSecs += (stateData->timeStep * stateData->direction); // move  forward
    			satelliteCalculateForDateTimeSecs(satelliteData, stateData->currentDateTimeSecs, &currentSatelliteData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

    			if (!stateData->foundStart && currentSatelliteData.elevation < 0)
    			{
    				stateData->foundStart = true;
    			}
    			if (stateData->foundStart)
    			{
    				if (stateData->timeStep == 1)
    				{
    					if (currentSatelliteData.elevation < 0)
    					{
    						stateData->found = true;
    					}
    					else
    					{
    						stateData->direction = 1;
    					}
    				}
    				else
    				{
    					stateData->timeStep /= 2;
    					if (currentSatelliteData.elevation < 0)
    					{
    						stateData->direction = -1;
    					}
    					else
    					{
    						stateData->direction = 1;
    					}
    				}

    			}
    			stateData->iterations++;

    		} while (	!stateData->found &&
    					(stateData->iterations < maxIterations) &&
						(stateData->totalIterations < MAX_TOTAL_ITERATIONS) &&
						 stateData->currentDateTimeSecs < limitDateTimeSecs);

    		if (stateData->currentDateTimeSecs >= limitDateTimeSecs)
    		{
    			stateData->state = PREDICTION_STATE_LIMIT;
    			return false;
    		}

    		if (!(stateData->iterations < maxIterations) || !(stateData->totalIterations < MAX_TOTAL_ITERATIONS))
    		{
    			stateData->state = PREDICTION_STATE_ITERATION_LIMIT;
    			return false;
    		}

    		if (stateData->found)
    		{
    			nextPass->satelliteLOS = stateData->currentDateTimeSecs;

				nextPass->satellitePassDuration = nextPass->satelliteLOS - nextPass->satelliteAOS;

				//satelliteGetMaximumElevation(satelliteData , nextPass);// Use lazy calculation now

				stateData->state = PREDICTION_STATE_COMPLETE;
    		}
    		break;

    	case PREDICTION_STATE_NONE:
    	case PREDICTION_STATE_COMPLETE:
    	case PREDICTION_STATE_LIMIT:
    	case PREDICTION_STATE_ITERATION_LIMIT:
    		return false;
    		break;
    }

	return true;
}



uint16_t satelliteGetMaximumElevation(satelliteData_t *satelliteData, uint32_t passNumber)
{
	float lastEl, halfPointElevation;
	satelliteResults_t resultsData;
	satellitePass_t *pass;
	time_t_custom dataTime;

// Step size of 1 does not seem to be needed even for passes which are directly overhead within less than 1.0 deg
#define MAX_ELE_FIND_STEP  2
#define MAX_ELE_MIN_STEP_CHANGE_DEG 0.01

	pass = &satelliteData->predictions.passes[passNumber];

	if (pass->satelliteMaxElevation >= 0)
	{
		return pass->satelliteMaxElevation;
	}

	dataTime = (pass->satelliteAOS + pass->satelliteLOS)/2;// max height will be in middle of the pass... Probably
	satelliteCalculateForDateTimeSecs(satelliteData, dataTime, &resultsData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

	pass->satelliteMaxElevation = (int16_t)(resultsData.elevation + FLOAT_ROUNDING_CONSTANT);

	halfPointElevation = resultsData.elevation;

	dataTime -= MAX_ELE_FIND_STEP;// try prior to mid point time

	satelliteCalculateForDateTimeSecs(satelliteData, dataTime, &resultsData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

	if ((resultsData.elevation - halfPointElevation) > MAX_ELE_MIN_STEP_CHANGE_DEG)
	{
		do
		{
			lastEl = resultsData.elevation;
			pass->satelliteMaxElevation = (int16_t)(resultsData.elevation + FLOAT_ROUNDING_CONSTANT);
			dataTime -= MAX_ELE_FIND_STEP;
			satelliteCalculateForDateTimeSecs(satelliteData, dataTime, &resultsData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

		} while ((resultsData.elevation - lastEl) > MAX_ELE_MIN_STEP_CHANGE_DEG);
	}
	else
	{
		dataTime += MAX_ELE_FIND_STEP * 2;// step in twice the direction because the previous test set the dateTime to one second before the mid time point.
		satelliteCalculateForDateTimeSecs(satelliteData, dataTime, &resultsData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);
		if ((resultsData.elevation - halfPointElevation) > MAX_ELE_MIN_STEP_CHANGE_DEG)
		{
			do
			{
				lastEl = resultsData.elevation;
				pass->satelliteMaxElevation = (int16_t)(resultsData.elevation + FLOAT_ROUNDING_CONSTANT);
				dataTime += MAX_ELE_FIND_STEP;
				satelliteCalculateForDateTimeSecs(satelliteData, dataTime, &resultsData, SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY);

			} while ((resultsData.elevation - lastEl) > MAX_ELE_MIN_STEP_CHANGE_DEG);
		}
	}

	return pass->satelliteMaxElevation;
}
