#include "stopwatch_screen.h"

#include <stdio.h>
#include "esp_timer.h"
#include "time_digits.h"

extern "C" {
    extern const lv_font_t science_gothic_48;
    extern const lv_font_t science_gothic_18;
}

namespace
{
    constexpr uint32_t COLOR_BACKGROUND = 0x000000;
    constexpr uint32_t COLOR_TEXT       = 0x969696;
    constexpr uint32_t COLOR_ACCENT     = 0x00897B;
    constexpr uint32_t COLOR_STOP       = 0xB23B3B;

    constexpr uint32_t UPDATE_MS        = 50;    
    constexpr int      BUTTON_WIDTH     = 100;
    constexpr int      BUTTON_HEIGHT    = 44;
    constexpr int      BUTTONS_OFFSET_Y = -34;   

    // Стан секундоміра
    bool    running       = false;
    int64_t startUs       = 0;   
    int64_t accumulatedUs = 0;   

    lv_obj_t   *minSecLabel = nullptr;
    lv_obj_t   *centiLabel  = nullptr;
    lv_obj_t   *startButton = nullptr;
    lv_obj_t   *startLabel  = nullptr;
    lv_timer_t *updateTimer = nullptr;

    int lastMinSec = -1;
    int lastCenti  = -1;

    int64_t elapsedUs()
    {
        int64_t total = accumulatedUs;
        if (running)
        {
            total += esp_timer_get_time() - startUs;
        }
        return total;
    }

    void refresh()
    {
        int64_t totalCenti = elapsedUs() / 10000;

        int centi   = static_cast<int>(totalCenti % 100);
        int seconds = static_cast<int>((totalCenti / 100) % 60);
        int minutes = static_cast<int>((totalCenti / 6000) % 100);   

        int minSec = minutes * 100 + seconds;
        char buffer[8];

        if (minSec != lastMinSec)
        {
            lastMinSec = minSec;
            snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
            setDigitRowText(minSecLabel, buffer);
        }

        if (centi != lastCenti)
        {
            lastCenti = centi;
            snprintf(buffer, sizeof(buffer), "%02d", centi);
            setDigitRowText(centiLabel, buffer);
        }
    }

    void updateStartButton()
    {
        uint32_t color = running ? COLOR_STOP : COLOR_ACCENT;

        lv_label_set_text(startLabel, running ? "stop" : "start");
        lv_obj_set_style_border_color(startButton, lv_color_hex(color), 0);
        lv_obj_set_style_text_color(startLabel, lv_color_hex(color), 0);
    }

    void onTimer(lv_timer_t *)
    {
        refresh();
    }

    void onStartClicked(lv_event_t *)
    {
        if (running)
        {
            accumulatedUs += esp_timer_get_time() - startUs;
            running = false;
            lv_timer_pause(updateTimer);
        }
        else
        {
            startUs = esp_timer_get_time();
            running = true;
            lv_timer_resume(updateTimer);
        }

        updateStartButton();
        refresh();
    }

    void onResetClicked(lv_event_t *)
    {
        running = false;
        accumulatedUs = 0;
        lv_timer_pause(updateTimer);

        lastMinSec = lastCenti = -1;
        updateStartButton();
        refresh();
    }

    void onScreenLoaded(lv_event_t *)
    {
        refresh();
        if (running)
        {
            lv_timer_resume(updateTimer);
        }
    }

    void onScreenUnloaded(lv_event_t *)
    {
        lv_timer_pause(updateTimer);
    }

    lv_obj_t *createButton(lv_obj_t *parent, const char *text,
                           uint32_t color, lv_event_cb_t onClick,
                           lv_obj_t **outLabel)
    {
        lv_obj_t *button = lv_button_create(parent);

        lv_obj_set_size(button, BUTTON_WIDTH, BUTTON_HEIGHT);
        lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(button, 2, 0);
        lv_obj_set_style_border_color(button, lv_color_hex(color), 0);
        lv_obj_set_style_shadow_width(button, 0, 0);

        lv_obj_set_style_transform_width(button, 0, LV_STATE_PRESSED);
        lv_obj_set_style_transform_height(button, 0, LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(button, lv_color_hex(color), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(button, LV_OPA_20, LV_STATE_PRESSED);

        lv_obj_t *label = lv_label_create(button);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
        lv_obj_set_style_text_font(label, &science_gothic_18, 0);
        lv_obj_center(label);

        lv_obj_add_event_cb(button, onClick, LV_EVENT_CLICKED, nullptr);

        if (outLabel != nullptr)
        {
            *outLabel = label;
        }

        return button;
    }

    lv_obj_t *createRow(lv_obj_t *parent, int columnGap)
    {
        lv_obj_t *row = lv_obj_create(parent);

        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 0, 0);
        lv_obj_set_style_pad_column(row, columnGap, 0);
        lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_END,
            LV_FLEX_ALIGN_CENTER);

        return row;
    }
}

lv_obj_t *createStopwatchScreen()
{
    lv_obj_t *screen = lv_obj_create(nullptr);

    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *timeRow = createRow(screen, 8);
    lv_obj_set_size(timeRow, 230, 80);
    lv_obj_align(timeRow, LV_ALIGN_CENTER, 0, -30);

    minSecLabel = createDigitRow(timeRow, &science_gothic_48, COLOR_TEXT, "00:00");

    centiLabel = createDigitRow(timeRow, &science_gothic_18, COLOR_TEXT, "00");
    lv_obj_set_style_translate_y(centiLabel, -5, 0);

    lv_obj_t *buttonRow = createRow(screen, 12);
    lv_obj_set_size(buttonRow, 2 * BUTTON_WIDTH + 12, BUTTON_HEIGHT);
    lv_obj_align(buttonRow, LV_ALIGN_BOTTOM_MID, 0, BUTTONS_OFFSET_Y);
    lv_obj_set_flex_align(buttonRow,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    startButton = createButton(buttonRow, "start", COLOR_ACCENT, onStartClicked, &startLabel);
    createButton(buttonRow, "reset", COLOR_TEXT, onResetClicked, nullptr);

    updateTimer = lv_timer_create(onTimer, UPDATE_MS, nullptr);
    lv_timer_pause(updateTimer);

    lv_obj_add_event_cb(screen, onScreenLoaded,   LV_EVENT_SCREEN_LOADED,   nullptr);
    lv_obj_add_event_cb(screen, onScreenUnloaded, LV_EVENT_SCREEN_UNLOADED, nullptr);

    lastMinSec = lastCenti = -1;
    updateStartButton();
    refresh();

    return screen;
}