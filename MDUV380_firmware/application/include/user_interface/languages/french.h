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
 * Translators: F1CXG, F1RMB
 *
 *
 * Rev: 4
 */
#ifndef USER_INTERFACE_LANGUAGES_FRENCH_H_
#define USER_INTERFACE_LANGUAGES_FRENCH_H_
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
const stringsTable_t frenchLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME				= "Français",
.menu					= "Menu",
.credits				= "Crédits",
.zone					= "Zone",
.rssi					= "RSSI",
.battery				= "Batterie",
.contacts				= "Contacts",
.last_heard				= "Derniers reçus",
.firmware_info				= "Info Firmware",
.options				= "Options",
.display_options			= "Affichage",
.sound_options				= "Audio", // MaxLen: 16
.channel_details			= "Détails canal",
.language				= "Langue",
.new_contact				= "Nouv. contact",
.dmr_contacts				= "Contacts DMR", // MaxLen: 16
.contact_details			= "Détails contact",
.hotspot_mode				= "Hotspot",
.built					= "Créé",
.zones					= "Zones",
.keypad					= "Clavier",
.ptt					= "PTT",
.locked					= "Verrouillé",
.press_sk2_plus_star			= "Pressez SK2 + *",
.to_unlock				= "pour déverr.",
.unlocked				= "Déverrouillé",
.power_off				= "Extinction...",
.error					= "ERREUR",
.rx_only				= "Rx Uniqmnt.",
.out_of_band				= "HORS BANDE",
.timeout				= "TIMEOUT",
.tg_entry				= "Entrez TG",
.pc_entry				= "Entrez PC",
.user_dmr_id				= "DMR ID Perso.",
.contact 				= "Contact",
.accept_call				= "Répondre à",
.private_call				= "Appel Privé",
.squelch				= "Squelch",
.quick_menu 				= "Menu Rapide",
.filter					= "Filtre",
.all_channels				= "Tous Canaux",
.gotoChannel				= "Aller",
.scan					= "Scan",
.channelToVfo				= "Canal --> VFO",
.vfoToChannel				= "VFO --> Canal",
.vfoToNewChannel			= "VFO --> Nv. Can.", // MaxLen: 16
.group					= "Groupe",
.private				= "Privé",
.all					= "Tous",
.type					= "Type",
.timeSlot				= "Timeslot",
.none					= "Aucun",
.contact_saved				= "Contact sauvé",
.duplicate				= "Dupliqué",
.tg					= "TG",
.pc					= "PC",
.ts					= "TS",
.mode					= "Mode",
.colour_code				= "Code Couleur",
.n_a					= "ND",
.bandwidth				= "BW", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "Pas",
.tot					= "TOT",
.off					= "Off",
.zone_skip				= "Saut Zone",
.all_skip				= "Saut Compl.",
.yes					= "Oui",
.no					= "Non",
.tg_list				= "Lst TG",
.on					= "On",
.timeout_beep				= "Son timeout",
.list_full				= "Liste pleine",
.dmr_cc_scan				= "Scan CC", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits				= "Lim. Bandes",
.beep_volume				= "Vol. bip",
.dmr_mic_gain				= "DMR mic",
.fm_mic_gain				= "FM mic", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "Appui long",
.key_repeat				= "Répét°",
.dmr_filter_timeout			= "Tps filtre",
.brightness				= "Rétro écl.",
.brightness_off				= "Écl. min",
.contrast				= "Contraste",
.screen_invert				= "Inversé",
.screen_normal				= "Normal",
.backlight_timeout			= "Timeout",
.scan_delay				= "Délai scan",
.yes___in_uppercase			= "OUI",
.no___in_uppercase			= "NON",
.DISMISS				= "CACHER",
.scan_mode				= "Mode scan",
.hold					= "Bloque",
.pause					= "Pause",
.list_empty				= "Liste Vide",
.delete_contact_qm			= "Efface contact ?",
.contact_deleted			= "Contact effacé",
.contact_used				= "Contact utilisé",
.in_tg_list				= "ds la liste TG",
.select_tx				= "Select° TX",
.edit_contact				= "Édite Contact",
.delete_contact				= "Efface Contact",
.group_call				= "Appel de Groupe",
.all_call				= "All Call",
.tone_scan				= "Scan tons",
.low_battery			        = "BATT. FAIBLE !!!",//// MaxLen: 16
.Auto					= "Auto", // MaxLen 16 (with .mode + ':')
.manual					= "Manuel",  // MaxLen 16 (with .mode + ':') }
.ptt_toggle				= "Bascule PTT", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling			= "Filtre PC", // MaxLen 16 (with ':' + .on ot .off)
.stop					= "Arrêt", // Maxlen 16 (with ':' + .scan_mode)
.one_line				= "1 ligne", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 lignes", // MaxLen 16 (with ':' + .contact)
.new_channel				= "Nouv. canal", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order				= "Ordre", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "Bip TX", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "Début", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "Les Deux", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold                          = "Seuil VOX", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail                               = "Queue VOX", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt				= "Prompt",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                                 = "Silence", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "Bip RX", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "Bip", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1			= "Voix", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1			= "Tx TA TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF				= "Squelch VHF",// Maxlen 16 (with : + XX%)
.squelch_220				= "Squelch 220",// Maxlen 16 (with : + XX%)
.squelch_UHF				= "Squelch UHF", // Maxlen 16 (with : + XX%)
.display_screen_invert 			= "Écran" , // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77",// Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "s/touches", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Commit Git",
.voice_prompt_level_2			= "Voix L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3			= "Voix L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "Filtre DMR",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter				= "Filtre TS", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "Contacts DTMF FM", // Maxlen: 16
.channel_power				= "Pce Canal", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "Maître",// Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey				= "Défini Quickkey", // MaxLen: 16
.dual_watch				= "Double Veille", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr					= "Puissance",
.user_power				= "Pce Perso.", // MaxLen: 16 (with ':' + 0..4100)
.temperature				= "Temperature", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "°C",
.seconds				= "secondes",
.radio_info				= "Infos radio",
.temperature_calibration		= "Étal. t°",
.pin_code				= "Code Pin",
.please_confirm				= "Confirmez", // MaxLen: 15
.vfo_freq_bind_mode			= "Freq. Liées",
.overwrite_qm				= "Écraser ?", //Maxlen: 14 chars
.eco_level				= "Niveau Eco",
.buttons				= "Boutons",
.leds					= "DELs",
.scan_dwell_time			= "Durée Scan",
.battery_calibration			= "Étal. Bat.",
.low					= "Bas",
.high					= "Haut",
.dmr_id					= "DMR ID",
.scan_on_boot				= "Scan On Boot",
.dtmf_entry				= "Entrez DTMF",
.name					= "Nom",
.carrier				= "Porteuse",
.zone_empty 				= "Zone vide", // Maxlen: 12 chars.
.time					= "Heure",
.uptime					= "En Funct. Depuis",
.hours					= "Heures",
.minutes				= "Minutes",
.satellite				= "Satellite",
.alarm_time				= "Heure Alarme",
.location				= "Emplacement",
.date					= "Date",
.timeZone				= "Fuseau",
.suspend				= "Veille",
.pass					= "Passe", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "ds",
.predicting				= "Prédiction",
.maximum				= "Max",
.satellite_short			= "Sat",
.local					= "Locale",
.UTC					= "UTC",
.symbols				= "NSEO", // symbols: N,S,E,W
.not_set				= "NON DÉFINI",
.general_options			= "Générales",
.radio_options				= "Radio",
.auto_night				= "Nuit auto", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "AGC Rx DMR",
.speaker_click_suppress			= "Suppr. Clic",
.gps					= "GPS",
.end_only				= "Fin Seul.",
.dmr_crc				= "Crc DMR",
.eco					= "Eco",
.safe_power_on				= "Sécu. Allum.", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "Arrêt Auto", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "APO avec RF", // MaxLen: 16 (with ':' + .yes or .no or .n_a)êt Auto" // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.brightness_night				= "Nite bright", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF				= "Freq VHF",
.gps_acquiring				= "Acquisition",
.altitude				= "Alt",
.calibration            		= "Étalonnage radio",
.freq_set_UHF                		= "Freq UHF",
.cal_frequency          		= "Freq",
.cal_pwr                		= "Niv. Puiss.",
.pwr_set                		= "Réglage",
.factory_reset          		= "RàZ Usine",
.rx_tune				= "Rx Tuning",
.transmitTalkerAliasTS2			= "Tx TA TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Texte",
.daytime_theme_day			= "Thème de jour", // MaxLen: 16
.daytime_theme_night			= "Thème de nuit", // MaxLen: 16
.theme_chooser				= "Sélect° du Thème", // Maxlen: 16
.theme_options				= "Thème",
.theme_fg_default			= "Texte défaut", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Fond", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Décoration", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Texte saisies", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "1er-plan boot", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Fond boot", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Texte notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Alarme notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Erreur notif", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Fond notif", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Nom menu", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Fond nom menu", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Item menu", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Sélect° menu", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "Valeur option", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Texte entête", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Fond entête", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "Barre RSSI", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "Barre RSSI S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "Nom canal", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Contact", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Info contact", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Nom zone", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "Fréq. RX", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "Fréq. TX", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "Valeurs CSS/SQL", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "Compteur TX", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polaire", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Point sat.", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "Numéro GPS", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "Point GPS", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "Point BeiDou", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Rouge", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Vert", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Bleu", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Volume", // MaxLen: 8
.distance_sort				= "Tri dist.", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Aff dist.", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS", // MaxLen 16
.aprs_smart				= "Smart", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "Canal", // MaxLen 16 (with ':' + .location)
.aprs_decay				= "Decay", // MaxLen 16 (with ':' + .on or .off)
.aprs_compress				= "Compress", // MaxLen 16 (with ':' + .on or .off)
.aprs_interval				= "Interval", // MaxLen 16 (with ':' + 0.2..60 + 'min')
.aprs_message_interval			= "Interval msg", // MaxLen 16 (with ':' + 3..30)
.aprs_slow_rate				= "Slow Rate", // MaxLen 16 (with ':' + 1..100 + 'min')
.aprs_fast_rate				= "Fast Rate", // MaxLen 16 (with ':' + 10..180 + 's')
.aprs_low_speed				= "Low Speed", // MaxLen 16 (with ':' + 2..30 + 'km/h')
.aprs_high_speed			= "Hi Speed", // MaxLen 16 (with ':' + 2..90 + 'km/h')
.aprs_turn_angle			= "T. Angle", // MaxLen 16 (with ':' + 5..90 + '°')
.aprs_turn_slope			= "T. Slope", // MaxLen 16 (with ':' + 1..255 + '°/v')
.aprs_turn_time				= "T. Time", // MaxLen 16 (with ':' + 5..180 + 's')
.auto_lock				= "Verr. auto", // MaxLen 16 (with ':' + .off or 0.5..15 (.5 step) + 'min')
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
#endif /* USER_INTERFACE_LANGUAGES_FRENCH_H_ */
