#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"
#include "system_info.h"
#include "Face.h"
#include "ScreenDriver.h"
#include "CanvasLGFX.h"
#include "CanvasManagerLGFX.h"

#define TAG "main"


ICanvasManager *canvasManager = new CanvasManagerLGFX();
Face *face;
ICanvas *canvas;

static void face_task(void *pvParameters)
{
    while (1)
    {
        face->Update();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
extern "C" void app_main(void)
{
    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // Initialize and run the application
    // Screen.begin();
    // Screen.getPanel()->fillScreen(TFT_BROWN);
    // Screen.getPanel()->setBrightness(255);
    // int id = canvasManager->createCanvas(240, 240, 1);
	// if (id == -1)
	// {
	// 	ESP_LOGI(TAG, "Create canvas failed");
	// 	return;
	// }
	// canvas = canvasManager->getCanvasWrapper(id);
	// if (canvas)
	// {
	// 	face = new Face(canvas, 50, 240, 240, BLACK, YELLOW);
	// }
    // xTaskCreate(face_task, "face_task", 4096, NULL, 5, NULL);
    auto& app = Application::GetInstance();
    app.Initialize();
    app.Run();  // This function runs the main event loop and never returns
}
