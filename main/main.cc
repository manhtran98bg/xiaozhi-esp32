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

void list_all_tasks(void)
{
    UBaseType_t taskCount = uxTaskGetNumberOfTasks();

    // Cấp phát mảng lưu trạng thái task
    TaskStatus_t *taskStatusArray = (TaskStatus_t *)pvPortMalloc(
        taskCount * sizeof(TaskStatus_t)
    );

    if (taskStatusArray == NULL) {
        ESP_LOGE(TAG, "Malloc failed");
        return;
    }

    uint32_t totalRunTime;
    taskCount = uxTaskGetSystemState(
        taskStatusArray,
        taskCount,
        &totalRunTime
    );

    ESP_LOGI(TAG, "------------------------------------------------");
    ESP_LOGI(TAG, "Task Name         | State | Prio | Core | Stack");
    ESP_LOGI(TAG, "------------------------------------------------");

    for (int i = 0; i < taskCount; i++) {
        const char *state;

        switch (taskStatusArray[i].eCurrentState) {
        case eRunning:   state = "RUN"; break;
        case eReady:     state = "RDY"; break;
        case eBlocked:   state = "BLK"; break;
        case eSuspended: state = "SUS"; break;
        case eDeleted:   state = "DEL"; break;
        default:         state = "UNK"; break;
        }

        ESP_LOGI(TAG,
            "%-16s | %-5s | %u | %lu",
            taskStatusArray[i].pcTaskName,
            state,
            (unsigned int)taskStatusArray[i].uxCurrentPriority,
            taskStatusArray[i].usStackHighWaterMark
        );
    }

    ESP_LOGI(TAG, "------------------------------------------------");

    vPortFree(taskStatusArray);
}

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

    auto& app = Application::GetInstance();
    app.Initialize();
    int id = canvasManager->createCanvas(128, 64, 1);
	if (id == -1)
	{
		ESP_LOGI(TAG, "Create canvas failed");
		return;
	}
	canvas = canvasManager->getCanvasWrapper(id);
	if (canvas)
	{
		face = new Face(canvas, 50, 240, 240, WHITE, BLACK);
	}
    xTaskCreate(face_task, "face_task", 4096, NULL, 24, NULL);
    list_all_tasks();
    app.Run();  // This function runs the main event loop and never returns
}
