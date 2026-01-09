#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "lvgl_display.h"

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

#include "RoboEye.h"

class OledDisplay : public LvglDisplay {
private:
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    RoboEyes* eyes_ = nullptr;
    lv_obj_t* screen_main_ = nullptr;
    lv_obj_t* screen_aux_ = nullptr;
    lv_obj_t* eyes_container_ = nullptr;
    lv_obj_t* top_bar_ = nullptr;
    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* content_left_ = nullptr;
    lv_obj_t* content_right_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* side_bar_ = nullptr;
    lv_obj_t *emotion_label_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;

    lv_timer_t *draw_eye_timer_ = nullptr;

    lv_draw_rect_dsc_t rect_dsc_;
    lv_draw_triangle_dsc_t triangle_dsc_;
    int32_t eyes_container_basex_ = 0;
    int32_t eyes_container_basey_ = 0;

    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

    void DrawFillRectangle(void *ctx, int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t color);
    void DrawFillRectangleRound(void *ctx, int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint16_t color);
    void DrawFillTriangle(void *ctx, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);
    void SetupUI_128x64();
    void SetupUI_128x32();
    virtual void SetHide(bool hide);

public:
    OledDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel, int width, int height, bool mirror_x, bool mirror_y);
    ~OledDisplay();

    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void SetEmotion(const char* emotion) override;
    virtual void SetTheme(Theme* theme) override;
    inline RoboEyes* GetEyes() {return eyes_;};
};

#endif // OLED_DISPLAY_H
