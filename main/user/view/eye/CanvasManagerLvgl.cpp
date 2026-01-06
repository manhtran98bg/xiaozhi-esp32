#include "CanvasManagerLvgl.h"
#include "CanvasLvgl.h"
#include "lvgl.h"
#include <stdlib.h>
#include "esp_log.h"

static const char *TAG = "CanvasManagerLvgl";
CanvasManagerLvgl::CanvasManagerLvgl()
{

    for (int i = 0; i < MAX_CANVAS; i++)
    {
        _entry[i].obj = nullptr;
        _entry[i].buffer = nullptr;
        _entry[i].w = 0;
        _entry[i].h = 0;
        _entry[i].format = (int)LV_COLOR_FORMAT_RGB565;
        _canvasWrapper[i] = nullptr;
    }
}

CanvasManagerLvgl::~CanvasManagerLvgl()
{
    for (int i = 0; i < MAX_CANVAS; i++)
    {
        if (_entry[i].obj)
            lv_obj_del((lv_obj_t *)_entry[i].obj);

        if (_entry[i].buffer)
            free(_entry[i].buffer);

        if (_canvasWrapper[i])
        {
            delete _canvasWrapper[i];
            _canvasWrapper[i] = nullptr;
        }
    }
}

bool CanvasManagerLvgl::valid(int id)
{
    return id >= 0 && id < MAX_CANVAS && _entry[id].obj != nullptr;
}
int CanvasManagerLvgl::createCanvas(int width, int height, int colorFormat)
{
    for (int i = 0; i < MAX_CANVAS; i++)
    {
        if (_entry[i].obj == nullptr)
        {
            LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 128, 64, LV_COLOR_FORMAT_RGB565);
            LV_DRAW_BUF_INIT_STATIC(draw_buf);
            
            // lv_draw_buf_t *draw_buf = lv_draw_buf_create(width, height, LV_COLOR_FORMAT_RGB565, 0);
            lv_obj_t *canvas = lv_canvas_create(lv_scr_act());
            if (!canvas)
            {
               ESP_LOGI(TAG, "lv_canvas_create failed, returned NULL");
                return -1;
            }
            lv_layer_t *layer = (lv_layer_t*)malloc(sizeof(lv_layer_t));
            if (!layer)
                return -1;
            
            lv_canvas_set_draw_buf(canvas, &draw_buf);
            lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
            lv_canvas_init_layer(canvas, layer);
            _entry[i].obj = canvas;
            _entry[i].buffer = (void*)layer;
            _entry[i].w = width;
            _entry[i].h = height;
            _entry[i].format = colorFormat;
            _canvasWrapper[i] = new CanvasLvgl(this, i);
            return i;
        }
    }
    return -1;
}

void CanvasManagerLvgl::deleteCanvas(int id)
{
    if (!valid(id))
        return;

    lv_obj_del((lv_obj_t *)_entry[id].obj);
    if (_entry[id].buffer)
    {
        free(_entry[id].buffer);
        _entry[id].buffer = nullptr;
    }

    _entry[id].obj = nullptr;
    _entry[id].buffer = nullptr;
    _entry[id].w = 0;
    _entry[id].h = 0;
    _entry[id].format = (int)LV_COLOR_FORMAT_RGB565;
    if (_canvasWrapper[id])
    {
        delete _canvasWrapper[id];
        _canvasWrapper[id] = nullptr;
    }
}

void *CanvasManagerLvgl::getCanvas(int id)
{
    return valid(id) ? _entry[id].obj : nullptr;
}

int CanvasManagerLvgl::getWidth(int id)
{
    return valid(id) ? _entry[id].w : -1;
}

int CanvasManagerLvgl::getHeight(int id)
{
    return valid(id) ? _entry[id].h : -1;
}

void *CanvasManagerLvgl::getCanvasBuffer(int id) 
{
    return valid(id) ? _entry[id].buffer : nullptr;
}
ICanvas *CanvasManagerLvgl::getCanvasWrapper(int id)
{
    if (id < 0 || id >= MAX_CANVAS)
        return nullptr;
    return _canvasWrapper[id];
}