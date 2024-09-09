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
#ifndef _OPENGD77_UIGLOBALS_H_
#define _OPENGD77_UIGLOBALS_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "main.h"
#include "functions/settings.h"
#include "functions/codeplug.h"
#include "functions/ticks.h"
#include "functions/aprs.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define FEET_PER_METER 3.2808399
#define MPS_PER_KNOT   0.51444444
#define MPS_TO_KMPH    3.60
#define MPS_TO_MPH     2.23693629
#define MPH_PER_KNOT   1.15077945
#define KMPH_PER_KNOT  1.852
#define KNOT_PER_KMPH  0.53995680 //35
#define MPS_PER_KMPH   0.277778


#define MILLISECS_PER_MIN  60000L
#define MILLISECS_PER_SEC  1000L

typedef uint32_t time_t_custom;     /* date/time in unix secs past 1-Jan-70 */

#define MAX_ZONE_SCAN_NUISANCE_CHANNELS       16
#define NUM_LASTHEARD_STORED                  32

#if defined(PLATFORM_RD5R)
#define DISPLAY_H_EXTRA_PIXELS                 0
#define DISPLAY_H_OFFSET                       0
#define DISPLAY_V_EXTRA_PIXELS                 0
#define DISPLAY_V_OFFSET                       0
#define MENU_ENTRY_HEIGHT                     10
#define SQUELCH_BAR_H                          5
#define V_OFFSET                               2
#define OVERRIDE_FRAME_HEIGHT                 11
#define VFO_LETTER_Y_OFFSET                    0
#define LH_ENTRY_V_OFFSET                      1
#define DISPLAY_Y_POS_HEADER                   0
#define DISPLAY_X_POS_MENU_OFFSET              0
#define DISPLAY_X_POS_MENU_TEXT_OFFSET       (DISPLAY_X_POS_MENU_OFFSET + 0)
#define DISPLAY_Y_POS_MENU_START             (16 + MENU_ENTRY_HEIGHT + 3)
#define DISPLAY_Y_POS_MENU_ENTRY_HIGHLIGHT   (DISPLAY_Y_POS_MENU_START - 1)
#define DISPLAY_Y_POS_BAR                      8
#define DISPLAY_Y_POS_CONTACT                 12
#define DISPLAY_Y_POS_CONTACT_TX              28
#define DISPLAY_Y_POS_CONTACT_TX_FRAME        26
#define DISPLAY_Y_POS_CHANNEL_FIRST_LINE      24
#define DISPLAY_Y_POS_CHANNEL_SECOND_LINE     38
#define DISPLAY_Y_POS_SQUELCH_BAR             14
#define DISPLAY_Y_POS_CSS_INFO                13
#define DISPLAY_Y_POS_SQL_INFO                22
#define DISPLAY_Y_POS_TX_TIMER                12
#define DISPLAY_Y_POS_RX_FREQ                 31
#define DISPLAY_Y_POS_TX_FREQ                 40
#define DISPLAY_Y_POS_ZONE                    40
#define DISPLAY_Y_POS_RSSI_VALUE              16
#define DISPLAY_Y_POS_RSSI_BAR                27
#define TITLE_BOX_HEIGHT                      15
#elif defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701)
#define DISPLAY_H_EXTRA_PIXELS                32
#define DISPLAY_H_OFFSET                     (DISPLAY_H_EXTRA_PIXELS / 2)
#if defined(PLATFORM_VARIANT_DM1701)
#define DISPLAY_V_EXTRA_PIXELS               (64 - DISPLAY_Y_OFFSET)
#else
#define DISPLAY_V_EXTRA_PIXELS                64
#endif
#define DISPLAY_V_OFFSET                     (DISPLAY_V_EXTRA_PIXELS / 2)
#define MENU_ENTRY_HEIGHT                     16
#define SQUELCH_BAR_H                          9
#define V_OFFSET                               0
#define OVERRIDE_FRAME_HEIGHT                 16
#define VFO_LETTER_Y_OFFSET                    8
#define LH_ENTRY_V_OFFSET                      0
#define DISPLAY_Y_POS_HEADER                   2
#define DISPLAY_X_POS_MENU_OFFSET              4
#define DISPLAY_X_POS_MENU_TEXT_OFFSET       (DISPLAY_X_POS_MENU_OFFSET + 4)
#define DISPLAY_Y_POS_MENU_START             (16 + MENU_ENTRY_HEIGHT)
#if defined(PLATFORM_VARIANT_DM1701)
#define DISPLAY_Y_POS_MENU_ENTRY_HIGHLIGHT   (32 + DISPLAY_V_OFFSET - (MENU_ENTRY_HEIGHT / 2))
#else
#define DISPLAY_Y_POS_MENU_ENTRY_HIGHLIGHT   (32 + DISPLAY_V_OFFSET)
#endif
#define DISPLAY_Y_POS_BAR                     10
#define DISPLAY_Y_POS_CONTACT                (16 + 8)
#define DISPLAY_Y_POS_CONTACT_TX             (34 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_CONTACT_TX_FRAME       (34 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_CHANNEL_FIRST_LINE     (32 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_CHANNEL_SECOND_LINE    (48 + 48)
#define DISPLAY_Y_POS_SQUELCH_BAR             16
#define DISPLAY_Y_POS_CSS_INFO               (16 + 8)
#define DISPLAY_Y_POS_SQL_INFO               (25 + 8)
#define DISPLAY_Y_POS_TX_TIMER                (8 + 16)
#define DISPLAY_Y_POS_RX_FREQ                (40 + 40)
#define DISPLAY_Y_POS_TX_FREQ                (48 + 48)
#define DISPLAY_Y_POS_ZONE                   (50 + DISPLAY_V_EXTRA_PIXELS)
#define DISPLAY_Y_POS_RSSI_VALUE             (18 + 16)
#define DISPLAY_Y_POS_RSSI_BAR               (40 + DISPLAY_V_OFFSET)
#define TITLE_BOX_HEIGHT                      21
#elif defined(PLATFORM_MD2017)
#define DISPLAY_H_EXTRA_PIXELS                32
#define DISPLAY_H_OFFSET                     (DISPLAY_H_EXTRA_PIXELS / 2)
#define DISPLAY_V_EXTRA_PIXELS                64
#define DISPLAY_V_OFFSET                     (DISPLAY_V_EXTRA_PIXELS / 2)
#define MENU_ENTRY_HEIGHT                     16
#define SQUELCH_BAR_H                          9
#define V_OFFSET                               0
#define OVERRIDE_FRAME_HEIGHT                 16
#define VFO_LETTER_Y_OFFSET                    8
#define LH_ENTRY_V_OFFSET                      0
#define DISPLAY_Y_POS_HEADER                   2
#define DISPLAY_X_POS_MENU_OFFSET              4
#define DISPLAY_X_POS_MENU_TEXT_OFFSET       (DISPLAY_X_POS_MENU_OFFSET + 4)
#define DISPLAY_Y_POS_MENU_START             (16 + MENU_ENTRY_HEIGHT)
#define DISPLAY_Y_POS_MENU_ENTRY_HIGHLIGHT   (32 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_BAR                     10
#define DISPLAY_Y_POS_CONTACT                (16 + 8)
#define DISPLAY_Y_POS_CONTACT_TX             (34 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_CONTACT_TX_FRAME       (34 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_CHANNEL_FIRST_LINE     (32 + DISPLAY_V_OFFSET)
#define DISPLAY_Y_POS_CHANNEL_SECOND_LINE    (48 + 48)
#define DISPLAY_Y_POS_SQUELCH_BAR             16
#define DISPLAY_Y_POS_CSS_INFO               (16 + 8)
#define DISPLAY_Y_POS_SQL_INFO               (25 + 8)
#define DISPLAY_Y_POS_TX_TIMER                (8 + 16)
#define DISPLAY_Y_POS_RX_FREQ                (40 + 40)
#define DISPLAY_Y_POS_TX_FREQ                (48 + 48)
#define DISPLAY_Y_POS_ZONE                   (50 + DISPLAY_V_EXTRA_PIXELS)
#define DISPLAY_Y_POS_RSSI_VALUE             (DISPLAY_V_OFFSET - 12)
#define DISPLAY_Y_POS_RSSI_BAR               (DISPLAY_V_OFFSET + 10)
#define TITLE_BOX_HEIGHT                      21
#elif defined(PLATFORM_MD9600)
#define DISPLAY_H_EXTRA_PIXELS                 0
#define DISPLAY_H_OFFSET                       0
#define DISPLAY_V_EXTRA_PIXELS                 0
#define DISPLAY_V_OFFSET                       0
#define MENU_ENTRY_HEIGHT                     16
#define SQUELCH_BAR_H                          9
#define V_OFFSET                               0
#define OVERRIDE_FRAME_HEIGHT                 16
#define VFO_LETTER_Y_OFFSET                    8
#define LH_ENTRY_V_OFFSET                      0
#define DISPLAY_Y_POS_HEADER                   2
#define DISPLAY_X_POS_MENU_OFFSET              0
#define DISPLAY_X_POS_MENU_TEXT_OFFSET       (DISPLAY_X_POS_MENU_OFFSET + 0)
#define DISPLAY_Y_POS_MENU_START             (16 + MENU_ENTRY_HEIGHT)
#define DISPLAY_Y_POS_MENU_ENTRY_HIGHLIGHT    32
#define DISPLAY_Y_POS_BAR                     10
#define DISPLAY_Y_POS_CONTACT                 16
#define DISPLAY_Y_POS_CONTACT_TX              34
#define DISPLAY_Y_POS_CONTACT_TX_FRAME        34
#define DISPLAY_Y_POS_CHANNEL_FIRST_LINE      32
#define DISPLAY_Y_POS_CHANNEL_SECOND_LINE     48
#define DISPLAY_Y_POS_SQUELCH_BAR             16
#define DISPLAY_Y_POS_CSS_INFO                16
#define DISPLAY_Y_POS_SQL_INFO                25
#define DISPLAY_Y_POS_TX_TIMER                8
#define DISPLAY_Y_POS_RX_FREQ                 32
#define DISPLAY_Y_POS_TX_FREQ                 48
#define DISPLAY_Y_POS_ZONE                    50
#define DISPLAY_Y_POS_RSSI_VALUE              18
#define DISPLAY_Y_POS_RSSI_BAR                40
#define TITLE_BOX_HEIGHT                      21
#else
#define DISPLAY_H_EXTRA_PIXELS                 0
#define DISPLAY_H_OFFSET                       0
#define DISPLAY_V_EXTRA_PIXELS                 0
#define DISPLAY_V_OFFSET                       0
#define MENU_ENTRY_HEIGHT                     16
#define SQUELCH_BAR_H                          9
#define V_OFFSET                               0
#define OVERRIDE_FRAME_HEIGHT                 16
#define VFO_LETTER_Y_OFFSET                    8
#define LH_ENTRY_V_OFFSET                      0
#define DISPLAY_Y_POS_HEADER                   2
#define DISPLAY_X_POS_MENU_OFFSET              0
#define DISPLAY_X_POS_MENU_TEXT_OFFSET       (DISPLAY_X_POS_MENU_OFFSET + 0)
#define DISPLAY_Y_POS_MENU_START             (16 + MENU_ENTRY_HEIGHT)
#define DISPLAY_Y_POS_MENU_ENTRY_HIGHLIGHT    32
#define DISPLAY_Y_POS_BAR                     10
#define DISPLAY_Y_POS_CONTACT                 16
#define DISPLAY_Y_POS_CONTACT_TX              34
#define DISPLAY_Y_POS_CONTACT_TX_FRAME        34
#define DISPLAY_Y_POS_CHANNEL_FIRST_LINE      32
#define DISPLAY_Y_POS_CHANNEL_SECOND_LINE     48
#define DISPLAY_Y_POS_SQUELCH_BAR             16
#define DISPLAY_Y_POS_CSS_INFO                16
#define DISPLAY_Y_POS_SQL_INFO                25
#define DISPLAY_Y_POS_TX_TIMER                8
#define DISPLAY_Y_POS_RX_FREQ                 32
#define DISPLAY_Y_POS_TX_FREQ                 48
#define DISPLAY_Y_POS_ZONE                    50
#define DISPLAY_Y_POS_RSSI_VALUE              18
#define DISPLAY_Y_POS_RSSI_BAR                40
#define TITLE_BOX_HEIGHT                      21
#endif

