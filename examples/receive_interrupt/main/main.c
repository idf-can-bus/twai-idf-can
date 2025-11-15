/**
 * @file main.c
 * @brief CAN receiver example using ESP32 TWAI controller (interrupt mode with queue)
 * 
 * This example demonstrates receiving CAN messages using a producer-consumer pattern:
 * - Producer task: Continuously receives messages and enqueues them
 * - Consumer task: Processes messages from the queue with statistics
 * 
 * This pattern prevents message loss during processing by using a queue buffer.
 * The TWAI driver uses interrupts internally, so this example just wraps
 * that with an additional queue for backpressure handling.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "examples_utils.h"
#include "can_twai.h"
#include "config_twai.h"

static const char *TAG = "receive_interrupt_twai";

// Queue capacity tuned for bursty traffic
#define RX_QUEUE_LENGTH 64

// Task configuration
#define PRODUCER_TASK_STACK 4096
#define CONSUMER_TASK_STACK 4096
#define PRODUCER_TASK_PRIO  12
#define CONSUMER_TASK_PRIO  10

static QueueHandle_t rx_queue;

static inline void received_to_queue(twai_message_t *msg) {
    // TWAI backend: block on driver receive (driver handles IRQ internally)
    if (can_twai_receive(msg)) {
        (void)xQueueSend(rx_queue, msg, 0);
    } else {
        // No frame within adapter timeout; yield briefly
        sleep_ms_min_ticks(1);
    }
}

static void can_rx_producer_task(void *arg)
{
    twai_message_t message;
    for (;;) {
        received_to_queue(&message);
    }
}

static void can_rx_consumer_task(void *arg)
{
    (void)arg;
    twai_message_t message;
    const bool print_during_receive = false;

    for (;;) {
        if (xQueueReceive(rx_queue, &message, portMAX_DELAY) == pdTRUE) {
            process_received_message(&message, print_during_receive);
        }
    }
}

void app_main(void)
{
    // Initialize hardware
    ESP_LOGI(TAG, "Initializing TWAI controller...");
    if (!can_twai_init(&TWAI_HW_CFG)) {
        ESP_LOGE(TAG, "Failed to initialize TWAI controller");
        return;
    }

    // Create RX queue
    rx_queue = xQueueCreate(RX_QUEUE_LENGTH, sizeof(twai_message_t));
    if (rx_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create RX queue");
        return;
    }

    // Identify yourself as receiver
    ESP_LOGI(TAG, "Receiver interrupt-driven");

    // Start tasks
    BaseType_t ok1 = xTaskCreate(can_rx_producer_task, "can_rx_prod", PRODUCER_TASK_STACK, NULL, PRODUCER_TASK_PRIO, NULL);
    BaseType_t ok2 = xTaskCreate(can_rx_consumer_task, "can_rx_cons", CONSUMER_TASK_STACK, NULL, CONSUMER_TASK_PRIO, NULL);
    if (ok1 != pdPASS || ok2 != pdPASS) {
        ESP_LOGE(TAG, "Failed to create tasks (prod=%ld, cons=%ld)", (long)ok1, (long)ok2);
        return;
    }
}
