#include "settings_screen.h"

#include "navigation.h"

extern "C" {
    extern const lv_font_t science_gothic_18;
}

namespace
{
    constexpr uint32_t COLOR_BACKGROUND = 0x000000;
    constexpr uint32_t COLOR_TEXT       = 0x969696;
    constexpr uint32_t COLOR_ACCENT     = 0x00897B;

    constexpr int BUTTON_SIZE   = 48;
    constexpr int BUTTON_MARGIN = 20;   

    void onBrightnessClicked(lv_event_t *)
    {
        navigateTo(ScreenId::Brightness, LV_DIR_LEFT);
    }

    void createBrightnessButton(lv_obj_t *parent)
    {
        lv_obj_t *button = lv_button_create(parent);

        lv_obj_set_size(button, BUTTON_SIZE, BUTTON_SIZE);
        lv_obj_align(button, LV_ALIGN_TOP_LEFT, BUTTON_MARGIN, BUTTON_MARGIN);

        lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(button, 2, 0);
        lv_obj_set_style_border_color(button, lv_color_hex(COLOR_ACCENT), 0);
        lv_obj_set_style_shadow_width(button, 0, 0);

        lv_obj_t *icon = lv_obj_create(button);
        lv_obj_set_size(icon, 16, 16);
        lv_obj_center(icon);
        lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(icon, lv_color_hex(COLOR_TEXT), 0);
        lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(icon, 0, 0);
        lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE);  

        lv_obj_add_event_cb(button, onBrightnessClicked, LV_EVENT_CLICKED, nullptr);
    }
}

lv_obj_t *createSettingsScreen()
{
    lv_obj_t *screen = lv_obj_create(nullptr);

    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "settings");
    lv_obj_set_style_text_color(title, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_set_style_text_font(title, &science_gothic_18, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    createBrightnessButton(screen);

    return screen;
}