#include "brightness_screen.h"

#include <stdio.h>
#include "bsp/esp32_s3_touch_lcd_1_69.h"

extern "C" {
    extern const lv_font_t science_gothic_48;
    extern const lv_font_t science_gothic_18;
}

namespace
{
    constexpr uint32_t COLOR_BACKGROUND = 0x000000;
    constexpr uint32_t COLOR_TEXT       = 0x969696;
    constexpr uint32_t COLOR_ACCENT     = 0x00897B;
    constexpr uint32_t COLOR_TRACK      = 0x303030;

    constexpr int MIN_BRIGHTNESS = 5;     
    constexpr int MAX_BRIGHTNESS = 100;

    constexpr int SLIDER_WIDTH  = 180;
    constexpr int SLIDER_HEIGHT = 16;

    int currentBrightness = 40;           

    lv_obj_t *valueLabel = nullptr;

    void showValue(int value)
    {
        char buffer[8];
        snprintf(buffer, sizeof(buffer), "%d", value);
        lv_label_set_text(valueLabel, buffer);
    }

    void onSliderChanged(lv_event_t *e)
    {
        lv_obj_t *slider = static_cast<lv_obj_t *>(lv_event_get_target(e));

        int value = lv_slider_get_value(slider);
        applyBrightness(value);
        showValue(currentBrightness);
    }
}

void applyBrightness(int percent)
{
    if (percent < MIN_BRIGHTNESS) percent = MIN_BRIGHTNESS;
    if (percent > MAX_BRIGHTNESS) percent = MAX_BRIGHTNESS;

    currentBrightness = percent;
    bsp_display_brightness_set(percent);
}

lv_obj_t *createBrightnessScreen()
{
    lv_obj_t *screen = lv_obj_create(nullptr);

    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "brightness");
    lv_obj_set_style_text_color(title, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_set_style_text_font(title, &science_gothic_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

    valueLabel = lv_label_create(screen);
    lv_obj_set_style_text_color(valueLabel, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_set_style_text_font(valueLabel, &science_gothic_48, 0);
    lv_obj_align(valueLabel, LV_ALIGN_CENTER, 0, -20);
    showValue(currentBrightness);

    lv_obj_t *slider = lv_slider_create(screen);
    lv_obj_set_size(slider, SLIDER_WIDTH, SLIDER_HEIGHT);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 50);
    lv_slider_set_range(slider, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    lv_slider_set_value(slider, currentBrightness, LV_ANIM_OFF);

    lv_obj_set_style_bg_color(slider, lv_color_hex(COLOR_TRACK), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(COLOR_ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(COLOR_TEXT), LV_PART_KNOB);

    lv_obj_add_event_cb(slider, onSliderChanged, LV_EVENT_VALUE_CHANGED, nullptr);

    return screen;
}