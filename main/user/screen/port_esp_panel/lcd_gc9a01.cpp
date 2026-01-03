/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

#include "lcd_gc9a01.h"

#include "ScreenDriver.h"

static const char *TAG = "port_esp_lcd_gc9a01";

static esp_err_t panel_gc9a01_del(esp_lcd_panel_t *panel);
static esp_err_t panel_gc9a01_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_gc9a01_init(esp_lcd_panel_t *panel);
static esp_err_t panel_gc9a01_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_gc9a01_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_gc9a01_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_gc9a01_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_gc9a01_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_gc9a01_disp_on_off(esp_lcd_panel_t *panel, bool off);

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    int x_gap;
    int y_gap;
    uint8_t fb_bits_per_pixel;

} gc9a01_panel_t;

esp_err_t esp_lcd_new_panel_gc9a01_lgfx(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    gc9a01_panel_t *gc9a01 = NULL;
    gpio_config_t io_conf = { 0 };

    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    gc9a01 = (gc9a01_panel_t *)calloc(1, sizeof(gc9a01_panel_t));
    ESP_GOTO_ON_FALSE(gc9a01, ESP_ERR_NO_MEM, err, TAG, "no mem for gc9a01 panel");

    if (panel_dev_config->reset_gpio_num >= 0) {
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num;
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }

    gc9a01->io = io;
    gc9a01->reset_gpio_num = panel_dev_config->reset_gpio_num;
    gc9a01->reset_level = panel_dev_config->flags.reset_active_high;

    gc9a01->base.del = panel_gc9a01_del;
    gc9a01->base.reset = panel_gc9a01_reset;
    gc9a01->base.init = panel_gc9a01_init;
    gc9a01->base.draw_bitmap = panel_gc9a01_draw_bitmap;
    gc9a01->base.invert_color = panel_gc9a01_invert_color;
    gc9a01->base.set_gap = panel_gc9a01_set_gap;
    gc9a01->base.mirror = panel_gc9a01_mirror;
    gc9a01->base.swap_xy = panel_gc9a01_swap_xy;
    gc9a01->base.disp_on_off = panel_gc9a01_disp_on_off;

    *ret_panel = &(gc9a01->base);
    ESP_LOGI(TAG, "LCD panel create success, version: %d.%d.%d", ESP_LCD_GC9A01_VER_MAJOR, ESP_LCD_GC9A01_VER_MINOR,
             ESP_LCD_GC9A01_VER_PATCH);

    return ESP_OK;

err:
    if (gc9a01) {
        if (panel_dev_config->reset_gpio_num >= 0) {
            gpio_reset_pin((gpio_num_t)panel_dev_config->reset_gpio_num);
        }
        free(gc9a01);
    }
    return ret;
}

static esp_err_t panel_gc9a01_del(esp_lcd_panel_t *panel)
{
    gc9a01_panel_t *gc9a01 = __containerof(panel, gc9a01_panel_t, base);

    if (gc9a01->reset_gpio_num >= 0) {
        gpio_reset_pin((gpio_num_t)gc9a01->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del gc9a01 panel @%p", gc9a01);
    free(gc9a01);
    return ESP_OK;
}

static esp_err_t panel_gc9a01_reset(esp_lcd_panel_t *panel)
{
    gc9a01_panel_t *gc9a01 = __containerof(panel, gc9a01_panel_t, base);
    esp_lcd_panel_io_handle_t io = gc9a01->io;

    // perform hardware reset
    if (gc9a01->reset_gpio_num >= 0) {
        gpio_set_level((gpio_num_t)gc9a01->reset_gpio_num, gc9a01->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level((gpio_num_t)gc9a01->reset_gpio_num, !gc9a01->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return ESP_OK;
}


static esp_err_t panel_gc9a01_init(esp_lcd_panel_t *panel)
{
    ESP_LOGI(TAG, "Initialize GC9A01 panel");
    Screen.begin();
    return ESP_OK;
}

static esp_err_t panel_gc9a01_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    gc9a01_panel_t *gc9a01 = __containerof(panel, gc9a01_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");

    x_start += gc9a01->x_gap;
    x_end += gc9a01->x_gap;
    y_start += gc9a01->y_gap;
    y_end += gc9a01->y_gap;

    Screen.drawRect((uint16_t *)color_data, x_start, y_start, x_end - x_start, y_end - y_start);

    return ESP_OK;
}

static esp_err_t panel_gc9a01_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{;
    return ESP_OK;
}

static esp_err_t panel_gc9a01_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    return ESP_OK;
}

static esp_err_t panel_gc9a01_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    return ESP_OK;
}

static esp_err_t panel_gc9a01_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    gc9a01_panel_t *gc9a01 = __containerof(panel, gc9a01_panel_t, base);
    gc9a01->x_gap = x_gap;
    gc9a01->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_gc9a01_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    return ESP_OK;
}
