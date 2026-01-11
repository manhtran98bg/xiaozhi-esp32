#ifndef COMMON_h
#define COMMON_h

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_random.h"

// #define esp_random() (rand() % UINT32_MAX)

#define RANDOM_RANGE(min, max) \
    ((int)(esp_random() % ((max) - (min)) + (min)))

#define RANDOM(c) \
    ((int)((c) > 0 ? (esp_random() % (c)) : 0))
inline uint32_t portMillis() { return (xTaskGetTickCount() * portTICK_PERIOD_MS); }
#endif