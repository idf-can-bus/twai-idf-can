/**
 * @file main.c
 * @brief CAN sender example using ESP32 TWAI controller
 * 
 * This example demonstrates sending CAN messages using the TWAI controller.
 * It sends test messages with heartbeat counter and timestamp, compatible
 * with the receive examples for statistics and sequence checking.
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

static const char *TAG = "send_twai";

void app_main(void)
{
    // Identify example and backend
    uint8_t sender_id = default_sender_id_from_mac();
    ESP_LOGI(TAG, "=== example: send-single, backend: %s, SEND_ID:%u ===",
        can_backend_get_name(), (unsigned)sender_id);

    // Initialize hardware
    ESP_LOGI(TAG, "Initializing CAN backend: %s ...", can_backend_get_name());
    if (!can_twai_init(&TWAI_HW_CFG)) {
        ESP_LOGE(TAG, "Failed to initialize %s backend", can_backend_get_name());
        return;
    }
    
    // Global variables
    twai_message_t message;
    uint8_t heartbeat = 0;
    bool success = false;

    // Example settings
    const uint32_t send_interval_ms = 10;
    bool print_during_send = false;
    uint64_t index = 0;
    const uint64_t max_index = 2000;
    

    while (1)
    {
        // Create message 
        fullfill_test_messages(sender_id, heartbeat, &message);
        // Request statistics periodically
        if ((index % max_index == 0) && (index != 0)) {
            set_test_flag(&message, TEST_FLAG_STATS_REQUEST);
        }

        // Send it
        success = can_twai_send(&message);
        if (!success)
        {
            ESP_LOGE(TAG, "Failed to send message");
            print_can_message(&message);
        }
        else {
            debug_send_message(&message, print_during_send);
            index++;
        }

        // Next heartbeat
        heartbeat = next_heartbeat(heartbeat);

        // Wait for send interval
        sleep_ms_min_ticks(send_interval_ms);
    }
}

