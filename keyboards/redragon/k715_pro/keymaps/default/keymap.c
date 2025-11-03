/* Copyright 2024 temp4gh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "k715_pro.h"

#include "i2c_master.h"

enum layer_names
{
    _BASE,
    _FnLay,
};


#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE] =   {   ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [_FnLay] =  {   ENCODER_CCW_CW(RM_VALD, RM_VALU)  },
};
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
    KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,  KC_F6,   KC_F7,   KC_F8,   KC_F9,      KC_F10,  KC_F11,  KC_F12,            KC_MUTE,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,   KC_7,    KC_8,    KC_9,    KC_0,       KC_MINS, KC_EQL,  KC_BSPC, KC_INS,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,       KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN,    KC_QUOT,          KC_ENT,  KC_PGUP,
    KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   KC_N,    KC_M,    KC_COMM, KC_DOT,     KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                   KC_RALT, MO(_FnLay), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [_FnLay] = LAYOUT(
    EE_CLR,           KC_BRID, KC_BRIU, A(KC_TAB),G(KC_H),G(KC_D), G(KC_S), KC_MPRV, KC_MPLY, KC_MNXT,   KC_MUTE, KC_VOLD, KC_VOLU,          XXXXXXX,
    DB_TOGG, BT_CHN1, BT_CHN2, BT_CHN3, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, RM_NEXT,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_HOME, KC_END,  XXXXXXX, RM_PREV,
    KC_NUM , DB_ADC , XXXXXXX, DB_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_SCRL,   KC_PSCR,          XXXXXXX, RM_VALU,
    XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_PAUS, XXXXXXX, RM_HUEU, RM_VALD,
    XXXXXXX, XXXXXXX, XXXXXXX,                            RM_TOGG,                   XXXXXXX, _______,   XXXXXXX, RM_SPDD, RM_HUED, RM_SPDU
    )
};

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable=true;
    // debug_matrix=true;
    //debug_keyboard=true;
    //debug_mouse=true;
  }

  bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
      case DB_ADC:
        if (record->event.pressed) {
            //read_ADC_pins(1);
            chThdSleepMilliseconds(10);
            // uprintf("I2C frequency: %d\n", I2C1_CLOCK_SPEED);
            chThdSleepMilliseconds(10);
            uprintf("[%08lu] Starting I2C scan...\n", timer_read32());
            chThdSleepMilliseconds(10);
            for (uint8_t query_dev_addr = 0x50; query_dev_addr <= 0x57; query_dev_addr++) {
                uprintf("[%08lu] Pinging I2C address 0x%02X...\n", timer_read32(), query_dev_addr);
                chThdSleepMilliseconds(10);
                i2c_status_t result = i2c_ping_address(query_dev_addr << 1, 1000);
                if (result == I2C_STATUS_SUCCESS) {
                    uprintf("[%08lu] Device found!\n", timer_read32());
                } else {
                    uprintf("[%08lu] Error: %d\n", timer_read32(), result);  // -1 is error, -2 is timeout
                }
                chThdSleepMilliseconds(10);
                uint8_t data;
                uprintf("[%08lu] Reading byte at I2C addr 0x%02X, reg addr 0x00...\n", timer_read32(), query_dev_addr);
                chThdSleepMilliseconds(10);
                result = i2c_read_register(query_dev_addr << 1, 0x00, &data, 1, 1000);
                if (result == I2C_STATUS_SUCCESS) {
                    uprintf("[%08lu]   byte: 0x%02X\n", timer_read32(), data);
                } else {
                    uprintf("[%08lu]   Error: %d\n", timer_read32(), result);
                }
                uprintf("[%08lu] Reading byte at I2C add 0x%02X, reg addr 0x0000...\n", timer_read32(), query_dev_addr);
                chThdSleepMilliseconds(10);
                result = i2c_read_register16(query_dev_addr << 1, 0x00, &data, 1, 1000);
                if (result == I2C_STATUS_SUCCESS) {
                    uprintf("[%08lu]   byte: 0x%02X\n", timer_read32(), data);
                } else {
                    uprintf("[%08lu]   Error: %d\n", timer_read32(), result);
                }
            }
            uprintf("[%08lu] I2C scan complete.\n", timer_read32());
        } else {
          // Do something else when release
        }
        return false; // Skip all further processing of this key
      default:
        return true; // Process all other keycodes normally
    }
  }

  void suspend_power_down_user(void) {
    // code will run multiple times while keyboard is suspended
    uprintf("[%08lu] Going to sleep\n", timer_read32());
}

void suspend_wakeup_init_user(void) {
    // code will run on keyboard wakeup
    uprintf("[%08lu] Waking up\n", timer_read32());
}

bool shutdown_user(bool jump_to_bootloader) {
    if (jump_to_bootloader) {
        // red for bootloader
        rgb_matrix_set_color_all(RGB_RED);
        uprintf("[%08lu] Jumping to bootloader\n", timer_read32());
    } else {
        // off for soft reset
        rgb_matrix_set_color_all(RGB_OFF);
        uprintf("[%08lu] Soft reset\n", timer_read32());
    }
    // force flushing -- otherwise will never happen
    rgb_matrix_update_pwm_buffers();
    // false to not process kb level
    return false;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (get_highest_layer(layer_state) > 0) {
        uint8_t layer = get_highest_layer(layer_state);

        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint8_t index = g_led_config.matrix_co[row][col];

                if (index >= led_min && index < led_max && index != NO_LED &&
                keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS) {
                    rgb_matrix_set_color(index, RGB_GREEN);
                }
            }
        }
    }
    return false;   // don't continue running the keyboard-level callback
}
