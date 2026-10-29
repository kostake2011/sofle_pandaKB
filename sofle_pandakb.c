#include "quantum.h"

#ifdef SWAP_HANDS_ENABLE

__attribute__((weak))
const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] =
    // Mirror layout: q <-> p, w <-> o, etc.
{ \
 { {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5} }, \
 { {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6} }, \
 { {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7} }, \
 { {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8} }, \
 { {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9} }, \
                                                     \
 { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0} }, \
 { {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1} }, \
 { {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2} }, \
 { {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3} }, \
 { {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4} }, \
};

// При swap-hands левый энкодер (0) меняется местами с правым (1)
__attribute__((weak))
const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS] = {1, 0};

#endif

#ifdef RGB_MATRIX_ENABLE
/* Индикаторные LED: индекс 0 (лево) и 36 (право).
 * Не зависят от эффекта Vial — перекрашиваются каждый кадр.
 * Яркость ~50% (0–128). Caps перекрывает цвет слоя. */
bool rgb_matrix_indicators_user(void) {
    uint8_t r = 0, g = 0, b = 0;

    /* max_brightness=128 → ~50%; BASE выкл, слой/Caps — цвет */
    if (host_keyboard_led_state().caps_lock) {
        r = 64; g = 0; b = 0; /* красный ~50% */
    } else {
        switch (get_highest_layer(layer_state)) {
            case 0: /* BASE — выкл */
                r = 0; g = 0; b = 0;
                break;
            case 1: /* FN — синий */
                r = 0; g = 20; b = 64;
                break;
            case 2: /* RAIS — зелёный */
                r = 0; g = 64; b = 20;
                break;
            default: /* COLM / прочие — фиолетовый */
                r = 50; g = 0; b = 64;
                break;
        }
    }

    rgb_matrix_set_color(0, r, g, b);
    rgb_matrix_set_color(36, r, g, b);
    return false;
}
#endif
