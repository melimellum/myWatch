#include "bsp/esp32_s3_touch_lcd_1_69.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rtc.h"
#include "clock_screen_one.h"
#include "navigation.h"
#include "brightness_screen.h"

static const char *TAG = "WATCH";

static constexpr int      DISPLAY_BRIGHTNESS = 40;
static constexpr uint32_t CLOCK_POLL_MS      = 250;   
static constexpr int      LOCK_TIMEOUT_MS    = 100;   
static constexpr int      RTC_FAIL_LIMIT     = 5;     
static constexpr uint32_t TASK_STACK_BYTES   = 4096;
static constexpr UBaseType_t TASK_PRIORITY   = 3;

static constexpr bool FORCE_SET_TIME = false;

static const RTC_Time DEFAULT_TIME = {
    .hours = 12,
    .minutes = 30,
    .seconds = 0,
    .day = 12,
    .month = 12,
    .year = 26,
    .weekday = 6     
};

static esp_err_t setupRTC()
{
    esp_err_t err = initRTC();
    if (err != ESP_OK)
    {
        return err;
    }

    RTC_Time time;
    bool oscillatorStopped = false;

    err = readRTC(&time, &oscillatorStopped);
    if (err != ESP_OK)
    {
        return err;
    }

    if (FORCE_SET_TIME || oscillatorStopped || !rtcTimeIsValid(&time))
    {
        ESP_LOGW(TAG, "Setting default time");
        err = setRTC(&DEFAULT_TIME);
    }

    return err;
}

static void clockTask(void *)
{
    RTC_Time time;
    int lastSecond = -1;
    int failCount = 0;

    TickType_t lastWake = xTaskGetTickCount();

    while (true)
    {
        esp_err_t err = readRTC(&time);

        if (err != ESP_OK)
        {
            failCount++;
            if (failCount == RTC_FAIL_LIMIT)
            {
                ESP_LOGE(TAG, "RTC read failed %d times: %s",
                         RTC_FAIL_LIMIT, esp_err_to_name(err));
            }
        }
        else
        {
            failCount = 0;

            if (time.seconds != lastSecond)
            {
                if (bsp_display_lock(LOCK_TIMEOUT_MS))
                {
                    updateClockScreen(&time);
                    bsp_display_unlock();

                    lastSecond = time.seconds;
                }
            }
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(CLOCK_POLL_MS));
    }
}

extern "C" void app_main(void)
{
    bsp_display_start();
    applyBrightness(DISPLAY_BRIGHTNESS); 

    esp_err_t err = setupRTC();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "RTC setup error: %s", esp_err_to_name(err));
    }

    bsp_display_lock(0);
    navigationInit();
    bsp_display_unlock();

    BaseType_t ok = xTaskCreate(clockTask, "clock", TASK_STACK_BYTES,
                                nullptr, TASK_PRIORITY, nullptr);
    if (ok != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create clock task");
    }
}