#include <stdio.h>
#include "bsp/esp32_s3_touch_lcd_1_69.h"

extern "C" void app_main(void)
{
    bsp_display_start();

    bsp_display_brightness_set(100);

    bsp_display_lock(0);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello World!");
    lv_obj_center(label);

    bsp_display_unlock();
}