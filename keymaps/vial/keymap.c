/* Copyright 2020 Josef Adamcik
 * Vial default keymap imported from user .vil export
 */
#include QMK_KEYBOARD_H
#include "oled.c"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5,                    KC_6, KC_7, KC_8, KC_9, KC_0, KC_GRAVE,
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,                    KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC,
        KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G,                    KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOTE,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_MUTE,     C(S(KC_T)), KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_BSLS,
                         KC_LCTL, KC_LGUI, KC_LALT, MO(1), KC_SPACE,      KC_ENTER, KC_BSPC, KC_DEL, KC_PSCR, KC_RBRC
    ),
    [1] = LAYOUT(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6,                    KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
        XXXXXXX, XXXXXXX, KC_UP, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        KC_CAPS, KC_LEFT, KC_DOWN, KC_RIGHT, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_HOME, KC_PGUP,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_END, KC_PGDN,
                         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_SPACE,      KC_ENTER, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [2] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                    _______, _______, _______, _______, _______, _______,
        _______, KC_INS, KC_PSCR, KC_APP, XXXXXXX, XXXXXXX,                    KC_PGUP, _______, KC_UP, _______, _______, KC_BSPC,
        _______, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX, KC_CAPS,                    KC_PGDN, KC_LEFT, KC_DOWN, KC_RIGHT, KC_DEL, KC_BSPC,
        _______, KC_UNDO, KC_CUT, KC_COPY, KC_PASTE, XXXXXXX, _______,     _______, XXXXXXX, _______, XXXXXXX, _______, XXXXXXX, _______,
                         _______, _______, _______, _______, _______,      _______, _______, _______, _______, _______
    ),
    [3] = LAYOUT(
        KC_GRAVE, KC_1, KC_2, KC_3, KC_4, KC_5,                    KC_6, KC_7, KC_8, KC_9, KC_0, KC_GRAVE,
        KC_ESC, KC_Q, KC_W, KC_F, KC_P, KC_G,                    KC_J, KC_L, KC_U, KC_Y, KC_SCLN, KC_BSPC,
        KC_TAB, KC_A, KC_R, KC_S, KC_T, KC_D,                    KC_H, KC_N, KC_E, KC_I, KC_O, KC_QUOTE,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_MUTE,     XXXXXXX, KC_K, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RSFT,
                         KC_LGUI, KC_LALT, KC_LCTL, _______, KC_ENTER,      KC_SPACE, _______, KC_RCTL, KC_RALT, KC_RGUI
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(C(KC_TAB), C(S(KC_TAB))) },
    [1] = { ENCODER_CCW_CW(XXXXXXX, XXXXXXX), ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [2] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [3] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
};
#endif
