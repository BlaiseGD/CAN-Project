#!/usr/bin/env bash
# Creates the virtual CAN interface vcan0 used by the demo.
# Requires: iproute2 + the vcan kernel module (loaded automatically via modprobe).
set -euo pipefail

IFACE="${1:-vcan0}"

# remove existing interface so the script is idempotent
sudo ip link del "$IFACE" 2>/dev/null || true

# load the vcan kernel module if not already loaded
sudo modprobe vcan

# create and bring up the virtual CAN interface
sudo ip link add dev "$IFACE" type vcan
sudo ip link set up "$IFACE"

ip link show "$IFACE"
echo "virtual CAN interface '$IFACE' is ready"