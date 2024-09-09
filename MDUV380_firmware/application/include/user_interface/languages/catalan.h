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
 * Translators: EA3IGM, EA5SW, EB3AM, EA3BIL
 *
 *
 * Rev: 05Marzo2024 EA3BIL
 */
#ifndef USER_INTERFACE_LANGUAGES_CATALAN_H_
#define USER_INTERFACE_LANGUAGES_CATALAN_H_
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
const stringsTable_t catalanLanguage=
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME			= "Català",
.menu					= "Menú",
.credits				= "Crèdits",
.zone					= "Zona",
.rssi					= "Senyal",
.battery				= "Batería",
.contacts				= "Contactes",
.last_heard				= "Escoltats",
.firmware_info			= "Info programari",
.options				= "Opcions",
.display_options		= "Opcions Pantalla",
.sound_options			= "Opcions Só", // MaxLen: 16
.channel_details		= "Detalls Canal",
.language				= "Idioma",
.new_contact			= "Nou contacte",
.dmr_contacts			= "Contactes DMR", // MaxLen: 16
.contact_details		= "Detall Ctte",
.hotspot_mode			= "HotSpot",
.built					= "Compilat",
.zones					= "Zones",
.keypad					= "Teclat",
.ptt					= "PTT",
.locked					= "Blocat",
.press_sk2_plus_star	= "Prem SK2 + (*)",
.to_unlock				= "per desblocar",
.unlocked				= "Desblocat",
.power_off				= "Apagant...",
.error					= "ERROR",
.rx_only				= "Només RX",
.out_of_band			= "FORA DE BANDA",
.timeout				= "TIMEOUT",
.tg_entry				= "Entreu TG",
.pc_entry				= "Entreu ID",
.user_dmr_id			= "ID DMR d'Usuari",
.contact				= "Contacte",
.accept_call			= "Accept. trucada?",
.private_call			= "Trucada privada",
.squelch				= "Squelch",
.quick_menu				= "Menú Ràpid",
.filter					= "Filtre",
.all_channels			= "Llista total",
.gotoChannel			= "Anar al canal",
.scan					= "Escaneig",
.channelToVfo			= "Canal -> VFO",
.vfoToChannel			= "VFO -> Canal",
.vfoToNewChannel		= "VFO -> Nou Canal", // MaxLen: 16
.group					= "Grup",
.private				= "Privada",
.all					= "Tots",
.type					= "Tipus",
.timeSlot				= "TS",
.none					= "Cap",
.contact_saved			= "Contacte Desat",
.duplicate				= "Duplicat",
.tg						= "TG",
.pc						= "ID",
.ts						= "TS",
.mode					= "Mode",
.colour_code			= "Codi Color",
.n_a					= "N/D",
.bandwidth				= "BW", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "Salt",
.tot					= "TOT",
.off					= "NO",
.zone_skip				= "Saltar Zona",
.all_skip				= "Saltar Tot",
.yes					= "Sí",
.no						= "No",
.tg_list				= "TG Llista",
.on						= "Sí",
.timeout_beep			= "Avís T.O.T.",
.list_full				= "Llista plena",
.dmr_cc_scan			= "Escan CC", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits			= "Límit bandes",
.beep_volume			= "Volum tons",
.dmr_mic_gain			= "DMR mic",
.fm_mic_gain			= "FM mic", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "Prem llarg",
.key_repeat				= "Prem rpt",
.dmr_filter_timeout		= "Filtre temps",
.brightness				= "Brillantor",
.brightness_off			= "Brillan. min",
.contrast				= "Contrast",
.screen_invert			= "Invers",
.screen_normal			= "Normal",
.backlight_timeout		= "Temps llum",
.scan_delay				= "Temps Scan",
.yes___in_uppercase		= "SÍ",
.no___in_uppercase		= "NO",
.DISMISS				= "PASSAR",
.scan_mode				= "Mode Scan",
.hold					= "Parar",
.pause					= "Pausa",
.list_empty				= "Llista buida",
.delete_contact_qm		= "Esborrar ctte?",
.contact_deleted		= "Ctte esborrat",
.contact_used			= "Ctte en ús a",
.in_tg_list			= "a la llista TG",
.select_tx				= "Selec. TX",
.edit_contact			= "Editar Ctte",
.delete_contact			= "Esborrar Ctte",
.group_call				= "Cridar a Grup",
.all_call				= "Cridar a Tots",
.tone_scan				= "Scan CCTCS",//// MaxLen: 16
.low_battery			= "BATERIA BAIXA !!",//// MaxLen: 16
.Auto					= "Auto", // MaxLen 16 (with .mode + ':')
.manual					= "Manual",  // MaxLen 16 (with .mode + ':')
.ptt_toggle				= "Enclavar PTT", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling	= "Gestió PC", // MaxLen 16 (with ':' + .on ot .off)
.stop					= "Stop", // Maxlen 16 (with ':' + .scan_mode)
.one_line				= "1 línia", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 línies", // MaxLen 16 (with ':' + .contact)
.new_channel			= "Nou canal", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order			= "Prio.", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "To DMR", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "Inici", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "Tots", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold			= "Nivell VOX", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail				= "Cua VOX", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt			= "Avís",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent					= "Silenci", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX beep", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "Beep", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1	= "Veu", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1	= "Enviar TA 1", // Maxlen 16 (with : + .off .text APRS .both)
.squelch_VHF			= "Squelch VHF",// Maxlen 16 (with : + XX%)
.squelch_220			= "Squelch 220",// Maxlen 16 (with : + XX%)
.squelch_UHF			= "Squelch UHF", // Maxlen 16 (with : + XX%)
.display_screen_invert = "Color" , // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No Keys", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2	= "Veu L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3	= "Veu L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "Filtre DMR",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter			= "Filtre TS", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list		= "Contactes DTMF", // Maxlen: 16
.channel_power			= "Pot. Ch", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master			= "Master",// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey			= "Ajust Quickkey", // MaxLen: 16
.dual_watch				= "Dual Watch", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr					= "Pot.",
.user_power				= "Pot. usuari",
.temperature			= "Temperatura", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "°C",
.seconds				= "segons",
.radio_info				= "Info ràdio",
.temperature_calibration	= "Cal. temp.",
.pin_code				= "Codi pin",
.please_confirm			= "Confirmeu", // MaxLen: 15
.vfo_freq_bind_mode		= "Copiar Freq.",
.overwrite_qm			= "Reemplaçar?", //Maxlen: 14 chars
.eco_level				= "Nivell eco",
.buttons				= "Botons",
.leds					= "LEDs",
.scan_dwell_time		= "Temps Mostra",
.battery_calibration	= "Cal. bat.",
.low					= "Baix",
.high					= "Alt",
.dmr_id					= "ID DMR",
.scan_on_boot			= "Scan inicial",
.dtmf_entry				= "Escriu DTMF",
.name					= "Nom",
.carrier				= "Portadora",
.zone_empty 				= "Zona buida", // Maxlen: 12 chars.
.time					= "Hora",
.uptime					= "Arrenca en",
.hours					= "Hores",
.minutes				= "Minuts",
.satellite				= "Satèl.lit",
.alarm_time				= "Hora Avís",
.location				= "Ubicació",
.date					= "Data",
.timeZone				= "Fus Horari",
.suspend				= "Hibernar",
.pass					= "Passada", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "en",
.predicting				= "Predicció",
.maximum				= "Màx",
.satellite_short		= "Sat",
.local					= "Local",
.UTC					= "UTC",
.symbols				= "NSEO", // symbols: N,S,E,W
.not_set				= "NO DEFINIT",
.general_options		= "Opcions Generals",
.radio_options			= "Opcions Ràdio",
.auto_night				= "Auto nit", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "DMR Rx AGC",
.speaker_click_suppress			= "Supr. sorolls",
.gps					= "GPS",
.end_only				= "Nomès fi",
.dmr_crc				= "Crc DMR",
.eco					= "Eco",
.safe_power_on				= "Inici segur", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "Auto apagat", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "APO amb RF", // MaxLen: 16 (with ':' + .yes or .no or .n_a)
.brightness_night				= "Llum nit", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF			= "Freq VHF",
.gps_acquiring			= "Buscant GPS",
.altitude				= "Alt",
.calibration            = "Calibració ràdio",
.freq_set_UHF                = "Adjust freq.",
.cal_frequency          = "Cal. Freq.",
.cal_pwr                = "Cal Potència",
.pwr_set                = "Ajust Potència",
.factory_reset          = "Reset de fàbrica",
.rx_tune				= "Ajust Rx",
.transmitTalkerAliasTS2	= "Enviar TA 2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Text",
.daytime_theme_day			= "Tema dia", // MaxLen: 16
.daytime_theme_night			= "Tema nit", // MaxLen: 16
.theme_chooser				= "Elecció tema", // Maxlen: 16
.theme_options				= "Opcions Color",
.theme_fg_default			= "Text Predet", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Fons", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Decoracíó", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Text Inici", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "Front Inici", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Fons Inici", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Text notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Alerta notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Error notif", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Fons notif", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Menú Nom", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Menú N. fons", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Menú sel", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Menú resalt", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "Valor Opció", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Text Títol", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Text Títol fons", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "RSSI ind", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "RSSI barra S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Nom Canal", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Contacte", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Contacte Detall", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Nom Zona", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "RX freq", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "TX freq", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "CSS/SQL valors", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "TX compta", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polar", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Sat. spot", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "GPS n", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "GPS spot", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "BeiDou spot", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Vermell", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Verd", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Blau", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Volume", // MaxLen: 8
.distance_sort				= "Dist sort", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Show dist", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS options", // MaxLen 16
.aprs_smart				= "Smart", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "Canal", // MaxLen 16 (with ':' + .location)
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
#endif /* USER_INTERFACE_LANGUAGES_CATALAN_H_ */