#define FREQUENCY_X_POS  /* '>Ta'*/ ((3 * 8) + 4)
#define MAX_POWER_SETTING_NUM                  9
#define NUM_PC_OR_TG_DIGITS                    8
#define MIN_TG_OR_PC_VALUE                     1
#define MAX_TG_OR_PC_VALUE              16777215

// not partitioned address scheme
#define ALL_CALL_VALUE                  16777215 // 0xFFFFFF
// partitioned address scheme
#define MIN_ALL_CALL_VALUE              16777200 // 0xFFFFF0 // min..max: 16 values
#define MAX_ALL_CALL_VALUE              16777215 // 0xFFFFFF

#define RSSI_UPDATE_COUNTER_RELOAD           200

#define FREQ_ENTER_DIGITS_MAX                 12

#define MIN_ENTRIES_BEFORE_USING_SLICES       40 // Minimal number of available IDs before using slices stuff
#define ID_SLICES                             14 // Number of slices in whole DMRIDs DB

#define TIMESLOT_DURATION                     30

#define SCAN_SHORT_PAUSE_TIME                500 //time to wait after carrier detected to allow time for full signal detection. (CTCSS or DMR fast)

#define SCAN_DMR_DUPLEX_SLOW_MIN_DWELL_TIME       (TIMESLOT_DURATION * 6)  //minimum time between steps when scanning DMR Duplex in slow mode.
#define SCAN_DMR_SIMPLEX_SLOW_MIN_DWELL_TIME      (TIMESLOT_DURATION * 10) //minimum time between steps when scanning DMR Simplex in slow mode. (needs extra time to capture TDMA Pulsing)
#define SCAN_DMR_DUPLEX_FAST_MIN_DWELL_TIME       TIMESLOT_DURATION        //minimum time between steps when scanning DMR Duplex in fast mode.
//
// Extra ms time added to dwell time, in ChannelMode screen only, introduced
// when the SPI_Flash multiple read/read bug (MK22) got spotted and fixed.
//
#if defined(CPU_MK22FN512VLL12)
#define SCAN_DMR_DUPLEX_FAST_EXTRA_DWELL_TIME     8
#else
#define SCAN_DMR_DUPLEX_FAST_EXTRA_DWELL_TIME     0
#endif
//
#define SCAN_DMR_SIMPLEX_FAST_MIN_DWELL_TIME      (TIMESLOT_DURATION * 2)  //minimum time between steps when scanning DMR Simplex in fast mode. (needs extra time to capture TDMA Pulsing)

