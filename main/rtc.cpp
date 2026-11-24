#include "rtc.h"

#include "bsp/esp32_s3_touch_lcd_1_69.h"
#include "driver/i2c_master.h"

static constexpr uint8_t  RTC_ADDRESS      = 0x51;
static constexpr uint8_t  RTC_REG_SECONDS  = 0x04;
static constexpr uint8_t  RTC_OS_FLAG      = 0x80;   
static constexpr uint32_t RTC_I2C_SPEED_HZ = 100000;
static constexpr int      RTC_TIMEOUT_MS   = 100;

static i2c_master_dev_handle_t rtcDevice = nullptr;

esp_err_t initRTC()
{
    if (rtcDevice != nullptr)
    {
        return ESP_OK;
    }

    i2c_master_bus_handle_t bus = bsp_i2c_get_handle();

    if (bus == nullptr)
    {
        return ESP_FAIL;
    }

    i2c_device_config_t config = {};
    config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    config.device_address  = RTC_ADDRESS;
    config.scl_speed_hz    = RTC_I2C_SPEED_HZ;

    return i2c_master_bus_add_device(bus, &config, &rtcDevice);
}

static uint8_t bcdToDec(uint8_t value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}

static uint8_t decToBcd(uint8_t value)
{
    return ((value / 10) << 4) | (value % 10);
}

bool rtcTimeIsValid(const RTC_Time *time)
{
    if (time == nullptr)
    {
        return false;
    }

    return time->hours   <= 23 &&
           time->minutes <= 59 &&
           time->seconds <= 59 &&
           time->day     >= 1  && time->day   <= 31 &&
           time->month   >= 1  && time->month <= 12 &&
           time->year    <= 99 &&
           time->weekday <= 6;
}

esp_err_t setRTC(const RTC_Time *time)
{
    if (time == nullptr || !rtcTimeIsValid(time))
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (rtcDevice == nullptr)
    {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t data[8];

    data[0] = RTC_REG_SECONDS;
    data[1] = decToBcd(time->seconds);
    data[2] = decToBcd(time->minutes);
    data[3] = decToBcd(time->hours);
    data[4] = decToBcd(time->day);
    data[5] = decToBcd(time->weekday);
    data[6] = decToBcd(time->month);
    data[7] = decToBcd(time->year);

    return i2c_master_transmit(rtcDevice, data, sizeof(data), RTC_TIMEOUT_MS);
}

esp_err_t readRTC(RTC_Time *time, bool *oscillatorStopped)
{
    if (time == nullptr)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (rtcDevice == nullptr)
    {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t reg = RTC_REG_SECONDS;
    uint8_t data[7];

    esp_err_t result = i2c_master_transmit_receive(
        rtcDevice, &reg, 1, data, sizeof(data), RTC_TIMEOUT_MS);

    if (result != ESP_OK)
    {
        return result;
    }

    if (oscillatorStopped != nullptr)
    {
        *oscillatorStopped = (data[0] & RTC_OS_FLAG) != 0;
    }

    time->seconds = bcdToDec(data[0] & 0x7F);
    time->minutes = bcdToDec(data[1] & 0x7F);
    time->hours   = bcdToDec(data[2] & 0x3F);

    time->day     = bcdToDec(data[3] & 0x3F);
    time->weekday = bcdToDec(data[4] & 0x07);
    time->month   = bcdToDec(data[5] & 0x1F);
    time->year    = bcdToDec(data[6]);

    return ESP_OK;
}