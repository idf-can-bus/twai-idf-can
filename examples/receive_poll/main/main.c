/**
 * @file main.c
 * @brief CAN receive example using ESP32 TWAI controller (polling mode)
 * 
 * This example demonstrates receiving CAN messages using the TWAI controller
 * in polling mode. Messages are checked periodically without using interrupts.
 * It processes test messages with sequence checking and statistics.
 * 
 * Hardware requirements:
 * - ESP32 with TWAI controller
 * - CAN transceiver (e.g., SN65HVD230)
 * - 120-ohm termination resistors at each end of CAN bus
 * 
 * Configuration: See examples/config_twai.h
 * 
 * @author Ivo Marvan
 * @date 2025
 */

#include "esp_log.h"
#include "examples_utils.h"
#include "can_twai.h"
#include "config_twai.h"

static const char *TAG = "receive_poll_twai";

void app_main(void)
{
    // Initialize hardware
    ESP_LOGI(TAG, "Initializing TWAI controller...");
    if (!can_twai_init(&TWAI_HW_CFG)) {
        ESP_LOGE(TAG, "Failed to initialize TWAI controller");
        return;
    }

    // Global variables
    twai_message_t message;
    bool success = false;

    // Example settings
    bool print_during_send = false;
    const uint32_t receive_interval_ms = 1;

    // Identify yourself as receiver   
    ESP_LOGI(TAG, "Receiver poll driven");

    while (1)
    {
        // Receive message
        success = can_twai_receive(&message);
        if (success)
        {
            process_received_message(&message, print_during_send);
        }

        // Wait a while
        sleep_ms_min_ticks(receive_interval_ms);
    }
}
