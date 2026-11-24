#include "apps_screen.h"

#include "navigation.h"

extern "C" {
    extern const lv_font_t science_gothic_18;
}

namespace
{
    constexpr uint32_t COLOR_BACKGROUND = 0x000000;
    constexpr uint32_t COLOR_TEXT       = 0x969696;
    constexpr uint32_t COLOR_ACCENT     = 0x00897B;

    constexpr int ICON_SIZE   = 48;
    constexpr int ICON_MARGIN = 20;   

    void onStopwatchClicked(lv_event_t *)
    {
        navigateTo(ScreenId::Stopwatch, LV_DIR_LEFT);
    }

    void createStopwatchIcon(lv_obj_t *parent)
    {
        lv_obj_t *button = lv_button_create(parent);

        lv_obj_set_size(button, ICON_SIZE, ICON_SIZE);
        lv_obj_align(button, LV_ALIGN_TOP_LEFT, ICON_MARGIN, ICON_MARGIN);

        lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(button, 2, 0);
        lv_obj_set_style_border_color(button, lv_color_hex(COLOR_ACCENT), 0);
        lv_obj_set_style_shadow_width(button, 0, 0);

        lv_obj_t *hand = lv_obj_create(button);
        lv_obj_set_size(hand, 3, 14);
        lv_obj_align(hand, LV_ALIGN_CENTER, 0, -5);
        lv_obj_set_style_radius(hand, 2, 0);
        lv_obj_set_style_bg_color(hand, lv_color_hex(COLOR_TEXT), 0);
        lv_obj_set_style_bg_opa(hand, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(hand, 0, 0);
        lv_obj_remove_flag(hand, LV_OBJ_FLAG_CLICKABLE);   

        lv_obj_add_event_cb(button, onStopwatchClicked, LV_EVENT_CLICKED, nullptr);
    }
}

lv_obj_t *createAppsScreen()
{
    lv_obj_t *screen = lv_obj_create(nullptr);

    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    createStopwatchIcon(screen);

    return screen;
}