/**
 * @file led_driver.h
 * @brief LED control and status indication
 * @version 0.1.0
 */

#ifndef TSX_LINK_LED_DRIVER_H
#define TSX_LINK_LED_DRIVER_H

#include "esp_err.h"
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LED states
 */
typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_BOOT,
    LED_STATE_INITIALIZING,
    LED_STATE_RUNNING,
    LED_STATE_ERROR,
    LED_STATE_WIFI_CONNECTING,
    LED_STATE_WIFI_CONNECTED,
    LED_STATE_ETHERNET_CONNECTED,
    LED_STATE_ACTIVITY
} led_state_t;

/**
 * @brief Initialize LED driver
 * @return ESP_OK on success
 */
esp_err_t led_driver_init(void);

/**
 * @brief Set LED state
 * @param state LED state
 * @return ESP_OK on success
 */
esp_err_t led_set_state(led_state_t state);

/**
 * @brief Get current LED state
 * @return Current LED state
 */
led_state_t led_get_state(void);

/**
 * @brief Control individual LED
 * @param led_pin GPIO pin
 * @param on Turn on (1) or off (0)
 * @return ESP_OK on success
 */
esp_err_t led_set_pin(uint32_t led_pin, uint8_t on);

#ifdef __cplusplus
}
#endif

#endif // TSX_LINK_LED_DRIVER_H
