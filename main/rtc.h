#pragma once

#include <stdint.h>
#include "esp_err.h"

struct RTC_Time
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    uint8_t day;
    uint8_t month;
    uint8_t year;      
    uint8_t weekday;   
};

esp_err_t initRTC();
esp_err_t setRTC(const RTC_Time *time);

esp_err_t readRTC(RTC_Time *time, bool *oscillatorStopped = nullptr);

bool rtcTimeIsValid(const RTC_Time *time);