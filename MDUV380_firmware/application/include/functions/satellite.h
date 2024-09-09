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

#ifndef _SATELLITE_H_
#define _SATELLITE_H_

#include "user_interface/uiGlobals.h"

#define NUM_SATELLITES 25
#define NUM_SATELLITE_PREDICTIONS 15
#define ADDITION_DATA_SIZE 24

#define deg2rad(deg) M_PI / 180.0 * deg
#define rad2deg(rad) rad * 180.0 / M_PI

typedef enum
{
	PREDICTION_RESULT_NONE,
	PREDICTION_RESULT_OK,
	PREDICTION_RESULT_LIMIT,
	PREDICTION_RESULT_TIMEOUT
} predictionResult_t;

typedef struct
{
	time_t_custom				satelliteAOS;
	time_t_custom				satelliteLOS;
	int16_t						satelliteMaxElevation;
	time_t_custom				satellitePassDuration;
	predictionResult_t			valid;
} satellitePass_t;



typedef struct
{
	uint32_t numPasses;
	uint32_t numPassBeingPredicted;
	uint32_t listDisplayPassSearchStartIndex;
	satellitePass_t passes[NUM_SATELLITE_PREDICTIONS];
	uint32_t selectedPassNumber;
	bool 	isPredicting;
	bool	isVisible;
} satellitePredictions_t;



typedef enum satellitePreductionState { PREDICTION_STATE_NONE, PREDICTION_STATE_INIT_AOS, PREDICTION_STATE_FIND_AOS, PREDICTION_STATE_INIT_LOS, PREDICTION_STATE_FIND_LOS, PREDICTION_STATE_COMPLETE, PREDICTION_STATE_LIMIT, PREDICTION_STATE_ITERATION_LIMIT } satellitePreductionState_t;

typedef struct
{
	time_t_custom	currentDateTimeSecs;
	int 			timeStep;
    bool 			found;
    int 			iterations;
    int 			totalIterations;
    bool 			foundStart;
    int 			direction;
    satellitePreductionState_t state;
} predictionStateMachineData_t;

typedef struct
{
	float 		DE;
	float		TE_FloatPart;		// Epoch time (days)
	float		SI;		// Sin Inclination (deg)
	float		CI;		// Cos Inclination (deg)
	float		RA;		// R.A.A.N (deg)
	float		EC;		// Eccentricity
	float		WP;		// Arg perifee (deg)
	float		MA;		// Mean anomaly (rev/d)
	float		MM;		// Mean motion (rev/d)
	float 		N0; 	// MM / 86400.0;			// Mean motion rads/s
	float		A0;		// pow(currentSatelliteData_GM / kepData->N0 / kepData->N0, 1.0 / 3.0);	// Semi major axis km
	float		b0;		// Semi minor axis (km)
	float		M2;		// Decay rate (rev/d/d)
	long		RV;		// Orbit number
	float		ALAT;		// Sat attitude (deg) 0 = nominal
	float		QD;		// Node precession rate, rad/day
	float		WD;		// Perigee precession rate, rad/day
	float		DC;		// Drag coeff. (Angular momentum rate)/(Ang mom)  s^-1
	float		GHAE;		// GHA Aries, epoch

} satelliteKeps_t;


typedef struct
{
	uint32_t 	rxFreq;// resultant  corrected for Doppler
	uint32_t 	txFreq;	// resultant  corrected for Doppler
	uint16_t	txCTCSS;
	uint16_t	armCTCSS;
} satellite_txRxFreqs;

typedef enum
{
	SATELLITE_VOICE_FREQ 	= 0,
	SATELLITE_APRS_FREQ,
	SATELLITE_OTHER_FREQ,
} satelliteFreq_t;

typedef struct
{
	char 				name[17];
	satelliteKeps_t 	keps;
	satellite_txRxFreqs freqs[3];
    char 				AdditionalData[ADDITION_DATA_SIZE];
    satellitePredictions_t predictions;
} satelliteData_t;

typedef struct
{
	float		Ex;
	float		Ey;
	float		Ny;
	float		Nx;
	float		Nz;
	float		Ox;
	float		Oy;
	float		Oz;
	float		Ux;		// Observer's unit vectors UP EAST and NORTH in GEOCENTRIC coords.
	float		Uy;
	float		Uz;
	float		VOy;
	float		VOx;
	float		VOz;
	float		LatInRadians;
	float		LonInRadians;
	float		HeightInKilometers;
} satelliteObserver_t;


typedef struct
{
	float		elevation;			// Elevaton
	float		azimuth;			// Azimuth
	int			elevationAsInteger;
	int			azimuthAsInteger;
#if NEEDS_SATELLITE_LAT_LONG
	float		longitude;			// Lon, + East
	float		latitude;			// Lat, + North
#endif
	satellite_txRxFreqs	freqs[3];
} satelliteResults_t;

typedef struct __attribute__((__packed__))
{
	char 		TLE_Name[8];
	uint8_t 	TLE_Line1[12];
	uint8_t 	TLE_Line2[28];
	uint32_t	rxFreq1;
	uint32_t	txFreq1;
	uint16_t	txCTCSS1;
	uint16_t	armCTCSS1;
	uint32_t 	rxFreq2;
    uint32_t 	txFreq2;
	uint32_t 	rxFreq3;
    uint32_t 	txFreq3;
    char 		AdditionalData[ADDITION_DATA_SIZE];

} codeplugSatelliteData_t;


typedef union
{
	codeplugSatelliteData_t data[NUM_SATELLITES];
	uint8_t forceToSize[2520];
} codeplugSatelliteCuctsomDataUnion_t;

typedef enum
{
	SATELLITE_PREDICTION_LEVEL_TIME_AND_ELEVATION_ONLY, SATELLITE_PREDICTION_LEVEL_TIME_EL_AND_AZ, SATELLITE_PREDICTION_LEVEL_FULL
} satellitePredictionLevel_t;

extern satelliteData_t satelliteDataNative[NUM_SATELLITES];
extern satelliteData_t *currentActiveSatellite;
extern satelliteFreq_t currentSatelliteFreqIndex;

void satelliteSetObserverLocation(float lat,float lon,int height);
void satelliteTLE2Native(const char *satelliteName,const uint8_t *kep1,const uint8_t *kep2,satelliteData_t *kepDataOut);
void satelliteCalculateForDateTimeSecs(const satelliteData_t *satelliteData, time_t_custom dateTimeSecs, satelliteResults_t *currentSatelliteData, satellitePredictionLevel_t predictionLevel);
bool satellitePredictNextPassFromDateTimeSecs(predictionStateMachineData_t *stateData, const satelliteData_t *satelliteData, time_t_custom startDateTimeSecs, time_t_custom limitDateTimeSecs, int maxIterations, satellitePass_t *nextPass);
uint16_t satelliteGetMaximumElevation(satelliteData_t *satelliteData, uint32_t passNumber);
#endif
