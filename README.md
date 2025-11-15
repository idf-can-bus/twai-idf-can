# Library for ESP32 + CAN bus: twai-idf-can

Simplified high-level adapter for ESP32 TWAI (CAN) controller with automatic error recovery.

This library provides a user-friendly interface for ESP32's built-in TWAI (Two-Wire Automotive Interface) controller, wrapping ESP-IDF's native TWAI driver with convenient initialization, message handling, and automatic error recovery mechanisms.

## Features

- ✅ **Simplified Configuration** - Modular configuration structure (wiring, parameters, timing, timeouts)
- ✅ **Non-blocking Operations** - Send and receive with configurable timeouts
- ✅ **Automatic Error Recovery** - Bus-off detection and recovery, controller state monitoring
- ✅ **Well Documented** - Full Doxygen documentation with examples
- ✅ **ESP-IDF v5.0+** - Compatible with modern ESP-IDF versions
- ✅ **Multiple ESP32 Variants** - Works with ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, and others

## Hardware Requirements

### ESP32 Built-in TWAI Controller

The ESP32 TWAI controller requires a CAN transceiver to interface with the physical CAN bus. The library was tested with **SN65HVD230** 

### Typical Wiring (SN65HVD230 Example)

```
ESP32          SN65HVD230         CAN Bus
GPIO_TX  ----> TX
GPIO_RX  <---- RX
3.3V     ----> VCC
GND      ----> GND
                CANH   --------> CAN_H
                CANL   --------> CAN_L
```

**Important Notes:**
- A **120-ohm termination resistor** must be placed at **one end only** of the CAN bus !!!
- Typical GPIO pins: TX=GPIO_5, RX=GPIO_4 (configurable)
- Maximum cable length depends on bitrate (1 Mbps ≤ 40m, 125 kbps ≤ 500m)

## Installation

### Using ESP Component Registry (Recommended)

Add to your project's `idf_component.yml`:

```yaml
dependencies:
  esp32-can-twai:
    version: "^1.0.0"
```

### Manual Installation

Clone into your project's `components` directory:

```bash
cd your_project/components
git clone https://github.com/esp32-can/esp32-can-twai.git
```

## Quick Start

### 1. Include Headers

```c
#include "can_twai.h"
```

### 2. Configure Hardware

```c
twai_backend_config_t config = {
    // GPIO Wiring
    .wiring = {
        .tx_gpio    = GPIO_NUM_5,
        .rx_gpio    = GPIO_NUM_4,
        .clkout_io  = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
    },
    
    // Controller Parameters
    .params = {
        .controller_id  = 0,
        .mode           = TWAI_MODE_NORMAL,  // or TWAI_MODE_NO_ACK, TWAI_MODE_LISTEN_ONLY
        .tx_queue_len   = 20,
        .rx_queue_len   = 20,
        .alerts_enabled = TWAI_ALERT_NONE,
        .clkout_divider = 0,
        .intr_flags     = ESP_INTR_FLAG_LEVEL1,
    },
    
    // Timing and Filter (use ESP-IDF macros)
    .tf = {
        .timing = TWAI_TIMING_CONFIG_1MBITS(),  // or 500KBITS, 250KBITS, etc.
        .filter = TWAI_FILTER_CONFIG_ACCEPT_ALL(),
    },
    
    // Runtime Timeouts
    .timeouts = {
        .receive_timeout         = pdMS_TO_TICKS(100),
        .transmit_timeout        = pdMS_TO_TICKS(100),
        .bus_off_timeout         = pdMS_TO_TICKS(1000),
        .bus_not_running_timeout = pdMS_TO_TICKS(100),
    }
};
```

### 3. Initialize

```c
if (!can_twai_init(&config)) {
    ESP_LOGE("APP", "Failed to initialize TWAI");
    return;
}
ESP_LOGI("APP", "TWAI initialized successfully");
```

### 4. Send Messages

```c
twai_message_t tx_msg = {
    .identifier = 0x123,
    .data_length_code = 8,
    .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
};

if (can_twai_send(&tx_msg)) {
    ESP_LOGI("APP", "Message sent successfully");
} else {
    ESP_LOGE("APP", "Failed to send message");
}
```

### 5. Receive Messages

```c
twai_message_t rx_msg;

if (can_twai_receive(&rx_msg)) {
    ESP_LOGI("APP", "Received message ID: 0x%lX", rx_msg.identifier);
    // Process message data...
} else {
    // No message available (timeout)
}
```

### 6. Cleanup (Optional)

```c
can_twai_deinit();
```

## Advanced Usage

### Extended CAN IDs

```c
twai_message_t msg = {
    .flags = TWAI_MSG_FLAG_EXTD,  // Enable extended ID
    .identifier = 0x12345678,      // 29-bit ID
    .data_length_code = 8,
    // ...
};
```

### Custom Acceptance Filter

```c
// Accept only messages with ID 0x100-0x1FF
twai_filter_config_t filter = {
    .acceptance_code = (0x100 << 21),
    .acceptance_mask = ~(0xFF << 21),
    .single_filter = true
};

config.tf.filter = filter;
```

### Different Bitrates

```c
// Available ESP-IDF macros:
.timing = TWAI_TIMING_CONFIG_1MBITS()    // 1 Mbps
.timing = TWAI_TIMING_CONFIG_800KBITS()  // 800 kbps
.timing = TWAI_TIMING_CONFIG_500KBITS()  // 500 kbps
.timing = TWAI_TIMING_CONFIG_250KBITS()  // 250 kbps
.timing = TWAI_TIMING_CONFIG_125KBITS()  // 125 kbps
.timing = TWAI_TIMING_CONFIG_100KBITS()  // 100 kbps
.timing = TWAI_TIMING_CONFIG_50KBITS()   // 50 kbps
.timing = TWAI_TIMING_CONFIG_25KBITS()   // 25 kbps
```

