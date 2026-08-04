# CAN Project

Minimal C++17 CAN bus demo for Linux using [SocketCAN](https://docs.kernel.org/networking/can.html).

An engine ECU broadcasts RPM / throttle / coolant temperature as CAN frames every 10&nbsp;ms on a virtual CAN interface (`vcan0`), and a second socket listens and prints what it receives.

## Requirements (Linux only)

CAN sockets are a Linux kernel feature — this project **will not compile on macOS/Windows**.

- Linux (any distro with a 5.x+ kernel)
- `g++` or `clang++`
- `cmake` (3.16+) and `make`
- `iproute2` + `vcan` kernel module (usually built-in) for the virtual interface

## Install dependencies (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install -y build-essential cmake iproute2
```

## Setup the virtual CAN interface

```bash
chmod +x scripts/setup_vcan.sh
./scripts/setup_vcan.sh
```

You should see a `vcan0` interface in the output.

## Build & run

```bash
cmake -B build
cmake --build build
./build/can_demo
```

Expected output (20 received frames):

```
rx  id=0x120 len=8 data=13 88 3 E8 23 28 0 0
rx  id=0x120 len=8 data=13 8A 3 E8 23 28 0 0
rx  id=0x120 len=8 data=13 8C 3 E8 23 28 0 0
...
```

## See the frames with can-utils

```bash
sudo apt install -y can-utils   # candump, cansend, cansniffer
candump vcan0                   # live view of the bus
```

Test a manual transmit:

```bash
cansend vcan0 123#DEADBEEF
```

Then run the demo — it will print the frame from the listener socket if you point it at the right node.

## Message format (big-endian)

| Byte(s) | Field          |
|---------|----------------|
| 0–1     | RPM (uint16)   |
| 2–3     | Throttle % × 100 (uint16) |
| 4–5     | Coolant temp °C × 100 (uint16) |
| 6–7     | Reserved (0)   |

All sent on CAN ID `0x120`.