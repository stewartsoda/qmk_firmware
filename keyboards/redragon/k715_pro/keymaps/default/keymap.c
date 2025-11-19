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
    _BASE_Win,
    _FnLay_Win,
    _BASE_Mac,
    _FnLay_Mac
};


#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE_Win] =   {   ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [_FnLay_Win] =  {   ENCODER_CCW_CW(RM_VALD, RM_VALU)  },
    [_BASE_Mac] =   {   ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [_FnLay_Mac] =  {   ENCODER_CCW_CW(RM_VALD, RM_VALU)  },
};
#endif

/*
       ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┐
       │Esc│   │ F1│ F2│ F3│ F4│ │ F5│ F6│ F7│ F8│ │ F9│F10│F11│F12│ │Kn.│
       ├───┼───┼───┼───┼───┼───┼─┴─┬─┴─┬─┴─┬─┴─┬─┴─┼───┼───┼───┴───┤ ├───┤
       │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Bakspc│ │Ins│
       ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤*├───┤
       │ Tab │ q │ w │ e │ r │ t │ y │ u │ i │ o │ p │ [ │ ] │  \  │*│Del│
       ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤*├───┤
       │ Caps │ a │ s │ d │ f │ g │ h │ j │ k │ l │ ; │ ' │  Enter │ │PgU│
       ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬┬──┴┐├───┤
       │ LShift │ z │ x │ c │ v │ b │ n │ m │ , │ . │ / │ RSft ││ ↑ ││PgD│
       ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴──┬┴──┬┴──┬┬──┴┼───┼┴──┬┘
       │LCrl│GUI │LAlt│         Space          │RAt│Fn │RCl││ ← │ ↓ │ → │
       └────┴────┴────┴────────────────────────┴───┴───┴───┘└───┴───┴───┘
key(col)

       ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┐
R0     │ 0 │   │ 2 │ 3 │ 4 │ 5 │ │ 6 │ 7 │ 8 │ 9 │ │ 10│ 11│ 12│ 13│ │ 15│
       ├───┼───┼───┼───┼───┼───┼─┴─┬─┴─┬─┴─┬─┴─┬─┴─┼───┼───┼───┴───┤ ├───┤
R1     │ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 10│ 11│ 12│   13  │ │ 14│
       ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤*├───┤
R2     │  0  │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 10│ 11│ 12│  13 │*│ 14│
       ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤*├───┤
R3     │   0  │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 10│ 11│   13   │ │ 14│
       ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬┬──┴┐├───┤
R4     │    0   │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 10│ 11│  12  ││ 13││ 14│
       ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴──┬┴──┬┴──┬┬──┴┼───┼┴──┬┘
R5     │  0 │  1 │  2 │           6            │ 9 │ 10│ 11││ 12│ 13│ 14│
       └────┴────┴────┴────────────────────────┴───┴───┴───┘└───┴───┴───┘


LED Index
       ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┐
R0     │ 0 │   │ 1 │ 2 │ 3 │ 4 │ │ 5 │ 6 │ 7 │ 8 │ │ 9 │ 10│ 11│ 12│ │X13│  <-- No LED in index 13
       ├───┼───┼───┼───┼───┼───┼─┴─┬─┴─┬─┴─┬─┴─┬─┴─┼───┼───┼───┴───┤ ├───┤
R1     │ 14│ 15│ 16│ 17│ 18│ 19│ 20│ 21│ 22│ 23│ 24│ 25│ 26│   27  │ │ 28│
       ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤*├───┤  <-- Tied to B8 ("A" indicator - Caps)
R2     │  29 │ 30│ 31│ 32│ 33│ 34│ 35│ 36│ 37│ 38│ 39│ 40│ 41│  42 │*│ 43│  <-- 88 ("M" indicator)
       ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤*├───┤  <-- 89 (Bat indicator)
R3     │  44  │ 45│ 46│ 47│ 48│ 49│ 50│ 51│ 52│ 53│ 54│ 55│ 56 *57 │ │ 58│  <-- Note that "Enter" has two LEDs, 56+57, 57 is the key-associated LED
       ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬┬──┴┐├───┤
R4     │ *59 60 │ 61│ 62│ 63│ 64│ 65│ 66│ 67│ 68│ 69│ 70│  71  ││ 72││ 73│  <-- Note that "LShift" has two LEDs, 59+60, 59 is teh key-associated LED
       ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴──┬┴──┬┴──┬┬──┴┼───┼┴──┬┘
R5     │ 74 │ 75 │ 76 │ 77   78  *79*  80  81  │ 82│ 83│ 84││ 85│ 86│ 87│   <-- Note that "Space" has 5 LEDs
       └────┴────┴────┴────────────────────────┴───┴───┴───┘└───┴───┴───┘       The LED in index 79 is the center/associated with the key in the matrix
*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE_Win] = LAYOUT(
    KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,  KC_F6,   KC_F7,   KC_F8,   KC_F9,      KC_F10,  KC_F11,  KC_F12,            KC_MUTE,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,   KC_7,    KC_8,    KC_9,    KC_0,       KC_MINS, KC_EQL,  KC_BSPC, KC_INS,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,       KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN,    KC_QUOT,          KC_ENT,  KC_PGUP,
    KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   KC_N,    KC_M,    KC_COMM, KC_DOT,     KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                   KC_RALT, MO(_FnLay_Win), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [_FnLay_Win] = LAYOUT(
    EE_CLR,           KC_BRID, KC_BRIU, A(KC_TAB),G(KC_H),G(KC_D), G(KC_S), KC_MPRV, KC_MPLY, KC_MNXT,   KC_MUTE, KC_VOLD, KC_VOLU,          XXXXXXX,
    DB_TOGG, BT_CHN1, BT_CHN2, BT_CHN3, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, RM_NEXT,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_HOME, KC_END,  XXXXXXX, RM_PREV,
    KC_NUM , DB_KEY , XXXXXXX, DB_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_SCRL,   KC_PSCR,          XXXXXXX, RM_VALU,
    XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_PAUS, XXXXXXX, RM_HUEU, RM_VALD,
    XXXXXXX, XXXXXXX, XXXXXXX,                            RM_TOGG,                   XXXXXXX, _______,   XXXXXXX, RM_SPDD, RM_HUED, RM_SPDU
    ),
    [_BASE_Mac] = LAYOUT(
    KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,  KC_F6,   KC_F7,   KC_F8,   KC_F9,      KC_F10,  KC_F11,  KC_F12,            KC_MUTE,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,   KC_7,    KC_8,    KC_9,    KC_0,       KC_MINS, KC_EQL,  KC_BSPC, KC_INS,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,       KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN,    KC_QUOT,          KC_ENT,  KC_PGUP,
    KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   KC_N,    KC_M,    KC_COMM, KC_DOT,     KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                   KC_RALT, MO(_FnLay_Mac), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [_FnLay_Mac] = LAYOUT(
    EE_CLR,           KC_BRID, KC_BRIU, A(KC_TAB),G(KC_H),G(KC_D), G(KC_S), KC_MPRV, KC_MPLY, KC_MNXT,   KC_MUTE, KC_VOLD, KC_VOLU,          XXXXXXX,
    DB_TOGG, BT_CHN1, BT_CHN2, BT_CHN3, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, RM_NEXT,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_HOME, KC_END,  XXXXXXX, RM_PREV,
    KC_NUM , DB_KEY , XXXXXXX, DB_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_SCRL,   KC_PSCR,          XXXXXXX, RM_VALU,
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
      case DB_KEY:  //debug key
        if (record->event.pressed) {
            debug_method(1);
        } else {
          // Do something else when release
        }
        return false; // Skip all further processing of this key
      default:
        return true; // Process all other keycodes normally
    }
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
//  Set caps lock LED to RED
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(44, 255,255,255); // assuming caps lock is at index 44
    }
    return true;   // continue running the keyboard-level callback
}
