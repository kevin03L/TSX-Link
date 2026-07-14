/**
 * @file logger.h
 * @brief System logging and event recording
 * @version 0.1.0
 */

#ifndef TSX_LINK_LOGGER_H
#define TSX_LINK_LOGGER_H

#include "esp_err.h"
#include "esp_log.h"
#include <cstdint>
#include <cstring>
#include <ctime>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log level enumeration
 */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} log_level_t;

/**
 * @brief Event type enumeration
 */
typedef enum {
    EVENT_TYPE_SYSTEM = 0,
    EVENT_TYPE_NETWORK,
    EVENT_TYPE_PROTOCOL,
    EVENT_TYPE_ERROR,
    EVENT_TYPE_ALARM
} event_type_t;

/**
 * @brief Log entry structure
 */
typedef struct {
    uint32_t timestamp;
    log_level_t level;
    event_type_t type;
    char tag[32];
    char message[256];
    uint32_t sequence_number;
} log_entry_t;

/**
 * @brief Initialize logger subsystem
 * @return ESP_OK on success
 */
esp_err_t logger_init(void);

/**
 * @brief Log a message
 * @param level Log level
 * @param type Event type
 * @param tag Source tag
 * @param message Message format string
 * @return ESP_OK on success
 */
esp_err_t logger_log(log_level_t level, event_type_t type, const char *tag, const char *message);

/**
 * @brief Get log entry count
 * @return Number of entries in log
 */
uint32_t logger_get_entry_count(void);

/**
 * @brief Get log entry by index
 * @param index Entry index
 * @param entry Pointer to log entry structure
 * @return ESP_OK on success
 */
esp_err_t logger_get_entry(uint32_t index, log_entry_t *entry);

/**
 * @brief Clear all log entries
 * @return ESP_OK on success
 */
esp_err_t logger_clear(void);

/**
 * @brief Export logs as JSON
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return Number of bytes written
 */
size_t logger_export_json(char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // TSX_LINK_LOGGER_H
