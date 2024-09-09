/* -*- coding: windows-1252-unix; -*- */
/*
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
/*
 * Translators: OZ1MAX
 *
 *
 * Rev: 2
 */
#ifndef USER_INTERFACE_LANGUAGES_DANISH_H_
#define USER_INTERFACE_LANGUAGES_DANISH_H_
/********************************************************************
 *
 * VERY IMPORTANT.
 * This file should not be saved with UTF-8 encoding
 * Use Notepad++ on Windows with ANSI encoding
 * or emacs on Linux with windows-1252-unix encoding
 *
 ********************************************************************/
#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
__attribute__((section(".upper_text")))
#endif
const stringsTable_t danishLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 			= "Dansk",
.menu					= "Menu",
.credits				= "Credits",
.zone					= "Zone",
.rssi					= "RSSI",
.battery				= "Batteri",
.contacts				= "Kontakter",
.last_heard				= "Sidst Hørt",
.firmware_info			= "Firmware info",
.options				= "Valg",
.display_options		= "Display Valg",
.sound_options				= "Lyd valg", // MaxLen: 16
.channel_details		= "Kanal detaljer",
.language				= "Sprog",
.new_contact			= "Ny Kontakt",
.dmr_contacts				= "DMR kontakter", // MaxLen: 16
.contact_details		= "Kontakt Detaljer",
.hotspot_mode			= "Hotspot mode",
.built					= "Version",
.zones					= "Zoner",
.keypad					= "Keypad",
.ptt					= "PTT",
.locked					= "Låst",
.press_sk2_plus_star	= "Tast SK2 + *",
.to_unlock				= "Lås op",
.unlocked				= "Oplåst",
.power_off				= "Lukker Ned...",
.error					= "FEJL",
.rx_only				= "Kun Rx",
.out_of_band			= "Ude af FRQ",
.timeout				= "TIMEOUT",
.tg_entry				= "Indtast TG",
.pc_entry				= "Indtast PC",
.user_dmr_id			= "Bruger DMR ID",
.contact 				= "Kontakt",
.accept_call			= "Modtag kald?",
.private_call			= "Privat kald",
.squelch				= "Squelch",
.quick_menu 			= "Quick Menu",
.filter					= "Filter",
.all_channels			= "Alle kanaler",
.gotoChannel			= "Goto",
.scan					= "Scan",
.channelToVfo			= "Kanal --> VFO",
.vfoToChannel			= "VFO --> Kanal",
.vfoToNewChannel		= "VFO --> Ny Kanal", // MaxLen: 16
.group					= "Gruppe",
.private				= "Privat",
.all					= "Alle",
.type					= "Type",
.timeSlot				= "Timeslot",
.none					= "Ingen",
.contact_saved			= "Kontakt Gemt",
.duplicate				= "Duplet",
.tg						= "TG",
.pc						= "PC",
.ts						= "TS",
.mode					= "Mode",
.colour_code			= "Color kode",
.n_a					= "N/A",
.bandwidth				= "BW", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")åndbrede",
.stepFreq				= "Step",
.tot					= "TOT",
.off					= "Fra",
.zone_skip				= "Zone Skip",
.all_skip				= "Alle Skip",
.yes					= "Ja",
.no						= "Nej",
.tg_list				= "TG Lst",
.on						= "On",
.timeout_beep			= "Timeout bip",
.list_full				= "List full",
.dmr_cc_scan			= "CC Scan", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits			= "Åben FRQ",
.beep_volume			= "Bip vol",
.dmr_mic_gain			= "DMR mic",
.fm_mic_gain				= "FM mic", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "Lang Tast",
.key_repeat				= "Tast rpt",
.dmr_filter_timeout		= "Filter tid",
.brightness				= "Lys styrke",
.brightness_off				= "Min Lys",
.contrast				= "Kontrast",
.screen_invert			= "Sort",
.screen_normal			= "Normal",
.backlight_timeout		= "Timeout",
.scan_delay				= "Scan delay",
.yes___in_uppercase					= "JA",
.no___in_uppercase						= "NEJ",
.DISMISS				= "FORTRYD",
.scan_mode				= "Scan mode",
.hold					= "Hold",
.pause					= "Pause",
.list_empty				= "Tom Liste",
.delete_contact_qm			= "Slet kontakt?",
.contact_deleted			= "kontakt slettet",
.contact_used				= "Kontakt brugt",
.in_tg_list				= "i TG-listen",
.select_tx				= "Valg TX",
.edit_contact				= "Rediger Kontakt",
.delete_contact				= "Slet Kontakt",
.group_call				= "Gruppe Kald",
.all_call				= "Alle Kald",
.tone_scan				= "Tone scan",//// MaxLen: 16
.low_battery			= "LAVT BATTERI !!!",//// MaxLen: 16
.Auto					= "Auto", // MaxLen 16 (with .mode + ':')
.manual					= "Manual",  // MaxLen 16 (with .mode + ':')
.ptt_toggle				= "PTT toggle", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling			= "Handle PC", // MaxLen 16 (with ':' + .on ot .off)
.stop					= "Stop", // Maxlen 16 (with ':' + .scan_mode)
.one_line				= "1 linje", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 linjer", // MaxLen 16 (with ':' + .contact)
.new_channel			= "Ny kanal", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order				= "Order", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "DMR beep", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "Start", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "Begge", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold                          = "VOX Thres.", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail                               = "VOX Tail", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt				= "Prompt",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                                 = "Stille", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX bip", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "BIBp", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1					= "Voice", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1	= "TA Tx 1", // Maxlen 16 (with : + .off .text APRS .both)
.squelch_VHF			= "VHF Squelch",// Maxlen 16 (with : + XX%)
.squelch_220			= "220 Squelch",// Maxlen 16 (with : + XX%)
.squelch_UHF			= "UHF Squelch", // Maxlen 16 (with : + XX%)
.display_screen_invert = "Farve" , // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No Keys", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2	= "Tale L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3	= "Tale L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "DMR Filter",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Tale",
.dmr_ts_filter			= "TS Filter", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "FM DTMF Kontakt", // Maxlen: 16
.channel_power				= "Ch Power", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "Master",// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey				= "Set Quicktast", // MaxLen: 16
.dual_watch				= "Dual Watch", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr					= "Pwr",
.user_power				= "Bruger Power",
.temperature				= "Temperatur", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "°C",
.seconds				= "sekunder",
.radio_info				= "Radio info",
.temperature_calibration		= "Temp Cal",
.pin_code				= "Pin kode",
.please_confirm				= "KONFIRMATION", // MaxLen: 15
.vfo_freq_bind_mode			= "Freq. Bind",
.overwrite_qm				= "Overskriv ?", //Maxlen: 14 chars
.eco_level				= "Eco Level",
.buttons				= "Tastatur",
.leds					= "LEDs",
.scan_dwell_time			= "Scan dwell",
.battery_calibration			= "Batt. Cal",
.low					= "Lav",
.high					= "Høj",
.dmr_id					= "DMR ID",
.scan_on_boot				= "Scan ved start",
.dtmf_entry				= "DTMF entry",
.name					= "Navn",
.carrier				= "Carrier",
.zone_empty 				= "Zone tom", // Maxlen: 12 chars.
.time					= "Time",
.uptime					= "Oppetid",
.hours					= "Timer",
.minutes				= "Minuter",
.satellite				= "Satellite",
.alarm_time				= "Alarm tid",
.location				= "Lokation",
.date					= "Dato",
.timeZone				= "Tidszone",
.suspend				= "Udeluk",
.pass					= "Pass", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "in",
.predicting				= "Beregner",
.maximum				= "Max",
.satellite_short		= "Sat",
.local					= "Local",
.UTC					= "UTC",
.symbols				= "NSEW", // symbols: N,S,E,W
.not_set				= "IKKE SAT",
.general_options		= "General optioner",
.radio_options			= "Radio optioner",
.auto_night				= "Auto nat", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "DMR Rx AGC",
.speaker_click_suppress			= "Klick Suppr.",
.gps					= "GPS",
.end_only				= "Kun Slut",
.dmr_crc				= "DMR crc",
.eco					= "Eco",
.safe_power_on				= "Sikker Pwr-On", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "Auto Pwr-Off", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "APO med RF", // MaxLen: 16 (with ':' + .yes or .no or .n_a)
.brightness_night				= "Nat Lys", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF			= "Freq VHF",
.gps_acquiring			= "Henter",
.altitude				= "Alt",
.calibration            = "Kalibrerer",
.freq_set_UHF                = "Freq UHF",
.cal_frequency          = "Freq",
.cal_pwr                = "Power niveu",
.pwr_set                = "Power Juster",
.factory_reset          = "Factory Reset",
.rx_tune				= "Rx Tuning",
.transmitTalkerAliasTS2	= "TA Tx TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Tekst",
.daytime_theme_day			= "Dag thema", // MaxLen: 16
.daytime_theme_night			= "Nat thema", // MaxLen: 16
.theme_chooser				= "Thema vælger", // Maxlen: 16
.theme_options				= "Thema mulighed",
.theme_fg_default			= "Tekst Standard", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Baggrund", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Dekoration", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Tekst input", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "Forgr. boot", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Baggr. boot", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Tekst notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Advarsel notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Fejl notif", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Baggr. notif", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Menu navn", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Menu navn bag", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Menu valg", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Menu highlight", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "indstil værdi", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Hoved tekst", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Hoved tekst bkg", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "RSSI bar", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "RSSI bar S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Kanal navn", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "kontakt", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "kontakt info", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Zone navn", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "RX frek", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "TX frek", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "CSS/SQL værdi", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "TX tæller", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polar", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Sat. spot", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "GPS nummer", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "GPS spot", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "BeiDou spot", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Rød", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Grøn", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Blå", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Lydstyrke", // MaxLen: 8
.distance_sort				= "Dist sort", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Vis dist", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS værdi", // MaxLen 16
.aprs_smart				= "Smart", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "Kanal", // MaxLen 16 (with ':' + .location)
.aprs_decay				= "Decay", // MaxLen 16 (with ':' + .on or .off)
.aprs_compress				= "Compress", // MaxLen 16 (with ':' + .on or .off)
.aprs_interval				= "Interval", // MaxLen 16 (with ':' + 0.2..60 + 'min')
.aprs_message_interval			= "Msg Interval", // MaxLen 16 (with ':' + 3..30)
.aprs_slow_rate				= "Langsom Rate", // MaxLen 16 (with ':' + 1..100 + 'min')
.aprs_fast_rate				= "Hurtig Rate", // MaxLen 16 (with ':' + 10..180 + 's')
.aprs_low_speed				= "Lav hastighed", // MaxLen 16 (with ':' + 2..30 + 'km/h')
.aprs_high_speed			= "Hi hastighed", // MaxLen 16 (with ':' + 2..90 + 'km/h')
.aprs_turn_angle			= "T. Vinkel", // MaxLen 16 (with ':' + 5..90 + '°')
.aprs_turn_slope			= "T. Sløjfe", // MaxLen 16 (with ':' + 1..255 + '°/v')
.aprs_turn_time				= "T. Tid", // MaxLen 16 (with ':' + 5..180 + 's')
.auto_lock				= "Auto lås", // MaxLen 16 (with ':' + .off or 0.5..15 (.5 step) + 'min')
.trackball				= "Trackball", // MaxLen 16 (with ':' + .on or .off)
.dmr_force_dmo				= "Force DMO", // MaxLen 16 (with ':' + .n_a or .on or .off)
};
/********************************************************************
 *
 * VERY IMPORTANT.
 * This file should not be saved with UTF-8 encoding
 * Use Notepad++ on Windows with ANSI encoding
 * or emacs on Linux with windows-1252-unix encoding
 *
 ********************************************************************/
#endif /* USER_INTERFACE_LANGUAGES_DANISH_H_ */
