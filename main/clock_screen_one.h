#pragma once

#include "lvgl.h"
#include "rtc.h"

lv_obj_t *createClockScreen();
void updateClockScreen(const RTC_Time *time);