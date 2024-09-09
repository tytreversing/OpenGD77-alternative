/* -*- coding: binary; -*- */
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
/* WARNING: THIS FILE USES BINARY (OCTAL) ENCODING, IT USES REMAPPED GLYPHS
 *
 * Translators: SQ7PTE
 *
 * Rev: 4.8
 */
#ifndef USER_INTERFACE_LANGUAGES_POLISH_H_
#define USER_INTERFACE_LANGUAGES_POLISH_H_
/********************************************************************
 *
 * VERY IMPORTANT.
 * This file should not be saved with UTF-8 encoding
 * Use Notepad++ on Windows with ANSI encoding
 * or emacs on Linux with binary encoding
 *
 ********************************************************************/
#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
__attribute__((section(".upper_text")))
#endif
const stringsTable_t polishLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 			= "Polski", // MaxLen: 16
.menu					= "Spis", // MaxLen: 16
.credits				= "WspÛàtwÛrcy", // MaxLen: 16
.zone					= "Strefy", // MaxLen: 16
.rssi					= "Sygnaà", // MaxLen: 16
.battery				= "Bateria", // MaxLen: 16
.contacts				= "Kontakty", // MaxLen: 16
.last_heard				= "Ostatnio aktywne", // MaxLen: 16
.firmware_info				= "Wersja programu", // MaxLen: 16
.options				= "Opcje ogÛlne", // MaxLen: 16
.display_options			= "Opcje ekranu", // MaxLen: 16
.sound_options				= "Opcje dêwiÜku", // MaxLen: 16
.channel_details			= "Detale kanaàu", // MaxLen: 16
.language				= "JÜzyk", // MaxLen: 16
.new_contact				= "Nowy kontakt", // MaxLen: 16
.dmr_contacts				= "DMR kontakt", // MaxLen: 16
.contact_details			= "Detale kontaktu", // MaxLen: 16
.hotspot_mode				= "HotSpot", // MaxLen: 16
.built					= "Kompilacja", // MaxLen: 16
.zones					= "Strefy", // MaxLen: 16
.keypad					= "Klawiatura", // MaxLen: 12 (with .ptt)
.ptt					= "PTT", // MaxLen: 12 (with .keypad)
.locked					= "zablokowana", // MaxLen: 15
.press_sk2_plus_star			= "SK2 + *", // MaxLen: 16
.to_unlock				= "aby odblokowaÇ", // MaxLen: 16
.unlocked				= "Odblokowane", // MaxLen: 15
.power_off				= "WyàÑczanie...", // MaxLen: 16
.error					= "BáÉD", // MaxLen: 8
.rx_only				= "Tylko odbiÛr", // MaxLen: 14
.out_of_band				= "POZA PASMEM", // MaxLen: 14
.timeout				= "CZAS MINÉá", // MaxLen: 8
.tg_entry				= "Wpisz numer TG", // MaxLen: 15
.pc_entry				= "Wpisz numer PC", // MaxLen: 15
.user_dmr_id				= "ID uíytkownika", // MaxLen: 15
.contact 				= "Kontakt", // MaxLen: 15
.accept_call				= "ZaakceptowaÇ?", // MaxLen: 16
.private_call				= "Prywatne", // MaxLen: 16
.squelch				= "Squelch", // MaxLen: 8
.quick_menu 				= "Szybkie opcje", // MaxLen: 16
.filter					= "Filtr", // MaxLen: 7 (with ':' + settings: .none, "CC", "CC,TS", "CC,TS,TG")
.all_channels				= "Wszystkie", // MaxLen: 16
.gotoChannel				= "Idê do",  // MaxLen: 11 (" 1024")
.scan					= "Skanowanie", // MaxLen: 16
.channelToVfo				= "Kanaà > VFO", // MaxLen: 16
.vfoToChannel				= "VFO > Kanaà", // MaxLen: 16
.vfoToNewChannel			= "VFO > Nowy kanaà", // MaxLen: 16
.group					= "Grupa", // MaxLen: 16 (with .type)
.private				= "Prywatny", // MaxLen: 16 (with .type)
.all					= "Wszystko", // MaxLen: 16 (with .type)
.type					= "Typ", // MaxLen: 16 (with .type)
.timeSlot				= "Szczelina", // MaxLen: 16 (plus ':' and  .none, '1' or '2')
.none					= "Brak", // MaxLen: 16 (with .timeSlot, "Rx CTCSS:" and ""Tx CTCSS:", and .filter)
.contact_saved				= "Zapisz kontakt", // MaxLen: 16
.duplicate				= "Duplikat", // MaxLen: 16
.tg					= "TG",  // MaxLen: 8
.pc					= "PC", // MaxLen: 8
.ts					= "TS", // MaxLen: 8
.mode					= "Tryb",  // MaxLen: 12
.colour_code				= "Kolor kodu", // MaxLen: 16 (with ':' * .n_a)
.n_a					= "N/A",// MaxLen: 16 (with ':' * .colour_code)
.bandwidth				= "Pasmo", // MaxLen: 16 (with ':' + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "Krok", // MaxLen: 7 (with ':' + xx.xxkHz fitted)
.tot					= "TOT", // MaxLen: 16 (with ':' + .off or 15..3825)
.off					= "Wyà.", // MaxLen: 16 (with ':' + .timeout_beep, .band_limits)
.zone_skip				= "Pomiã kanaà", // MaxLen: 16 (with ':' + .yes or .no)
.all_skip				= "Pomiã All", // MaxLen: 16 (with ':' + .yes or .no)
.yes					= "Tak", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.no					= "Nie", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.tg_list				= "Lst TG", // MaxLen: 16 (with ':' and codeplug group name)
.on					= "Wà.", // MaxLen: 16 (with ':' + .band_limits)
.timeout_beep				= "Czas bipa", // MaxLen: 16 (with ':' + .n_a or 5..20 + 's')
.list_full				= "List full",
.dmr_cc_scan				= "Skan. CC", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits				= "Limit pasma", // MaxLen: 16 (with ':' + .on or .off)
.beep_volume				= "Gàos bipa", // MaxLen: 16 (with ':' + -24..6 + 'dB')
.dmr_mic_gain				= "DMR mic", // MaxLen: 16 (with ':' + -33..12 + 'dB')
.fm_mic_gain				= "FM mic", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "Key dàugi", // MaxLen: 11 (with ':' + x.xs fitted)
.key_repeat				= "Key krÛtki", // MaxLen: 11 (with ':' + x.xs fitted)
.dmr_filter_timeout			= "Czas filtra", // MaxLen: 16 (with ':' + 1..90 + 's')
.brightness				= "JasnoçÇ", // MaxLen: 16 (with ':' + 0..100 + '%')
.brightness_off				= "Mini jasnoçÇ", // MaxLen: 16 (with ':' + 0..100 + '%')
.contrast				= "Kontrast", // MaxLen: 16 (with ':' + 12..30)
.screen_invert				= "OdwrÛcony", // MaxLen: 16
.screen_normal				= "Normalny", // MaxLen: 16
.backlight_timeout			= "åwiecenie", // MaxLen: 16 (with ':' + .no to 30)
.scan_delay				= "Czas skan.", // MaxLen: 16 (with ':' + 1..30 + 's')
.yes___in_uppercase			= "TAK", // MaxLen: 8 (choice above green/red buttons)
.no___in_uppercase			= "NIE", // MaxLen: 8 (choice above green/red buttons)
.DISMISS				= "ODWOáAÅ", // MaxLen: 8 (choice above green/red buttons)
.scan_mode				= "Tryb skan.", // MaxLen: 16 (with ':' + .hold or .pause)
.hold					= "StÛj", // MaxLen: 16 (with ':' + .scan_mode)
.pause					= "Pauza", // MaxLen: 16 (with ':' + .scan_mode)
.list_empty				= "Lista pusta", // MaxLen: 16
.delete_contact_qm			= "Usuã kontakt?", // MaxLen: 16
.contact_deleted			= "Kontakt usuniety", // MaxLen: 16
.contact_used				= "Kontakt istnieje", // MaxLen: 16
.in_tg_list				= "na liçcie tg", // MaxLen: 16
.select_tx				= "Wybierz TX", // MaxLen: 16
.edit_contact				= "Edytuj kontakt", // MaxLen: 16
.delete_contact				= "Usuã kontakt", // MaxLen: 16
.group_call				= "Grupy", // MaxLen: 16
.all_call				= "áÑcze wszystkich", // MaxLen: 16
.tone_scan				= "Skanuj",//// MaxLen: 16
.low_battery				= "SáABA BATERIA!!!",//// MaxLen: 16
.Auto					= "Automat", // MaxLen 16 (with .mode + ':')
.manual					= "Manualny",  // MaxLen 16 (with .mode + ':')
.ptt_toggle				= "Staàe PTT", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling  		= "ZezwÛl PC", // MaxLen 16 (with ':' + .on ot .off)
.stop					= "Stop", // Maxlen 16 (with ':' + .scan_mode)
.one_line				= "1 linia", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 linie", // MaxLen 16 (with ':' + .contact)
.new_channel				= "Nowy kanaà", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order				= "WybÛr", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "WybÛr bipa", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "Start", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "Oba", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold                          = "VOX Thres.", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail                               = "VOX Tail", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt				= "Bip audio",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                                 = "Brak", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX beep", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "Bip", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1			= "Gàos", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1			= "TA Tx TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF				= "VHF Squelch",// Maxlen 16 (with : + XX%)
.squelch_220				= "220 Squelch",// Maxlen 16 (with : + XX%)
.squelch_UHF				= "UHF Squelch", // Maxlen 16 (with : + XX%)
.display_screen_invert 		= "Kolor" , // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No Keys", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2			= "Gàos 2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3			= "Gàos 3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "DMR Filtr",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter				= "TS Filtr", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "FM DTMF kontakt", // Maxlen: 16
.channel_power				= "Moc kanaàu", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "Master",// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey				= "Set Quick Key", // MaxLen: 16
.dual_watch				= "Nasàuch 1i2 VFO", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr					= "Moc",
.user_power				= "Max moc +W-",
.temperature				= "Temperatura", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "∞C",
.seconds				= "sekund",
.radio_info				= "Info o radiu",
.temperature_calibration		= "Temp Cal",
.pin_code				= "Pin Kod",
.please_confirm				= "To potwierdê..", // MaxLen: 15
.vfo_freq_bind_mode			= "WiÑí L1iL2",
.overwrite_qm				= "PrzypisaÇ ?", //Maxlen: 14 chars
.eco_level				= "Eco poziom",
.buttons				= "Klawisz",
.leds					= "åwiatào LED",
.scan_dwell_time			= "Scan dwell",
.battery_calibration			= "Batt. Cal",
.low					= "Niski",
.high					= "Wysoki",
.dmr_id					= "DMR ID",
.scan_on_boot				= "Skan->Start",
.dtmf_entry				= "DTMF wpis",
.name					= "Nazwa",
.carrier				= "Carrier",
.zone_empty 				= "Zone empty", // Maxlen: 12 chars.
.time					= "Czas",
.uptime					= "Upàyw czasu",
.hours					= "Godzina",
.minutes				= "Minuta",
.satellite				= "Satelita",
.alarm_time				= "Alarm czasu",
.location				= "Lokalizacja",
.date					= "Data",
.timeZone				= "Strefa",
.suspend				= "ZawiesiÇ",
.pass					= "Pass", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "in",
.predicting				= "Przewidywanie",
.maximum				= "Max",
.satellite_short			= "Sat",
.local					= "Local",
.UTC					= "UTC",
.symbols				= "NSEW", // symbols: N,S,E,W
.not_set				= "Nie ustawiony",
.general_options			= "GàÛwne opcje",
.radio_options				= "Opcje radia",
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
.theme_options				= "Ustaw. motywu",
.theme_fg_default			= "Tekst domyçlny", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Tào", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Dekoracja", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Tekst 1 plan", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "1 plan uruchom", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Tào rozruchu ", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Tekstowe powiad.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Ostrzeíenie", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "BàÑd", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Powiadom. w tle", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Nazwa menu", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Nazwa menu tào", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Menu 1-plan", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Menu wybrane", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "WartoçÇ opcji", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Tekst nagàÛwka", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Tào nagà. tekst.", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "Pasek RSSI", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "Pasek RSSI S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Nazwa kanaàu", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Kontakt", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Info kontaktu", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Nazwa strefy", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "CzÜstotliwoçÇ RX", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "CzestotliwoçÇ TX", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "Wart. CSS/SQL", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "Licznik TX", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polar", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Satelity", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "GPS numer", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "GPS miejsce", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "BeiDou", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Czerwony", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Zielony", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Niebieski", // MaxLen 16 (with ':' + 3 digits value)
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
.aprs_turn_angle			= "T. Angle", // MaxLen 16 (with ':' + 5..90 + '<degree>')
.aprs_turn_slope			= "T. Slope", // MaxLen 16 (with ':' + 1..255 + '<degree>/v')
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
 * or emacs on Linux with binary encoding
 *
 ********************************************************************/
#endif /* USER_INTERFACE_LANGUAGES_POLISH_H_ */
