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

The ESP32 TWAI controller requires a CAN transceiver to interface with the physical CAN bus. The library was tested with **SN65HVD230**.

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
- **CAN bus termination:** The general recommendation is to place one 120-ohm termination resistor at each end of a long CAN bus. However, the author's experience with short experimental setups shows that using only one 120-ohm resistor for the entire bus often works better.
- **SN65HVD230 Modules:** Some SN65HVD230 (or similar) hub modules already include an integrated 120-ohm termination resistor. Depending on your network topology, you may need to add or remove these resistors to ensure the bus has the correct number of terminations.
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
git clone --recurse-submodules https://github.com/esp32-can/esp32-can-twai.git
```
(Use --recurse-submodules because this project includes the submodule [examples-utils-idf-can](https://github.com/idf-can-bus/examples-utils-idf-can.git).
You can also initialize submodules later with `git submodule update --init --recursive`.)

## Project Layout

The component is organized into a small set of well-defined directories:

```text
twai-idf-can/
├─ src/                     # Implementation of the TWAI adapter
│   └─ can_twai.c
├─ include/                 # Public headers (API and configuration types)
│   ├─ can_twai.h
│   └─ can_twai_config.h
├─ examples/                # Example applications using this component
│   ├─ send/
│   ├─ receive_poll/
│   └─ receive_interrupt/
└─ components/
    └─ examples-utils-idf-can/  # Submodule with shared utilities for examples
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
The actual configuration for examples is in [config_twai.h](examples/config_twai.h).

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

```
can_twai_deinit();
```

---
See examples for more detailed basic usage examples:
- [examples/send/](./examples/send/main/main.c)
- [examples/receive_poll/](./examples/receive_poll/main/main.c)
- [examples/receive_interrupt/](./examples/receive_interrupt/main/main.c)
---

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

## Doxygen Documentation

All public headers (`include/can_twai.h`, `include/can_twai_config.h` and `components/examples-utils-idf-can/include/examples_utils.h`) are fully documented with Doxygen comments.
To integrate this component into your own documentation, add these include directories to your Doxygen `INPUT` paths.

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
2. Verify termination resistors (see wiring notes above)
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

Check ESP-IDF documentation for TWAI driver details.


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

You can also build individual examples using the Makefile targets:

```bash
make send
make receive_poll
make receive_interrupt
```

For CI/CD pipelines, you can run:

```bash
./build_all_examples.sh build
```

This script returns exit code `0` when all examples build successfully and `1` if any example fails.

This setup should be sufficient for most example builds and CI use cases.

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

Available targets for `set_target_all.sh` are the same as for the `idf.py set-target` command (for example: esp32, esp32s2, esp32s3, esp32c3, esp32c6, esp32h2, ...).


### Using idf.py Directly

You can work with individual examples using `idf.py` commands:

```bash
idf.py build                    # Build the current example
idf.py -p /dev/ttyUSB0 flash    # Flash the firmware
idf.py -p /dev/ttyUSB0 monitor  # Open serial monitor
```

To combine flashing and monitoring in one step:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

You can stop the ESP-IDF monitor with `Ctrl+]` or with `Ctrl+T` followed by `X`.

---
**For tests you can use the send example on one ESP32 board and one of the receive examples on another board connected to the same CAN bus.**

Use the `examples/send` project on Board 1 (sender) and one of the `examples/receive_*` projects on Board 2 (receiver). Connect both boards via CAN transceivers as shown below; for short test setups, a single 120Ω termination resistor at one end of the bus is usually sufficient.

```
Board1-TX → Transceiver1-TX → CAN_H ←→ CAN_H ← Transceiver2-TX ← Board2-TX
Board1-RX ← Transceiver1-RX ← CAN_L ←→ CAN_L → Transceiver2-RX → Board2-RX
                               ↑
                          120Ω resistor
```

---

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

### Hardware Configuration for Examples

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

My other related CAN bus libraries for ESP-IDF are maintained in the **[idf-can-bus](https://github.com/idf-can-bus)** organization on GitHub.

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Author

Ivo Marvan, 2025

