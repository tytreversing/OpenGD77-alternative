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
 * Translators: YO3IDG
 *
 *
 * Rev:
 */
#ifndef USER_INTERFACE_LANGUAGES_ROMANIAN_H_
#define USER_INTERFACE_LANGUAGES_ROMANIAN_H_
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
const stringsTable_t romanianLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 				= "Romana", // MaxLen: 16
.menu					= "Meniu", // MaxLen: 16
.credits				= "Mentiuni", // MaxLen: 16
.zone					= "Zona", // MaxLen: 16
.rssi					= "RSSI", // MaxLen: 16
.battery				= "Baterie", // MaxLen: 16
.contacts				= "Contacte", // MaxLen: 16
.last_heard				= "Ultimii auziti", // MaxLen: 16
.firmware_info				= "Despre", // MaxLen: 16
.options				= "Setari", // MaxLen: 16
.display_options			= "Afisare", // MaxLen: 16
.sound_options				= "Sunet", // MaxLen: 16
.channel_details			= "Detalii canal", // MaxLen: 16
.language				= "Limba", // MaxLen: 16
.new_contact				= "Contact nou", // MaxLen: 16
.dmr_contacts				= "Contacte DMR", // MaxLen: 16
.contact_details			= "Detalii contact", // MaxLen: 16
.hotspot_mode				= "Hotspot", // MaxLen: 16
.built					= "Versiune", // MaxLen: 16
.zones					= "Zone", // MaxLen: 16
.keypad					= "Taste", // MaxLen: 12 (with .ptt)
.ptt					= "PTT", // MaxLen: 12 (with .keypad, .mode)
.locked					= "Blocat", // MaxLen: 15
.press_sk2_plus_star			= "Apasa SK2 + *", // MaxLen: 16
.to_unlock				= "pt. deblocare", // MaxLen: 16
.unlocked				= "Deblocat", // MaxLen: 15
.power_off				= "Oprire...", // MaxLen: 16
.error					= "ERROARE", // MaxLen: 8
.rx_only				= "Doar Rx", // MaxLen: 14
.out_of_band				= "LIMITE BANDA", // MaxLen: 14
.timeout				= "TIMEOUT", // MaxLen: 8
.tg_entry				= "Scrie TG", // MaxLen: 15
.pc_entry				= "Scrie PC", // MaxLen: 15
.user_dmr_id				= "User DMR ID", // MaxLen: 15
.contact 				= "Contact", // MaxLen: 15
.accept_call				= "Raspunde lui", // MaxLen: 16
.private_call				= "Apel privat", // MaxLen: 16
.squelch				= "Squelch", // MaxLen: 8
.quick_menu 				= "Scurtaturi", // MaxLen: 16
.filter					= "Filtru", // MaxLen: 7 (with ':' + settings: .none, "CC", "CC,TS", "CC,TS,TG")
.all_channels				= "Toate zonele", // MaxLen: 16 (actually max. 12 because it is used in stby screen )
.gotoChannel				= "Sari la",  // MaxLen: 11 (" 1024")
.scan					= "Cautare", // MaxLen: 16
.channelToVfo				= "Canal --> OFV", // MaxLen: 16
.vfoToChannel				= "OFV --> Canal", // MaxLen: 16
.vfoToNewChannel			= "OFV --> Memorie", // MaxLen: 16
.group					= "Grup", // MaxLen: 16 (with .type)
.private				= "Privat", // MaxLen: 16 (with .type)
.all					= "Toti", // MaxLen: 16 (with .type)
.type					= "Tip", // MaxLen: 16 (with .type)
.timeSlot				= "Timeslot", // MaxLen: 16 (plus ':' and  .none, '1' or '2')
.none					= "Nimic", // MaxLen: 16 (with .timeSlot, "Rx CTCSS:" and ""Tx CTCSS:", .filter/.mode/.dmr_beep)
.contact_saved				= "Contact salvat", // MaxLen: 16
.duplicate				= "Duplicat", // MaxLen: 16
.tg					= "TG",  // MaxLen: 8
.pc					= "PC", // MaxLen: 8
.ts					= "TS", // MaxLen: 8
.mode					= "Mod",  // MaxLen: 12
.colour_code				= "Cod Culoare", // MaxLen: 16 (with ':' * .n_a)
.n_a					= "N/A",// MaxLen: 16 (with ':' * .colour_code)
.bandwidth				= "BW", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "Pas", // MaxLen: 7 (with ':' + xx.xxkHz fitted)
.tot					= "TOT", // MaxLen: 16 (with ':' + .off or 15..3825)
.off					= "Oprit", // MaxLen: 16 (with ':' + .timeout_beep, .band_limits, aprs_decay)
.zone_skip				= "Sari Zona", // MaxLen: 16 (with ':' + .yes or .no)
.all_skip				= "Sari tot", // MaxLen: 16 (with ':' + .yes or .no)
.yes					= "Da", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.no					= "Nu", // MaxLen: 16 (with ':' + .zone_skip, .all_skip, .aprs_decay)
.tg_list				= "TG Lst", // MaxLen: 16 (with ':' and codeplug group name)
.on					= "Pornit", // MaxLen: 16 (with ':' + .band_limits)
.timeout_beep				= "Timeout beep", // MaxLen: 16 (with ':' + .n_a or 5..20 + 's')
.list_full				= "Toata lista",
.dmr_cc_scan				= "Cauta CC", // MaxLen: 12 (with ':' + settings: .on or .off).
.band_limits				= "Limite f.", // MaxLen: 16 (with ':' + .on or .off)
.beep_volume				= "Beep vol", // MaxLen: 16 (with ':' + -24..6 + 'dB')
.dmr_mic_gain				= "DMR mic", // MaxLen: 16 (with ':' + -33..12 + 'dB')
.fm_mic_gain				= "FM mic", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "Apas lung", // MaxLen: 11 (with ':' + x.xs fitted)
.key_repeat				= "Apas repet", // MaxLen: 11 (with ':' + x.xs fitted)
.dmr_filter_timeout			= "Timp filtru", // MaxLen: 16 (with ':' + 1..90 + 's')
.brightness				= "Lumina", // MaxLen: 16 (with ':' + 0..100 + '%')
.brightness_off				= "Lum. Min", // MaxLen: 16 (with ':' + 0..100 + '%')
.contrast				= "Contrast", // MaxLen: 16 (with ':' + 12..30)
.screen_invert				= "Invers", // MaxLen: 16
.screen_normal				= "Normal", // MaxLen: 16
.backlight_timeout			= "Timeout", // MaxLen: 16 (with ':' + .no to 30s)
.scan_delay				= "Aman cautare", // MaxLen: 16 (with ':' + 1..30 + 's')
.yes___in_uppercase			= "DA", // MaxLen: 8 (choice above green/red buttons)
.no___in_uppercase			= "NU", // MaxLen: 8 (choice above green/red buttons)
.DISMISS				= "ANULARE", // MaxLen: 8 (choice above green/red buttons)
.scan_mode				= "Mod caut", // MaxLen: 16 (with ':' + .hold, .pause or .stop)
.hold					= "Stai", // MaxLen: 16 (with ':' + .scan_mode)
.pause					= "Pauza", // MaxLen: 16 (with ':' + .scan_mode)
.list_empty				= "Lista goala", // MaxLen: 16
.delete_contact_qm			= "Sterge contact?", // MaxLen: 16
.contact_deleted			= "Contact sters", // MaxLen: 16
.contact_used				= "Contact exitent", // MaxLen: 16
.in_tg_list				= "in lista TG", // MaxLen: 16
.select_tx				= "Select TX", // MaxLen: 16
.edit_contact				= "Modif. contact", // MaxLen: 16
.delete_contact				= "Sterge contact", // MaxLen: 16
.group_call				= "Apel Grup", // MaxLen: 16
.all_call				= "Apel Toti", // MaxLen: 16
.tone_scan				= "Cautare Ton", // MaxLen: 16
.low_battery				= "ZERO BATERIE !!!", // MaxLen: 16
.Auto					= "Auto", // MaxLen 16 (with .mode + ':', .mode)
.manual					= "Manual",  // MaxLen 16 (with .mode + ':', .mode)
.ptt_toggle				= "Tine PTT", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling			= "Admite PC", // MaxLen 16 (with ':' + .on or .off) // actually 9 chars in RO
.stop					= "Stop", // Maxlen 16 (with ':' + .scan_mode/.dmr_beep)
.one_line				= "1 linie", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 linii", // MaxLen 16 (with ':' + .contact)
.new_channel				= "Canal Nou", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order				= "Ordine", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "DMR beep", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "Start", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "Toate", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold				= "Nivel VOX", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail				= "Coada VOX", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt				= "Prompt",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent				= "Silent", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX beep", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "Beep", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1			= "Voce", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1			= "TA Tx TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF				= "VHF Squelch",// Maxlen 16 (with : + XX%)
.squelch_220				= "220 Squelch",// Maxlen 16 (with : + XX%)
.squelch_UHF				= "UHF Squelch", // Maxlen 16 (with : + XX%)
.display_screen_invert 			= "Ecran", // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "Fara tast", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2			= "Voce L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3			= "Voce L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "Filtru DMR",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Apelant",
.dmr_ts_filter				= "Filtru TS", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "Contacte FM DTMF", // Maxlen: 16
.channel_power				= "Putere", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "Master",// Displayed if per-channel power is not enabled  the .channel_power, and also with .location in APRS settings.
.set_quickkey				= "Tast. Rapida", // MaxLen: 16
.dual_watch				= "Dual Watch", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or .ts or .pwr or .both)
.pwr					= "Pwr",
.user_power				= "Putere Util.",
.temperature				= "Temperatura", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "�C",
.seconds				= "secunde",
.radio_info				= "Stare aparat",
.temperature_calibration		= "Cal temp",
.pin_code				= "Cod pin",
.please_confirm				= "Confirmati", // MaxLen: 15
.vfo_freq_bind_mode			= "Freq. Bind",
.overwrite_qm				= "Suprascrie ?", //Maxlen: 14 chars
.eco_level				= "Nivel Eco",
.buttons				= "Butoane",
.leds					= "LEDs",
.scan_dwell_time			= "Stationare",
.battery_calibration			= "Cal. Bat.",
.low					= "Mic",
.high					= "Mare",
.dmr_id					= "DMR ID",
.scan_on_boot				= "Caut prn",
.dtmf_entry				= "Introd. DTMF",
.name					= "Nume",
.carrier				= "Semnal",
.zone_empty 				= "Zona goala", // Maxlen: 12 chars.
.time					= "Timp",
.uptime					= "Funct.",
.hours					= "Ore",
.minutes				= "Minute",
.satellite				= "Sateliti",
.alarm_time				= "Ora alarma",
.location				= "Locatie",
.date					= "Data",
.timeZone				= "Fus Orar",
.suspend				= "Suspenda",
.pass					= "Treci", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "in",
.predicting				= "Predictie",
.maximum				= "Max",
.satellite_short			= "Sat",
.local					= "Local",
.UTC					= "UTC",
.symbols				= "NSEV", // symbols: N,S,E,W
.not_set				= "NESETAT",
.general_options			= "General",
.radio_options				= "Radio",
.auto_night				= "Tema Auto", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "DMR Rx AGC",
.speaker_click_suppress			= "Fara Click",
.gps					= "GPS",
.end_only				= "Sfarsit",
.dmr_crc				= "DMR crc",
.eco					= "Eco",
.safe_power_on				= "Prn sigur", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "Opr auto", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "Prn cu RF", // MaxLen: 16 (with ':' + .yes or .no or .n_a)
.brightness_night			= "Ilum. noapte", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF				= "Frecv VHF",
.gps_acquiring				= "Achizitie",
.altitude				= "Alt",
.calibration				= "Calibrare",
.freq_set_UHF				= "Frecv UHF",
.cal_frequency				= "Frecv",
.cal_pwr				= "Nivel Putere",
.pwr_set				= "Modifica",
.factory_reset				= "Reset Fabrica",
.rx_tune				= "Modif. Rx",
.transmitTalkerAliasTS2		= "TA Tx TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Text",
.daytime_theme_day			= "Tema Zi", // MaxLen: 16
.daytime_theme_night			= "Tema Noapte", // MaxLen: 16
.theme_chooser				= "Alege Tema", // Maxlen: 16
.theme_options				= "Tema",
.theme_fg_default			= "Text implicit", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Fundal", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Decoratii", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Text scriere", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "Primplan pornire", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Fundal pornire", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Text notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Atentie notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Eroare notif.", // MaxLen: 16 (+ colour rect)
.theme_bg_notification			= "Fundal notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Nume meniu", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Nume meniu fund", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Articol meniu", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Selectie meniu", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "Setare valoare", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Text titlu", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Text titlu fund", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "RSSI indicator", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "RSSI indic. S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Nume canal", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Contact", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Info contact", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Nume zona", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "Frecv. RX", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "Frecv. TX", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "Valori CSS/SQL", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "Numarator TX", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polar", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Sat. spot", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "GPS numar", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "GPS spot", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "BeiDou spot", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Rosu", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Verde", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Albastru", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Volum", // MaxLen: 8
.distance_sort				= "Dist sort", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Afis dist", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS", // MaxLen 16
.aprs_smart				= "Smart", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "Canal", // MaxLen 16 (with ':' + .location)
.aprs_decay				= "Decay", // MaxLen 16 (with ':' + .on or .off)
.aprs_compress				= "Compresie", // MaxLen 16 (with ':' + .on or .off)
.aprs_interval				= "Interval", // MaxLen 16 (with ':' + 0.2..60 + 'min')
.aprs_message_interval			= "Msg Interval", // MaxLen 16 (with ':' + 3..30)
.aprs_slow_rate				= "Rata lent", // MaxLen 16 (with ':' + 1..100 + 'min')
.aprs_fast_rate				= "Rata rapid", // MaxLen 16 (with ':' + 10..180 + 's')
.aprs_low_speed				= "Vit mica", // MaxLen 16 (with ':' + 2..30 + 'km/h')
.aprs_high_speed			= "Vit mare", // MaxLen 16 (with ':' + 2..90 + 'km/h')
.aprs_turn_angle			= "Unghi T.", // MaxLen 16 (with ':' + 5..90 + '°')
.aprs_turn_slope			= "Panta T.", // MaxLen 16 (with ':' + 1..255 + '°/v')
.aprs_turn_time				= "Timp T.", // MaxLen 16 (with ':' + 5..180 + 's')
.auto_lock				= "Autobloc.", // MaxLen 16 (with ':' + .off or 0.5..15 (.5 step) + 'min')
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
#endif /* USER_INTERFACE_LANGUAGES_ROMANIAN_H_ */
