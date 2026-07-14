/**
 * @file uni_telway.h
 * @brief Uni-Telway protocol implementation for Schneider TSX PLC communication
 * @version 0.1.0
 * 
 * Uni-Telway is the proprietary protocol used by Schneider Electric PLCs
 * for serial communication over RS485/RS232 links.
 */

#ifndef TSX_LINK_UNI_TELWAY_H
#define TSX_LINK_UNI_TELWAY_H

#include "esp_err.h"
#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Uni-Telway frame structure
 */
typedef struct {
    uint8_t address;           /**< Station address (1-64) */
    uint8_t function;          /**< Function code */
    uint8_t *data;             /**< Data payload */
    size_t data_length;        /**< Data length */
    uint8_t checksum;          /**< Checksum byte */
} uni_telway_frame_t;

/**
 * @brief Uni-Telway function codes
 */
typedef enum {
    UT_FUNC_READ_COILS = 0x01,
    UT_FUNC_READ_DISCRETE_INPUTS = 0x02,
    UT_FUNC_READ_HOLDING_REGISTERS = 0x03,
    UT_FUNC_READ_INPUT_REGISTERS = 0x04,
    UT_FUNC_WRITE_SINGLE_COIL = 0x05,
    UT_FUNC_WRITE_SINGLE_REGISTER = 0x06,
    UT_FUNC_WRITE_MULTIPLE_COILS = 0x0F,
    UT_FUNC_WRITE_MULTIPLE_REGISTERS = 0x10,
    UT_FUNC_READ_DEVICE_IDENTIFICATION = 0x2B
} uni_telway_function_t;

/**
 * @brief Initialize Uni-Telway protocol handler
 * @param uart_port UART port number
 * @param baudrate Communication speed
 * @return ESP_OK on success
 */
esp_err_t uni_telway_init(int uart_port, uint32_t baudrate);

/**
 * @brief Encode Uni-Telway request frame
 * @param frame Frame to encode
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return Number of bytes written, or negative on error
 */
int uni_telway_encode_frame(const uni_telway_frame_t *frame, uint8_t *buffer, size_t buffer_size);

/**
 * @brief Decode Uni-Telway response frame
 * @param buffer Input buffer
 * @param buffer_size Buffer size
 * @param frame Decoded frame
 * @return Number of bytes consumed, or negative on error
 */
int uni_telway_decode_frame(const uint8_t *buffer, size_t buffer_size, uni_telway_frame_t *frame);

/**
 * @brief Calculate Uni-Telway checksum
 * @param data Data buffer
 * @param length Data length
 * @return Checksum value
 */
uint8_t uni_telway_calculate_checksum(const uint8_t *data, size_t length);

/**
 * @brief Send request frame to PLC
 * @param frame Frame to send
 * @param timeout_ms Timeout in milliseconds
 * @return ESP_OK on success
 */
esp_err_t uni_telway_send_frame(const uni_telway_frame_t *frame, uint32_t timeout_ms);

/**
 * @brief Receive response frame from PLC
 * @param frame Received frame
 * @param timeout_ms Timeout in milliseconds
 * @return ESP_OK on success
 */
esp_err_t uni_telway_recv_frame(uni_telway_frame_t *frame, uint32_t timeout_ms);

/**
 * @brief Read holding registers from PLC
 * @param address PLC address
 * @param start_register Register start address
 * @param count Number of registers
 * @param values Output buffer for register values
 * @return Number of registers read, or negative on error
 */
int uni_telway_read_registers(uint8_t address, uint16_t start_register, uint16_t count, uint16_t *values);

/**
 * @brief Write single register to PLC
 * @param address PLC address
 * @param register_address Register address
 * @param value Register value
 * @return ESP_OK on success
 */
esp_err_t uni_telway_write_register(uint8_t address, uint16_t register_address, uint16_t value);

/**
 * @brief Read coils from PLC
 * @param address PLC address
 * @param start_coil Coil start address
 * @param count Number of coils
 * @param values Output buffer for coil values
 * @return Number of coils read, or negative on error
 */
int uni_telway_read_coils(uint8_t address, uint16_t start_coil, uint16_t count, uint8_t *values);

/**
 * @brief Write single coil to PLC
 * @param address PLC address
 * @param coil_address Coil address
 * @param value Coil value
 * @return ESP_OK on success
 */
esp_err_t uni_telway_write_coil(uint8_t address, uint16_t coil_address, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif // TSX_LINK_UNI_TELWAY_H
