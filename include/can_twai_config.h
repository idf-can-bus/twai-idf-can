/**
 * @file can_twai_config.h
 * @brief Configuration types for ESP32 TWAI (CAN) adapter
 * 
 * This header defines modular configuration structures for the TWAI adapter,
 * splitting configuration into logical groups: wiring, parameters, timing/filter,
 * and runtime timeouts. This approach provides better organization and makes
 * configuration more maintainable.
 * 
 * @author Ivo Marvan
 * @date 2025
 */

#pragma once

#include "driver/twai.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIO wiring configuration for TWAI controller
 * 
 * Defines the physical GPIO pins used for TWAI communication.
 */
typedef struct {
    gpio_num_t tx_gpio;     /**< TX GPIO pin number */
    gpio_num_t rx_gpio;     /**< RX GPIO pin number */
    gpio_num_t clkout_io;   /**< Clock output GPIO (use TWAI_IO_UNUSED if not used) */
    gpio_num_t bus_off_io;  /**< Bus-off indicator GPIO (use TWAI_IO_UNUSED if not used) */
} twai_wiring_config_t;

/**
 * @brief TWAI controller parameters (non-GPIO)
 * 
 * Contains controller-specific settings such as mode, queue lengths,
 * alerts, and interrupt configuration.
 */
typedef struct {
    int              controller_id;  /**< Controller ID (typically 0 for single controller) */
    twai_mode_t      mode;           /**< Operating mode (TWAI_MODE_NORMAL, TWAI_MODE_NO_ACK, TWAI_MODE_LISTEN_ONLY) */
    int              tx_queue_len;   /**< Transmit queue length (recommended: 20) */
    int              rx_queue_len;   /**< Receive queue length (recommended: 20) */
    uint32_t         alerts_enabled; /**< Enabled alerts bitmask (TWAI_ALERT_* flags) */
    int              clkout_divider; /**< Clock output divider (0 to disable) */
    int              intr_flags;     /**< Interrupt allocation flags (ESP_INTR_FLAG_*) */
} twai_params_config_t;

/**
 * @brief Bit timing and acceptance filter configuration
 * 
 * Wraps ESP-IDF's timing and filter structures for convenience.
 * Use ESP-IDF macros like TWAI_TIMING_CONFIG_1MBITS() for timing
 * and TWAI_FILTER_CONFIG_ACCEPT_ALL() for filters.
 */
typedef struct {
    twai_timing_config_t timing;     /**< Bit timing configuration (use TWAI_TIMING_CONFIG_* macros) */
    twai_filter_config_t filter;     /**< Acceptance filter configuration (use TWAI_FILTER_CONFIG_* macros) */
} twai_tf_config_t;

/**
 * @brief Runtime timeout configuration
 * 
 * Defines timeouts used during send/receive operations and error recovery.
 * All timeouts are specified in FreeRTOS ticks.
 */
typedef struct {
    TickType_t receive_timeout;         /**< Receive timeout in ticks (use pdMS_TO_TICKS() macro) */
    TickType_t transmit_timeout;        /**< Transmit timeout in ticks (use pdMS_TO_TICKS() macro) */
    TickType_t bus_off_timeout;         /**< Bus-off recovery wait time in ticks */
    TickType_t bus_not_running_timeout; /**< Controller restart wait time in ticks */
} twai_timeouts_config_t;

/**
 * @brief Complete TWAI backend configuration
 * 
 * Aggregates all configuration components into a single structure.
 * This is the main configuration type passed to can_twai_init().
 * 
 * @note All fields must be properly initialized before passing to can_twai_init()
 */
typedef struct {
    twai_wiring_config_t   wiring;   /**< GPIO wiring configuration */
    twai_params_config_t   params;   /**< Controller parameters */
    twai_tf_config_t       tf;       /**< Timing and filter configuration */
    twai_timeouts_config_t timeouts; /**< Runtime timeout configuration */
} twai_backend_config_t;

#ifdef __cplusplus
}
#endif

