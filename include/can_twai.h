/**
 * @file can_twai.h
 * @brief High-level adapter for ESP32 TWAI (CAN) controller
 * 
 * This library provides a simplified, user-friendly interface for ESP32's
 * built-in TWAI (Two-Wire Automotive Interface) controller, also known as CAN.
 * It wraps ESP-IDF's TWAI driver with convenient initialization, send/receive
 * functions, and automatic error recovery mechanisms.
 * 
 * Features:
 * - Simplified configuration with modular structure
 * - Non-blocking send and receive operations
 * - Automatic bus-off recovery
 * - Controller state monitoring and reset
 * - Configurable timeouts for all operations
 * 
 * Typical usage:
 * @code
 * // 1. Configure hardware
 * twai_backend_config_t config = {
 *     .wiring = { .tx_gpio = GPIO_NUM_5, .rx_gpio = GPIO_NUM_4, ... },
 *     .params = { .mode = TWAI_MODE_NORMAL, ... },
 *     .tf = { .timing = TWAI_TIMING_CONFIG_1MBITS(), ... },
 *     .timeouts = { .receive_timeout = pdMS_TO_TICKS(100), ... }
 * };
 * 
 * // 2. Initialize
 * if (!can_twai_init(&config)) {
 *     // Handle error
 * }
 * 
 * // 3. Send messages
 * twai_message_t msg = { .identifier = 0x123, .data_length_code = 8, ... };
 * can_twai_send(&msg);
 * 
 * // 4. Receive messages
 * twai_message_t rx_msg;
 * if (can_twai_receive(&rx_msg)) {
 *     // Process received message
 * }
 * 
 * // 5. Cleanup
 * can_twai_deinit();
 * @endcode
 * 
 * @author Ivo Marvan
 * @date 2025
 */

#pragma once

#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "can_twai_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize TWAI (CAN) hardware
 * 
 * Installs and starts the TWAI driver with the provided configuration.
 * This function must be called before any send/receive operations.
 * 
 * @param[in] cfg Pointer to complete TWAI configuration structure
 * 
 * @return true if initialization was successful
 * @return false if initialization failed (check logs for details)
 * 
 * @note This function saves a copy of the configuration for later use
 *       in timeout and error recovery operations
 * @note The TWAI driver is automatically started after successful installation
 * 
 * @see can_twai_deinit()
 */
bool can_twai_init(const twai_backend_config_t *cfg);

/**
 * @brief Deinitialize TWAI (CAN) hardware
 * 
 * Stops the TWAI driver and uninstalls it, freeing all associated resources.
 * After calling this function, can_twai_init() must be called again before
 * any send/receive operations.
 * 
 * @return true if deinitialization was successful
 * @return false if deinitialization failed (check logs for details)
 * 
 * @note This function attempts to stop and uninstall the driver gracefully
 * 
 * @see can_twai_init()
 */
bool can_twai_deinit(void);

/**
 * @brief Send a CAN message (non-blocking)
 * 
 * Transmits a CAN message with the configured timeout. If transmission fails
 * due to bus-off or controller errors, automatic recovery is attempted.
 * 
 * @param[in] msg Pointer to message to transmit
 * 
 * @return true if message was successfully queued for transmission
 * @return false if transmission failed or message is invalid
 * 
 * @note This function validates message length before transmission
 * @note On error, can_twai_reset_if_needed() is automatically called
 * @note Timeout is configured via twai_backend_config_t.timeouts.transmit_timeout
 * 
 * @see can_twai_receive()
 */
bool can_twai_send(const twai_message_t *msg);

/**
 * @brief Receive a CAN message (non-blocking)
 * 
 * Attempts to receive a CAN message from the RX queue with the configured timeout.
 * Returns immediately if no message is available within the timeout period.
 * 
 * @param[out] msg Pointer to buffer where received message will be stored
 * 
 * @return true if a message was successfully received
 * @return false if no message was received (timeout or error)
 * 
 * @note Timeout errors (ESP_ERR_TIMEOUT) are not logged as they are expected
 *       during normal polling operation
 * @note On real errors, can_twai_reset_if_needed() is automatically called
 * @note Timeout is configured via twai_backend_config_t.timeouts.receive_timeout
 * 
 * @see can_twai_send()
 */
bool can_twai_receive(twai_message_t *msg);

/**
 * @brief Check TWAI controller status and reset if necessary
 * 
 * Monitors the TWAI controller state and performs recovery actions if needed:
 * - If bus-off state is detected, initiates recovery and waits
 * - If controller is not running, stops and restarts it
 * 
 * This function is automatically called by can_twai_send() and can_twai_receive()
 * on errors, but can also be called manually for proactive monitoring.
 * 
 * @note Recovery timeouts are configured via twai_backend_config_t.timeouts
 * @note This function logs warnings when recovery actions are taken
 * 
 * @see can_twai_send()
 * @see can_twai_receive()
 */
void can_twai_reset_if_needed(void);

#ifdef __cplusplus
}
#endif