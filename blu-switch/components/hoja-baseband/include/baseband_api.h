#pragma once
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Bluetooth mode selection.
 */
typedef enum
{
    BB_MODE_SWITCH = 0, /**< Nintendo Switch Pro Controller (BT Classic) */
    BB_MODE_SINPUT = 6, /**< SInput PC gamepad (BT Classic)              */
} bb_mode_t;

/**
 * @brief Unified input data fed to the Baseband every polling cycle.
 *        Analog values are 12-bit unsigned (0 – 4095).
 *        Bit-fields are 1-bit booleans (active-high).
 */
typedef struct
{
    /* Analog sticks */
    uint16_t lx, ly;
    uint16_t rx, ry;
    /* Analog triggers */
    uint16_t lt, rt;
    /* D-pad */
    uint8_t dpad_up : 1;
    uint8_t dpad_down : 1;
    uint8_t dpad_left : 1;
    uint8_t dpad_right : 1;
    /* Face buttons – Nintendo layout */
    uint8_t button_south : 1; /**< B  */
    uint8_t button_east : 1;  /**< A  */
    uint8_t button_west : 1;  /**< Y  */
    uint8_t button_north : 1; /**< X  */
    /* Shoulder / trigger digital */
    uint8_t trigger_l : 1;  /**< L  */
    uint8_t trigger_zl : 1; /**< ZL */
    uint8_t trigger_r : 1;  /**< R  */
    uint8_t trigger_zr : 1; /**< ZR */
    /* Menu */
    uint8_t button_plus : 1;  /**< +  / Start  */
    uint8_t button_minus : 1; /**< -  / Select */
    uint8_t button_home : 1;
    uint8_t button_capture : 1;
    /* Stick clicks */
    uint8_t button_stick_left : 1;
    uint8_t button_stick_right : 1;
    /* Extra (paddle / GL / GR) */
    uint8_t trigger_gl : 1;
    uint8_t trigger_gr : 1;
    /* Power status */
    uint8_t bat_lvl; /**< 0-4 */
    bool bat_charging;
} bb_input_s;

/**
 * @brief Live configuration sent once at startup (colours, MAC, VID/PID, …).
 *        Set current_mac to all-zeros to reuse the saved random MAC.
 */
typedef struct
{
    uint8_t rgb_body[3];
    uint8_t rgb_buttons[3];
    uint8_t rgb_gripl[3];
    uint8_t rgb_gripr[3];
    uint8_t current_mac[6];
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t sub_id;
    bool clear_pairing; /**< true → erase paired host MAC and re-pair */
} bb_live_config_s;

/** Raw 8-byte HD-rumble data forwarded from the Switch host. */
typedef void (*bb_haptic_switch_cb_t)(const uint8_t *raw_8bytes);
/** Simplified ERM intensity (0-255) for left and right motors. */
typedef void (*bb_haptic_standard_cb_t)(uint8_t left, uint8_t right);

/* ── API ─────────────────────────────────────────────────────────────────── */

/** Load (or initialise) settings from NVS. Must be called before
 *  baseband_start(). Handles nvs_flash_init() internally. */
void baseband_init(void);

/** Start Bluetooth in the selected mode. */
void baseband_start(bb_mode_t mode, const bb_live_config_s *config);

/** Send one input frame. Call from your polling task after reading GPIO/ADC. */
void baseband_send_input(const bb_input_s *input);

/** Optional haptic callbacks – register before baseband_start(). */
void baseband_set_haptic_switch_cb(bb_haptic_switch_cb_t cb);
void baseband_set_haptic_standard_cb(bb_haptic_standard_cb_t cb);

/** Returns true while a BT host is connected. */
bool baseband_is_connected(void);
