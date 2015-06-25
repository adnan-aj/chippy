/* Ripped and shortened from /usr/src/linux/drivers/video/console/fonts.c */

#include "font.h"

const struct font_desc *fonts[] = {
    &font_vga_8x8,
    &font_vga_8x16,
    &font_vga_6x11,
    &font_7x14,
    &font_sun_8x16,
    &font_sun_12x22,
    &font_10x18,
    &font_acorn_8x8,
    &font_pearl_8x8,
    &font_mini_4x6,
};
