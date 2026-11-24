idf_component_register(SRCS "brightness_screen.cpp" "settings_screen.cpp" "navigation.cpp" "clock_screen_one.cpp" "main.cpp" "rtc.cpp" "fonts/science_gothic_48.c" "fonts/science_gothic_18.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp32_s3_touch_lcd_1_69)
