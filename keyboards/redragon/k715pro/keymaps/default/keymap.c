/* Copyright 2023 temp4gh
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

static bool swd_disabled = false;


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

//column 14 not working right
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
    KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,  KC_F6,   KC_F7,   KC_F8,   KC_F9,      KC_F10,  KC_F11,  KC_F12,            KC_END,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,   KC_7,    KC_8,    KC_9,    KC_0,       KC_MINS, KC_EQL,  KC_BSPC, KC_INS,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,       KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN,    KC_QUOT,          KC_ENT,  KC_PGUP,
    KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   KC_N,    KC_M,    KC_COMM, KC_DOT,     KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                   KC_RALT, MO(_FnLay), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [_FnLay] = LAYOUT(
    QK_BOOT,          KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE,   KC_VOLD, KC_VOLU, KC_MAIL,          XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, RM_NEXT,
    EE_CLR , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   RM_PREV, RM_NEXT, XXXXXXX, XXXXXXX,
    XXXXXXX, RM_SATU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RM_SPDD,   RM_SPDU,          XXXXXXX, XXXXXXX,
    XXXXXXX,          RM_SATD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RM_HUEU, RM_HUED,   XXXXXXX, XXXXXXX, RM_VALU, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX,                            RM_TOGG,                   XXXXXXX, _______,   XXXXXXX, RM_SPDD, RM_VALD, RM_SPDU
    )
};

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour  debug_enable=true;
  debug_matrix=true;
  debug_keyboard=true;
  //debug_mouse=true;
  swd_disabled = false;
  //toggle_jtag();
}

void disable_jtag(void) {
    if (!swd_disabled) {
        uprintf("SWD is now being DISABLED\n");
        AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG_Msk) | AFIO_MAPR_SWJ_CFG_DISABLE;
        swd_disabled = true;
    }
    return;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    static uint16_t bootloader_key_timer = 0;
    static bool bootloader_other_key_recorded = false;

    if (keycode != KC_CAPS) { bootloader_other_key_recorded = true; }


  // If console is enabled, it will print the matrix position and status of each key pressed
#ifdef CONSOLE_ENABLE
    uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif

    switch (keycode) {
        case KC_CAPS:
            if (record->event.pressed) {
                bootloader_key_timer = timer_read();
                bootloader_other_key_recorded = false;  // start tracking other keys
            } else {
                if (!bootloader_other_key_recorded) {  // only go to bootloader if no other key has been pressed
                    if (timer_elapsed(bootloader_key_timer) >= 5000) {
                        #ifdef CONSOLE_ENABLE
                            uprintf("KC_CAPS key held for more than 5s\n");
                        #endif
                        // do whatever JTAG stuff here
                        disable_jtag();
                    }
                }
                bootloader_key_timer = 0;  // reset timer counter on release so it can be used for tracking if CapsLock is pressed
            }
            break;
        default:
            break;
    }

  return true;
}
