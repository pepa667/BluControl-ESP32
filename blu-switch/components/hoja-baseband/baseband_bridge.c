/*
 * baseband_bridge.c
 *
 * Two jobs in one file:
 *
 *  1. Provides every global / weak-symbol that HOJA-ESP32-Baseband's BT
 *     cores expect to find at link time:
 *       global_loaded_settings, global_live_data, get_timestamp_us/ms,
 *       app_set_*, app_save_host_mac, btm_hcif_mode_change_cb, …
 *
 *  2. Implements the public baseband_* API declared in baseband_api.h,
 *     which is the only interface that the application's main.c uses.
 *
 * The Baseband's own headers live in PRIV_INCLUDE_DIRS and are therefore
 * invisible to the rest of the project – no name conflicts with HOJA-LIB-ESP32.
 */

/* ── Baseband-private headers (via PRIV_INCLUDE_DIRS) ───────────────────── */
#include "hoja_includes.h"
#include "hoja.h"
#include "core_bt_switch.h"
#include "core_bt_sinput.h"
#include "imu_tool.h"

/* ── Our public API ─────────────────────────────────────────────────────── */
#include "baseband_api.h"

/* ── IDF headers ─────────────────────────────────────────────────────────── */
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_random.h"

static const char *TAG = "bb_bridge";

#ifndef HOJA_SETTINGS_NAMESPACE
#define HOJA_SETTINGS_NAMESPACE "hsettings"
#endif

/* ── Globals required by Baseband BT cores ─────────────────────────────── */
hoja_settings_s global_loaded_settings = {0};
hoja_live_s global_live_data = {0};

/* ── Bridge-internal state ──────────────────────────────────────────────── */
static volatile uint64_t _report_us = 8000;
static volatile uint64_t _report_us_default = 8000;
static volatile bool _sniff = true;
static volatile bool _bt_connected = false;

static bb_mode_t _current_mode = BB_MODE_SWITCH;
static bb_haptic_switch_cb_t _haptic_sw_cb = NULL;
static bb_haptic_standard_cb_t _haptic_std_cb = NULL;

/* ── Timestamp helpers ───────────────────────────────────────────────────── */
uint64_t get_timestamp_us(void) { return (uint64_t)esp_timer_get_time(); }
uint64_t get_timestamp_ms(void) { return get_timestamp_us() / 1000; }

/* ── Report-rate pacing ─────────────────────────────────────────────────── */
void app_set_report_timer(uint64_t us)
{
    _report_us_default = us;
    if (!_sniff)
        _report_us = us;
}
uint64_t app_get_report_timer(void) { return _report_us; }

/* ── HCI sniff-mode callback (BT power saving) ──────────────────────────── */
#define HCI_MODE_ACTIVE 0x00
#define HCI_MODE_SNIFF 0x02
void btm_hcif_mode_change_cb(bool ok, uint16_t handle, uint8_t mode, uint16_t interval)
{
    (void)handle;
    if (!ok)
        return;
    if (mode == HCI_MODE_ACTIVE)
    {
        _sniff = false;
        _report_us = _report_us_default;
    }
    else if (mode == HCI_MODE_SNIFF)
    {
        _sniff = true;
        _report_us = (uint64_t)interval * 1000;
    }
}

/* ── Connected / power status ───────────────────────────────────────────── */
void app_set_connected_status(uint8_t status)
{
    _bt_connected = (status > 0);
    ESP_LOGI(TAG, "BT connected status → %d", status);
}

void app_set_power_setting(i2c_power_code_t code)
{
    ESP_LOGI(TAG, "Power code %d → restarting", (int)code);
    esp_restart();
}

