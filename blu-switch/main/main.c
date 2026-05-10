#include "main.h"

#define LOG_TAG "BLU_SWITCH"

/* ── Shared input frame written by button_task / stick_task ──────────────── */
static bb_input_s g_bb_input = {0};

/* ── Analog trigger press booleans (used by button_task) ─────────────────── */
#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG)
static bool is_left_trigger_pressed  = false;
#endif
#if defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
static bool is_right_trigger_pressed = false;
#endif

/* ── SHIFT button state (minimal-button mode) ───────────────────────────── */
#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
static bool      is_shift_pressed = false;
static gpio_num_t shift_gpio_num  = GPIO_NUM_NC;
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 * button_task  –  reads digital buttons → writes g_bb_input
 * ═══════════════════════════════════════════════════════════════════════════ */
static void button_task(void)
{
    blu_refresh_buttons();

#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
    is_shift_pressed = blu_get_button_state(shift_gpio_num);

    if (!is_shift_pressed) {
        /* Normal layer: A, B, L, R, START (+), SELECT (-) */
        g_bb_input.button_east  |= blu_buttons.button_A.value;
        g_bb_input.button_south |= blu_buttons.button_B.value;
        g_bb_input.trigger_l    |= blu_buttons.trigger_l.value;
        g_bb_input.trigger_r    |= blu_buttons.trigger_r.value;
        g_bb_input.button_plus  |= blu_buttons.special_start.value;
        g_bb_input.button_minus |= blu_buttons.special_select.value;
    } else {
        /* SHIFT layer: A→X, B→Y, L→ZL, R→ZR, START→HOME, SELECT→CAPTURE */
        g_bb_input.button_north   |= blu_buttons.button_A.value;
        g_bb_input.button_west    |= blu_buttons.button_B.value;
        g_bb_input.trigger_zl     |= blu_buttons.trigger_l.value;
        g_bb_input.trigger_zr     |= blu_buttons.trigger_r.value;
        g_bb_input.button_home    |= blu_buttons.special_start.value;
        g_bb_input.button_capture |= blu_buttons.special_select.value;
    }
#else
    g_bb_input.button_east  |= blu_buttons.button_A.value;
    g_bb_input.button_south |= blu_buttons.button_B.value;
    g_bb_input.button_west  |= blu_buttons.button_Y.value;
    g_bb_input.button_north |= blu_buttons.button_X.value;

    g_bb_input.dpad_right |= blu_buttons.dpad_right.value;
    g_bb_input.dpad_down  |= blu_buttons.dpad_down.value;
    g_bb_input.dpad_left  |= blu_buttons.dpad_left.value;
    g_bb_input.dpad_up    |= blu_buttons.dpad_up.value;

#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_BTN_L
    g_bb_input.trigger_l  |= blu_buttons.trigger_l.value;
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
    g_bb_input.trigger_l  |= is_left_trigger_pressed;
#endif
#else
    g_bb_input.trigger_l  |= blu_buttons.trigger_l.value;
#endif

#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_BTN_ZL
    g_bb_input.trigger_zl |= blu_buttons.trigger_zl.value;
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
    g_bb_input.trigger_zl |= is_left_trigger_pressed;
#endif
#else
    g_bb_input.trigger_zl |= blu_buttons.trigger_zl.value;
#endif

#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_BTN_R
    g_bb_input.trigger_r  |= blu_buttons.trigger_r.value;
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
    g_bb_input.trigger_r  |= is_right_trigger_pressed;
#endif
#else
    g_bb_input.trigger_r  |= blu_buttons.trigger_r.value;
#endif

#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_BTN_ZR
    g_bb_input.trigger_zr |= blu_buttons.trigger_zr.value;
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
    g_bb_input.trigger_zr |= is_right_trigger_pressed;
#endif
#else
    g_bb_input.trigger_zr |= blu_buttons.trigger_zr.value;
#endif

    g_bb_input.button_plus    |= blu_buttons.special_start.value;
    g_bb_input.button_minus   |= blu_buttons.special_select.value;
    g_bb_input.button_home    |= blu_buttons.special_home.value;
    g_bb_input.button_capture |= blu_buttons.special_capture.value;

    g_bb_input.button_stick_left  |= blu_buttons.button_stick_left.value;
    g_bb_input.button_stick_right |= blu_buttons.button_stick_right.value;
#endif /* CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES */
}

/* ═══════════════════════════════════════════════════════════════════════════
 * stick_task  –  reads ADC sticks / analog triggers → writes g_bb_input
 * ═══════════════════════════════════════════════════════════════════════════ */
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
static blu_analog_stick_data_t *analog_stick_data;
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
static blu_btn_stick_data_t *button_stick_data;
#endif

