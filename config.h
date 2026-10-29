#pragma once

/* Handedness: пин GP4. Ключа "pin_low_is_left" в data-driven НЕТ
 * (схема split.handedness допускает только pin / matrix_grid),
 * поэтому LOW_IS_LEFT остаётся здесь. */
#define SPLIT_HAND_PIN_LOW_IS_LEFT

/* Стабильность split (matrix_state уже в keyboard.json).
 * Master = сторона с USB (удобно для продажи). */
#define SPLIT_MAX_CONNECTION_ERRORS 10
#define SPLIT_CONNECTION_CHECK_TIMEOUT 500

/* VBUS detection (нет data-driven ключа) */
#define USB_VBUS_PIN GP19

/* Разводка PIO: PIO0 — split serial, PIO1 — WS2812 */
#define SERIAL_PIO_USE_PIO0
#define WS2812_PIO_USE_PIO1

/* Double-tap reset -> bootloader */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP17
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 1000U

/* OLED: 128x32 — дефолт драйвера, OLED_DISPLAY_128X32 удалён из QMK.
 * Размер/шрифт/таймаут — в конфиге драйвера остаются здесь. */
#define OLED_BRIGHTNESS 180
#define OLED_TIMEOUT 0
#define OLED_FONT_H "lib/glcdfont.c"

/* I2C1 для OLED на Stemma QT */
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3
