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
 * Translators: PU4RON
 *
 *
 * Rev: 2
 */
#ifndef USER_INTERFACE_LANGUAGES_PORTUGUESE_BRAZIL_H_
#define USER_INTERFACE_LANGUAGES_PORTUGUESE_BRAZIL_H_
/********************************************************************
 *
 * MUITO IMPORTANTE.
 * Este arquivo não deve ser salvo com a codificação UTF-8
 * Use o Notepad ++ no Windows com codificação ANSI
 * ou emacs no Linux com codificação windows-1252-unix
 *
 ********************************************************************/
#if defined(PLATFORM_GD77) || defined(PLATFORM_GD77S) || defined(PLATFORM_DM1801) || defined(PLATFORM_DM1801A) || defined(PLATFORM_RD5R)
__attribute__((section(".upper_text")))
#endif
const stringsTable_t portuguesBrazilLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 			= "Portugues BR",
.menu					= "Menu",
.credits				= "Créditos",
.zone					= "Zona",
.rssi					= "RSSI",
.battery				= "Bateria",
.contacts				= "Contatos",
.last_heard				= "Último escutado",
.firmware_info			= "Firmware",
.options				= "Opções",
.display_options		= "Opções display",
.sound_options			= "Opções de áudio",
.channel_details		= "Detalhes Canal",
.language				= "Idioma",
.new_contact			= "Novo contato",
.dmr_contacts			= "DMR Contatos", // MaxLen: 16
.contact_details		= "Detalhes contato",
.hotspot_mode			= "Hotspot",
.built					= "Built",
.zones					= "Zonas",
.keypad					= "Teclado",
.ptt 					= "PTT",
.locked 				= "Bloqueado",
.press_sk2_plus_star	= "Aperte SK2 + *",
.to_unlock				= "Desbloquear",
.unlocked				= "Desbloqueado",
.power_off				= "Desligando...",
.error					= "ERRO",
.rx_only				= "Apenas Rx",
.out_of_band			= "FORA DE BANDA",
.timeout				= "TIMEOUT",
.tg_entry				= "Inserir TG",
.pc_entry				= "Inserir CP",
.user_dmr_id			= "DMR ID usuário",
.contact 				= "Contato",
.accept_call			= "Aceitar chamada?",
.private_call			= "Chamada privada",
.squelch				= "Squelch",
.quick_menu 			= "Menu rápido",
.filter					= "Filtro",
.all_channels			= "Lista canais",
.gotoChannel			= "Ir para",
.scan					= "Escanear",
.channelToVfo			= "Canal -> VFO",
.vfoToChannel			= "VFO -> Canal",
.vfoToNewChannel		= "VFO -> NovoCanal",
.group					= "Grupo",
.private				= "Privado",
.all					= "Todos",
.type					= "Tipo",
.timeSlot				= "TimeSlot",
.none					= "Nenhum",
.contact_saved			= "Contato gravado",
.duplicate				= "Duplicado",
.tg						= "TG",
.pc						= "CP",
.ts						= "TS",
.mode					= "Modo",
.colour_code			= "Código cor",
.n_a					= "N/A",
.bandwidth				= "BW", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "Step",
.tot					= "TOT",
.off					= "Off",
.zone_skip				= "Saltar Zona",
.all_skip				= "Saltar Todos",
.yes					= "Sim",
.no						= "Não",
.tg_list				= "TG Lst",
.on						= "On",
.timeout_beep			= "TOT Sons",
.list_full				= "List full",
.dmr_cc_scan			= "CC Scan", // MaxLen: 12 (with ':' + settings: .on or .off)
.band_limits			= "Limite banda",
.beep_volume			= "Volume bip",
.dmr_mic_gain			= "Gan MIC DMR",
.fm_mic_gain			= "Gan MIC FM",
.key_long				= "Key long",
.key_repeat				= "Key rpt",
.dmr_filter_timeout		= "Filtro Tempo",
.brightness				= "Brilho",
.brightness_off			= "Min brilho",
.contrast				= "Contraste",
.screen_invert			= "Invertido",
.screen_normal			= "Normal",
.backlight_timeout		= "Timeout",
.scan_delay				= "Tempo Scan",
.yes___in_uppercase		= "SIM",
.no___in_uppercase		= "NÃO",
.DISMISS				= "RECUSAR",
.scan_mode				= "Modo Scan",
.hold					= "Reter",
.pause					= "Pausa",
.list_empty				= "Lista vazia",
.delete_contact_qm		= "Apagar contato?",
.contact_deleted		= "Contato apagado",
.contact_used			= "Contato usado",
.in_tg_list			= "na lista TG",
.select_tx				= "Selecione TX",
.edit_contact			= "Editar contato",
.delete_contact			= "Apagar contato",
.group_call				= "Chamada grupo",
.all_call				= "Todas chamadas",
.tone_scan				= "Tone scan",
.low_battery			= "BATERIA FRACA!",
.Auto					= "Auto",
.manual					= "Manual",
.ptt_toggle				= "PTT Fixo",
.private_call_handling	= "Filtro CP",
.stop					= "Parar",
.one_line				= "1 linha",
.two_lines				= "2 linhas",
.new_channel			= "Novo canal",
.priority_order			= "Ordem",
.dmr_beep				= "DMR beep",
.start					= "Inicio",
.both					= "Ambos",
.vox_threshold          = "VOX Gan.",
.vox_tail               = "VOX Ret.",
.audio_prompt			= "Prompt", // Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                 = "Silencioso", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "RX beep", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "Beep", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1	= "Voz", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1	= "TA Tx TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF			= "VHF Squelch", // Maxlen 16 (with : + XX%)
.squelch_220			= "220 Squelch", // Maxlen 16 (with : + XX%)
.squelch_UHF			= "UHF Squelch", // Maxlen 16 (with : + XX%)
.display_screen_invert = "Cor display" , // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77", // Do not translate
.talkaround 				= "Talkaround", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "No Keys", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2	= "Voz L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3	= "Voz L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "Filtro DMR", // MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "Talker",
.dmr_ts_filter			= "Filtro TS", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list	    = "FM DTMF Contatos", // Maxlen: 16
.channel_power		    = "Pot. Canal", // Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master			= "Master", // Displayed if per-channel power is not enabled  the .channel_power
.set_quickkey			= "Tecla rápida", // MaxLen: 16
.dual_watch				= "Duplo canal", // MaxLen: 16
.info					= "Info", // MaxLen: 16 (with ':' + .off or.ts or .pwr or .both)
.pwr					= "Ligado",
.user_power				= "Potencia usuario",
.temperature			= "Temperatura", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "°C",
.seconds				= "segundos",
.radio_info				= "Info Radio",
.temperature_calibration = "Cal Temperatura",
.pin_code				= "senha",
.please_confirm			= "Confirmar", // MaxLen: 15
.vfo_freq_bind_mode		= "Freq. Bind",
.overwrite_qm			= "Substituir", // Maxlen: 14 chars
.eco_level				= "Nivel Eco",
.buttons				= "Botões",
.leds					= "LEDs",
.scan_dwell_time		= "Scan dwell",
.battery_calibration	= "Calib Bateria",
.low					= "Baixa",
.high					= "Alta",
.dmr_id					= "DMR ID",
.scan_on_boot			= "Inicio scan",
.dtmf_entry				= "DTMF entry",
.name					= "Nome",
.carrier				= "Carrier",
.zone_empty 				= "Zona vazia", // Maxlen: 12 chars.
.time					= "Hora",
.uptime					= "Uptime",
.hours					= "Hora",
.minutes				= "Minutos",
.satellite				= "Satelite",
.alarm_time				= "Hora Alarm",
.location				= "Location",
.date					= "Data",
.timeZone				= "Timezone",
.suspend				= "Suspend",
.pass					= "Senha", // For satellite screen
.elevation				= "El",
.azimuth				= "Az",
.inHHMMSS				= "in",
.predicting				= "Predicting",
.maximum				= "Max",
.satellite_short		= "Sat",
.local					= "Local",
.UTC					= "UTC",
.symbols				= "NSEW", // symbols: N,S,E,W
.not_set				= "NOT SET",
.general_options		= "Opções Gerais",
.radio_options			= "Opções do Radio",
.auto_night				= "Auto noite", // MaxLen: 16 (with .on or .off)
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
.gps_acquiring			= "Recebendo",
.altitude				= "Alt",
.calibration            = "Calibração",
.freq_set_UHF                = "Freq UHF",
.cal_frequency          = "Freq",
.cal_pwr                = "Nivel Potencia",
.pwr_set                = "Ajuste Potencia",
.factory_reset          = "Reset Fabrica",
.rx_tune				= "Sintonia RX",
.transmitTalkerAliasTS2	= "TA Tx TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "Text",
.daytime_theme_day			= "Tema Dia", // MaxLen: 16
.daytime_theme_night			= "Tema Noite", // MaxLen: 16
.theme_chooser				= "Seletor Tema", // Maxlen: 16
.theme_options				= "Opções Tema",
.theme_fg_default			= "Texto Padrão", // MaxLen: 16 (+ colour rect)
.theme_bg				= "Fundo Tela", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "Decorativo", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "Texto entrada", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "Foregr. boot", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "Boot Fundo", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "Text notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "Aviso notif.", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "Notif Erro", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "Notif.Fundo", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "Menu nome", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "Menu Nome Fund", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "Menu item", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "Menu destaque", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "Opções valor", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "Text Cabeç", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "Text cabeç fund", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "RSSI bar", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "RSSI bar S9+", // MaxLen: 16 (+colour rect)
.theme_fg_channel_name			= "Nome Canal", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "Contato", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "Info Contato", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "Nome Zona", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "Freq RX", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "Freq TX", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "CSS/SQL valor", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "TX contador", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "Polar", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "Sat. spot", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "GPS number", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "GPS spot", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "BeiDou spot", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "Verm", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "Verd", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "Azul", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "Volume", // MaxLen: 8
.distance_sort				= "Dist sort", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "Mostra dist", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS opções", // MaxLen 16
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
.trackball                             = "Trackball", // MaxLen 16 (with ':' + .on or .off)
.dmr_force_dmo				= "Force DMO", // MaxLen 16 (with ':' + .n_a or .on or .off)
};
/*********************************************************************
 *
 * MUITO IMPORTANTE.
 * Este arquivo não deve ser salvo com a codificação UTF-8
 * Use o Notepad ++ no Windows com codificação ANSI
 * ou emacs no Linux com codificação windows-1252-unix
 *
 ********************************************************************/
#endif /* USER_INTERFACE_LANGUAGES_PORTUGUESE_H_ */
