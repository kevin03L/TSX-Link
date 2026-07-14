/**
 * @file led_driver.cpp
 * @brief LED control and status indication implementation
 * @version 0.1.0
 */

#include "led_driver.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "LEDDriver"

// LED pin configuration
#define LED_POWER_PIN GPIO_NUM_3
#define LED_WIFI_PIN GPIO_NUM_4
#define LED_ETHERNET_PIN GPIO_NUM_5
#define LED_ACTIVITY_PIN GPIO_NUM_6

static led_state_t current_state = LED_STATE_OFF;
static TaskHandle_t led_task_handle = NULL;

/**
 * @brief LED animation task
 */
static void led_task(void *pvParameters)
{
    while (1) {
        switch (current_state) {
            case LED_STATE_OFF:
                gpio_set_level(LED_POWER_PIN, 0);
                gpio_set_level(LED_WIFI_PIN, 0);
                gpio_set_level(LED_ETHERNET_PIN, 0);
                gpio_set_level(LED_ACTIVITY_PIN, 0);
                break;

            case LED_STATE_BOOT:
                // All LEDs blink together
                gpio_set_level(LED_POWER_PIN, 1);
                gpio_set_level(LED_WIFI_PIN, 1);
                gpio_set_level(LED_ETHERNET_PIN, 1);
                gpio_set_level(LED_ACTIVITY_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(LED_POWER_PIN, 0);
                gpio_set_level(LED_WIFI_PIN, 0);
                gpio_set_level(LED_ETHERNET_PIN, 0);
                gpio_set_level(LED_ACTIVITY_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;

            case LED_STATE_INITIALIZING:
                // Power LED on, others blink
                gpio_set_level(LED_POWER_PIN, 1);
                gpio_set_level(LED_WIFI_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(200));
                gpio_set_level(LED_WIFI_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            case LED_STATE_RUNNING:
                gpio_set_level(LED_POWER_PIN, 1);
                gpio_set_level(LED_WIFI_PIN, 0);
                gpio_set_level(LED_ETHERNET_PIN, 0);
                gpio_set_level(LED_ACTIVITY_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;

            case LED_STATE_ERROR:
                // Power LED blinks rapidly
                gpio_set_level(LED_POWER_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(LED_POWER_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;

            case LED_STATE_WIFI_CONNECTING:
                gpio_set_level(LED_POWER_PIN, 1);
                gpio_set_level(LED_WIFI_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(300));
                gpio_set_level(LED_WIFI_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(300));
                break;

            case LED_STATE_WIFI_CONNECTED:
                gpio_set_level(LED_POWER_PIN, 1);
                gpio_set_level(LED_WIFI_PIN, 1);
                gpio_set_level(LED_ETHERNET_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;

            case LED_STATE_ETHERNET_CONNECTED:
                gpio_set_level(LED_POWER_PIN, 1);
                gpio_set_level(LED_WIFI_PIN, 0);
                gpio_set_level(LED_ETHERNET_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;

            case LED_STATE_ACTIVITY:
                gpio_set_level(LED_ACTIVITY_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(50));
                gpio_set_level(LED_ACTIVITY_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(50));
                break;

            default:
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
        }
    }
}

/**
 * @brief Initialize LED driver
 */
esp_err_t led_driver_init(void)
{
    esp_err_t ret;

    // Configure GPIO pins as outputs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_POWER_PIN) | 
                       (1ULL << LED_WIFI_PIN) | 
                       (1ULL << LED_ETHERNET_PIN) | 
                       (1ULL << LED_ACTIVITY_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Initialize all LEDs to off
    gpio_set_level(LED_POWER_PIN, 0);
    gpio_set_level(LED_WIFI_PIN, 0);
    gpio_set_level(LED_ETHERNET_PIN, 0);
    gpio_set_level(LED_ACTIVITY_PIN, 0);

    // Create LED task
    ret = xTaskCreate(led_task, "LED_Task", 2048, NULL, 10, &led_task_handle);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create LED task");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "LED driver initialized");
    return ESP_OK;
}

/**
 * @brief Set LED state
 */
esp_err_t led_set_state(led_state_t state)
{
    if (state >= LED_STATE_ACTIVITY) {
        return ESP_ERR_INVALID_ARG;
    }

    current_state = state;
    return ESP_OK;
}

/**
 * @brief Get current LED state
 */
led_state_t led_get_state(void)
{
    return current_state;
}

/**
 * @brief Control individual LED
 */
esp_err_t led_set_pin(uint32_t led_pin, uint8_t on)
{
    esp_err_t ret = gpio_set_level((gpio_num_t)led_pin, on ? 1 : 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO set level failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}
