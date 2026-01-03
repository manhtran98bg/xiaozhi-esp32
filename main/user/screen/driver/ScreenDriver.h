#pragma once
#include "DisplayBackend.hpp"

class ScreenDriver
{
public:
    ScreenDriver();
    void begin();
    void drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h);
    void drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

    PanelGc9a01 *getPanel() { return _panel; }

private:
    static constexpr int MAX_SPRITES = 2;
    LGFX_Sprite* _sprites[MAX_SPRITES] = { nullptr };

    PanelGc9a01 *_panel;

};

extern ScreenDriver Screen;
