/**
 * @file main.cpp
 * @brief TSX-Link main application entry point
 * @version 0.1.0
 * 
 * Main application loop for TSX-Link gateway.
 * Initializes all subsystems and manages the main event loop.
 */

#include <cstdio>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"

// Component includes - to be implemented
// #include "config_manager.h"
// #include "logger.h"
// #include "led_driver.h"

static const char *TAG = "TSX-Link";

/**
 * @brief Initialize NVS (Non-Volatile Storage)
 * @return ESP_OK on success
 */
static esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition erased, reinitializing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

/**
 * @brief Initialize network interface
 * @return ESP_OK on success
 */
static esp_err_t init_network(void)
{
    esp_err_t ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize netif: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

/**
 * @brief Main application entry point
 */
extern "C" void app_main(void)
{
    esp_err_t ret;

    printf("\n\n");
    printf("╔═════════════════════════════════════════════════╗\n");
    printf("║  TSX-Link v0.1.0 - Industrial Gateway          ║\n");
    printf("║  Schneider TSX3721001 Wi-Fi/Ethernet Bridge    ║\n");
    printf("║  Open Source - MIT License                     ║\n");
    printf("╚═════════════════════════════════════════════════╝\n\n");

    // Initialize NVS
    ESP_LOGI(TAG, "Initializing NVS...");
    ret = init_nvs();
    if (ret != ESP_OK) {
        printf("NVS initialization failed\n");
        return;
    }

    // Initialize network interface
    ESP_LOGI(TAG, "Initializing network interface...");
    ret = init_network();
    if (ret != ESP_OK) {
        printf("Network interface initialization failed\n");
        return;
    }

    ESP_LOGI(TAG, "TSX-Link core initialized successfully");

    // Main event loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
