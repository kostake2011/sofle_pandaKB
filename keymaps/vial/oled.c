// SPDX-License-Identifier: GPL-2.0-or-later
// Sofle RGB (PandaKB): левый экран — слой/WPM/индикаторы/Bongo Cat,
// правый — Walker Duck, оверлеи энкодеров, zZz-заставка после 5 минут.
// Экран в режиме OLED_ROTATION_270 — логическое поле 32x128.

#include "transactions.h"
#include "wpm.h"
#include "oled_assets.h"
#include <string.h>

#ifndef IS_ENCODEREVENT
#    define IS_ENCODEREVENT(event) ((event).type == ENCODER_CW_EVENT || (event).type == ENCODER_CCW_EVENT)
#endif

// ---------- настройки ----------
#define IDLE_TIMEOUT_MS     300000  // 5 минут до zZz
#define OVERLAY_MS          1000    // длительность оверлея энкодера
#define DUCK_FRAME_MS       110     // темп Duck
#define BONGO_SLOW_MS       650     // интервал лап при wpm=0
#define BONGO_FAST_MS       140     // интервал лап при wpm>=30
#define RPC_PERIOD_MS       250     // реже RPC — меньше нагрузка на TRRS

// ---------- глифы 8x8 (page-формат, LSB = верх) ----------
static const uint8_t zzz8[8]     PROGMEM = {0x41, 0x61, 0x51, 0x49, 0x45, 0x43, 0x41, 0x00};
static const uint8_t arrow_l[8]  PROGMEM = {0x08, 0x0C, 0x0E, 0x7F, 0x08, 0x08, 0x08, 0x00};
static const uint8_t arrow_r[8]  PROGMEM = {0x00, 0x08, 0x08, 0x08, 0x7F, 0x0E, 0x0C, 0x08};

// ---------- синхронизация оверлеев между половинами ----------
typedef struct {
    uint8_t  left;         // 0 нет, 1 VOL-, 2 VOL+, 3 MUTE
    uint8_t  right;        // 0 нет, 1 TAB-, 2 TAB+, 3 REOPEN
    uint32_t left_until;
    uint32_t right_until;
} overlay_sync_t;

static overlay_sync_t ovl = {0, 0, 0, 0};

static void overlay_rpc_handler(uint8_t in_size, const void *in_data, uint8_t out_size, void *out_data) {
    if (in_size == sizeof(overlay_sync_t)) {
        memcpy(&ovl, in_data, sizeof(ovl));
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_OVERLAY_SYNC, overlay_rpc_handler);
}