static void stick_task(void)
{
    /* ── Left stick ─────────────────────────────────────────────────────── */
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG)
    analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_LEFT);
    g_bb_input.lx = GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis);
    g_bb_input.ly = GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis);
#elif defined(CONFIG_BLUCONTROL_LEFT_STICK_N64)
    g_bb_input.lx = GET_JOYSTICK_X_AXIS(n64_left_joystick_data.x_axis.value);
    g_bb_input.ly = GET_JOYSTICK_Y_AXIS(n64_left_joystick_data.y_axis.value);
#elif defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
    button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
    if (!is_shift_pressed) {
        g_bb_input.lx = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
        g_bb_input.ly = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
    } else {
        /* SHIFT: left stick buttons → DPAD */
        g_bb_input.dpad_right |= (button_stick_data->x_axis > 0);
        g_bb_input.dpad_left  |= (button_stick_data->x_axis < 0);
        g_bb_input.dpad_up    |= (button_stick_data->y_axis > 0);
        g_bb_input.dpad_down  |= (button_stick_data->y_axis < 0);
        g_bb_input.lx = GET_JOYSTICK_X_AXIS(0);
        g_bb_input.ly = GET_JOYSTICK_Y_AXIS(0);
    }
#else
    g_bb_input.lx = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
    g_bb_input.ly = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
#endif
#else
    g_bb_input.lx = GET_JOYSTICK_X_AXIS(0);
    g_bb_input.ly = GET_JOYSTICK_Y_AXIS(0);
#endif

    /* ── Right stick ────────────────────────────────────────────────────── */
#if defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_RIGHT);
    g_bb_input.rx = GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis);
    g_bb_input.ry = GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis);
#elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
    g_bb_input.rx = GET_JOYSTICK_X_AXIS(n64_right_joystick_data.x_axis.value);
    g_bb_input.ry = GET_JOYSTICK_Y_AXIS(n64_right_joystick_data.y_axis.value);
#elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
    button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_RIGHT);
    g_bb_input.rx = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
    g_bb_input.ry = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
#else
    g_bb_input.rx = GET_JOYSTICK_X_AXIS(0);
    g_bb_input.ry = GET_JOYSTICK_Y_AXIS(0);
#endif

    /* ── Analog triggers ────────────────────────────────────────────────── */
#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG)
    {
        uint16_t raw = GET_ANALOG_TRIGGER_AXIS(
            blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_LEFT));
        g_bb_input.lt = raw;
        is_left_trigger_pressed = CHECK_IF_TRIGGER_IS_PRESSED(raw, LEFT_TRIGGER_ACT);
    }
#endif
#if defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    {
        uint16_t raw = GET_ANALOG_TRIGGER_AXIS(
            blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_RIGHT));
        g_bb_input.rt = raw;
        is_right_trigger_pressed = CHECK_IF_TRIGGER_IS_PRESSED(raw, RIGHT_TRIGGER_ACT);
    }
#endif
}

/* ═══════════════════════════════════════════════════════════════════════════
 * haptic_cb  –  drives rumble GPIOs from Baseband haptic events
 * ═══════════════════════════════════════════════════════════════════════════ */
static void haptic_cb(uint8_t left_intensity, uint8_t right_intensity)
{
#if LEFT_RUMBLE_GPIO >= 0
    gpio_set_level(LEFT_RUMBLE_GPIO,  left_intensity  > 0 ? 1 : 0);
#endif
#if RIGHT_RUMBLE_GPIO >= 0
    gpio_set_level(RIGHT_RUMBLE_GPIO, right_intensity > 0 ? 1 : 0);
#endif
}

/* ═══════════════════════════════════════════════════════════════════════════
 * input_poll_task  –  pinned to Core 1.
 *   Clears the frame, reads all inputs, pushes to Baseband, repeats at 1 ms.
 *
 *   Watchdog: if BT is not connected within CONNECT_TIMEOUT_MS after boot,
 *   restart — the Baseband has no recovery path after a failed reconnect.
 *
 *   Clear-pairing combo: hold HOME + CAPTURE + MINUS for 3 s while
 *   disconnected → wipes NVS host MAC and restarts into pairing mode.
 * ═══════════════════════════════════════════════════════════════════════════ */
#define CONNECT_TIMEOUT_MS   15000   /* restart if not paired in 15 s          */
#define CLEAR_HOLD_MS         3000   /* hold combo this long to clear pairing   */

