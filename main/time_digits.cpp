#include "time_digits.h"

namespace
{
    int maxDigitWidth(const lv_font_t *font)
    {
        int width = 0;

        for (char c = '0'; c <= '9'; ++c)
        {
            int glyphWidth = lv_font_get_glyph_width(font, c, 0);
            if (glyphWidth > width)
            {
                width = glyphWidth;
            }
        }

        return width;
    }
}

lv_obj_t *createDigitRow(lv_obj_t *parent, const lv_font_t *font,
                         uint32_t color, const char *pattern)
{
    lv_obj_t *row = lv_obj_create(parent);

    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_column(row, 0, 0);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    int digitWidth = maxDigitWidth(font);

    for (const char *p = pattern; *p != '\0'; ++p)
    {
        char text[2] = { *p, '\0' };

        lv_obj_t *label = lv_label_create(row);
        lv_label_set_text(label, text);
        lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);   
        lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
        lv_obj_set_style_text_font(label, font, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        bool isDigit = (*p >= '0' && *p <= '9');
        int width = isDigit ? digitWidth : lv_font_get_glyph_width(font, *p, 0);
        lv_obj_set_width(label, width);
    }

    return row;
}

void setDigitRowText(lv_obj_t *row, const char *text)
{
    int32_t count = static_cast<int32_t>(lv_obj_get_child_count(row));

    for (int32_t i = 0; i < count && text[i] != '\0'; ++i)
    {
        lv_obj_t *label = lv_obj_get_child(row, i);

        if (lv_label_get_text(label)[0] != text[i])
        {
            char single[2] = { text[i], '\0' };
            lv_label_set_text(label, single);
        }
    }
}