void housekeeping_task_user(void) {
    static uint32_t rpc_timer = 0;
    /* Шлём оверлей только пока он активен — не долбим serial каждые 100ms */
    if (!is_keyboard_master()) {
        return;
    }
    uint32_t now = timer_read32();
    bool active = (int32_t)(ovl.left_until - now) > 0 || (int32_t)(ovl.right_until - now) > 0;
    if (!active) {
        return;
    }
    if (timer_elapsed32(rpc_timer) > RPC_PERIOD_MS) {
        rpc_timer = now;
        transaction_rpc_send(USER_OVERLAY_SYNC, sizeof(ovl), &ovl);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (IS_ENCODEREVENT(record->event)) {
        if (record->event.pressed) {
            uint8_t idx = record->event.key.col; // 0 = левый, 1 = правый
            if (idx == 0) {
                ovl.left      = (record->event.type == ENCODER_CW_EVENT) ? 2 : 1;
                ovl.left_until  = timer_read32() + OVERLAY_MS;
            } else {
                ovl.right     = (record->event.type == ENCODER_CW_EVENT) ? 2 : 1;
                ovl.right_until = timer_read32() + OVERLAY_MS;
            }
        }
        return true;
    }
    if (record->event.pressed) {
        // нажатия энкодеров — матричные позиции (4,5) и (9,5)
        if (record->event.key.row == 4 && record->event.key.col == 5) {
            ovl.left = 3; ovl.left_until = timer_read32() + OVERLAY_MS;
        }
        if (record->event.key.row == 9 && record->event.key.col == 5) {
            ovl.right = 3; ovl.right_until = timer_read32() + OVERLAY_MS;
        }
    }
    return true;
}

// ---------- zZz: сборка кадра в локальном буфере ----------
static void fb_blit8(uint8_t *fb, uint8_t x, uint8_t line, const uint8_t *spr, bool mirror, uint8_t scale) {
    for (uint8_t c = 0; c < 8; c++) {
        uint8_t s = pgm_read_byte(&spr[mirror ? (7 - c) : c]);
        uint32_t acc = 0; // 8 бит -> 8*scale строк
        for (uint8_t b = 0; b < 8; b++) {
            if (s & (1 << b)) acc |= ((1u << scale) - 1u) << (b * scale);
        }
        for (uint8_t xs = 0; xs < scale; xs++) {
            uint8_t col = x + c * scale + xs;
            if (col > 31) continue;
            for (uint8_t pb = 0; pb < scale; pb++) {
                uint8_t y = (line + pb) * 8;
                fb[(y >> 3) * 32 + col] |= (uint8_t)(acc >> (pb * 8));
            }
        }
    }
}

#define SLEEP_STEPS 4
static const struct { uint8_t scale; uint8_t x; uint8_t line; } sleep_steps[SLEEP_STEPS] = {
    {1, 22, 13},  // маленькое z внизу
    {2, 14, 10},  // Z среднее выше
    {3, 6, 5},    // Z крупное
    {3, 2, 0},    // самое верхнее — «поле заполнено»
};

static void render_sleep(bool left_hand) {
    static uint8_t  step = 0;
    static uint32_t step_timer = 0;
    uint16_t dur = (step >= SLEEP_STEPS) ? 1500 : 900;
    if (timer_elapsed32(step_timer) > dur) {
        step_timer = timer_read32();
        step = (step + 1) % (SLEEP_STEPS + 1); // последний шаг = пауза на полном экране
    }
    uint8_t fb[512];
    memset(fb, 0, sizeof(fb));
    /* Оба OLED в ROTATION_270 смотрят на пользователя одинаково —
     * буквы Z не зеркалим, координаты те же, что слева. */
    (void)left_hand;
    for (uint8_t i = 0; i < step && i < SLEEP_STEPS; i++) {
        fb_blit8(fb, sleep_steps[i].x, sleep_steps[i].line, zzz8, false, sleep_steps[i].scale);
    }
    oled_set_cursor(0, 0);
    oled_write_raw((const char *)fb, 512);
}

// ---------- левый экран ----------
static const char PROGMEM layer_names[4][5] = {"BASE", "LOWR", "RAIS", "COLM"};

static uint32_t wpm_smooth = 0;
static uint8_t  bongo_frame = 0;
static uint32_t bongo_timer = 0;

static void render_left(void) {
    // оверлей или имя слоя (строка 0)
    if ((int32_t)(ovl.left_until - timer_read32()) > 0) {
        oled_set_cursor(0, 0);
        switch (ovl.left) {
            case 1:
                oled_write_raw_P((const char *)arrow_l, 8);
                oled_set_cursor(2, 0); oled_write_P(PSTR("VOL"), false);
                break;
            case 2:
                oled_write_P(PSTR("VOL"), false);
                oled_set_cursor(3, 0); oled_write_raw_P((const char *)arrow_r, 8);
                break;
            default:
                oled_write_P(PSTR("MUTE"), false);
                break;
        }
        oled_set_cursor(5, 0); oled_write_P(PSTR("  "), false);
    } else {
        uint8_t layer = get_highest_layer(layer_state);
        if (layer > 3) layer = 3;
        oled_set_cursor(0, 0);
        oled_write_P((const char *)layer_names[layer], false);
        oled_set_cursor(4, 0); oled_write_P(PSTR(" "), false);
    }

    // WPM (плавное усреднение)
    uint8_t wpm = get_current_wpm();
    wpm_smooth = (wpm_smooth * 3 + wpm) / 4;
    char num[4];
    num[0] = (wpm >= 100) ? '1' : ' ';
    num[1] = ((wpm / 10) % 10) + '0';
    num[2] = (wpm % 10) + '0';
    num[3] = 0;
    if (num[0] == ' ' && num[1] == '0') num[1] = ' ';
    oled_set_cursor(0, 2); oled_write_P(PSTR("WPM"), false);
    oled_set_cursor(0, 3); oled_write(num, false);

    // индикаторы — только активные
    led_t ls = host_keyboard_led_state();
    oled_set_cursor(0, 5);
    oled_write_P(ls.caps_lock ? PSTR("CAPS") : PSTR("    "), false);
    oled_set_cursor(0, 6);
    oled_write_P(ls.num_lock ? PSTR("NUM") : PSTR("   "), false);
    oled_set_cursor(0, 7);
    oled_write_P(ls.scroll_lock ? PSTR("SCRL") : PSTR("    "), false);

    /* Bongo Cat: скорость от сглаженного WPM.
     * Раньше: 650 - wpm*17 — при wpm>=39 unsigned underflow,
     * интервал становился ~65с и кот «зависал». */
    uint16_t iv;
    if (wpm_smooth >= 30) {
        iv = BONGO_FAST_MS;
    } else {
        iv = (uint16_t)(BONGO_SLOW_MS -
                        (wpm_smooth * (BONGO_SLOW_MS - BONGO_FAST_MS)) / 30);
        if (iv < BONGO_FAST_MS) iv = BONGO_FAST_MS;
    }
    if (timer_elapsed32(bongo_timer) > iv) {
        bongo_timer = timer_read32();
        bongo_frame = (wpm_smooth < 3) ? 0 : (bongo_frame == 1 ? 2 : 1);
    }
    oled_set_cursor(0, 13);
    oled_write_raw_P((const char *)bongo_cat_frames[bongo_frame], BONGO_CAT_BYTES);
}

// ---------- правый экран ----------
static uint8_t  duck_frame = 0;
static uint32_t duck_timer = 0;

static void render_right(void) {
    if ((int32_t)(ovl.right_until - timer_read32()) > 0) {
        oled_set_cursor(0, 1);
        switch (ovl.right) {
            case 1:
                oled_write_raw_P((const char *)arrow_l, 8);
                oled_set_cursor(2, 1); oled_write_P(PSTR("TAB"), false);
                break;
            case 2:
                oled_write_P(PSTR("TAB"), false);
                oled_set_cursor(3, 1); oled_write_raw_P((const char *)arrow_r, 8);
                break;
            default:
                oled_write_raw_P((const char *)arrow_l, 8);
                oled_set_cursor(2, 1); oled_write_P(PSTR("TAB"), false);
                oled_set_cursor(4, 1); oled_write_P(PSTR("+"), false);
                break;
        }
    } else {
        oled_set_cursor(0, 1); oled_write_P(PSTR("     "), false);
    }

    if (timer_elapsed32(duck_timer) > DUCK_FRAME_MS) {
        duck_timer = timer_read32();
        duck_frame = (duck_frame + 1) % DUCK_COUNT;
    }
    oled_set_cursor(0, 4);
    oled_write_raw_P((const char *)duck_frames[duck_frame], DUCK_BYTES);
}

// ---------- точка входа ----------
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

bool oled_task_user(void) {
    bool left = is_keyboard_left();
    bool idle = last_input_activity_elapsed() > IDLE_TIMEOUT_MS;
    static bool was_idle = false;
    if (idle != was_idle) {
        was_idle = idle;
        oled_clear();
    }
    if (idle) {
        render_sleep(left);
    } else if (left) {
        render_left();
    } else {
        render_right();
    }
    return false;
}