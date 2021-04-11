/* Copyright 2021 Daniel Llin Ferrero
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
#include <eeprom.h>

rgblight_config_t col_base_layer;
rgblight_config_t col_comp_layer;
rgblight_config_t col_navi_layer;
rgblight_config_t col_mous_layer;
rgblight_config_t col_meta_layer;
rgblight_config_t *active_color = 0;
uint8_t cc_keys = 0;

#define EECONFIG_COLOR_BASE_LAYER (uint32_t *)64
#define EECONFIG_COLOR_COMP_LAYER (uint32_t *)68
#define EECONFIG_COLOR_NAVI_LAYER (uint32_t *)72
#define EECONFIG_COLOR_MOUS_LAYER (uint32_t *)76
#define EECONFIG_COLOR_META_LAYER (uint32_t *)80

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    BASE,
    COMP,
    NAVI,
    MOUS,
    META
};

#define T_COMP TT(COMP)
#define T_NAVI TT(NAVI)
#define T_MOUS TO(MOUS)
#define T_BASE TO(BASE)
#define T_META TO(META)

#define C_LEFT KC_LCTL  // MT(MOD_LCTL, KC_DEL)
#define C_RIGT KC_RCTL  // MT(MOD_RCTL, KC_BSPC)
#define A_LEFT MT(MOD_LALT, KC_DEL)   // KC_LALT
#define A_RIGT MT(MOD_LALT, KC_BSPC)  // KC_LALT

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    QMKBEST = SAFE_RANGE,
    QMKURL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [BASE] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_EQL,  \
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_NUHS, KC_RBRC, \
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, \
        KC_RALT, SH_TT,   KC_LEAD,                                                           KC_LEAD, SH_TT, KC_RALT, \
                          KC_LGUI, T_COMP,  KC_SPC,  T_NAVI,      T_NAVI,  KC_ENT,  T_COMP,  KC_RGUI,                   \
                                   C_LEFT,  A_LEFT,                        A_RIGT,  C_RIGT                              \
    ),
    [COMP] = LAYOUT(
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,       KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  \
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_F12,  \
        _______, KC_APP,  KC_NO,   KC_GRV,  KC_NUBS, KC_DEL,      KC_BSPC, KC_QUOT, KC_SCLN, KC_LBRC, KC_MINS, _______, \
        _______, _______, _______,                                                           _______, _______, _______, \
                          _______, _______, _______, _______,     _______, _______, _______, _______,                   \
                                   _______, _______,                       _______, _______                             \
    ),
    [NAVI] = LAYOUT(
        _______, KC_F13,  KC_HOME, KC_PGUP, KC_PGDN, KC_END,      KC_HOME, KC_PGUP, KC_PGDN, KC_END,  KC_VOLU, KC_F15,  \
        _______, KC_F14,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,     KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_VOLD, KC_F16,  \
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, T_MOUS,      T_MOUS,  KC_MPLY, KC_MPRV, KC_MNXT, KC_MUTE, _______, \
        _______, _______, _______,                                                           _______, _______, _______, \
                          _______, _______, _______, _______,     _______, _______, _______, _______,                   \
                                   _______, _______,                       _______, _______                             \
    ),
    [MOUS] = LAYOUT(
        T_META,  KC_WH_U, KC_BTN1, KC_BTN3, KC_BTN2, XXXXXXX,     XXXXXXX, KC_BTN1, KC_BTN3, KC_BTN2, KC_WH_U, T_META,  \
        XXXXXXX, KC_WH_D, KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R,     KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, KC_WH_D, XXXXXXX, \
        _______, XXXXXXX, KC_ACL0, KC_ACL1, KC_ACL1, T_BASE,      T_BASE,  KC_ACL0, KC_ACL1, KC_ACL2, XXXXXXX, _______, \
        _______, _______, _______,                                                           _______, _______, _______, \
                          _______, _______, _______, _______,     _______, _______, _______, _______,                   \
                                   _______, _______,                       _______, _______                             \
    ),
    [META] = LAYOUT(
        EEP_RST, RESET, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RESET,   EEP_RST, \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, T_BASE,      T_BASE,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
        XXXXXXX, XXXXXXX, XXXXXXX,                                                           XXXXXXX, XXXXXXX, XXXXXXX, \
                          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   \
                                   XXXXXXX, XXXXXXX,                       XXXXXXX, XXXXXXX                             \
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QMKBEST:
            if (record->event.pressed) {
                // when keycode QMKBEST is pressed
                SEND_STRING("QMK is the best thing ever!");
            } else {
                // when keycode QMKBEST is released
            }
            break;
        case QMKURL:
            if (record->event.pressed) {
                // when keycode QMKURL is pressed
                SEND_STRING("https://qmk.fm/\n");
            } else {
                // when keycode QMKURL is released
            }
            break;
    }
    return true;
}

/* Updates the current LED color */
void set_color(rgblight_config_t *col) {
  rgblight_sethsv_noeeprom(
    (*col).hue,
    (*col).sat,
    (*col).val
  );
}

