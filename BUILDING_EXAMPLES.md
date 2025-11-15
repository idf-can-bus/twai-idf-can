# Building All Examples

This document describes how to build all TWAI-IDF-CAN examples with a single command.

## Prerequisites

- ESP-IDF v5.0 or newer installed

### Setup ESP-IDF Environment

Before building, you must activate the ESP-IDF environment and set the target chip:

```bash
# 1. Activate ESP-IDF environment
. $HOME/esp/esp-idf/export.sh   # Or your IDF installation path

# 2. Set target chip for ALL examples at once (recommended!)
./set_target_all.sh esp32s3      # For ESP32-S3
# ./set_target_all.sh esp32      # For original ESP32
# ./set_target_all.sh esp32s2    # For ESP32-S2
# ./set_target_all.sh esp32c3    # For ESP32-C3
# ./set_target_all.sh esp32c6    # For ESP32-C6

# Alternatively, set target manually for each example:
# cd examples/send && idf.py set-target esp32s3
# cd ../receive_poll && idf.py set-target esp32s3
# cd ../receive_interrupt && idf.py set-target esp32s3
```

**Important:** Each example needs `set-target` configured before first build!

## Quick Start

**Complete workflow for building all examples:**

```bash
# 1. Activate ESP-IDF
. $HOME/esp/esp-idf/export.sh

# 2. Set target for all examples
./set_target_all.sh esp32s3

# 3. Build all examples
./build_all_examples.sh
```

## Method 1: Using Bash Script (Recommended)

The `build_all_examples.sh` script provides the most flexibility and colored output.

### Build all examples

```bash
./build_all_examples.sh build
```

or simply:

```bash
./build_all_examples.sh
```

### Clean all examples

```bash
./build_all_examples.sh fullclean
```

### Other actions

The script supports any `idf.py` command:

```bash
./build_all_examples.sh menuconfig  # Open menuconfig for each example
./build_all_examples.sh size        # Show size for each example
./build_all_examples.sh reconfigure # Reconfigure all examples
```

**Note:** Commands like `flash` and `monitor` require a serial port and are interactive, so they're not practical for batch processing all examples. Flash/monitor examples individually.

## Method 2: Using Makefile

The Makefile provides standard GNU Make targets.

### Build all examples

```bash
make
```

or:

```bash
make build
```

### Clean all examples

```bash
make clean
```

### Build individual examples

```bash
make send
make receive_poll
make receive_interrupt
```

### Show help

```bash
make help
```

## Method 3: Manual Build (Traditional)

You can still build examples individually:

```bash
cd examples/send
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Hardware Configuration

All examples share the same hardware configuration defined in `examples/config_twai.h`.

**Before building**, modify this file to match your setup:

```c
.wiring = {
    .tx_gpio = GPIO_NUM_39,  // ← Current: GPIO_39 (for ESP32-S3)
    .rx_gpio = GPIO_NUM_40,  // ← Current: GPIO_40 (for ESP32-S3)
    // ...
},
.tf = {
    .timing = TWAI_TIMING_CONFIG_1MBITS(),  // ← Current: 1 Mbps
    // ...
}
```

**Important:** The default configuration is set for ESP32-S3. Change GPIO pins to match your specific chip!

### GPIO Pin Compatibility

Different ESP32 variants have different available GPIO pins:

| Chip | Example TX/RX | Notes |
|------|---------------|-------|
| ESP32 | GPIO_5 / GPIO_4 | Most GPIOs available |
| ESP32-S2 | GPIO_2 / GPIO_3 | Limited GPIO count |
| ESP32-S3 | GPIO_39 / GPIO_40 | Some pins used by Flash/PSRAM (default in config) |
| ESP32-C3 | GPIO_2 / GPIO_3 | Limited GPIO count |
| ESP32-C6 | GPIO_2 / GPIO_3 | Limited GPIO count |

**Always check your chip's datasheet** for GPIO availability before wiring!

## Troubleshooting

### Error: IDF_PATH is not set

Make sure you've sourced the ESP-IDF environment:

```bash
. $HOME/esp/esp-idf/export.sh
```

Or add it to your `.bashrc` for automatic loading.

### Error: GPIO_NUM_XX undeclared

This means the GPIO pin is not available on your target chip. 

**Solution:**
1. Check which chip you're targeting: `idf.py --version` shows current target
2. Set correct target: `idf.py set-target esp32s3` (or your chip)
3. Modify `examples/config_twai.h` to use available GPIO pins for your chip
4. See "GPIO Pin Compatibility" table above for recommendations

### Error: Target not set

If you see build errors about missing components or configuration:

```bash
cd examples/send
idf.py set-target esp32s3  # Set your chip type
idf.py build
```

### Build fails for one example

The script will continue building remaining examples and report all failures at the end. Check the output for specific error messages.

### Permission denied when running script

Make the script executable:

```bash
chmod +x build_all_examples.sh
```

## Examples Overview

### 1. **send** - Sender Example
- Sends CAN messages periodically
- Demonstrates basic message transmission
- Incrementing counter in each message

### 2. **receive_poll** - Polling Receiver
- Receives messages using continuous polling
- Simple blocking receive loop
- Good for basic applications

### 3. **receive_interrupt** - Interrupt Receiver
- Uses producer-consumer pattern with queue
- Non-blocking interrupt-driven reception
- Prevents message loss during processing
- Recommended for production use

## Build Output

Each example will be built in its own `build/` directory:

```
examples/
├── send/
│   └── build/
│       └── twai_send_example.bin
├── receive_poll/
│   └── build/
│       └── twai_receive_poll_example.bin
└── receive_interrupt/
    └── build/
        └── twai_receive_interrupt_example.bin
```

## Continuous Integration

For CI/CD pipelines, use the bash script:

```bash
# In your CI script
./build_all_examples.sh build
```

The script returns:
- Exit code 0: All examples built successfully
- Exit code 1: One or more examples failed

## Related Documentation

- [Main README](README.md) - Library documentation
- [examples/config_twai.h](examples/config_twai.h) - Hardware configuration
- [ESP-IDF TWAI Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html)

