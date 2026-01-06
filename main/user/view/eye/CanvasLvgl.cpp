#include "CanvasLvgl.h"
#include "ICanvasManager.h"
#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static const char *TAG = "CanvasLvgl";
static lv_draw_rect_dsc_t rect_dsc;
lv_draw_triangle_dsc_t triangle_dsc;
lv_draw_line_dsc_t line_dsc;

static inline lv_color_t lv_color_from565(uint16_t c)
{
    uint8_t r = (c >> 11) & 0x1F; // 5 bit red
    uint8_t g = (c >> 5) & 0x3F;  // 6 bit green
    uint8_t b = c & 0x1F;         // 5 bit blue

    r = (r * 527 + 23) >> 6; // convert 5-bit → 8-bit
    g = (g * 259 + 33) >> 6; // convert 6-bit → 8-bit
    b = (b * 527 + 23) >> 6; // convert 5-bit → 8-bit
    return lv_color_make(r, g, b);
}

static inline bool _adjust_abs(int32_t &x, int32_t &w)
{
    if (w < 0)
    {
        x += w;
        w = -w;
    }
    return !w;
}

CanvasLvgl::CanvasLvgl(ICanvasManager *canvasManager, int id)
    : _canvasManager(canvasManager), _id(id)
{
    _clip_r = width();
    _clip_b = height();
    lv_draw_line_dsc_init(&line_dsc);
    lv_draw_triangle_dsc_init(&triangle_dsc);
    lv_draw_rect_dsc_init(&rect_dsc);
}

bool CanvasLvgl::clipping(int32_t &x, int32_t &y, int32_t &w, int32_t &h)
{
    auto cl = _clip_l;
    if (x < cl)
    {
        w += x - cl;
        x = cl;
    }
    auto cr = _clip_r + 1 - x;
    if (w > cr)
        w = cr;
    if (w < 1)
        return false;

    auto ct = _clip_t;
    if (y < ct)
    {
        h += y - ct;
        y = ct;
    }
    auto cb = _clip_b + 1 - y;
    if (h > cb)
        h = cb;
    if (h < 1)
        return false;

    return true;
}

void CanvasLvgl::drawFillRectangle(int32_t x0, int32_t y0,
                                   int32_t w, int32_t h,
                                   uint16_t color)
{

    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        ESP_LOGI(TAG, "canvas is null");
        return;
    }
    _adjust_abs(x0, w);
    _adjust_abs(y0, h);
    if (color == 0)
        rect_dsc.bg_color = lv_color_white();
    else if (color == 1)
        rect_dsc.bg_color = lv_color_black();
    lv_area_t coords_rect = {x0, y0, x0 + w, y0 + h};
    lv_layer_t *layer = (lv_layer_t *)_canvasManager->getCanvasBuffer(_id);
    lv_draw_rect(layer, &rect_dsc, &coords_rect);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void CanvasLvgl::drawFillRectangleRound(int32_t x0, int32_t y0,
                                        int32_t w, int32_t h, int32_t radius,
                                        uint16_t color)
{

    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        ESP_LOGI(TAG, "canvas is null");
        return;
    }
    _adjust_abs(x0, w);
    _adjust_abs(y0, h);
    if (color == 0)
        rect_dsc.bg_color = lv_color_white();
    else if (color == 1)
        rect_dsc.bg_color = lv_color_black();
    rect_dsc.radius = radius;
    lv_area_t coords_rect = {x0, y0, x0 + w, y0 + h};
    lv_layer_t *layer = (lv_layer_t *)_canvasManager->getCanvasBuffer(_id);
    lv_draw_rect(layer, &rect_dsc, &coords_rect);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void CanvasLvgl::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{

    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        ESP_LOGI(TAG, "canvas is null");
        return;
    }
    if (color == 0)
        line_dsc.color = lv_color_white();
    else if (color == 1)
        line_dsc.color = lv_color_black();
    line_dsc.width = 1;
    line_dsc.p1.x = x0;
    line_dsc.p1.y = y0;
    line_dsc.p2.x = x1;
    line_dsc.p2.y = y1;
    lv_layer_t *layer = (lv_layer_t *)_canvasManager->getCanvasBuffer(_id);
    lv_draw_line(layer, &line_dsc);
    vTaskDelay(pdMS_TO_TICKS(5));
}
void CanvasLvgl::drawFillTriangle(int32_t x0, int32_t y0,
                                  int32_t x1, int32_t y1,
                                  int32_t x2, int32_t y2,
                                  uint16_t color)
{
    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        ESP_LOGI(TAG, "canvas is null");
        return;
    }
    if (color == 0)
        triangle_dsc.color = lv_color_white();
    else if (color == 1)
        triangle_dsc.color = lv_color_black();
    line_dsc.width = 1;
    triangle_dsc.p[0].x = x0;
    triangle_dsc.p[0].y = y0;
    triangle_dsc.p[1].x = x1;
    triangle_dsc.p[1].y = y1;
    triangle_dsc.p[2].x = x2;
    triangle_dsc.p[2].y = y2;
    lv_layer_t *layer = (lv_layer_t *)_canvasManager->getCanvasBuffer(_id);
    lv_draw_triangle(layer, &triangle_dsc);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void CanvasLvgl::drawFastHLine(int32_t x, int32_t y, int32_t w,
                               uint16_t color)
{
    drawLine(x, y, x + w, y, color);
}

void CanvasLvgl::clear(uint16_t color)
{
    // int w = width();
    // int h = height();
    // if (w < 0 || h < 0)
    // {
    //     Serial.println("clear failed ");
    //     return;
    // }
    // drawFillRectangle(0, 0, 120, 120, 0);
}
void CanvasLvgl::push(int x, int y)
{
    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
        return;
    lv_layer_t *layer = (lv_layer_t *)_canvasManager->getCanvasBuffer(_id);
    if (!layer)
        return;
    lv_obj_set_pos(canvas, x, y);
    lv_canvas_finish_layer(canvas, layer);
}
int32_t CanvasLvgl::width() const
{
    return _canvasManager->getWidth(_id);
}

int32_t CanvasLvgl::height() const
{
    return _canvasManager->getHeight(_id);
}

void CanvasLvgl::setBackgroundColor(uint16_t color)
{

    // lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    // if (!canvas)
    //     return;
    // lv_canvas_set_palette(canvas, 0, lv_color_from565(color));
    // _bgColor565 = color;
    // lv_color_t c;
    // c.full = 0;
    // lv_canvas_fill_bg(canvas, c, LV_OPA_COVER);
}
void CanvasLvgl::setForegroundColor(uint16_t color)
{
    // lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    // if (!canvas)
    //     return;
    // lv_canvas_set_palette(canvas, 1, lv_color_from565(color));
    // _fgColor565 = color;
}