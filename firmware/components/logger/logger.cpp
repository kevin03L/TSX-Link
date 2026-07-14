/**
 * @file logger.cpp
 * @brief System logging and event recording implementation
 * @version 0.1.0
 */

#include "logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "esp_system.h"

#define MAX_LOG_ENTRIES 512
#define TAG "Logger"

static log_entry_t log_buffer[MAX_LOG_ENTRIES];
static uint32_t log_count = 0;
static uint32_t log_sequence = 0;
static SemaphoreHandle_t log_mutex = NULL;

/**
 * @brief Initialize logger subsystem
 */
esp_err_t logger_init(void)
{
    log_mutex = xSemaphoreCreateMutex();
    if (log_mutex == NULL) {
        return ESP_ERR_NO_MEM;
    }

    log_count = 0;
    log_sequence = 0;

    return ESP_OK;
}

/**
 * @brief Log a message
 */
esp_err_t logger_log(log_level_t level, event_type_t type, const char *tag, const char *message)
{
    if (log_mutex == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(log_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    time_t now = time(NULL);

    uint32_t index = log_count % MAX_LOG_ENTRIES;
    log_entry_t *entry = &log_buffer[index];

    entry->timestamp = (uint32_t)now;
    entry->level = level;
    entry->type = type;
    entry->sequence_number = log_sequence++;
    
    strncpy(entry->tag, tag, sizeof(entry->tag) - 1);
    entry->tag[sizeof(entry->tag) - 1] = '\0';
    
    strncpy(entry->message, message, sizeof(entry->message) - 1);
    entry->message[sizeof(entry->message) - 1] = '\0';

    log_count++;

    xSemaphoreGive(log_mutex);
    return ESP_OK;
}

/**
 * @brief Get log entry count
 */
uint32_t logger_get_entry_count(void)
{
    if (log_mutex == NULL) {
        return 0;
    }

    if (xSemaphoreTake(log_mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return 0;
    }

    uint32_t count = (log_count > MAX_LOG_ENTRIES) ? MAX_LOG_ENTRIES : log_count;
    xSemaphoreGive(log_mutex);
    return count;
}

/**
 * @brief Get log entry by index
 */
esp_err_t logger_get_entry(uint32_t index, log_entry_t *entry)
{
    if (entry == NULL || log_mutex == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (xSemaphoreTake(log_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    uint32_t total_count = (log_count > MAX_LOG_ENTRIES) ? MAX_LOG_ENTRIES : log_count;
    if (index >= total_count) {
        xSemaphoreGive(log_mutex);
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t buffer_index;
    if (log_count < MAX_LOG_ENTRIES) {
        buffer_index = index;
    } else {
        buffer_index = (log_count + index) % MAX_LOG_ENTRIES;
    }

    memcpy(entry, &log_buffer[buffer_index], sizeof(log_entry_t));
    xSemaphoreGive(log_mutex);
    return ESP_OK;
}

/**
 * @brief Clear all log entries
 */
esp_err_t logger_clear(void)
{
    if (log_mutex == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(log_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    memset(log_buffer, 0, sizeof(log_buffer));
    log_count = 0;
    log_sequence = 0;

    xSemaphoreGive(log_mutex);
    return ESP_OK;
}

/**
 * @brief Export logs as JSON
 */
size_t logger_export_json(char *buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0 || log_mutex == NULL) {
        return 0;
    }

    if (xSemaphoreTake(log_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return 0;
    }

    size_t written = 0;
    written += snprintf(buffer + written, buffer_size - written, "[\n");

    uint32_t total_count = (log_count > MAX_LOG_ENTRIES) ? MAX_LOG_ENTRIES : log_count;

    for (uint32_t i = 0; i < total_count; i++) {
        uint32_t buffer_index;
        if (log_count < MAX_LOG_ENTRIES) {
            buffer_index = i;
        } else {
            buffer_index = (log_count + i) % MAX_LOG_ENTRIES;
        }

        log_entry_t *entry = &log_buffer[buffer_index];
        written += snprintf(
            buffer + written,
            buffer_size - written,
            "  {\"seq\":%u,\"ts\":%u,\"level\":%d,\"type\":%d,\"tag\":\"%s\",\"msg\":\"%s\"}%s\n",
            entry->sequence_number,
            entry->timestamp,
            entry->level,
            entry->type,
            entry->tag,
            entry->message,
            (i < total_count - 1) ? "," : ""
        );

        if (written >= buffer_size - 1) {
            break;
        }
    }

    written += snprintf(buffer + written, buffer_size - written, "]\n");

    xSemaphoreGive(log_mutex);
    return written;
}
