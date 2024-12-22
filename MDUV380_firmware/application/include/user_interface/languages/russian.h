#ifndef USER_INTERFACE_LANGUAGES_RUSSIAN_H_
#define USER_INTERFACE_LANGUAGES_RUSSIAN_H_
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
const stringsTable_t russianLanguage =
{
.magicNumber                            = { LANGUAGE_TAG_MAGIC_NUMBER, LANGUAGE_TAG_VERSION },
.LANGUAGE_NAME 				= "�������", // MaxLen: 16
.menu					= "����", // MaxLen: 16
.credits				= "������", // MaxLen: 16
.zone					= "����", // MaxLen: 16
.rssi					= "RSSI", // MaxLen: 16
.battery				= "�����������", // MaxLen: 16
.contacts				= "��������", // MaxLen: 16
.last_heard				= "����. �����", // MaxLen: 16
.firmware_info				= "� ��������", // MaxLen: 16
.options				= "���������", // MaxLen: 16
.display_options			= "�����", // MaxLen: 16
.sound_options				= "����", // MaxLen: 16
.channel_details			= "�����. ������", // MaxLen: 16
.language				= "����� �����", // MaxLen: 16
.new_contact				= "����� �������", // MaxLen: 16
.dmr_contacts				= "�������� DMR", // MaxLen: 16
.contact_details			= "�����. ��������", // MaxLen: 16
.hotspot_mode				= "�������", // MaxLen: 16
.built					= "������", // MaxLen: 16
.zones					= "����", // MaxLen: 16
.keypad					= "����������", // MaxLen: 12 (with .ptt)
.ptt					= "PTT", // MaxLen: 12 (with .keypad, .mode)
.locked					= "����������", // MaxLen: 15
.press_sk2_plus_star			= "������ SK2 � *", // MaxLen: 16
.to_unlock				= "��� �������.", // MaxLen: 16
.unlocked				= "��������������", // MaxLen: 15
.power_off				= "����������...", // MaxLen: 16
.error					= "������", // MaxLen: 8
.rx_only				= "������ �����", // MaxLen: 14
.out_of_band				= "��� ���������", // MaxLen: 14
.timeout				= "�������", // MaxLen: 8
.tg_entry				= "����� ������", // MaxLen: 15
.pc_entry				= "ID ������������", // MaxLen: 15
.user_dmr_id				= "DMR ID", // MaxLen: 15
.contact 				= "�������", // MaxLen: 15
.accept_call				= "������� �����", // MaxLen: 16
.private_call				= "��������� �����", // MaxLen: 16
.squelch				= "�������.", // MaxLen: 8
.quick_menu 				= "������� ����", // MaxLen: 16
.filter					= "������", // MaxLen: 7 (with ':' + settings: .none, "CC", "CC,TS", "CC,TS,TG")
.all_channels				= "��� ������", // MaxLen: 16
.gotoChannel				= "� ������",  // MaxLen: 11 (" 1024")
.scan					= "������������", // MaxLen: 16
.channelToVfo				= "����� � VFO", // MaxLen: 16
.vfoToChannel				= "VFO � �����", // MaxLen: 16
.vfoToNewChannel			= "VFO � ���. �����", // MaxLen: 16
.group					= "������", // MaxLen: 16 (with .type)
.private				= "������", // MaxLen: 16 (with .type)
.all					= "���", // MaxLen: 16 (with .type)
.type					= "���", // MaxLen: 16 (with .type)
.timeSlot				= "��������", // MaxLen: 16 (plus ':' and  .none, '1' or '2')
.none					= "���", // MaxLen: 16 (with .timeSlot, "Rx CTCSS:" and ""Tx CTCSS:", .filter/.mode/.dmr_beep)
.contact_saved				= "���������", // MaxLen: 16
.duplicate				= "�����������", // MaxLen: 16
.tg					= "TG",  // MaxLen: 8
.pc					= "������", // MaxLen: 8
.ts					= "TS", // MaxLen: 8
.mode					= "�����",  // MaxLen: 12
.colour_code				= "��������", // MaxLen: 16 (with ':' * .n_a)
.n_a					= "���",// MaxLen: 16 (with ':' * .colour_code)
.bandwidth				= "������", // MaxLen: 16 (with : + .n_a, "25kHz" or "12.5kHz")
.stepFreq				= "���", // MaxLen: 7 (with ':' + xx.xxkHz fitted)
.tot					= "������ Tx", // MaxLen: 16 (with ':' + .off or 15..3825)
.off					= "����", // MaxLen: 16 (with ':' + .timeout_beep, .band_limits, aprs_decay)
.zone_skip				= "������� � ����", // MaxLen: 16 (with ':' + .yes or .no)
.all_skip				= "������� ������", // MaxLen: 16 (with ':' + .yes or .no)
.yes					= "��", // MaxLen: 16 (with ':' + .zone_skip, .all_skip)
.no					= "���", // MaxLen: 16 (with ':' + .zone_skip, .all_skip, .aprs_decay)
.tg_list				= "������ �����", // MaxLen: 16 (with ':' and codeplug group name)
.on					= "���.", // MaxLen: 16 (with ':' + .band_limits)
.timeout_beep				= "���� ��������", // MaxLen: 16 (with ':' + .n_a or 5..20 + 's')
.list_full				= "������ �����",
.dmr_cc_scan				= "���� �-����", // MaxLen: 12 (with ':' + settings: .on or .off).
.band_limits				= "���������", // MaxLen: 16 (with ':' + .on or .off)
.beep_volume				= "�����. ����.", // MaxLen: 16 (with ':' + -24..6 + 'dB')
.dmr_mic_gain				= "����. DMR", // MaxLen: 16 (with ':' + -33..12 + 'dB')
.fm_mic_gain				= "����. FM", // MaxLen: 16 (with ':' + 0..31)
.key_long				= "����. ���.", // MaxLen: 11 (with ':' + x.xs fitted)
.key_repeat				= "����. ����.", // MaxLen: 11 (with ':' + x.xs fitted)
.dmr_filter_timeout			= "������ DMR", // MaxLen: 16 (with ':' + 1..90 + 's')
.brightness				= "�������", // MaxLen: 16 (with ':' + 0..100 + '%')
.brightness_off				= "���. �������", // MaxLen: 16 (with ':' + 0..100 + '%')
.contrast				= "��������", // MaxLen: 16 (with ':' + 12..30)
.screen_invert				= "������.", // MaxLen: 16
.screen_normal				= "����.", // MaxLen: 16
.backlight_timeout			= "����� �����.", // MaxLen: 16 (with ':' + .no to 30s)
.scan_delay				= "����� ����.", // MaxLen: 16 (with ':' + 1..30 + 's')
.yes___in_uppercase			= "��", // MaxLen: 8 (choice above green/red buttons)
.no___in_uppercase			= "���", // MaxLen: 8 (choice above green/red buttons)
.DISMISS				= "�����", // MaxLen: 8 (choice above green/red buttons)
.scan_mode				= "����� ����.", // MaxLen: 16 (with ':' + .hold, .pause or .stop)
.hold					= "�����.", // MaxLen: 16 (with ':' + .scan_mode)
.pause					= "�����", // MaxLen: 16 (with ':' + .scan_mode)
.list_empty				= "������ ����", // MaxLen: 16
.delete_contact_qm			= "������� �������?", // MaxLen: 16
.contact_deleted			= "������� ������", // MaxLen: 16
.contact_used				= "������������", // MaxLen: 16
.in_tg_list				= "� ������ �����", // MaxLen: 16
.select_tx				= "������� Tx", // MaxLen: 16
.edit_contact				= "���. �������", // MaxLen: 16
.delete_contact				= "����. �������", // MaxLen: 16
.group_call				= "��������� �����", // MaxLen: 16
.all_call				= "������� ����", // MaxLen: 16
.tone_scan				= "���� �������", // MaxLen: 16
.low_battery				= "��������� !!!", // MaxLen: 16
.Auto					= "����", // MaxLen 16 (with .mode + ':', .mode)
.manual					= "����.",  // MaxLen 16 (with .mode + ':', .mode)
.ptt_toggle				= "����. PTT", // MaxLen 16 (with ':' + .on or .off)
.private_call_handling			= "����. ������", // MaxLen 16 (with ':' + .on or .off)
.stop					= "����", // Maxlen 16 (with ':' + .scan_mode/.dmr_beep)
.one_line				= "1 ������", // MaxLen 16 (with ':' + .contact)
.two_lines				= "2 ������", // MaxLen 16 (with ':' + .contact)
.new_channel				= "���. �����", // MaxLen: 16, leave room for a space and four channel digits after
.priority_order				= "�������", // MaxLen 16 (with ':' + 'Cc/DB/TA')
.dmr_beep				= "������ DMR", // MaxLen 16 (with ':' + .star/.stop/.both/.none)
.start					= "� ���.", // MaxLen 16 (with ':' + .dmr_beep)
.both					= "���", // MaxLen 16 (with ':' + .dmr_beep)
.vox_threshold                          = "������. VOX", // MaxLen 16 (with ':' + .off or 1..30)
.vox_tail                               = "�������� VOX", // MaxLen 16 (with ':' + .n_a or '0.0s')
.audio_prompt				= "�����",// Maxlen 16 (with ':' + .silent, .beep or .voice_prompt_level_1)
.silent                                 = "����", // Maxlen 16 (with : + audio_prompt)
.rx_beep				= "������ RX", // MaxLen 16 (with ':' + .carrier/.talker/.both/.none)
.beep					= "������", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_1			= "�����", // Maxlen 16 (with : + audio_prompt, satellite "mode")
.transmitTalkerAliasTS1			= "����� TS1", // Maxlen 16 (with : + .on or .off)
.squelch_VHF				= "�������. VHF",// Maxlen 16 (with : + XX%)
.squelch_220				= "�������. 220",// Maxlen 16 (with : + XX%)
.squelch_UHF				= "�������. UHF", // Maxlen 16 (with : + XX%)
.display_screen_invert 			= "�����", // Maxlen 16 (with : + .screen_normal or .screen_invert)
.openGD77 				= "OpenGD77 RUS",// Do not translate
.talkaround 				= "����. �����", // Maxlen 16 (with ':' + .on , .off or .n_a)
.APRS 					= "APRS", // Maxlen 16 (with : + .transmitTalkerAliasTS1 or transmitTalkerAliasTS2)
.no_keys 				= "��� ����.", // Maxlen 16 (with : + audio_prompt)
.gitCommit				= "Git commit",
.voice_prompt_level_2			= "����� L2", // Maxlen 16 (with : + audio_prompt)
.voice_prompt_level_3			= "����� L3", // Maxlen 16 (with : + audio_prompt)
.dmr_filter				= "������ DMR",// MaxLen: 12 (with ':' + settings: "TG" or "Ct" or "TGL")
.talker					= "����������",
.dmr_ts_filter				= "������ TS", // MaxLen: 12 (with ':' + settings: .on or .off)
.dtmf_contact_list			= "�������� DTMF", // Maxlen: 16
.channel_power				= "��������", //Displayed as "Ch Power:" + .from_master or "Ch Power:"+ power text e.g. "Power:500mW" . Max total length 16
.from_master				= "���.",// Displayed if per-channel power is not enabled  the .channel_power, and also with .location in APRS settings.
.set_quickkey				= "�����. ������.", // MaxLen: 16
.dual_watch				= "������� Rx", // MaxLen: 16
.info					= "����������", // MaxLen: 16 (with ':' + .off or .ts or .pwr or .both)
.pwr					= "����.",
.user_power				= "���� ����.",
.temperature				= "�����������", // MaxLen: 16 (with ':' + .celcius or .fahrenheit)
.celcius				= "�C",
.seconds				= "���.",
.radio_info				= "� �����",
.temperature_calibration		= "���. ����.",
.pin_code				= "���-���",
.please_confirm				= "�����������", // MaxLen: 15
.vfo_freq_bind_mode			= "����. Tx � Rx",
.overwrite_qm				= "����������?", //Maxlen: 14 chars
.eco_level				= "�����������",
.buttons				= "������",
.leds					= "���������",
.scan_dwell_time			= "����� ����.",
.battery_calibration			= "���. ���.",
.low					= "����.",
.high					= "�����.",
.dmr_id					= "DMR ID",
.scan_on_boot				= "��������",
.dtmf_entry				= "���� DTMF",
.name					= "���",
.carrier				= "�������",
.zone_empty 				= "���� �����", // Maxlen: 12 chars.
.time					= "�����",
.uptime					= "������",
.hours					= "�",
.minutes				= "���",
.satellite				= "��������",
.alarm_time				= "����� ����.",
.location				= "����������",
.date					= "����",
.timeZone				= "���. ����",
.suspend				= "�����",
.pass					= "������", // For satellite screen
.elevation				= "��.",
.azimuth				= "��.",
.inHHMMSS				= "�����",
.predicting				= "����������",
.maximum				= "����",
.satellite_short		= "�����.",
.local					= "�����.",
.UTC					= "UTC",
.symbols				= "NSEW", // symbols: N,S,E,W
.not_set				= "�� ������",
.general_options		= "�����",
.radio_options			= "����������",
.auto_night				= "���� ���.", // MaxLen: 16 (with .on or .off)
.dmr_rx_agc				= "��� DMR",
.speaker_click_suppress			= "������. �����.",
.gps					= "GPS",
.end_only				= "� �����",
.dmr_crc				= "DMR CRC",
.eco					= "�����������",
.safe_power_on				= "�����. ���.", // MaxLen: 16 (with ':' + .on or .off)
.auto_power_off				= "���� ����.", // MaxLen: 16 (with ':' + 30/60/90/120/180 or .no)
.apo_with_rf				= "����� �� Rx", // MaxLen: 16 (with ':' + .yes or .no or .n_a)
.brightness_night				= "����. ���.", // MaxLen: 16 (with : + 0..100 + %)
.freq_set_VHF			= "������� VHF",
.gps_acquiring			= "�����",
.altitude				= "���.",
.calibration            = "����������",
.freq_set_UHF           = "������� UHF",
.cal_frequency          = "�������",
.cal_pwr                = "��������",
.pwr_set                = "����.",
.factory_reset          = "�����",
.rx_tune				= "��������� Rx",
.transmitTalkerAliasTS2	= "����� TS2", // Maxlen 16 (with : + .ta_text, 'APRS' , .both or .off)
.ta_text				= "�����",
.daytime_theme_day			= "�����. ����", // MaxLen: 16
.daytime_theme_night			= "����. ����", // MaxLen: 16
.theme_chooser				= "����� �����", // Maxlen: 16
.theme_options				= "����",
.theme_fg_default			= "����� �� �����.", // MaxLen: 16 (+ colour rect)
.theme_bg				= "���", // MaxLen: 16 (+ colour rect)
.theme_fg_decoration			= "�����.", // MaxLen: 16 (+ colour rect)
.theme_fg_text_input			= "���� ������", // MaxLen: 16 (+ colour rect)
.theme_fg_splashscreen			= "���. ���� ����.", // MaxLen: 16 (+ colour rect)
.theme_bg_splashscreen			= "��� ��� ����.", // MaxLen: 16 (+ colour rect)
.theme_fg_notification			= "����� ����.", // MaxLen: 16 (+ colour rect)
.theme_fg_warning_notification		= "��������", // MaxLen: 16 (+ colour rect)
.theme_fg_error_notification		= "������", // MaxLen: 16 (+ colour rect)
.theme_bg_notification                  = "��� ����.", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_name			= "����. ����", // MaxLen: 16 (+ colour rect)
.theme_bg_menu_name			= "��� ����. ����", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item			= "����� ����", // MaxLen: 16 (+ colour rect)
.theme_fg_menu_item_selected		= "������ ����", // MaxLen: 16 (+ colour rect)
.theme_fg_options_value			= "����. �����.", // MaxLen: 16 (+ colour rect)
.theme_fg_header_text			= "���������", // MaxLen: 16 (+ colour rect)
.theme_bg_header_text			= "��� �����.", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar			= "RSSI", // MaxLen: 16 (+ colour rect)
.theme_fg_rssi_bar_s9p			= "RSSI S9+", // Maxlen: 16 (+colour rect)
.theme_fg_channel_name			= "�����", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact		= "�������", // MaxLen: 16 (+ colour rect)
.theme_fg_channel_contact_info		= "� ��������", // MaxLen: 16 (+ colour rect)
.theme_fg_zone_name			= "����", // MaxLen: 16 (+ colour rect)
.theme_fg_rx_freq			= "������� Rx", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_freq			= "������� Tx", // MaxLen: 16 (+ colour rect)
.theme_fg_css_sql_values		= "����. CSS/SQL", // MaxLen: 16 (+ colour rect)
.theme_fg_tx_counter			= "������ Tx", // MaxLen: 16 (+ colour rect)
.theme_fg_polar_drawing			= "���������", // MaxLen: 16 (+ colour rect)
.theme_fg_satellite_colour		= "�������", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_number			= "����� GPS", // MaxLen: 16 (+ colour rect)
.theme_fg_gps_colour			= "������� GPS", // MaxLen: 16 (+ colour rect)
.theme_fg_bd_colour			= "������� BeiDou", // MaxLen: 16 (+ colour rect)
.theme_colour_picker_red		= "�������", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_green		= "�������", // MaxLen 16 (with ':' + 3 digits value)
.theme_colour_picker_blue		= "�����", // MaxLen 16 (with ':' + 3 digits value)
.volume					= "�����.", // MaxLen: 8
.distance_sort				= "�� �����.", // MaxLen 16 (with ':' + .on or .off)
.show_distance				= "����������", // MaxLen 16 (with ':' + .on or .off)
.aprs_options				= "APRS", // MaxLen 16
.aprs_smart				= "���.", // MaxLen 16 (with ':' + .mode)
.aprs_channel				= "�����", // MaxLen 16 (with ':' + .location)
.aprs_decay				= "����. ������.", // MaxLen 16 (with ':' + .on or .off)
.aprs_compress				= "������", // MaxLen 16 (with ':' + .on or .off)
.aprs_interval				= "��������", // MaxLen 16 (with ':' + 0.2..60 + 'min')
.aprs_message_interval			= "������. �����.", // MaxLen 16 (with ':' + 3..30)
.aprs_slow_rate				= "���. �������", // MaxLen 16 (with ':' + 1..100 + 'min')
.aprs_fast_rate				= "���. �������", // MaxLen 16 (with ':' + 10..180 + 's')
.aprs_low_speed				= "���. ��������", // MaxLen 16 (with ':' + 2..30 + 'km/h')
.aprs_high_speed			= "����. ��������", // MaxLen 16 (with ':' + 2..90 + 'km/h')
.aprs_turn_angle			= "����", // MaxLen 16 (with ':' + 5..90 + '???')
.aprs_turn_slope			= "�����", // MaxLen 16 (with ':' + 1..255 + '???/v')
.aprs_turn_time				= "�����", // MaxLen 16 (with ':' + 5..180 + 's')
.auto_lock				= "��������.", // MaxLen 16 (with ':' + .off or 0.5..15 (.5 step) + 'min')
.trackball				= "�������", // MaxLen 16 (with ':' + .on or .off)
.dmr_force_dmo				= "������. DMO", // MaxLen 16 (with ':' + .n_a or .on or .off)
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
