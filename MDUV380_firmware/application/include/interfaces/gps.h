/*
 * Copyright (C) 2021-2024 Roger Clark, VK3KYY / G4KYF
 *                         Colin Durbridge, G4EML
 *                         Daniel Caujolle-Bert, F1RMB
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

#ifndef _OPENGD77_GPS_H_
#define _OPENGD77_GPS_H_

#if defined(HAS_GPS)

#define GPS_LINE_LENGTH         81U // NMEA specs is 82 max length, including <CR><LF>, but we need an extra NULL terminator
#define GPS_DMA_BUFFER_SIZE     64U
#define GPS_STORAGE_MAX         32U

typedef struct
{
    uint16_t	Number;
    int16_t		El;
    int16_t 	Az;
    int8_t		RSSI;
} gpsSatellitesData_t;

#define GPS_STATUS_HAS_FIX               (1 << 0)  // Set as long as Fix is valid
#define GPS_STATUS_HAS_POSITION          (1 << 1)
#define GPS_STATUS_HAS_HDOP              (1 << 2)
#define GPS_STATUS_HAS_COURSE            (1 << 3)
#define GPS_STATUS_HAS_SPEED             (1 << 4)
#define GPS_STATUS_HAS_HEIGHT            (1 << 5)
#define GPS_STATUS_HAS_TIME              (1 << 6)
#define GPS_STATUS_2D_FIX                (1 << 7)  // 2D fix type
#define GPS_STATUS_3D_FIX                (1 << 8)  // 3D fix type (only 2D or 3D is set at a time, if available)
#define GPS_STATUS_FIX_UPDATED           (1 << 9)  // Fix status changed (HAS_FIX bit)
#define GPS_STATUS_FIXTYPE_UPDATED       (1 << 10)  // Fix type status as changed (2D_FIX and 3D_FIX bits)
#define GPS_STATUS_POSITION_UPDATED      (1 << 11) // Current position changed
#define GPS_STATUS_TIME_UPDATED          (1 << 12) // Time changed
#define GPS_STATUS_HDOP_UPDATED          (1 << 13) // HDOP changed
#define GPS_STATUS_COURSE_UPDATED        (1 << 14) // Course changed
#define GPS_STATUS_SPEED_UPDATED         (1 << 15) // Speed changed
#define GPS_STATUS_HEIGHT_UPDATED        (1 << 16) // Height changed
#define GPS_STATUS_GPS_SATS_UPDATED      (1 << 17) // GPS: Name or Position or RSSI changed
#define GPS_STATUS_BD_SATS_UPDATED       (1 << 18) // BeiDou: Name or Position or RSSI changed

#define GPS_SPEED_THRESHOLD_MIN           53U // Minimum usable speed threshold (in hundredth of knot). more than 0.5399568034557235 kn == 1km/h

typedef struct
{
	uint32_t             Status;
	uint32_t             Latitude;
	double               LatitudeHiRes;
	uint32_t             Longitude;
	double               LongitudeHiRes;
	time_t_custom        Time;
	uint16_t             SatsInViewGP;
	uint16_t             SatsInViewBD;
	gpsSatellitesData_t  GPSatellites[GPS_STORAGE_MAX];
	gpsSatellitesData_t  BDSatellites[GPS_STORAGE_MAX];
	uint8_t              currentGPSIndex;
	uint8_t              currentBDIndex;
	uint16_t             AccuracyInCm; // HDOP
	int16_t              HeightInM; // It's possible to be below sea level in some places in the world
	uint16_t             CourseInHundredthDeg; // in hundredth of degree
	uint16_t             SpeedInHundredthKn;   // in hundredth of knot
} gpsData_t;

extern gpsData_t gpsData;

void gpsTick(void);
void gpsOn(void);
void gpsOff(void);
void gpsOnUsingQuickKey(bool on);
#if defined(PLATFORM_MD9600)
void gpsAndMicPower(bool on);
#endif
void gpsPower(bool on);
void gpsDataInputStartStop(bool enable);
#if defined(PLATFORM_MD9600) || defined(CPU_MK22FN512VLL12)
void gpsProcessChar(uint8_t rxchar);
#endif
void gpsInit(void);
#if defined(LOG_GPS_DATA)
void gpsLoggingStart(void);
void gpsLoggingStop(void);
void gpsLoggingClear(void);
#endif
#endif // HAS_GPS

#endif /* _OPENGD77_GPS_H_ */