/* ── MAC / NVS helpers ──────────────────────────────────────────────────── */
bool app_compare_mac(uint8_t *a, uint8_t *b)
{
    for (int i = 0; i < 6; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

static void _settings_save(void)
{
    nvs_handle_t h;
    if (nvs_open(HOJA_SETTINGS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK)
        return;
    nvs_set_blob(h, "hoja_settings", &global_loaded_settings, sizeof(hoja_settings_s));
    nvs_commit(h);
    nvs_close(h);
}

void app_save_host_mac(input_mode_t mode, uint8_t *addr)
{
    uint8_t *dst = NULL;
    if (mode == INPUT_MODE_SWPRO)
        dst = global_loaded_settings.paired_host_switch_mac;
    else if (mode == INPUT_MODE_SINPUT)
        dst = global_loaded_settings.paired_host_sinput_mac;
    if (!dst)
        return;
    memcpy(dst, addr, 6);
    _settings_save();
}

/* ── Haptics ─────────────────────────────────────────────────────────────── */
void app_set_switch_haptic(uint8_t *data)
{
    if (_haptic_sw_cb)
        _haptic_sw_cb(data);
}

void app_set_standard_haptic(uint8_t l, uint8_t r)
{
    if (_haptic_std_cb)
        _haptic_std_cb(l, r);
}

void app_set_sinput_haptic(uint8_t *data, uint8_t len)
{
    /* SInput type-2: byte[0]=left amp, byte[1]=right amp */
    if (_haptic_std_cb && len >= 2)
        _haptic_std_cb(data[0], data[1]);
}

/* ── Internal helpers ────────────────────────────────────────────────────── */
static void _gen_random_mac(uint8_t *mac)
{
    for (int i = 0; i < 6; i++)
        mac[i] = (uint8_t)(esp_random() & 0xFF);
    mac[0] = (mac[0] & 0xFE) | 0x02; /* locally-administered, unicast */
}

/* ══════════════════════════════════════════════════════════════════════════
 * Public API implementation
 * ══════════════════════════════════════════════════════════════════════════ */

void baseband_init(void)
{
    /* Initialise NVS flash */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Try to load saved settings */
    nvs_handle_t h;
    ret = nvs_open(HOJA_SETTINGS_NAMESPACE, NVS_READWRITE, &h);
    if (ret == ESP_OK)
    {
        size_t sz = 0;
        nvs_get_blob(h, "hoja_settings", NULL, &sz);
        if (sz == sizeof(hoja_settings_s))
        {
            nvs_get_blob(h, "hoja_settings", &global_loaded_settings, &sz);
            if (global_loaded_settings.magic == HOJA_MAGIC_NUM)
            {
                nvs_close(h);
                ESP_LOGI(TAG, "Settings loaded OK");
                return;
            }
        }
        nvs_close(h);
    }

    /* First-boot defaults */
    memset(&global_loaded_settings, 0, sizeof(hoja_settings_s));
    global_loaded_settings.magic = HOJA_MAGIC_NUM;
    _gen_random_mac(global_loaded_settings.device_mac_switch);
    _gen_random_mac(global_loaded_settings.device_mac_sinput);
    _settings_save();
    ESP_LOGI(TAG, "Settings initialised with defaults");
}

void baseband_start(bb_mode_t mode, const bb_live_config_s *cfg)
{
    _current_mode = mode;

    if (cfg)
    {
        memcpy(global_live_data.rgb_body, cfg->rgb_body, 3);
        memcpy(global_live_data.rgb_buttons, cfg->rgb_buttons, 3);
        memcpy(global_live_data.rgb_gripl, cfg->rgb_gripl, 3);
        memcpy(global_live_data.rgb_gripr, cfg->rgb_gripr, 3);
        memcpy(global_live_data.current_mac, cfg->current_mac, 6);
        global_live_data.vendor_id = cfg->vendor_id;
        global_live_data.product_id = cfg->product_id;
        global_live_data.sub_id = cfg->sub_id;

        if (cfg->clear_pairing)
        {
            if (mode == BB_MODE_SWITCH)
            {
                memset(global_loaded_settings.paired_host_switch_mac, 0, 6);
                _gen_random_mac(global_loaded_settings.device_mac_switch);
            }
            else
            {
                memset(global_loaded_settings.paired_host_sinput_mac, 0, 6);
                _gen_random_mac(global_loaded_settings.device_mac_sinput);
            }
            _settings_save();
        }
    }

    /* Battery default (full, not charging) */
    global_live_data.bat_status.bat_lvl = 4;
    global_live_data.bat_status.charging = 0;

    ESP_LOGI(TAG, "Starting BT mode %d", (int)mode);
    switch (mode)
    {
    case BB_MODE_SWITCH:
        core_bt_switch_start();
        break;
    case BB_MODE_SINPUT:
        core_bt_sinput_start();
        break;
    default:
        break;
    }
}

void baseband_send_input(const bb_input_s *in)
{
    static i2cinput_input_s d = {0};

    d.lx = in->lx;
    d.ly = in->ly;
    d.rx = in->rx;
    d.ry = in->ry;
    d.lt = in->lt;
    d.rt = in->rt;

    d.dpad_up = in->dpad_up;
    d.dpad_down = in->dpad_down;
    d.dpad_left = in->dpad_left;
    d.dpad_right = in->dpad_right;

    d.button_south = in->button_south;
    d.button_east = in->button_east;
    d.button_west = in->button_west;
    d.button_north = in->button_north;

    d.trigger_l = in->trigger_l;
    d.trigger_zl = in->trigger_zl;
    d.trigger_r = in->trigger_r;
    d.trigger_zr = in->trigger_zr;

    d.button_plus = in->button_plus;
    d.button_minus = in->button_minus;

    d.button_stick_left = in->button_stick_left;
    d.button_stick_right = in->button_stick_right;

    d.button_home = in->button_home;
    d.button_capture = in->button_capture;

    d.trigger_gl = in->trigger_gl;
    d.trigger_gr = in->trigger_gr;

    bat_status_u bat = {
        .bat_lvl = in->bat_lvl,
        .charging = in->bat_charging ? 1 : 0,
        .connection = 0,
    };
    d.power_stat = bat.val;

    if (_current_mode == BB_MODE_SWITCH)
        switch_bt_sendinput(&d);
    else
        sinput_bt_sendinput(&d);
}

void baseband_set_haptic_switch_cb(bb_haptic_switch_cb_t cb) { _haptic_sw_cb = cb; }
void baseband_set_haptic_standard_cb(bb_haptic_standard_cb_t cb) { _haptic_std_cb = cb; }
bool baseband_is_connected(void) { return _bt_connected; }
