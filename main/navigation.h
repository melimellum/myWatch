#pragma once

#include <stdint.h>
#include "lvgl.h"

enum class ScreenId : uint8_t
{
    Clock,
    Settings,
    Brightness,
    Apps,
    Stopwatch,
    Count,          
    None = 0xFF     
};

void navigationInit();

void navigateTo(ScreenId id, lv_dir_t direction = LV_DIR_NONE);