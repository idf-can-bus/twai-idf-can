/**
 * @file config_twai.h
 * @brief Hardware configuration for ESP32 TWAI (CAN) examples
 * 
 * This configuration is used by all TWAI examples (send, receive_poll, receive_interrupt).
 * Adjust GPIO pins and parameters according to your hardware setup.
 * 
 * Hardware requirements:
 * - ESP32 with built-in TWAI controller
 * - CAN transceiver (e.g., SN65HVD230, TJA1050, MCP2551)
 * - 120-ohm termination resistors at each end of the CAN bus
 * 
 * @author Ivo Marvan
 * @date 2025
 */

#pragma once

#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "can_twai_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default TWAI hardware configuration
 * 
 * This configuration defines the complete setup for the TWAI controller.
 * Modify these values to match your hardware connections.
 * 
 * **IMPORTANT:** GPIO pin availability varies by ESP32 variant:
 * - ESP32: Most GPIOs available (GPIO_4, GPIO_5 recommended)
 * - ESP32-S2/S3: Limited GPIOs (GPIO_2, GPIO_3 or GPIO_4, GPIO_5)
 * - ESP32-C3/C6: Limited GPIOs (GPIO_0, GPIO_1 or GPIO_2, GPIO_3)
 * 
 * Check your specific chip's datasheet for available GPIO pins!
 */
const twai_backend_config_t TWAI_HW_CFG = {
    // GPIO Wiring Configuration
    .wiring = {
        .tx_gpio    = GPIO_NUM_39,          // TWAI TX pin (connect to transceiver TX)
        .rx_gpio    = GPIO_NUM_40,          // TWAI RX pin (connect to transceiver RX)
        .clkout_io  = TWAI_IO_UNUSED,       // Clock output (unused)
        .bus_off_io = TWAI_IO_UNUSED,       // Bus-off indicator (unused)
    },
    
    // Controller Parameters
    .params = {
        .controller_id  = 0,                // Controller ID (0 for single controller)
        .mode           = TWAI_MODE_NORMAL, // Operating mode (NORMAL, NO_ACK, LISTEN_ONLY)
        .tx_queue_len   = 20,               // TX queue length
        .rx_queue_len   = 20,               // RX queue length
        .alerts_enabled = TWAI_ALERT_NONE,  // Alert flags (NONE, RX_DATA, TX_IDLE, etc.)
        .clkout_divider = 0,                // Clock output divider (0 = disabled)
        .intr_flags     = ESP_INTR_FLAG_LEVEL1, // Interrupt flags
    },
    
    // Timing and Filter Configuration
    .tf = {
        .timing = TWAI_TIMING_CONFIG_1MBITS(), // CAN bitrate (25K, 50K, 125K, 250K, 500K, 800K, 1M)
        .filter = TWAI_FILTER_CONFIG_ACCEPT_ALL(), // Accept all messages
    },
    
    // Runtime Timeouts
    .timeouts = {
        .receive_timeout         = pdMS_TO_TICKS(100),  // RX timeout (100ms)
        .transmit_timeout        = pdMS_TO_TICKS(100),  // TX timeout (100ms)
        .bus_off_timeout         = pdMS_TO_TICKS(1000), // Bus-off recovery wait (1s)
        .bus_not_running_timeout = pdMS_TO_TICKS(100),  // Controller restart wait (100ms)
    }
};

#ifdef __cplusplus
}
#endif

