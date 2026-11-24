#include "clock_screen_one.h"

#include <stdio.h>
#include "lvgl.h"

extern "C" {
    extern const lv_font_t science_gothic_48;
    extern const lv_font_t science_gothic_18;
}

namespace
{
    constexpr uint32_t COLOR_BACKGROUND = 0x000000;
    constexpr uint32_t COLOR_TEXT       = 0x969696;
    constexpr uint32_t COLOR_ACCENT     = 0x00897B;

    constexpr int TIME_ROW_WIDTH      = 230;
    constexpr int TIME_ROW_HEIGHT     = 80;
    constexpr int SECONDS_LABEL_WIDTH = 32;
    constexpr int INFO_ROW_OFFSET_Y   = 55;

    const char *const WEEKDAYS[] = {
        "sunday", "monday", "tuesday", "wednesday",
        "thursday", "friday", "saturday"
    };

    const char *const MONTHS[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    lv_obj_t *timeLabel    = nullptr;
    lv_obj_t *secondsLabel = nullptr;
    lv_obj_t *dateLabel    = nullptr;
    lv_obj_t *weekdayLabel = nullptr;

    int lastSeconds = -1;
    int lastMinutes = -1;
    int lastHours   = -1;
    int lastDay     = -1;
    int lastMonth   = -1;
    int lastWeekday = -1;

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
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER);

        return row;
    }

    lv_obj_t *createLabel(lv_obj_t *parent, const lv_font_t *font,
                          uint32_t color, const char *text)
    {
        lv_obj_t *label = lv_label_create(parent);

        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
        lv_obj_set_style_text_font(label, font, 0);

        return label;
    }
}

void createClockScreen()
{
    lv_obj_t *screen = lv_screen_active();

    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *timeRow = createRow(screen, 8);
    lv_obj_set_size(timeRow, TIME_ROW_WIDTH, TIME_ROW_HEIGHT);
    lv_obj_align(timeRow, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_flex_align(timeRow,
        LV_FLEX_ALIGN_CENTER,   
        LV_FLEX_ALIGN_END,      
        LV_FLEX_ALIGN_CENTER);  

    timeLabel = createLabel(timeRow, &science_gothic_48, COLOR_TEXT, "--:--");

    secondsLabel = createLabel(timeRow, &science_gothic_18, COLOR_TEXT, "--");
    lv_obj_set_width(secondsLabel, SECONDS_LABEL_WIDTH);
    lv_obj_set_style_text_align(secondsLabel, LV_TEXT_ALIGN_LEFT, 0);

    lv_obj_set_style_translate_y(secondsLabel, -5, 0);

    lv_obj_t *infoRow = createRow(screen, 12);
    lv_obj_set_size(infoRow, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(infoRow, LV_ALIGN_CENTER, 0, INFO_ROW_OFFSET_Y);

    dateLabel    = createLabel(infoRow, &science_gothic_18, COLOR_TEXT,   "-- ---");
    weekdayLabel = createLabel(infoRow, &science_gothic_18, COLOR_ACCENT, "---");

    lastSeconds = lastMinutes = lastHours = -1;
    lastDay = lastMonth = lastWeekday = -1;
}

void updateClockScreen(const RTC_Time *time)
{
    if (time == nullptr || timeLabel == nullptr)
    {
        return;
    }

    char buffer[16];

    if (time->hours != lastHours || time->minutes != lastMinutes)
    {
        lastHours   = time->hours;
        lastMinutes = time->minutes;

        snprintf(buffer, sizeof(buffer), "%02d:%02d", time->hours, time->minutes);
        lv_label_set_text(timeLabel, buffer);
    }

    if (time->seconds != lastSeconds)
    {
        lastSeconds = time->seconds;

        snprintf(buffer, sizeof(buffer), "%02d", time->seconds);
        lv_label_set_text(secondsLabel, buffer);
    }

    if (time->day != lastDay || time->month != lastMonth)
    {
        lastDay   = time->day;
        lastMonth = time->month;

        if (time->month >= 1 && time->month <= 12)
        {
            snprintf(buffer, sizeof(buffer), "%d %s",
                     time->day, MONTHS[time->month - 1]);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "-- ---");
        }

        lv_label_set_text(dateLabel, buffer);
    }

    if (time->weekday != lastWeekday)
    {
        lastWeekday = time->weekday;

        lv_label_set_text(weekdayLabel,
            time->weekday <= 6 ? WEEKDAYS[time->weekday] : "---");
    }
}