/*
 * Run every time the layer changes.
 * In here, we update the current LED color.
 */
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (biton32(state)) {
    case BASE:
        set_color(&col_base_layer);
        break;
    case COMP:
        set_color(&col_comp_layer);
        break;
    case NAVI:
        set_color(&col_navi_layer);
        break;
    case MOUS:
        set_color(&col_mous_layer);
        break;
    case META:
        set_color(&col_meta_layer);
        break;
    default:
        rgblight_sethsv (HSV_CORAL);
        break;
    }
  return state;
}

/*
 * Run each time the keyboard starts at the end of the startup sequence.
 * In here, we restore the EEPROM-saved colors.
 */
void keyboard_post_init_user(void) {
    col_base_layer.raw = eeprom_read_dword(EECONFIG_COLOR_BASE_LAYER);
    col_comp_layer.raw = eeprom_read_dword(EECONFIG_COLOR_COMP_LAYER);
    col_navi_layer.raw = eeprom_read_dword(EECONFIG_COLOR_NAVI_LAYER);
    col_mous_layer.raw = eeprom_read_dword(EECONFIG_COLOR_MOUS_LAYER);
    col_meta_layer.raw = eeprom_read_dword(EECONFIG_COLOR_META_LAYER);
    set_color(&col_base_layer);
}

/* 
 * Run at startup only after the EEPROM has been reset.
 * In here, we configure the initial colors by saving them to the EEPROM.
 */
void eeconfig_init_user(void) {
    // white
    col_base_layer.hue = 0;
    col_base_layer.sat = 0;
    col_base_layer.val = 255;

    // light baby blue
    col_comp_layer.hue = 131;
    col_comp_layer.sat = 200;
    col_comp_layer.val = 255;

    // orange
    col_navi_layer.hue = 35;
    col_navi_layer.sat = 250;
    col_navi_layer.val = 255;

    //pink
    col_mous_layer.hue = 216;
    col_mous_layer.sat = 200;
    col_mous_layer.val = 255;

    //red
    col_meta_layer.hue = 0;
    col_meta_layer.sat = 255;
    col_meta_layer.val = 255;

    eeprom_update_dword(EECONFIG_COLOR_BASE_LAYER, col_base_layer.raw);
    eeprom_update_dword(EECONFIG_COLOR_COMP_LAYER, col_comp_layer.raw);
    eeprom_update_dword(EECONFIG_COLOR_NAVI_LAYER, col_navi_layer.raw);
    eeprom_update_dword(EECONFIG_COLOR_MOUS_LAYER, col_mous_layer.raw);
    eeprom_update_dword(EECONFIG_COLOR_META_LAYER, col_meta_layer.raw);
    rgblight_enable();
    rgblight_mode(1);
}

LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    SEQ_ONE_KEY(KC_A) {
      SEND_STRING(SS_LSFT("1"));
    }
    SEQ_ONE_KEY(KC_S) {
      SEND_STRING(SS_LSFT("2"));
    }
    SEQ_ONE_KEY(KC_D) {
      SEND_STRING(SS_LSFT("3"));
    }
    SEQ_ONE_KEY(KC_F) {
      SEND_STRING(SS_LSFT("4"));
    }
    SEQ_ONE_KEY(KC_G) {
      SEND_STRING(SS_LSFT("5"));
    }
    SEQ_ONE_KEY(KC_H) {
      SEND_STRING(SS_LSFT("6"));
    }
    SEQ_ONE_KEY(KC_J) {
      SEND_STRING(SS_LSFT("7"));
    }
    SEQ_ONE_KEY(KC_K) {
      SEND_STRING(SS_LSFT("8"));
    }
    SEQ_ONE_KEY(KC_L) {
      SEND_STRING(SS_LSFT("9"));
    }
    SEQ_ONE_KEY(KC_NUHS) {
      SEND_STRING(SS_LSFT("0"));
    }
  }
}
