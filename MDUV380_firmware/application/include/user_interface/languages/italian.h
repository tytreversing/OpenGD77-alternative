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
 * Translators: IU4LEG, IZ2EIB
 *
 *
 * Rev: 2024.04.18 IZ2EIB & IU4LEG
 */
#ifndef USER_INTERFACE_LANGUAGES_ITALIAN_H_
#define USER_INTERFACE_LANGUAGES_ITALIAN_H_
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
const stringsTable_t italianLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 			   = "Italiano", // MaxLen: 16
.menu					   = "Menu", // MaxLen: 16
.credits				   = "Crediti", // MaxLen: 16
.zone					   = "Zone", // MaxLen: 16
.rssi					   = "RSSI", // MaxLen: 16
.battery				   = "Batteria", // MaxLen: 16
.contacts				   = "Contatti", // MaxLen: 16
.last_heard				   = "Ultimi Ricevuti", // MaxLen: 16
.firmware_info			   = "Informazioni", // MaxLen: 16
.options				   = "Opzioni", // MaxLen: 16
.display_options		   = "Opz. Display", // MaxLen: 16
.sound_options			   = "Opzioni Audio", // MaxLen: 16
.channel_details		   = "Dettagli canale", // MaxLen: 16
.language				   = "Lingua", // MaxLen: 16
.new_contact			   = "Nuovo Contatto", // MaxLen: 16
.dmr_contacts				= "Contatti DMR", // MaxLen: 16
.contact_details		   = "Det.gli Contatto", // MaxLen: 16
.hotspot_mode			   = "Hotspot", // MaxLen: 16
.built					   = "Versione", // MaxLen: 16
.zones					   = "Zone", // MaxLen: 16
.keypad					   = "Tastiera", // MaxLen: 12 (with .ptt)
.ptt					   = "PTT", // MaxLen: 12 (with .keypad)
.locked					   = "Bloccato", // MaxLen: 15
.press_sk2_plus_star	   = "Premi SK2 + *", // MaxLen: 16
.to_unlock				   = "Per sbloccare", // MaxLen: 16
.unlocked				   = "Sbloccato", // MaxLen: 15
.power_off				   = "Spegnimento...", // MaxLen: 16
.error					   = "ERRORE", // MaxLen: 8
.rx_only				   = "Solo Rx", // MaxLen: 14
.out_of_band			   = "FUORI BANDA", // MaxLen: 14
.timeout				   = "TIMEOUT", // MaxLen: 8
.tg_entry				   = "Inserisci TG", // MaxLen: 15
.pc_entry				   = "Inserisci CP", // MaxLen: 15
.user_dmr_id			   = "ID Utente DMR", // MaxLen: 15
.contact 				   = "Contatto",// MaxLen: 15
.accept_call			   = "Rispondere a", // MaxLen: 16
.private_call			   = "Chiamata Priv.", // MaxLen: 16
.squelch				   = "Squelch", // MaxLen: 8
.quick_menu 			   = "Menu rapido", // MaxLen: 16
.filter					   = "Filtro", // MaxLen: 7 (with ':' + settings: .none, "CC", "CC,TS", "CC,TS,TG")
.all_channels			   = "Tutti i canali", // MaxLen: 16
.gotoChannel			   = "Vai a", // MaxLen: 11 (" 1024")
.scan					   = "Scansione", // MaxLen: 16
.channelToVfo			   = "Canale --> VFO", // MaxLen: 16
.vfoToChannel			   = "VFO --> Canale", // MaxLen: 16
.vfoToNewChannel		   = "VFO --> Nuovo Ch", // MaxLen: 16
.group					   = "Gruppo", // MaxLen: 16 (with .type)
.private				   = "Privato", // MaxLen: 16 (with .type)
.all					   = "Tutti", // MaxLen: 16 (with .type)
.type					   = "Tipo", // MaxLen: 16 (with .type)
.timeSlot				   = "Timeslot", // MaxLen: 16 (plus ':' and  .none, '1' or '2')
.none					   = "Nessuno", // MaxLen: 16 (with .timeSlot, "Rx CTCSS:" and ""Tx CTCSS:", .filter and .mode )
.contact_saved			   = "Salvato", // MaxLen: 16
.duplicate				   = "Duplicato", // MaxLen: 16
.tg						   = "TG", // MaxLen: 8
.pc						   = "CP", // MaxLen: 8
.ts						   = "TS", // MaxLen: 8
.mode					   = "Modo", // MaxLen: 12
.colour_code			   = "Codice Colore", // MaxLen: 16 (with ':' * .n_a)
.n_a					   = "N/A", // MaxLen: 16 (with ':' * .colour_code)
.bandwidth				   = "Banda", // MaxLen: 16 (with ':' + .n_a, "25kHz" or "12.5kHz")
.stepFreq				   = "Passo", // MaxLen: 7 (with ':' + xx.xxkHz fitted)
.tot					   = "TOT", // MaxLen: 16 (with ':' + .off or 15..3825)
.off					   = "Off", // MaxLen: 16 (with ':' + .timeout_beep, .band_limits)
.zone_skip				   = "Salta Zona", // MaxLen: 16 (with ':' + .yes or .no)
.all_skip				   = "Salta Tutti",// MaxLen: 16 (with ':' + .yes or .no)
.yes					   = "Sì", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.no						   = "No", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.tg_list				   = "TG Lst", // MaxLen: 16 (with ':' and codeplug group name)
.on						   = "On", // MaxLen: 16 (with ':' + .band_limits)
.timeout_beep			   = "Bip Timeout", // MaxLen: 16 (with ':' + .n_a or 5..20 + 's')
.list_full				= "Elenco pieno",
.dmr_cc_scan			   = "Scan. CC", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits			   = "Limiti Banda", // MaxLen: 16 (with ':' + .on or .off)
.beep_volume			   = "Volume Bip", // MaxLen: 16 (with ':' + -24..6 + 'dB')
.dmr_mic_gain			   = "DMR mic", // MaxLen: 16 (with ':' + -33..12 + 'dB')
.fm_mic_gain			   = "FM mic", // MaxLen: 16 (with ':' + 0..31)
.key_long				   = "Tasto prol.", // MaxLen: 11 (with ':' + x.xs fitted)
.key_repeat				   = "Tasto repl.", // MaxLen: 11 (with ':' + x.xs fitted)
.dmr_filter_timeout		   = "Tempo Filtro", // MaxLen: 16 (with ':' + 1..90 + 's')
.brightness				   = "Luminosità", // MaxLen: 16 (with ':' + 0..100 + '%')
.brightness_off			   = "Min. Lum.", // MaxLen: 16 (with ':' + 0..100 + '%')
.contrast				   = "Contrasto", // MaxLen: 16 (with ':' + 12..30)
.screen_invert			   = "Invert.", // MaxLen: 16
.screen_normal			   = "Normale", // MaxLen: 16
.backlight_timeout		   = "Timeout", // MaxLen: 16 (with ':' + .no to 30s)
.scan_delay				   = "Ritardo Scan", // MaxLen: 16 (with ':' + 1..30 + 's')
.yes___in_uppercase		   = "SÌ", // MaxLen: 8 (choice above green/red buttons)
.no___in_uppercase		   = "NO", // MaxLen: 8 (choice above green/red buttons)
.DISMISS				   = "RIFIUTA", // MaxLen: 8 (choice above green/red buttons)
.scan_mode				   = "Modo Scan", // MaxLen: 16 (with ':' + .hold, .pause or .stop)
.hold					   = "Blocco", // MaxLen: 16 (with ':' + .scan_mode)
.pause					   = "Pausa", // MaxLen: 16 (with ':' + .scan_mode)
.list_empty				   = "Lista Vuota", // MaxLen: 16
.delete_contact_qm		   = "Canc. Contatto?", // MaxLen: 16
.contact_deleted		   = "Cancellato", // MaxLen: 16
.contact_used			   = "Contatto usato", // MaxLen: 16
.in_tg_list			   = "nella lista TG", // MaxLen: 16
.select_tx				   = "Seleziona TX", // MaxLen: 16
.edit_contact			   = "Modif. Contatto", // MaxLen: 16
.delete_contact			   = "Canc. Contatto", // MaxLen: 16
.group_call				   = "Chiama Gruppo", // MaxLen: 16
.all_call				   = "Chiama Tutti", // MaxLen: 16
.tone_scan				   = "Scan Toni",//// MaxLen: 16
.low_battery			   = "BATTERIA SCARICA",//// MaxLen: 16
.Auto					   = "Automatico", // MaxLen 16 (with .mode + ':')
.manual					   = "Manuale",  // MaxLen 16 (with .mode + ':')
.ptt_toggle				   = "Auto-PTT", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling	   = "Gest. CP", // MaxLen 16 (with ':' + .on ot .off)
.stop					   = "Fine", // Maxlen 16 (with ':' + .scan_mode)
.one_line				   = "1 linea", // MaxLen 16 (with ':' + .contact)
.two_lines				   = "2 linee", // MaxLen 16 (with ':' + .contact)
.new_channel			   = "Nuovo Ch", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order			   = "Prio.", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				   = "DMR bip", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					   = "Inizio", // MaxLen 16 (with ':' + .dmr_beep)
.both					   = "Ambedue", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold             = "Soglia VOX", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail                  = "Coda VOX", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt			   = "Guida",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                    = "Silenziosa", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX bip", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					   = "Bip", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1	   = "Voce", // Maxlen 16 (with : + audio_prompt)
.transmitTalkerAliasTS1	   = "TA Tx TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF			   = "VHF Squelch",// Maxlen 16 (with : + XX%)
.squelch_220			   = "220 Squelch",// Maxlen 16 (with : + XX%)
.squelch_UHF			   = "UHF Squelch", // Maxlen 16 (with : + XX%)
.display_screen_invert = "Sfondo" , // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				   = "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No tasti", // Maxlen 16 (with : + audio_prompt)
.gitCommit				   = "Git commit",
.voice_prompt_level_2	   = "Voce L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3	   = "Voce L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				   = "Filtro DMR",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter			   = "Filtro TS", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "Contatti DTMF FM", // Maxlen: 16
.channel_power				= "W Ch", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "di base",// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey				= "Imposta tasti", // MaxLen: 16
.dual_watch				= "Doppio ascolto", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr					= "Potenza",
.user_power				= "Volt PA",
.temperature				= "Temperatura", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "°C",
.seconds				= "secondi",
.radio_info				= "Info radio",
.temperature_calibration		= "Cal Temp",
.pin_code				= "Codice PIN",
.please_confirm				= "Conferma, prego", // MaxLen: 15
.vfo_freq_bind_mode			= "Abbina Freq.",
.overwrite_qm				= "Sovrascrivi ?", //Maxlen: 14 chars
.eco_level				= "Grado consumi",
.buttons				= "Bottoni",
.leds					= "LED",
.scan_dwell_time			= "Ciclo SCAN",
.battery_calibration			= "Cal BATT.",
.low					= "Bassa",
.high					= "Alta",
.dmr_id					= "2°idDMR",
.scan_on_boot				= "Scan su ON",
.dtmf_entry				= "Ins. DTMF",
.name					= "Nome",
.carrier				= "Portante",
.zone_empty 				= "Zona vuota", // Maxlen: 12 chars.
.time					= "Orario",
.uptime					= "Tempo Attività",
.hours					= "Ore",
.minutes				= "Minuti",
.satellite				= "Satellite",
.alarm_time				= "Allarme",
.location				= "Posizione",
.date					= "Data",
.timeZone				= "Fuso orario",
.suspend				= "Sospensione",
.pass					= "Pass", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "in",
.predicting				= "Previsione",
.maximum				= "Max",
.satellite_short		= "Sat",
.local					= "Locale",
.UTC					= "UTC",
.symbols				= "NSEO", // symbols: N,S,E,W
.not_set				= "NON IMPOSTATO",
.general_options		= "Opzioni generali",
.radio_options			= "Opzioni radio",
.auto_night				= "Notte/Giorno", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "AGC Rx DMR",
.speaker_click_suppress			= "Limita click",
.gps					= "GPS",
.end_only				= "Solo in fine",
.dmr_crc				= "CRC DMR",
.eco					= "ECO Ch",
.safe_power_on				= "Sicura su ON", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "Auto Pwr-Off", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "Reset APO RF", // MaxLen: 16 (with ':' + .yes or .no or .n_a)
.brightness_night				= "Luce notte", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF			= "Freq. VHF",
.gps_acquiring			= "Acquisizione",
.altitude				= "Alt.",
.calibration            = "Cal. Radio",
.freq_set_UHF                = "Freq. UHF",
.cal_frequency          = "Cal. Frequenza",
.cal_pwr                = "Cal. Potenza",
.pwr_set                = "Regola Potenza",
.factory_reset          = "Cal. di fabbrica",
.rx_tune				= "Sintonia Rx",
.transmitTalkerAliasTS2	= "TA Tx TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Testo",
.daytime_theme_day			= "Tema diurno", // MaxLen: 16
.daytime_theme_night			= "Tema notturno", // MaxLen: 16
.theme_chooser				= "Selezione Tema", // Maxlen: 16
.theme_options				= "Opzioni Tema",
.theme_fg_default			= "Testo predef.", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Sfondo", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Decorazione", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Ins. Testo", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "Avvio iniziale", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Sfondo iniziale", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Notif. testo", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Notif. avviso", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Notif. errore", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Sfondo Notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Nome Menu", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Sfondo N. Menu", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Elemento Menu", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Evidenzia Menu", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "Valore opzione", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Titolo", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Sfondo Titolo", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "Barra RSSI", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "Barra RSSI S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Nome Canale", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Contatto", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Info Contatto", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Nome Zona", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "Freq. Rx", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "Freq. Tx", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "Valore CSS/SQL", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "Contatore Tx", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polare", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Punto Sat.", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "Numero GPS", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "Punto GPS", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "Punto BeiDou", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Rosso", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Verde", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Blu", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Volume", // MaxLen: 8
.distance_sort				= "Ordina dist.", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Mostra dist.", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "Opzioni APRS", // MaxLen 16
.aprs_smart				= "Furbo", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "Canale", // MaxLen 16 (with ':' + .location)
.aprs_decay				= "Degrado", // MaxLen 16 (with ':' + .on or .off)
.aprs_compress				= "Compressione", // MaxLen 16 (with ':' + .on or .off)
.aprs_interval				= "Intervallo", // MaxLen 16 (with ':' + 0.2..60 + 'min')
.aprs_message_interval			= "Interv. Msg", // MaxLen 16 (with ':' + 3..30)
.aprs_slow_rate				= "C. lenta", // MaxLen 16 (with ':' + 1..100 + 'min')
.aprs_fast_rate				= "C. rapida", // MaxLen 16 (with ':' + 10..180 + 's')
.aprs_low_speed				= "V. bassa", // MaxLen 16 (with ':' + 2..30 + 'km/h')
.aprs_high_speed			= "V. alta", // MaxLen 16 (with ':' + 2..90 + 'km/h')
.aprs_turn_angle			= "G. Angolo", // MaxLen 16 (with ':' + 5..90 + '°')
.aprs_turn_slope			= "G. Pendio", // MaxLen 16 (with ':' + 1..255 + '°/v')
.aprs_turn_time				= "G. Tempo", // MaxLen 16 (with ':' + 5..180 + 's')
.auto_lock				= "Auto lock", // MaxLen 16 (with ':' + .off or 0.5..15 (.5 step) + 'min')
.trackball				= "Trackball", // MaxLen 16 (with ':' + .on or .off)
.dmr_force_dmo				= "Forza DMO", // MaxLen 16 (with ':' + .n_a or .on or .off)
};
/********************************************************************
 *
 * VERY IMPORTANT.
 * This file should not be saved with UTF-8 encoding
 * Use Notepad++ on Windows with ANSI encoding
 * or emacs on Linux with windows-1252-unix encoding
 *
 ********************************************************************/
#endif /* USER_INTERFACE_LANGUAGES_ITALIAN_H_ */
