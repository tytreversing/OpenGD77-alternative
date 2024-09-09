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
 * Translators: Roger Caballero Jonsson, SM0LTV
 *
 *
 * Rev: 1.2
 */
#ifndef USER_INTERFACE_LANGUAGES_SWEDISH_H_
#define USER_INTERFACE_LANGUAGES_SWEDISH_H_
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
const stringsTable_t swedishLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 				= "Svenska", // MaxLen: 16
.menu					= "Meny", // MaxLen: 16
.credits				= "Poäng", // MaxLen: 16
.zone					= "Zon", // MaxLen: 16
.rssi					= "Fältstyrka", // MaxLen: 16
.battery				= "Batteri", // MaxLen: 16
.contacts				= "Kontakter", // MaxLen: 16
.last_heard				= "Senast hörd", // MaxLen: 16
.firmware_info				= "Firmware info", // MaxLen: 16
.options				= "Alternativ", // MaxLen: 16
.display_options			= "Skärmalternativ", // MaxLen: 16
.sound_options				= "Ljud alternativ", // MaxLen: 16
.channel_details			= "Kanal detaljer", // MaxLen: 16
.language				= "Språk", // MaxLen: 16
.new_contact				= "Ny kontakt", // MaxLen: 16
.dmr_contacts				= "DMR kontakt", // MaxLen: 16
.contact_details			= "Kontakt detaljer", // MaxLen: 16
.hotspot_mode				= "Hotspot", // MaxLen: 16
.built					= "Skapad", // MaxLen: 16
.zones					= "Zoner", // MaxLen: 16
.keypad					= "", // MaxLen: 12 (with .ptt)
.ptt					= "PTT", // MaxLen: 12 (with .keypad)
.locked					= "Låst", // MaxLen: 15
.press_sk2_plus_star			= "Tryck SK2 + *", // MaxLen: 16
.to_unlock				= "för att låsa upp", // MaxLen: 16
.unlocked				= "Upplåst", // MaxLen: 15
.power_off				= "Stänger av...", // MaxLen: 16
.error					= "FEL", // MaxLen: 8
.rx_only				= "Endast Rx", // MaxLen: 14
.out_of_band				= "UTANFÖR BANDET", // MaxLen: 14
.timeout				= "TIDSGR.", // MaxLen: 8
.tg_entry				= "TG post", // MaxLen: 15
.pc_entry				= "PC post", // MaxLen: 15
.user_dmr_id				= "Användar DMR ID", // MaxLen: 15
.contact 				= "Kontakt", // MaxLen: 15
.accept_call				= "Acceptera anrop", // MaxLen: 16
.private_call				= "Privatanrop", // MaxLen: 16
.squelch				= "Brussp.", // MaxLen: 8
.quick_menu 				= "Snabb Menu", // MaxLen: 16
.filter					= "Filter", // MaxLen: 7 (with ':' + settings: .none, "CC", "CC,TS", "CC,TS,TG")
.all_channels				= "Alla kanaler", // MaxLen: 16
.gotoChannel				= "Gå till",  // MaxLen: 11 (" 1024")
.scan					= "Skanna", // MaxLen: 16
.channelToVfo				= "Kanal --> VFO", // MaxLen: 16
.vfoToChannel				= "VFO --> Kanal", // MaxLen: 16
.vfoToNewChannel			= "VFO --> Ny kanal", // MaxLen: 16
.group					= "Grupp", // MaxLen: 16 (with .type)
.private				= "Privat", // MaxLen: 16 (with .type)
.all					= "Alla", // MaxLen: 16 (with .type)
.type					= "Typ", // MaxLen: 16 (with .type)
.timeSlot				= "Timeslot", // MaxLen: 16 (plus ':' and  .none, '1' or '2')
.none					= "Ingen", // MaxLen: 16 (with .timeSlot, "Rx CTCSS:" and ""Tx CTCSS:", .filter/.mode/.dmr_beep)
.contact_saved				= "Kontakt sparad", // MaxLen: 16
.duplicate				= "Dubletter", // MaxLen: 16
.tg					= "TG",  // MaxLen: 8
.pc					= "PC", // MaxLen: 8
.ts					= "TS", // MaxLen: 8
.mode					= "Läge",  // MaxLen: 12
.colour_code				= "Color Code", // MaxLen: 16 (with ':' * .n_a)
.n_a					= "N/A",// MaxLen: 16 (with ':' * .colour_code)
.bandwidth				= "BW", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "Steg", // MaxLen: 7 (with ':' + xx.xxkHz fitted)
.tot					= "TOT", // MaxLen: 16 (with ':' + .off or 15..3825)
.off					= "Av", // MaxLen: 16 (with ':' + .timeout_beep, .calibration or .band_limits)
.zone_skip				= "Zon hopp", // MaxLen: 16 (with ':' + .yes or .no)
.all_skip				= "All hopp", // MaxLen: 16 (with ':' + .yes or .no)
.yes					= "Ja", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.no					= "N", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.tg_list				= "TG Lst", // MaxLen: 16 (with ':' and codeplug group name)
.on					= "På", // MaxLen: 16 (with ':' + .calibration or .band_limits)
.timeout_beep				= "Tidsgräns pip", // MaxLen: 16 (with ':' + .n_a or 5..20 + 's')
.list_full				= "List full", ///<<< TRANSLATE
.dmr_cc_scan				= "CC Scan", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits				= "Bandgräns", // MaxLen: 16 (with ':' + .on or .off)
.beep_volume				= "Pip vol", // MaxLen: 16 (with ':' + -24..6 + 'dB')
.dmr_mic_gain				= "DMR mik", // MaxLen: 16 (with ':' + -33..12 + 'dB')
.fm_mic_gain				= "FM mik", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "Tng lng", // MaxLen: 11 (with ':' + x.xs fitted)
.key_repeat				= "Tng rpt", // MaxLen: 11 (with ':' + x.xs fitted)
.dmr_filter_timeout			= "Filter tid", // MaxLen: 16 (with ':' + 1..90 + 's')
.brightness				= "Ljusstyrka", // MaxLen: 16 (with ':' + 0..100 + '%')
.brightness_off				= "Min ljus", // MaxLen: 16 (with ':' + 0..100 + '%')
.contrast				= "Kontrast", // MaxLen: 16 (with ':' + 12..30)
.screen_invert				= "Invert", // MaxLen: 16
.screen_normal				= "Normal", // MaxLen: 16
.backlight_timeout			= "Tidsgräns", // MaxLen: 16 (with ':' + .no to 30s)
.scan_delay				= "Scannfördr.", // MaxLen: 16 (with ':' + 1..30 + 's')
.yes___in_uppercase			= "JA", // MaxLen: 8 (choice above green/red buttons)
.no___in_uppercase			= "NEJ", // MaxLen: 8 (choice above green/red buttons)
.DISMISS				= "AVFÄRDA", // MaxLen: 8 (choice above green/red buttons)
.scan_mode				= "Scannläge", // MaxLen: 16 (with ':' + .hold, .pause or .stop)
.hold					= "Håll", // MaxLen: 16 (with ':' + .scan_mode)
.pause					= "Paus", // MaxLen: 16 (with ':' + .scan_mode)
.list_empty				= "Tom Lista", // MaxLen: 16
.delete_contact_qm			= "Radera kontakt?", // MaxLen: 16
.contact_deleted			= "Kontakt raderad", // MaxLen: 16
.contact_used				= "Kontakten använd", // MaxLen: 16
.in_tg_list				= "i TG-listan", // MaxLen: 16
.select_tx				= "Välj TX", // MaxLen: 16
.edit_contact				= "Redigera kontakt", // MaxLen: 16
.delete_contact				= "Radera kontakt", // MaxLen: 16
.group_call				= "Gruppanrop", // MaxLen: 16
.all_call				= "Allanrop", // MaxLen: 16
.tone_scan				= "Ton scann", // MaxLen: 16
.low_battery				= "LÅG BATTERINIVÅ", // MaxLen: 16
.Auto					= "Auto", // MaxLen 16 (with .mode + ':')
.manual					= "Manuell",  // MaxLen 16 (with .mode + ':')
.ptt_toggle				= "PTT spärr", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling			= "Tillåt PC", // MaxLen 16 (with ':' + .on or .off)
.stop					= "Stopp", // Maxlen 16 (with ':' + .scan_mode/.dmr_beep)
.one_line				= "1 linje", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 linjer", // MaxLen 16 (with ':' + .contact)
.new_channel				= "Ny kanal", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order				= "Beställ", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "DMR pip", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "Start", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "Båda", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold                          = "VOX Tröskel", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail                               = "VOX Släpp", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt				= "Prompt",// Maxlen 16 (with ':' + .silent, .normal, .beep or .voice_prompt_level_1)
.silent                                 = "Tyst", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX beep", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "Pip", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1			= "Röst", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1			= "TA Tx TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF				= "VHF Brussp.",// Maxlen 16 (with : + XX%)
.squelch_220				= "220 Brussp.",// Maxlen 16 (with : + XX%)
.squelch_UHF				= "UHF Brussp.", // Maxlen 16 (with : + XX%)
.display_screen_invert 		= "BG Färg", // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No Keys", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2			= "Röst L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3			= "Röst L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "DMR Filter",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter				= "TS Filter", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "FM DTMF kontakt.", // Maxlen: 16
.channel_power				= "Ch Effekt", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "Master",// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey				= "Set Snabbtangent", // MaxLen: 16
.dual_watch				= "Dubbel vakt", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or .ts or .pwr or .both)
.pwr					= "Eff.",
.user_power				= "Användar Endast",
.temperature				= "Temperatur", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "C",
.seconds				= "sekunder",
.radio_info				= "Radio info",
.temperature_calibration		= "Temp Kal",
.pin_code				= "Pin-kod",
.please_confirm				= "Bekräfta", // MaxLen: 15
.vfo_freq_bind_mode			= "Bind Freq.",
.overwrite_qm				= "Skrivöver ?", //Maxlen: 14 chars
.eco_level				= "Eko Nivå",
.buttons				= "Knappar",
.leds					= "LEDs",
.scan_dwell_time			= "Scannstopp",
.battery_calibration			= "Batt. Kal",
.low					= "Låg",
.high					= "Hög",
.dmr_id					= "DMR ID",
.scan_on_boot				= "Scanna vid Boot",
.dtmf_entry				= "DTMF post",
.name					= "Namn",
.carrier				= "Carrier",
.zone_empty 				= "Zone empty", // Maxlen: 12 chars.
.time					= "Tid",
.uptime					= "Upetid",
.hours					= "Timmar",
.minutes				= "Minuter",
.satellite				= "Satelit",
.alarm_time				= "Alarm tid",
.location				= "Plats",
.date					= "Datum",
.timeZone				= "Tidezon",
.suspend				= "Hänga",
.pass					= "Pasera",
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "in",
.predicting				= "Förutsäga",
.maximum				= "Max",
.satellite_short			= "Sat",
.local					= "Lokal",
.UTC					= "UTC",
.symbols				= "NSÖV", // symbols: N,S,E,W
.not_set				= "EJ INSTÄLLD",
.general_options			= "General options",
.radio_options			= "Radio options",
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
#endif /* USER_INTERFACE_LANGUAGES_SWEDISH_H_ */
