#include "lcd.h"

void lcd_init()
{
    tft180_set_dir(TFT180_PORTAIT);
    tft180_set_color(DEFAULT_PEN_COLOR, DEFAULT_BACKGROUND_COLOR);
    tft180_init();
}

void lcd_clear()
{
    tft180_clear();
}

void lcd_show_string(vuint16 x, vuint16 y, const char *dat)
{
    tft180_show_string(x * CHAR_WIDTH, y * CHAR_HEIGTH, dat);
}

void lcd_show_string_color(vuint16 x,
                           vuint16 y,
                           const char *dat,
                           vuint16 pen_color,
                           vuint16 background_color)
{
    tft180_set_color(pen_color, background_color);
    lcd_show_string(x, y, dat);
    tft180_set_color(DEFAULT_PEN_COLOR, DEFAULT_BACKGROUND_COLOR);
}
void lcd_show_vuint(vuint16 x, vuint16 y, vuint32 dat, vuint8 num)
{
    tft180_show_vuint(x * CHAR_WIDTH, y * CHAR_HEIGTH, dat, num);
}

void lcd_show_vuint_color(vuint16 x,
                          vuint16 y,
                          vuint32 dat,
                          vuint8 num,
                          vuint16 pen_color,
                          vuint16 background_color)
{
    tft180_set_color(pen_color, background_color);
    lcd_show_vuint(x, y, dat, num);
    tft180_set_color(DEFAULT_PEN_COLOR, DEFAULT_BACKGROUND_COLOR);
}

void lcd_show_int(vuint16 x, vuint16 y, int32 dat, vuint8 num)
{
    tft180_show_int(x * CHAR_WIDTH, y * CHAR_HEIGTH, dat, num);
}

void lcd_show_int_color(vuint16 x,
                        vuint16 y,
                        int32 dat,
                        vuint8 num,
                        vuint16 pen_color,
                        vuint16 background_color)
{
    tft180_set_color(pen_color, background_color);
    lcd_show_int(x, y, dat, num);
    tft180_set_color(DEFAULT_PEN_COLOR, DEFAULT_BACKGROUND_COLOR);
}

void lcd_show_float(vuint16 x,
                    vuint16 y,
                    const double dat,
                    vuint8 num,
                    vuint8 pointnum)
{
    tft180_show_float(x * CHAR_WIDTH, y * CHAR_HEIGTH, dat, num, pointnum);
}

void lcd_show_float_color(vuint16 x,
                          vuint16 y,
                          const double dat,
                          vuint8 num,
                          vuint8 pointnum,
                          vuint16 pen_color,
                          vuint16 background_color)
{
    tft180_set_color(pen_color, background_color);
    lcd_show_float(x, y, dat, num, pointnum);
    tft180_set_color(DEFAULT_PEN_COLOR, DEFAULT_BACKGROUND_COLOR);
}

void lcd_show_char(vuint16 x, vuint16 y, char dat)
{
    tft180_show_char(x * CHAR_WIDTH, y * CHAR_HEIGTH, dat);
}

void lcd_show_char_color(vuint16 x,
                         vuint16 y,
                         char dat,
                         vuint16 pen_color,
                         vuint16 background_color)
{
    tft180_set_color(pen_color, background_color);
    lcd_show_char(x, y, dat);
    tft180_set_color(DEFAULT_PEN_COLOR, DEFAULT_BACKGROUND_COLOR);
}

// 居中展示图像
void lcd_show_image_mid(const vuint8 *img,
                        vuint16 width,
                        vuint16 height,
                        vuint8 threshold)
{
    float scale_w = (float)TFT180_WIDTH_MAX / width;
    float scale_h = (float)TFT180_HEIGHT_MAX / height;

    float scale = scale_w < scale_h ? scale_w : scale_h;

    vuint16 display_w = (vuint16)(width * scale);
    vuint16 display_h = (vuint16)(height * scale);

    vuint16 start_x = (TFT180_WIDTH_MAX - display_w) / 2;
    vuint16 start_y = (TFT180_HEIGHT_MAX - display_h) / 2;

    tft180_show_gray_image(start_x, start_y, img, width, height,
                           display_w, display_h, threshold);
}

void lcd_show_image(const vuint8 *img,
                    vuint16 width,
                    vuint16 height,
                    vuint8 threshold)
{
    float scale_w = (float)TFT180_WIDTH_MAX / width;
    float scale_h = (float)TFT180_HEIGHT_MAX / height;

    float scale = scale_w < scale_h ? scale_w : scale_h;

    vuint16 display_w = (vuint16)(width * scale);
    vuint16 display_h = (vuint16)(height * scale);

    vuint16 start_x = (TFT180_WIDTH_MAX - display_w) / 2;

    tft180_show_gray_image(start_x, 0, img, width, height,
                           display_w, 112, threshold);
}