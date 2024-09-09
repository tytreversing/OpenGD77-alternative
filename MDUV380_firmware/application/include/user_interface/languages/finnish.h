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
 * Translators: OH1E
 *
 *
 * Rev: 14
 */
#ifndef USER_INTERFACE_LANGUAGES_FINNISH_H_
#define USER_INTERFACE_LANGUAGES_FINNISH_H_
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
const stringsTable_t finnishLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 		= "Suomi",
.menu			= "Menu",
.credits		= "Kiitokset",
.zone			= "Zone",
.rssi			= "RSSI Signaali",
.battery		= "Akun Tila",
.contacts		= "Kontaktit",
.last_heard		= "Viimeksi kuultu",
.firmware_info		= "Laiteohjelmisto",
.options		= "Yleis  Asetukset",
.display_options	= "Näytön Asetukset",
.sound_options		= "Ääni   Asetukset", 	// MaxLen: 16
.channel_details	= "Kanava Asetukset",
.language		= "Kieli",
.new_contact		= "Uusi kontakti",
.dmr_contacts		= "DMR kontaktit", 	// MaxLen: 16
.contact_details	= "Kontakti Asetus",
.hotspot_mode		= "Hotspotti tila",
.built			= "Koontikäännös",
.zones			= "Zonet",
.keypad			= "Näppäin", 		// MaxLen: 12 (with .ptt)
.ptt			= "PTT", 		// MaxLen: 12 (with .keypad)
.locked			= "Lukossa", 		// MaxLen: 15
.press_sk2_plus_star	= "Paina SK2 ja *", // MaxLen: 16
.to_unlock		= "avataksesi tämän",// MaxLen: 16
.unlocked		= "Näplukko avattu", 	// MaxLen: 15
.power_off		= "Virta pois...",
.error			= "VIRHE", 		// MaxLen: 8
.rx_only		= "Vain Rx",
.out_of_band		= "Bändial ulkopu", 	// MaxLen: 14
.timeout		= "AIKAKATK", 		// MaxLen: 8
.tg_entry		= "Aseta TG", 		// MaxLen: 15
.pc_entry		= "Aseta PC", 		// MaxLen: 15
.user_dmr_id		= "Käyttäjän DMR ID",
.contact 		= "Kontakti", 		// MaxLen: 15
.accept_call		= "Vastaa puheluun?",
.private_call		= "Priv. puhelu",
.squelch		= "K.Salpa", 		// MaxLen: 8
.quick_menu 		= "Pika Menu",
.filter			= "Suodata", 		// MaxLen: 7 (with ':' + settings: "None", "CC", "CC,TS", "CC,TS,TG")
.all_channels		= "Kaikki Kanavat",
.gotoChannel		= "Muistipaikk", 	// MaxLen: 11 (" 1024")
.scan			= "Skannaus",
.channelToVfo		= "Kanava --> VFO",
.vfoToChannel		= "VFO --> Kanava",
.vfoToNewChannel	= "VFO --> Uusi kan", 	// MaxLen: 16
.group			= "Ryhmä", 		// MaxLen: 16 (with .type)
.private		= "Privaatti", 		// MaxLen: 16 (with .type)
.all			= "Kaikki", 		// MaxLen: 16 (with .type)
.type			= "Tyyppi", 		// MaxLen: 16 (with .type)
.timeSlot		= "Aikaväli", 		// MaxLen: 16 (plus ':' and  .none, '1' or '2')
.none			= "Tyhjä", 		// MaxLen: 16 (with .timeSlot, "Rx CTCSS:" and ""Tx CTCSS:")
.contact_saved		= "Kontakti tallen.",
.duplicate		= "kaksoiskappale",
.tg			= "TG", 		// MaxLen: 8
.pc			= "PC", 		// MaxLen: 8
.ts			= "TS", 		// MaxLen: 8
.mode			= "Mode", 		// MaxLen: 12
.colour_code		= "Väri Koodi", 	// MaxLen: 16 (with ':' * .n_a)
.n_a			= "Pois", 		// MaxLen: 16 (with ':' * .colour_code)
.bandwidth		= "Kaistanl", 		// MaxLen: 16 (with ':' + .n_a, "25kHz" or "12.5kHz")
.stepFreq		= "Steppi", 		// MaxLen: 7 (with ':' + xx.xxkHz fitted)
.tot			= "TOT", 		// MaxLen: 16 (with ':' + .off or 15..3825)
.off			= "Ei", 		// MaxLen: 16 (with ':' + .timeout_beep, .band_limits)
.zone_skip		= "Skippaa zone", 	// MaxLen: 16 (with ':' + .yes or .no)
.all_skip		= "Skippaa kaik", 	// MaxLen: 16 (with ':' + .yes or .no)
.yes			= "Joo", 		// MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.no			= "Ei", 		// MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.tg_list		= "tg lista", 		// MaxLen: 16 (with ':' and codeplug group name)
.on			= "On", 		// MaxLen: 16 (with ':' + .band_limits)
.timeout_beep		= "Aikakatk beep", 	// MaxLen: 16 (with ':' + .n_a or 5..20 + 's')
.list_full		= "Lista täynnä",
.dmr_cc_scan		= "CC Skan.", 		// MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits		= "Bändi Rajoitu", 	// MaxLen: 16 (with ':' + .on or .off)
.beep_volume		= "NäpÄäniVoim", 	// MaxLen: 16 (with ':' + -24..6 + 'dB')
.dmr_mic_gain		= "DMR MicGain", 	// MaxLen: 16 (with ':' + -33..12 + 'dB')
.fm_mic_gain		= "FM MicGain", 	// MaxLen: 16 (with ':' + 0..31)
.key_long		= "Näp pitkä",	 	// MaxLen: 11 (with ':' + x.xs fitted)
.key_repeat		= "Näp toisto", 	// MaxLen: 11 (with ':' + x.xs fitted)
.dmr_filter_timeout	= "Suodin aika", 	// MaxLen: 16 (with ':' + 1..90 + 's')
.brightness		= "Kirkkaus", 		// MaxLen: 16 (with ':' + 0..100 + '%')
.brightness_off		= "Min kirkkaus", 	// MaxLen: 16 (with ':' + 0..100 + '%')
.contrast		= "Kontrasti", 		// MaxLen: 16 (with ':' + 12..30)
.screen_invert		= "Käänteinen",
.screen_normal		= "Normaali",
.backlight_timeout	= "TaustValoAika", 	// MaxLen: 16 (with ':' + .no to 30s)
.scan_delay		= "Skann. viive", 	// MaxLen: 16 (with ':' + 1..30 + 's')
.yes___in_uppercase	= "KYLLÄ", 		// MaxLen: 8 (choice above green/red buttons)
.no___in_uppercase	= "EI", 		// MaxLen: 8 (choice above green/red buttons)
.DISMISS		= "POISTU", 		// MaxLen: 8 (choice above green/red buttons)
.scan_mode		= "Skannaus", 		// MaxLen: 16 (with ':' + .hold or .pause)
.hold			= "Pysäyty", 		// MaxLen: 16 (with ':' + .scan_mode)
.pause			= "Pauseta", 		// MaxLen: 16 (with ':' + .scan_mode)
.list_empty		= "Tyhjä lista", 	// MaxLen: 16
.delete_contact_qm	= "Poista kontakti?", 	// MaxLen: 16
.contact_deleted	= "Kontakti Poistet", 	// MaxLen: 16
.contact_used		= "Kontakti käytöss", 	// MaxLen: 16
.in_tg_list		= "tg-luettelossa", 	// MaxLen: 16
.select_tx		= "Valitse TX", 	// MaxLen: 16
.edit_contact		= "Muokkaa Kontakti", 	// MaxLen: 16
.delete_contact		= "Poista Kontakti", 	// MaxLen: 16
.group_call		= "Ryhmä Puhelu", 	// MaxLen: 16
.all_call		= "Puhelu kaikille", 	// MaxLen: 16
.tone_scan		= "Aliääni scan",	// MaxLen: 16
.low_battery		= "Akku Vähissä !",	// MaxLen: 16
.Auto			= "Automaatti",		// MaxLen 16 (with .mode + ':')
.manual			= "Manuaali",		// MaxLen 16 (with .mode + ':')
.ptt_toggle		= "PTT Lukko",		// MaxLen 16 (with ':' + .on or .off)
.private_call_handling	= "Käsittele PC",	// MaxLen 16 (with ':' + .on ot .off)
.stop			= "Stoppaa", 		// Maxlen 16 (with ':' + .scan_mode)
.one_line		= "1 rivi", 		// MaxLen 16 (with ':' + .contact)
.two_lines		= "2 riviä", 		// MaxLen 16 (with ':' + .contact)
.new_channel		= "Uusi kanava", 	// MaxLen: 16, leave room for a space and four channel digits after
.priority_order		= "Järjest", 		// MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep		= "DMR piippi", 	// MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start			= "Alku", 		// MaxLen 16 (with ':' + .dmr_beep)
.both			= "Molemm",		// MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold          = "VOX Herkk.",		// MaxLen 16 (with ':' + .off or 1..30)
.vox_tail               = "VOX Viive",		// MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt		= "Merkki",		// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                 = "Vaimennus", 		// Maxlen 16 (with : + audio_prompt)
.rx_beep		= "RX beep", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep			= "Piippi", 		// Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1	= "Puhe", 		// Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1	= "TA Tx TS1", 		// Maxlen 16 (with : + .on or .off)
.squelch_VHF		= "VHF K.Salpa",	// Maxlen 16 (with : + XX%)
.squelch_220		= "220 K.Salpa",	// Maxlen 16 (with : + XX%)
.squelch_UHF		= "VHF K.Salpa", 	// Maxlen 16 (with : + XX%)
.display_screen_invert = "Väri" , 		// Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 		= "OpenGD77",		// Do not translate
.talkaround 		= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS			= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No Keys", // Maxlen 16 (with : + audio_prompt)
.gitCommit		= "Git commit",
.voice_prompt_level_2	= "Puhe L2", 		// Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3	= "Puhe L3", 		// Maxlen 16 (with : + audio_prompt)
.dmr_filter		= "DMR Suodin",		// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter		= "Suodata TS", 	// MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list	= "FM DTMF Kontakti", 	// Maxlen: 16
.channel_power		= "Ch Teho", 		// Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master		= "Master",		// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey		= "Aseta Pikanäpäin", 	// MaxLen: 16
.dual_watch		= "Dual Watch", 	// MaxLen: 16
.info			= "Info", 		// MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr			= "Teho",
.user_power		= "Käyttäjä Teho",
.temperature		= "Lämpötila", 		// MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius		= "°C",
.seconds		= "sekunttia",
.radio_info		= "Radio infot",
.temperature_calibration = "Lämp Kal.",
.pin_code		= "Pin Koodi",
.please_confirm		= "Varmista", 		// MaxLen: 15
.vfo_freq_bind_mode	= "Freq. Bind",
.overwrite_qm		= "Ylikirjoita ?", 	//Maxlen: 14 chars
.eco_level		= "Eco taso",
.buttons		= "Napit",
.leds			= "LEDit",
.scan_dwell_time	= "Skan. Dwell",
.battery_calibration	= "Akku. Kal.",
.low			= "Matala",
.high			= "Korkea",
.dmr_id			= "DMR ID",
.scan_on_boot		= "Skannaa Boot",
.dtmf_entry		= "Aseta DTMF",
.name			= "Nimi",
.carrier				= "Carrier",
.zone_empty 				= "Zone empty", // Maxlen: 12 chars.
.time			= "Aika",
.uptime			= "UPtime",
.hours			= "Tunti",
.minutes		= "Minuutti",
.satellite		= "Satelliitti",
.alarm_time		= "Hälytys aika",
.location		= "Location",
.date			= "Päivämäärä",
.timeZone		= "Timezone",           // F1RMB: Translation is way too long, get back to English
.suspend		= "Suspend",
.pass			= "Pass", 		// For satellite screen
.elevation		= "El",
.azimuth		= "Az",
.inHHMMSS		= "in",
.predicting		= "Predicting",
.maximum		= "Max",
.satellite_short	= "Sat",
.local			= "Lokaali",
.UTC			= "UTC",
.symbols		= "NSEW", 		// symbols: N,S,E,W
.not_set		= "NOT SET",
.general_options	= "Yleiset asetukse",
.radio_options		= "Radio asetukset",
.auto_night				= "Auto night", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "DMR Rx AGC",
.speaker_click_suppress			= "Click Suppr.",
.gps					= "GPS",
.end_only				= "End only",
.dmr_crc				= "DMR crc",
.eco					= "Eco",
.safe_power_on				= "Safe Pwr-On", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "Auto Pwr-Off", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "APO with RF", // MaxLen: 16 (with ':' + .yes or .no or .n_a)
.brightness_night				= "Nite bright", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF			= "Freq VHF",
.gps_acquiring			= "Acquiring",
.altitude				= "Alt",
.calibration            = "Calibration",
.freq_set_UHF                = "Freq UHF",
.cal_frequency          = "Freq",
.cal_pwr                = "Power level",
.pwr_set                = "Power Adjust",
.factory_reset          = "Factory Reset",
.rx_tune				= "Rx Tuning",
.transmitTalkerAliasTS2	= "TA Tx TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Text",
.daytime_theme_day			= "Day theme", // MaxLen: 16
.daytime_theme_night			= "Night theme", // MaxLen: 16
.theme_chooser				= "Theme chooser", // Maxlen: 16
.theme_options				= "Theme options",
.theme_fg_default			= "Text Default", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Background", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Decoration", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Text input", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "Foregr. boot", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Backgr. boot", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Text notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Warning notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Error notif", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Backgr. notif", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Menu name", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Menu name bkg", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Menu item", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Menu highlight", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "Option value", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Header text", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Header text bkg", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "RSSI bar", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "RSSI bar S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Channel name", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Contact", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Contact info", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Zone name", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "RX freq", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "TX freq", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "CSS/SQL values", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "TX counter", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polar", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Sat. spot", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "GPS number", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "GPS spot", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "BeiDou spot", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Red", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Green", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Blue", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Volume", // MaxLen: 8
.distance_sort				= "Dist sort", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Show dist", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS options", // MaxLen 16
.aprs_smart				= "Smart", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "Channel", // MaxLen 16 (with ':' + .location)
.aprs_decay				= "Decay", // MaxLen 16 (with ':' + .on or .off)
.aprs_compress				= "Compress", // MaxLen 16 (with ':' + .on or .off)
.aprs_interval				= "Interval", // MaxLen 16 (with ':' + 0.2..60 + 'min')
.aprs_message_interval			= "Msg Interval", // MaxLen 16 (with ':' + 3..30)
.aprs_slow_rate				= "Slow Rate", // MaxLen 16 (with ':' + 1..100 + 'min')
.aprs_fast_rate				= "Fast Rate", // MaxLen 16 (with ':' + 10..180 + 's')
.aprs_low_speed				= "Low Speed", // MaxLen 16 (with ':' + 2..30 + 'km/h')
.aprs_high_speed			= "Hi Speed", // MaxLen 16 (with ':' + 2..90 + 'km/h')
.aprs_turn_angle			= "T. Angle", // MaxLen 16 (with ':' + 5..90 + '°')
.aprs_turn_slope			= "T. Slope", // MaxLen 16 (with ':' + 1..255 + '°/v')
.aprs_turn_time				= "T. Time", // MaxLen 16 (with ':' + 5..180 + 's')
.auto_lock				= "Auto lock", // MaxLen 16 (with ':' + .off or 0.5..15 (.5 step) + 'min')
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
#endif /* USER_INTERFACE_LANGUAGES_FINNISH_H_ */
