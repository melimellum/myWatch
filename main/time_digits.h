#pragma once

#include "lvgl.h"

lv_obj_t *createDigitRow(lv_obj_t *parent, const lv_font_t *font,
                         uint32_t color, const char *pattern);

void setDigitRowText(lv_obj_t *row, const char *text);