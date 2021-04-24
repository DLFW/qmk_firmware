/* Copyright 2021 Daniel Llin Ferrero
      unregister_code(KC_LSFT);
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

#define CC_BIT_BASE 1
#define CC_BIT_COMP 2
#define CC_BIT_NAVI 4
#define CC_BIT_MOUS 8
#define CC_BIT_META 16

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    BASE,
    COMP,
    NAVI,
    MOUS,
    META
};

int current_layer = BASE;
int leader_on = 0;
int swap_on = 0;

int test_inc = 0;

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
    // set color keys
    SC_BASE = SAFE_RANGE,
    SC_COMP,
    SC_NAVI,
    SC_MOUS,
    SC_META,
    // change color keys
    CC_VU,
    CC_VD,
    CC_SU,
    CC_SD,
    CC_HU,
    CC_HD,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [BASE] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_EQL,  \
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_NUHS, KC_RBRC, \
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, \
        KC_RALT, SH_TT,   KC_LEAD,                                                           KC_LEAD, SH_TT,   KC_RALT, \
                          KC_LGUI, T_COMP,  KC_SPC, T_NAVI,      T_NAVI,  KC_ENT, T_COMP, KC_RGUI,                      \
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
        _______, KC_WH_D, KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R,     KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, KC_WH_D, XXXXXXX, \
        _______, XXXXXXX, KC_ACL0, KC_ACL1, KC_ACL1, T_BASE,      T_BASE,  KC_ACL0, KC_ACL1, KC_ACL2, XXXXXXX, _______, \
        _______, _______, _______,                                                           _______, _______, _______, \
                          _______, _______, _______, _______,     _______, _______, _______, _______,                   \
                                   _______, _______,                       _______, _______                             \
    ),
    [META] = LAYOUT(
        EEP_RST, RESET,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     XXXXXXX, CC_HU,   CC_SU,   CC_VU,   RESET,   EEP_RST, \
        XXXXXXX, SC_BASE, SC_COMP, SC_NAVI, SC_MOUS, SC_META,     XXXXXXX, CC_HD,   CC_SD,   CC_VD,   XXXXXXX, XXXXXXX, \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, T_BASE,      T_BASE,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
        XXXXXXX, XXXXXXX, XXXXXXX,                                                           XXXXXXX, XXXXXXX, XXXXXXX, \
                          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   \
                                   XXXXXXX, XXXXXXX,                       XXXXXXX, XXXXXXX                             \
    )
};

/* Updates the current LED color */
void set_color(rgblight_config_t *col) {
  rgblight_sethsv_noeeprom(
    (*col).hue,
    (*col).sat,
    (*col).val
  );
  //rgblight_sethsv_range(0, 255, 200, 5, 7);
  //rgblight_sethsv_range(0, 255, 200, 13, 15);
  //led[5].r = 250;
  //led[5].g = 0;
  //led[5].b = 0;
  //led[6].r = 0;
  //led[6].g = 250;
  //led[6].b = 0;
  //led[13].r = 250;
  //led[13].g = 0;
  //led[13].b = 0;
  //led[14].r = 0;
  //led[14].g = 0;
  //led[14].b = 250;
  //led[test_inc].r = 0;
  //led[test_inc].g = 0;
  //led[test_inc].b = 250;
  //rgblight_set();
  //test_inc +=1;
  //if (test_inc > 15) test_inc = 0;
  //rgblight_sethsv_slave(0, 255, 200);
  //rgblight_sethsv_range(0, 255, 200, (uint8_t)RGBLED_NUM / 2, (uint8_t)RGBLED_NUM);
  //rgblight_sethsv_slave
}