static void input_poll_task(void *arg)
{
    (void)arg;

    uint32_t not_connected_ms = 0;   /* tracks consecutive disconnected ms     */
    uint32_t clear_combo_ms   = 0;   /* tracks how long the clear combo is held */

    for (;;) {
        memset(&g_bb_input, 0, sizeof(g_bb_input));
        g_bb_input.bat_lvl      = 4;
        g_bb_input.bat_charging = false;

        stick_task();
        button_task();

        baseband_send_input(&g_bb_input);

        if (baseband_is_connected()) {
            not_connected_ms = 0;
            clear_combo_ms   = 0;

#ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
            if (blu_energy_is_clock_running()) blu_energy_stop_clock();
#endif
        } else {
#ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
            if (!blu_energy_is_clock_running()) blu_energy_start_clock();
#endif
            not_connected_ms += 1;

            /* ── Clear-pairing combo: HOME + CAPTURE + MINUS ──────────── */
            if (g_bb_input.button_home && g_bb_input.button_capture && g_bb_input.button_minus) {
                clear_combo_ms += 1;
                if (clear_combo_ms >= CLEAR_HOLD_MS) {
                    ESP_LOGW(LOG_TAG, "Clear-pairing combo → wiping NVS and restarting");
                    nvs_handle_t h;
                    if (nvs_open("hsettings", NVS_READWRITE, &h) == ESP_OK) {
                        nvs_erase_key(h, "hoja_settings");
                        nvs_commit(h);
                        nvs_close(h);
                    }
                    esp_restart();
                }
            } else {
                clear_combo_ms = 0;
            }

            /* ── Connection watchdog ────────────────────────────────────── */
            if (not_connected_ms >= CONNECT_TIMEOUT_MS) {
                ESP_LOGW(LOG_TAG, "BT connect timeout → restarting");
                esp_restart();
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * app_main
 * ═══════════════════════════════════════════════════════════════════════════ */
void app_main(void)
{
    ESP_LOGI(LOG_TAG, "BLU-SWITCH starting (Baseband BT)");

    blu_energy_init();
    blu_energy_start_clock();
    blu_init_hardware();

    /* ── SHIFT button (minimal-button mode) ─────────────────────────────── */
#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
    {
        const char *shift_pin_str = SHIFT_BUTTON_PIN;
        if (strlen(shift_pin_str) > 0) {
            shift_gpio_num = (gpio_num_t)atoi(shift_pin_str);
            gpio_config_t io_conf = {
                .intr_type    = GPIO_INTR_DISABLE,
                .mode         = GPIO_MODE_INPUT,
                .pin_bit_mask = (1ULL << shift_gpio_num),
#ifdef CONFIG_BLUCONTROL_BUTTONS_PRESS_STATE_HIGH
                .pull_down_en = GPIO_PULLDOWN_ENABLE,
                .pull_up_en   = GPIO_PULLUP_DISABLE,
#else
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .pull_up_en   = GPIO_PULLUP_ENABLE,
#endif
            };
            gpio_config(&io_conf);
        }
    }
#endif

    /* ── Rumble GPIO setup ───────────────────────────────────────────────── */
#if LEFT_RUMBLE_GPIO >= 0
    gpio_set_direction(LEFT_RUMBLE_GPIO,  GPIO_MODE_OUTPUT);
    gpio_set_level(LEFT_RUMBLE_GPIO, 0);
#endif
#if RIGHT_RUMBLE_GPIO >= 0
    gpio_set_direction(RIGHT_RUMBLE_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(RIGHT_RUMBLE_GPIO, 0);
#endif

    /* ── BluControl mode button / LED ───────────────────────────────────── */
    blucontrol_mode_init(true);

    /* ── Baseband BT ─────────────────────────────────────────────────────── */
    baseband_init();
    baseband_set_haptic_standard_cb(haptic_cb);

    static const bb_live_config_s live_cfg = {
        .rgb_body    = {0, 0, 0},
        .rgb_buttons = {0, 0, 0},
        .rgb_gripl   = {0, 0, 0},
        .rgb_gripr   = {0, 0, 0},
        /* current_mac all-zero → use saved random MAC */
        .vendor_id   = 0x057E,
        .product_id  = 0x2009,
        .sub_id      = 0x00,
        .clear_pairing = false,
    };
    baseband_start(BB_BT_MODE, &live_cfg);

    /* ── Input polling task pinned to Core 0 ────────────────────────────────
     * Must match the core where _switch_bt_task_standard runs (Core 0).
     * ESP32 D-caches are NOT hardware-coherent between cores — if this task
     * ran on Core 1, writes to _switch_input_data would be invisible to the
     * BT task on Core 0 until the cache line was evicted.
     * ─────────────────────────────────────────────────────────────────────── */
    xTaskCreatePinnedToCore(
        input_poll_task,
        "input_poll",
        4096,
        NULL,
        5,
        NULL,
        0
    );

    ESP_LOGI(LOG_TAG, "Started! BT mode: %s",
             BB_BT_MODE == BB_MODE_SINPUT ? "SInput" : "Switch Pro");
}

