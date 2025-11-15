# Quick Start Guide - TWAI IDF CAN Examples

## 🚀 Fast Track: Build All Examples

**Three simple commands to build all examples:**

```bash
# Step 1: Activate ESP-IDF
. $HOME/esp/esp-idf/export.sh

# Step 2: Set target for all examples (replace esp32s3 with your chip)
./set_target_all.sh esp32s3

# Step 3: Build all examples
./build_all_examples.sh
```

Done! All three examples are now built.

---

## 📋 Available Scripts

### `set_target_all.sh` - Configure Target Chip

Sets the target chip for all examples at once.

```bash
./set_target_all.sh esp32s3
```

**Available targets:**
- `esp32` - Original ESP32
- `esp32s2` - ESP32-S2
- `esp32s3` - ESP32-S3
- `esp32c3` - ESP32-C3
- `esp32c6` - ESP32-C6
- `esp32h2` - ESP32-H2

### `build_all_examples.sh` - Build Examples

Builds all examples with one command.

```bash
./build_all_examples.sh           # Build all
./build_all_examples.sh fullclean # Clean all
./build_all_examples.sh size      # Show size info
```

### Alternative: Using Makefile

```bash
make              # Build all examples
make clean        # Clean all examples
make send         # Build only send example
make help         # Show all options
```

---

## 🔧 Hardware Configuration

**Before building**, verify GPIO pins in `examples/config_twai.h`:

```c
.wiring = {
    .tx_gpio = GPIO_NUM_39,  // ← Current: GPIO_39 (for ESP32-S3)
    .rx_gpio = GPIO_NUM_40,  // ← Current: GPIO_40 (for ESP32-S3)
}
```

**Important:** Default configuration is for ESP32-S3. Modify for your chip!

### GPIO Recommendations by Chip

| Chip | Example TX / RX | Note |
|------|-----------------|------|
| ESP32 | GPIO_5 / GPIO_4 | Most flexible |
| ESP32-S2 | GPIO_2 / GPIO_3 | Limited pins |
| ESP32-S3 | GPIO_39 / GPIO_40 | Default in config (avoid Flash/PSRAM pins) |
| ESP32-C3 | GPIO_2 / GPIO_3 | Limited pins |
| ESP32-C6 | GPIO_2 / GPIO_3 | Limited pins |

---

## 📱 Flash & Monitor Individual Example

After building, flash to your board:

```bash
cd examples/send
idf.py -p /dev/ttyUSB0 flash monitor
```

**Tip:** Find your serial port with `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`

---

## 🎯 Examples Overview

### 1. **send** - Sender Example
Sends CAN messages with incrementing counter every second.

```bash
cd examples/send
idf.py -p /dev/ttyUSB0 flash monitor
```

### 2. **receive_poll** - Polling Receiver
Receives CAN messages using continuous polling (simple, blocking).

```bash
cd examples/receive_poll
idf.py -p /dev/ttyUSB0 flash monitor
```

### 3. **receive_interrupt** - Interrupt Receiver
Advanced: Uses producer-consumer pattern with queue. **Recommended for production.**

```bash
cd examples/receive_interrupt
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## ⚠️ Common Issues

### Error: `GPIO_NUM_XX undeclared`

**Problem:** GPIO pin not available on your chip.

**Solution:**
1. Edit `examples/config_twai.h`
2. Change TX/RX to valid pins for your chip (see table above)
3. Rebuild

### Error: `IDF_PATH not set`

**Solution:**
```bash
. $HOME/esp/esp-idf/export.sh
```

Add to `~/.bashrc` for automatic loading:
```bash
echo ". $HOME/esp/esp-idf/export.sh" >> ~/.bashrc
```

### Error: Target not set

**Solution:**
```bash
./set_target_all.sh esp32s3
```

---

## 📚 More Information

- **Full Build Guide**: See [BUILDING_EXAMPLES.md](BUILDING_EXAMPLES.md)
- **Library Documentation**: See [README.md](README.md)
- **Hardware Config**: Edit [examples/config_twai.h](examples/config_twai.h)

---

## 🧪 Testing Setup

For testing without a real CAN bus, use two ESP32 boards:

**Board 1 (Sender):**
```bash
cd examples/send
idf.py flash monitor
```

**Board 2 (Receiver):**
```bash
cd examples/receive_poll
idf.py flash monitor
```

**Wiring:**
```
Board1-TX → Transceiver1-TX → CAN_H ←→ CAN_H ← Transceiver2-TX ← Board2-TX
Board1-RX ← Transceiver1-RX ← CAN_L ←→ CAN_L → Transceiver2-RX → Board2-RX
                               ↑
                          120Ω resistor
```

**Don't forget:** 120Ω termination resistors at **one end only** of the CAN bus!

---

## 🎓 Next Steps

1. ✅ Build examples with scripts
2. ✅ Flash to your board
3. ✅ Test communication between two boards
4. 📖 Read [README.md](README.md) for API documentation
5. 🔧 Integrate library into your project

---

**Questions?** Check [BUILDING_EXAMPLES.md](BUILDING_EXAMPLES.md) for detailed troubleshooting.

