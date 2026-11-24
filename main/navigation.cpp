#include "navigation.h"

#include "clock_screen_one.h"
#include "settings_screen.h"
#include "brightness_screen.h"

namespace
{
    constexpr uint32_t ANIM_MS = 250;

    struct Screen
    {
        lv_obj_t *(*create)();   
        ScreenId swipeLeft;      
        ScreenId swipeRight;     
        lv_obj_t *obj;           
    };

    Screen screens[] = {
        /* Clock      */ { createClockScreen,      ScreenId::Settings, ScreenId::None,     nullptr },
        /* Settings   */ { createSettingsScreen,   ScreenId::None,     ScreenId::Clock,    nullptr },
        /* Brightness */ { createBrightnessScreen, ScreenId::None,     ScreenId::Settings, nullptr },
    };

    static_assert(sizeof(screens) / sizeof(screens[0]) == static_cast<size_t>(ScreenId::Count),
                  "screens[] must match ScreenId enum");

    ScreenId current = ScreenId::Clock;

    Screen &get(ScreenId id)
    {
        return screens[static_cast<size_t>(id)];
    }

    void onGesture(lv_event_t *e)
    {
        lv_indev_t *indev = lv_event_get_indev(e);
        if (indev == nullptr)
        {
            return;
        }

        lv_dir_t dir = lv_indev_get_gesture_dir(indev);
        ScreenId target = ScreenId::None;

        if (dir == LV_DIR_LEFT)
        {
            target = get(current).swipeLeft;
        }
        else if (dir == LV_DIR_RIGHT)
        {
            target = get(current).swipeRight;
        }

        if (target != ScreenId::None)
        {
            navigateTo(target, dir);
        }
    }

    lv_obj_t *ensureCreated(ScreenId id)
    {
        Screen &screen = get(id);

        if (screen.obj == nullptr)
        {
            screen.obj = screen.create();

            if (screen.obj != nullptr)
            {
                lv_obj_add_event_cb(screen.obj, onGesture, LV_EVENT_GESTURE, nullptr);
            }
        }

        return screen.obj;
    }
}

void navigationInit()
{
    current = ScreenId::Clock;

    lv_obj_t *obj = ensureCreated(current);
    if (obj != nullptr)
    {
        lv_screen_load(obj);
    }
}

void navigateTo(ScreenId id, lv_dir_t direction)
{
    if (id >= ScreenId::Count || id == current)
    {
        return;
    }

    lv_obj_t *obj = ensureCreated(id);
    if (obj == nullptr)
    {
        return;
    }

    current = id;

    switch (direction)
    {
    case LV_DIR_LEFT:
        lv_screen_load_anim(obj, LV_SCREEN_LOAD_ANIM_MOVE_LEFT, ANIM_MS, 0, false);
        break;

    case LV_DIR_RIGHT:
        lv_screen_load_anim(obj, LV_SCREEN_LOAD_ANIM_MOVE_RIGHT, ANIM_MS, 0, false);
        break;

    default:
        lv_screen_load(obj);
        break;
    }
}