#define SCAN_FREQ_CHANGE_SETTLING_INTERVAL     1 //Time after frequency is changed before RSSI sampling starts
#define SCAN_SKIP_CHANNEL_INTERVAL             1 //This is actually just an implicit flag value to indicate the channel should be skipped

#define CH_DETAILS_VFO_CHANNEL                -1

#if defined(PLATFORM_MD380) || defined(PLATFORM_MDUV380) || defined(PLATFORM_RT84_DM1701) || defined(PLATFORM_MD2017)
#define VFO_SWEEP_NUM_SAMPLES                160
#else
#define VFO_SWEEP_NUM_SAMPLES                128
#endif
#define VFO_SWEEP_PIXELS_PER_STEP              4
#define VFO_SWEEP_GAIN_STEP                    5
#define VFO_SWEEP_GAIN_MIN                     0
#define VFO_SWEEP_GAIN_MAX                   120
#define VFO_SWEEP_GAIN_DEFAULT                86
#define VFO_SWEEP_RSSI_NOISE_FLOOR_MIN         4
#define VFO_SWEEP_RSSI_NOISE_FLOOR_MAX        24
#define VFO_SWEEP_RSSI_NOISE_FLOOR_DEFAULT    14

#define SCREEN_LINE_BUFFER_SIZE               17 // 16 characters (for a 8 pixels font width) + NULL
#define LOCATION_TEXT_BUFFER_SIZE             32