### Manual Error Recovery

While error recovery is automatic, you can manually trigger it:

```c
can_twai_reset_if_needed();  // Checks state and recovers if needed
```

## API Reference

### Initialization Functions

- `bool can_twai_init(const twai_backend_config_t *cfg)` - Initialize TWAI controller
- `bool can_twai_deinit(void)` - Deinitialize TWAI controller

### Message Functions

- `bool can_twai_send(const twai_message_t *msg)` - Send CAN message (non-blocking)
- `bool can_twai_receive(twai_message_t *msg)` - Receive CAN message (non-blocking)

### Utility Functions

- `void can_twai_reset_if_needed(void)` - Check controller state and recover if needed

See `can_twai.h` for full Doxygen documentation.

## Configuration Types

### `twai_backend_config_t`

Main configuration structure containing:
- `wiring` - GPIO pin assignments
- `params` - Controller parameters (mode, queue lengths, alerts)
- `tf` - Timing and filter configuration
- `timeouts` - Runtime timeout values

See `can_twai_config.h` for detailed structure documentation.

## Error Handling

The library automatically handles common CAN bus errors:

- **Bus-off State** - Automatically initiates recovery and waits for bus to recover
- **Controller Not Running** - Stops and restarts the controller
- **TX Failures** - Triggers automatic state check and recovery
- **RX Errors** - Triggers automatic state check and recovery

All errors are logged via ESP-IDF's logging system. Set log level to DEBUG for detailed information:

```c
esp_log_level_set("can_backend_twai", ESP_LOG_DEBUG);
```

## Troubleshooting

### No Messages Received

1. Check physical wiring (especially CANH/CANL polarity)
2. Verify termination resistors (120Ω at each end)
3. Ensure matching bitrate on all nodes
4. Check acceptance filter configuration
5. Try `TWAI_MODE_NO_ACK` for testing without other nodes

### Bus-Off Errors

1. Check for electrical issues (short circuits, bad connections)
2. Verify termination resistors are present
3. Check cable quality and length
4. Ensure all nodes use the same bitrate

### Build Errors

1. Ensure ESP-IDF version is 5.0 or newer
2. Check that `driver` component is available
3. Verify component is in `components/` directory

## Examples

The library includes three ready-to-use examples in the `examples/` directory:

### Building Examples

**Build all examples at once:**

```bash
# Using bash script
./build_all_examples.sh

# Using Makefile
make
```

For detailed build instructions and options, see [BUILDING_EXAMPLES.md](BUILDING_EXAMPLES.md).

### Prerequisites for Building Examples

Before building examples, set up your environment:

```bash
# 1. Activate ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# 2. Set target chip for ALL examples at once
./set_target_all.sh esp32s3  # Use: esp32, esp32s2, esp32s3, esp32c3, esp32c6, etc.

# 3. Build all examples
./build_all_examples.sh
```

### 1. Send Example (`examples/send/`)

Demonstrates sending CAN messages periodically with an incrementing counter.

```bash
cd examples/send
idf.py set-target esp32s3  # Set your chip type first!
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### 2. Receive Polling Example (`examples/receive_poll/`)

Demonstrates receiving CAN messages using polling mode (continuous checking).

```bash
cd examples/receive_poll
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### 3. Receive Interrupt Example (`examples/receive_interrupt/`)

Demonstrates receiving CAN messages using a producer-consumer pattern with queue buffering.
This prevents message loss during processing.

```bash
cd examples/receive_interrupt
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### Hardware Configuration

All examples use the same hardware configuration defined in `examples/config_twai.h`.
Modify this file to match your GPIO connections and CAN bitrate.

**Default configuration (in `examples/config_twai.h`):**
- TX GPIO: GPIO_39 (for ESP32-S3)
- RX GPIO: GPIO_40 (for ESP32-S3)
- Bitrate: 1 Mbps
- Mode: Normal

**Important:** Modify `examples/config_twai.h` to match your hardware!

**GPIO Compatibility Notes:**
- ESP32: Most GPIOs available (e.g., GPIO_4/GPIO_5)
- ESP32-S2: Limited GPIOs (e.g., GPIO_2/GPIO_3)
- ESP32-S3: GPIO_39/GPIO_40 or GPIO_4/GPIO_5 (avoid Flash/PSRAM pins)
- ESP32-C3/C6: Limited GPIOs (e.g., GPIO_2/GPIO_3)
- Always verify GPIO availability for your specific chip in the datasheet!

For more examples with advanced features, see:
- [can-multibackend-idf](https://github.com/idf-can-bus/can-multibackend-idf) - Unified interface supporting multiple CAN backends

## Related Projects

- [mcp25xxx-multi-idf-can](https://github.com/idf-can-bus/mcp25xxx-multi-idf-can) - External MCP25xxx CAN controllers (single or multiple)
- [can-multibackend-idf](https://github.com/idf-can-bus/can-multibackend-idf) - Unified interface for multiple CAN backends

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Author

Ivo Marvan, 2025

## Contributing

Contributions are welcome! Please open an issue or pull request on GitHub.

## Support

For issues, questions, or suggestions:
- Open an issue on [GitHub](https://github.com/esp32-can/esp32-can-twai/issues)
- Check ESP-IDF documentation for TWAI driver details

