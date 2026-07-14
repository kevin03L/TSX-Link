/**
 * @file uni_telway.cpp
 * @brief Uni-Telway protocol implementation
 * @version 0.1.0
 */

#include "uni_telway.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <cstring>

#define TAG "Uni-Telway"

// Uni-Telway frame delimiters
#define UT_FRAME_START 0x02
#define UT_FRAME_END 0x03

// UART configuration
static int g_uart_port = -1;
static QueueHandle_t g_uart_queue = NULL;

/**
 * @brief UART event handler task
 */
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *)malloc(1024);

    while (1) {
        if (xQueueReceive(g_uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                    ESP_LOGV(TAG, "UART data event, length: %d", event.size);
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "UART FIFO overflow");
                    uart_flush_input(g_uart_port);
                    xQueueReset(g_uart_queue);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "UART RX buffer full");
                    break;
                case UART_BREAK:
                    ESP_LOGW(TAG, "UART break detected");
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGW(TAG, "UART parity error");
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGW(TAG, "UART frame error");
                    break;
                default:
                    break;
            }
        }
    }
    free(dtmp);
}

/**
 * @brief Initialize Uni-Telway protocol handler
 */
esp_err_t uni_telway_init(int uart_port, uint32_t baudrate)
{
    if (uart_port < 0 || uart_port >= UART_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    g_uart_port = uart_port;

    uart_config_t uart_config = {
        .baud_rate = (int)baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t ret = uart_param_config(uart_port, &uart_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "UART param config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = uart_driver_install(uart_port, 1024 * 2, 1024 * 2, 20, &g_uart_queue, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "UART driver install failed: %s", esp_err_to_name(ret));
        return ret;
    }

    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    ESP_LOGI(TAG, "Uni-Telway initialized on UART %d at %d baud", uart_port, baudrate);
    return ESP_OK;
}

/**
 * @brief Calculate Uni-Telway checksum
 */
uint8_t uni_telway_calculate_checksum(const uint8_t *data, size_t length)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

/**
 * @brief Encode Uni-Telway request frame
 */
int uni_telway_encode_frame(const uni_telway_frame_t *frame, uint8_t *buffer, size_t buffer_size)
{
    if (frame == NULL || buffer == NULL || buffer_size < 10) {
        return -1;
    }

    size_t pos = 0;

    // Frame start delimiter
    buffer[pos++] = UT_FRAME_START;

    // Station address
    buffer[pos++] = frame->address;

    // Function code
    buffer[pos++] = frame->function;

    // Data payload
    if (frame->data_length > 0 && frame->data != NULL) {
        if (pos + frame->data_length + 2 > buffer_size) {
            return -1;
        }
        memcpy(&buffer[pos], frame->data, frame->data_length);
        pos += frame->data_length;
    }

    // Calculate checksum
    uint8_t checksum = uni_telway_calculate_checksum(&buffer[1], pos - 1);
    buffer[pos++] = checksum;

    // Frame end delimiter
    buffer[pos++] = UT_FRAME_END;

    return pos;
}

/**
 * @brief Decode Uni-Telway response frame
 */
int uni_telway_decode_frame(const uint8_t *buffer, size_t buffer_size, uni_telway_frame_t *frame)
{
    if (buffer == NULL || frame == NULL || buffer_size < 5) {
        return -1;
    }

    // Check frame delimiters
    if (buffer[0] != UT_FRAME_START || buffer[buffer_size - 1] != UT_FRAME_END) {
        ESP_LOGW(TAG, "Invalid frame delimiters");
        return -1;
    }

    // Extract frame data
    frame->address = buffer[1];
    frame->function = buffer[2];
    
    // Data is between function code and checksum
    size_t data_length = buffer_size - 4; // exclude start, address, function, checksum, end
    if (data_length > 0) {
        frame->data_length = data_length;
        frame->data = (uint8_t *)&buffer[3];
    } else {
        frame->data_length = 0;
        frame->data = NULL;
    }

    // Verify checksum
    uint8_t expected_checksum = uni_telway_calculate_checksum(&buffer[1], buffer_size - 3);
    uint8_t received_checksum = buffer[buffer_size - 2];

    if (expected_checksum != received_checksum) {
        ESP_LOGW(TAG, "Checksum mismatch: expected 0x%02x, got 0x%02x", expected_checksum, received_checksum);
        return -1;
    }

    frame->checksum = received_checksum;
    return buffer_size;
}

/**
 * @brief Send request frame to PLC
 */
esp_err_t uni_telway_send_frame(const uni_telway_frame_t *frame, uint32_t timeout_ms)
{
    if (g_uart_port < 0 || frame == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t buffer[512];
    int frame_size = uni_telway_encode_frame(frame, buffer, sizeof(buffer));
    if (frame_size <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    int written = uart_write_bytes(g_uart_port, buffer, frame_size);
    if (written != frame_size) {
        ESP_LOGE(TAG, "Failed to send complete frame");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Frame sent: %d bytes", frame_size);
    return ESP_OK;
}

/**
 * @brief Receive response frame from PLC
 */
esp_err_t uni_telway_recv_frame(uni_telway_frame_t *frame, uint32_t timeout_ms)
{
    if (g_uart_port < 0 || frame == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t buffer[512];
    int length = 0;
    uint32_t start_time = esp_log_timestamp();

    // Read data until frame end delimiter or timeout
    while (length < (int)sizeof(buffer)) {
        int bytes_read = uart_read_bytes(g_uart_port, &buffer[length], 1, pdMS_TO_TICKS(100));
        
        if (bytes_read > 0) {
            length += bytes_read;
            
            // Check if we have a complete frame
            if (length >= 5 && buffer[0] == UT_FRAME_START && buffer[length - 1] == UT_FRAME_END) {
                return uni_telway_decode_frame(buffer, length, frame) > 0 ? ESP_OK : ESP_FAIL;
            }
        }

        // Check timeout
        if ((esp_log_timestamp() - start_time) > timeout_ms) {
            ESP_LOGW(TAG, "Receive timeout");
            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_ERR_NO_MEM;
}

/**
 * @brief Read holding registers from PLC
 */
int uni_telway_read_registers(uint8_t address, uint16_t start_register, uint16_t count, uint16_t *values)
{
    if (values == NULL || count == 0 || count > 125) {
        return -1;
    }

    uni_telway_frame_t request;
    uint8_t data[6];

    request.address = address;
    request.function = UT_FUNC_READ_HOLDING_REGISTERS;
    request.data = data;
    request.data_length = 4;

    // Build request data
    data[0] = (start_register >> 8) & 0xFF;
    data[1] = start_register & 0xFF;
    data[2] = (count >> 8) & 0xFF;
    data[3] = count & 0xFF;

    if (uni_telway_send_frame(&request, 1000) != ESP_OK) {
        return -1;
    }

    // TODO: Parse response and fill values array
    return count;
}

/**
 * @brief Write single register to PLC
 */
esp_err_t uni_telway_write_register(uint8_t address, uint16_t register_address, uint16_t value)
{
    uni_telway_frame_t frame;
    uint8_t data[4];

    frame.address = address;
    frame.function = UT_FUNC_WRITE_SINGLE_REGISTER;
    frame.data = data;
    frame.data_length = 4;

    data[0] = (register_address >> 8) & 0xFF;
    data[1] = register_address & 0xFF;
    data[2] = (value >> 8) & 0xFF;
    data[3] = value & 0xFF;

    return uni_telway_send_frame(&frame, 1000);
}

/**
 * @brief Read coils from PLC
 */
int uni_telway_read_coils(uint8_t address, uint16_t start_coil, uint16_t count, uint8_t *values)
{
    if (values == NULL || count == 0 || count > 2000) {
        return -1;
    }

    uni_telway_frame_t frame;
    uint8_t data[4];

    frame.address = address;
    frame.function = UT_FUNC_READ_COILS;
    frame.data = data;
    frame.data_length = 4;

    data[0] = (start_coil >> 8) & 0xFF;
    data[1] = start_coil & 0xFF;
    data[2] = (count >> 8) & 0xFF;
    data[3] = count & 0xFF;

    if (uni_telway_send_frame(&frame, 1000) != ESP_OK) {
        return -1;
    }

    // TODO: Parse response and fill values array
    return count;
}

/**
 * @brief Write single coil to PLC
 */
esp_err_t uni_telway_write_coil(uint8_t address, uint16_t coil_address, uint8_t value)
{
    uni_telway_frame_t frame;
    uint8_t data[4];

    frame.address = address;
    frame.function = UT_FUNC_WRITE_SINGLE_COIL;
    frame.data = data;
    frame.data_length = 4;

    data[0] = (coil_address >> 8) & 0xFF;
    data[1] = coil_address & 0xFF;
    data[2] = (value ? 0xFF : 0x00);
    data[3] = 0x00;

    return uni_telway_send_frame(&frame, 1000);
}
