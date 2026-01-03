#include "ScreenDriver.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ScreenDriver";
ScreenDriver Screen;

// Singleton
static PanelGc9a01 &GetTFT()
{
    static PanelGc9a01 tft;
    return tft;
}

ScreenDriver::ScreenDriver()
{
    _panel = &GetTFT();
}

void ScreenDriver::begin()
{
    _panel->init();
    _panel->setRotation(0);
    // _panel->setSwapBytes(true);
    _panel->fillScreen(BLACK);

}

//===== Draw functions ==================================================

void ScreenDriver::drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h)
{
    _panel->pushImageDMA(x, y, w, h, data);
}

void ScreenDriver::drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    int w = x2 - x1 + 1;
    int h = y2 - y1 + 1;
    if (w > 0 && h > 0)
        _panel->pushImageDMA(x1, y1, w, h, data);
}