#define OUT_OF_BAND_FALLBACK_FREQUENCY       43000000


#define SMETER_S0                             -129
#define SMETER_S1                             -125
#define SMETER_S2                             -121
#define SMETER_S3                             -117
#define SMETER_S4                             -113
#define SMETER_S5                             -109
#define SMETER_S6                             -105
#define SMETER_S7                             -101
#define SMETER_S8                             -97
#define SMETER_S9                             -93
#define SMETER_S9_10                          -83
#define SMETER_S9_20                          -73
#define SMETER_S9_30                          -63
#define SMETER_S9_40                          -53
#define SMETER_S9_50                          -43
#define SMETER_S9_60                          -33

#define DECLARE_SMETER_ARRAY(n, destWidth) \
		const int n ## Width = destWidth; \
		const int n ## NumUnits = (SMETER_S9_60 - SMETER_S0); \
		const int n ## Divider = (n ## NumUnits / ((float)n ## Width / (float)n ## NumUnits)); \
		const int n[13] = { \
				(0),                                                           \
				(((SMETER_S1    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S2    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S3    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S4    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S5    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S6    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S7    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S8    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S9    - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S9_20 - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(((SMETER_S9_40 - SMETER_S0) * n ## NumUnits) / n ## Divider), \
				(n ## Width)                                                   \
		};

extern const int DBM_LEVELS[16];

#define MAX_DMR_ID_CONTACT_TEXT_LENGTH 51

#define RX_BEEP_UNSET                      0x00
#define RX_BEEP_CARRIER_HAS_STARTED        0x01
#define RX_BEEP_CARRIER_HAS_STARTED_EXEC   0x02
#define RX_BEEP_TALKER_IDENTIFIED          0x04
#define RX_BEEP_TALKER_HAS_STARTED         0x08
#define RX_BEEP_TALKER_HAS_STARTED_EXEC    0x10
#define RX_BEEP_TALKER_HAS_ENDED           0x20
#define RX_BEEP_TALKER_HAS_ENDED_EXEC      0x40
#define RX_BEEP_CARRIER_HAS_ENDED          0x80

typedef enum
{
	TXSTOP_TIMEOUT,
	TXSTOP_RX_ONLY,
	TXSTOP_OUT_OF_BAND
} txTerminationReason_t;

typedef enum
{
	PRIVATE_CALL_NOT_IN_CALL = 0,
	PRIVATE_CALL_ACCEPT,
	PRIVATE_CALL,
	PRIVATE_CALL_DECLINED
} privateCallState_t;


typedef enum
{
	QSO_DISPLAY_IDLE,
	QSO_DISPLAY_DEFAULT_SCREEN,
	QSO_DISPLAY_CALLER_DATA,
	QSO_DISPLAY_CALLER_DATA_UPDATE
} qsoDisplayState_t;


typedef enum
{
	SCAN_STATE_SCANNING = 0,
	SCAN_STATE_SHORT_PAUSED,
	SCAN_STATE_PAUSED
} ScanState_t;

typedef enum
{
	SCAN_TYPE_NORMAL_STEP = 0,
	SCAN_TYPE_DUAL_WATCH
} ScanType_t;

typedef struct
{
	uint32_t			id;
	char 				text[MAX_DMR_ID_CONTACT_TEXT_LENGTH];
} dmrIdDataStruct_t;


typedef struct LinkItem
{
    struct LinkItem 	*prev;
    uint32_t 			id;
    uint32_t 			talkGroupOrPcId;
    char        		contact[MAX_DMR_ID_CONTACT_TEXT_LENGTH];
    char        		talkgroup[17];
    char 				talkerAlias[32];// 4 blocks of data. 6 bytes + 7 bytes + 7 bytes + 7 bytes . plus 1 for termination some more for safety.
    double				locationLat;
    double				locationLon;
    uint32_t			time;// current system time when this station was heard
    uint8_t				receivedTS;
    uint8_t				dmrMode;
    uint16_t			rxAGCGain;
    struct LinkItem 	*next;
} LinkItem_t;

// MessageBox
#define MESSAGEBOX_MESSAGE_LEN_MAX             ((16 * 3) + 2 /* \n */ + 1 /* \0 */) // Note: 14 char line length for MESSAGEBOX_DECORATION_FRAME

typedef enum
{
	MESSAGEBOX_TYPE_UNDEF,
	MESSAGEBOX_TYPE_INFO,
	MESSAGEBOX_TYPE_PIN_CODE
} messageBoxType_t;

typedef enum
{
	MESSAGEBOX_DECORATION_NONE,
	MESSAGEBOX_DECORATION_FRAME
} messageBoxDecoration_t;

typedef enum
{
	MESSAGEBOX_BUTTONS_NONE,
	MESSAGEBOX_BUTTONS_OK,
#if defined(PLATFORM_MD9600)
	MESSAGEBOX_BUTTONS_ENT,
#endif
	MESSAGEBOX_BUTTONS_YESNO,
	MESSAGEBOX_BUTTONS_DISMISS
} messageBoxButtons_t;

typedef bool (*messageBoxValidator_t)(void); // MessageBox callback function prototype.

typedef enum
{
	ALARM_TYPE_NONE,
	ALARM_TYPE_CLOCK,
	ALARM_TYPE_SATELLITE,
	ALARM_TYPE_CANCELLED
} alarmType_t;

typedef enum
{
	SATELLITE_PHASE_NONE,
	SATELLITE_PHASE_BEFORE_PASS,
	SATELLITE_PHASE_DURING_PASS,
	SATELLITE_PHASE_AFTER_PASS
} satellitePhase_t;

typedef struct
{
	uint32_t            userDMRId;
	uint32_t            receivedPcId;
	uint32_t            tgBeforePcMode;
	qsoDisplayState_t 	displayQSOState;
	qsoDisplayState_t 	displayQSOStatePrev;
	bool 				isDisplayingQSOData;
	bool				displayChannelSettings;
	bool				reverseRepeaterChannel;
	bool				reverseRepeaterVFO;
	int					currentSelectedChannelNumber;
	int					currentSelectedContactIndex;
	int					lastHeardCount;
	int					receivedPcTS;
	bool				dmrDisabled;
	uint32_t			manualOverrideDMRId;// This is a global so it will default to 0
	time_t_custom		dateTimeSecs;// Epoch (00:00:00 UTC, January 1, 1970)
#if defined(PLATFORM_MD9600)
	bool				sk2latched;
#endif
	volatile uint8_t	rxBeepState;
	bool				talkaround;
	DayTime_t           daytime;
	DayTime_t           daytimeOverridden;

	struct
	{
		ticksTimer_t		timer;
		int 				dwellTime;
		int 				direction;
		int					availableChannelsCount;
		int 				nuisanceDeleteIndex;
		int 				nuisanceDelete[MAX_ZONE_SCAN_NUISANCE_CHANNELS];
		ScanState_t 		state;
		bool 				active;
		bool 				toneActive; // tone scan active flag (CTCSS/DCS)
		bool				refreshOnEveryStep;
		bool				lastIteration;
		ScanType_t			scanType;
		int					stepTimeMilliseconds;
		int					scanSweepCurrentFreq;
		int					sweepSampleIndex;
		int					sweepStepSizeIndex;
		int					sweepSampleIndexIncrement;
#if defined(PLATFORM_MD9600)
		uint16_t			clickDiscriminator; // Due to click sound, we need to postpone the Audio Amp status checking.
#endif
	} Scan;

	struct
	{
		uint8_t 			tmpDmrDestinationFilterLevel;
		uint8_t 			tmpDmrCcTsFilterLevel;
		uint8_t 			tmpAnalogFilterLevel;
		bool				tmpTxRxLockMode;
		CodeplugCSSTypes_t	tmpToneScanCSS;
		uint8_t				tmpVFONumber;
		bool				tmpTalkaround;
		bool				tmpSortOrderIsDistance;
	} QuickMenu;

	struct
	{
		int 				index;
		char 				digits[FREQ_ENTER_DIGITS_MAX];
	} FreqEnter;

	struct
	{
		uint32_t			lastID;
		privateCallState_t	state;
	} PrivateCall;

	struct
	{
		bool				inhibitInitial;
	} VoicePrompts;

	struct
	{
		messageBoxType_t         type;
		messageBoxDecoration_t   decoration;
		messageBoxButtons_t      buttons;
		uint8_t                  pinLength;
		char                     message[MESSAGEBOX_MESSAGE_LEN_MAX]; // 3 lines max for MESSAGEBOX_TYPE_INFO type
		int                      keyPressed;
		messageBoxValidator_t    validatorCallback;

	} MessageBox;

	struct
	{
		ticksTimer_t                                nextPeriodTimer;
		bool                                        isKeying;
		uint8_t                                     buffer[17U]; // 16 tones + final time-length
		uint8_t                                     poLen;
		uint8_t                                     poPtr;
		struct_codeplugSignalling_DTMFDurations_t   durations;
		bool                                        inTone;
	} DTMFContactList;

	struct
	{
		uint32_t			alarmTime;
		alarmType_t			alarmType;
		uint32_t			currentSatellite;
	} SatelliteAndAlarmData;

} uiDataGlobal_t;


#if defined(PLATFORM_MDUV380) && !defined(PLATFORM_VARIANT_UV380_PLUS_10W)
extern const char 				*POWER_LEVELS[2][10];
#else
extern const char 				*POWER_LEVELS[];
#endif
extern const char 				*POWER_LEVEL_UNITS[];
extern const char 				*DMR_DESTINATION_FILTER_LEVELS[];
extern const char 				*DMR_CCTS_FILTER_LEVELS[];
extern const char 				*ANALOG_FILTER_LEVELS[];

extern uiDataGlobal_t 			uiDataGlobal;

extern settingsStruct_t 		originalNonVolatileSettings; // used to store previous settings in options edition related menus.

#if !defined(PLATFORM_GD77S)
extern aprsBeaconingSettings_t 	aprsSettingsCopy; // used to store previous APRS settings in options edition related menu.
#endif

extern struct_codeplugZone_t 	currentZone;
extern struct_codeplugRxGroup_t currentRxGroupData;
extern int						lastLoadedRxGroup;
extern struct_codeplugContact_t currentContactData;

extern LinkItem_t 				*LinkHead;

extern bool 					PTTToggledDown;
extern uint32_t					xmitErrorTimer;

#if ! defined(PLATFORM_GD77S)
#define DAYTIME_CURRENT ((uiDataGlobal.daytimeOverridden != UNDEFINED) ? uiDataGlobal.daytimeOverridden : uiDataGlobal.daytime)

extern ticksTimer_t autolockTimer;
#endif

extern const uint32_t DMRID_HEADER_LENGTH;
extern const uint32_t DMRID_MEMORY_LOCATION_1;
extern const uint32_t DMRID_MEMORY_LOCATION_2;
extern uint32_t dmrIDDatabaseMemoryLocation2;

#endif