void color_key_event(
  keyrecord_t *record,
  rgblight_config_t *col,
  uint8_t cc_bit,
  uint32_t *eeprom_address
) {

  if (record->event.pressed) {
    if (cc_keys == 0) {
        active_color = col;
        cc_keys |= cc_bit;
        set_color(active_color);
    }
  } else {
    if (cc_keys & cc_bit) {
        cc_keys ^= cc_bit;
        if (cc_keys == 0) {
            active_color = 0;
            eeprom_update_dword(eeprom_address, (*col).raw);
            set_color(&col_meta_layer);
        }
    }
  }
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case SC_BASE:
      color_key_event(
        record,
        &col_base_layer,
        CC_BIT_BASE,
        EECONFIG_COLOR_BASE_LAYER
      );
      return true;
    case SC_COMP:
      color_key_event(
        record,
        &col_comp_layer,
        CC_BIT_COMP,
        EECONFIG_COLOR_COMP_LAYER
      );
      return true;
    case SC_NAVI:
      color_key_event(
        record,
        &col_navi_layer,
        CC_BIT_NAVI,
        EECONFIG_COLOR_NAVI_LAYER
      );
      return true;
    case SC_MOUS:
      color_key_event(
        record,
        &col_mous_layer,
        CC_BIT_MOUS,
        EECONFIG_COLOR_MOUS_LAYER
      );
      return true;
    case SC_META:
      color_key_event(
        record,
        &col_meta_layer,
        CC_BIT_META,
        EECONFIG_COLOR_META_LAYER
      );
      return true;
    case CC_VU:
      if (active_color) {
          active_color->val += 5;
          if (active_color->val > RGBLIGHT_LIMIT_VAL) {
              active_color->val = 0;
          }
          set_color(active_color);
      }
      return true;
    case CC_VD:
      if (active_color) {
          active_color->val -= 5;
          if (active_color->val > RGBLIGHT_LIMIT_VAL) {
              active_color->val = RGBLIGHT_LIMIT_VAL;
          }
          set_color(active_color);
      }
      return true;
    case CC_HU:
      if (active_color) {
          active_color->hue += 2;
          set_color(active_color);
      }
      return true;
    case CC_HD:
      if (active_color) {
          active_color->hue -= 4;
          set_color(active_color);
      }
      return true;
    case CC_SU:
      if (active_color) {
          active_color->sat += 5;
          set_color(active_color);
      }
      return true;
    case CC_SD:
      if (active_color) {
          active_color->sat -= 5;
          set_color(active_color);
      }
      return true;
  }
  return true;
}

void update_leds(void) {
    switch (current_layer) {
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
}

/*
 * Run every time the layer changes.
 * In here, we update the current LED color.
 */
layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = biton32(state);
    update_leds();
    return state;
}

void leader_start(void) {
  // sequence started
}

void leader_end(void) {
  // sequence ended (no success/failure detection)
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
    //rgblight_mode_noeeprom(RGBLIGHT_MODE_BREATHING);
    //rgblight_mode_noeeprom(RGBLIGHT_MODE_KNIGHT);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
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

    // blue
    col_comp_layer.hue = 170;
    col_comp_layer.sat = 255;
    col_comp_layer.val = 255;

    // orange
    col_navi_layer.hue = 25;
    col_navi_layer.sat = 255;
    col_navi_layer.val = 255;

    //pink
    col_mous_layer.hue = 205;
    col_mous_layer.sat = 255;
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
    SEQ_ONE_KEY(KC_V) {  // pipe
      register_code(KC_RALT);
      register_code(KC_NUBS);
      unregister_code(KC_NUBS);
      unregister_code(KC_RALT);
    }
    SEQ_ONE_KEY(KC_COMM) {
      SEND_STRING(SS_RALT("8"));
    }
    SEQ_ONE_KEY(KC_DOT) {
      SEND_STRING(SS_RALT("9"));
    }
    SEQ_ONE_KEY(KC_I) {
      SEND_STRING(SS_RALT("7"));
    }
    SEQ_ONE_KEY(KC_O) {
      SEND_STRING(SS_RALT("0"));
    }
    SEQ_ONE_KEY(KC_U) {  // less than
      register_code(KC_NUBS);
      unregister_code(KC_NUBS);
    }
    SEQ_ONE_KEY(KC_P) {  // greater than
      register_code(KC_LSFT);
      register_code(KC_NUBS);
      unregister_code(KC_NUBS);
      unregister_code(KC_LSFT);
    }
    SEQ_ONE_KEY(KC_SLSH) {  // question mark
      register_code(KC_LSFT);
      register_code(KC_MINS);
      unregister_code(KC_MINS);
      unregister_code(KC_LSFT);
    }
    SEQ_ONE_KEY(KC_RBRC) {  // reversed question mark
      register_code(KC_LSFT);
      register_code(KC_RALT);
      register_code(KC_MINS);
      unregister_code(KC_MINS);
      unregister_code(KC_RALT);
      unregister_code(KC_LSFT);
    }
    SEQ_ONE_KEY(KC_Q) {  // reversed exclamation mark
      register_code(KC_LSFT);
      register_code(KC_RALT);
      register_code(KC_1);
      unregister_code(KC_1);
      unregister_code(KC_RALT);
      unregister_code(KC_LSFT);
    }
  }
}
