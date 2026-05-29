#!/usr/bin/env python3
import argparse
import sys
import time
from collections import deque
import matplotlib.pyplot as plt
import matplotlib.animation as animation
try:
    import serial  # pyserial
except ImportError as e:
    print("Missing dependency: pyserial. Install with: pip install pyserial", file=sys.stderr)
    raise

MAG_PREFIX = "MAG_RAW,"

def parse_mag_line(line: str):
    # Expected: MAG_RAW,<ms>,<x>,<y>,<z>
    if not line.startswith(MAG_PREFIX):
        return None
    parts = line.strip().split(",")
    if len(parts) != 5:
        return None
    try:
        ms = int(parts[1])
        x = int(parts[2])
        y = int(parts[3])
        z = int(parts[4])
        return ms, x, y, z
    except ValueError:
        return None

def main():
    ap = argparse.ArgumentParser(description="Real-time plot of magnetometer raw Y vs X from serial.")
    ap.add_argument("--port", required=True, help="Serial port (e.g. /dev/ttyUSB0, COM5)")
    ap.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    ap.add_argument("--window", type=int, default=2000, help="Number of points kept in the plot (default: 2000)")
    ap.add_argument("--timeout", type=float, default=0.2, help="Serial read timeout seconds (default: 0.2)")
    args = ap.parse_args()

    ser = serial.Serial(args.port, args.baud, timeout=args.timeout)

    xs = deque(maxlen=args.window)
    ys = deque(maxlen=args.window)

    fig, ax = plt.subplots()
    (scat,) = ax.plot([], [], "k.", markersize=2)
    ax.set_title("Magnetometer raw: Y vs X")
    ax.set_xlabel("X (raw)")
    ax.set_ylabel("Y (raw)")
    ax.grid(True)
    # ── CHANGEMENT 1 : ratio 1:1 garanti en permanence ──────────────────────
    ax.set_aspect("equal", adjustable="box")

    last_autoscale = time.time()

    def init():
        scat.set_data([], [])
        return (scat,)

    def update(_frame):
        nonlocal last_autoscale
        read_any = False
        while True:
            try:
                raw = ser.readline()
            except serial.SerialException:
                break
            if not raw:
                break
            line = raw.decode("utf-8", errors="ignore")
            parsed = parse_mag_line(line)
            if parsed is None:
                continue
            _ms, x, y, _z = parsed
            xs.append(x)
            ys.append(y)
            read_any = True

        if read_any:
            scat.set_data(list(xs), list(ys))

        now = time.time()
        if now - last_autoscale > 1.0 and len(xs) > 10:
            last_autoscale = now
            # ── CHANGEMENT 2 : plage identique sur X et Y, centrée sur les données ──
            cx = (max(xs) + min(xs)) / 2
            cy = (max(ys) + min(ys)) / 2
            # demi-étendue = max des deux étendues + marge 10 %
            half = max(max(xs) - min(xs), max(ys) - min(ys)) / 2 * 1.1 + 1
            ax.set_xlim(cx - half, cx + half)
            ax.set_ylim(cy - half, cy + half)

        return (scat,)

    ani = animation.FuncAnimation(fig, update, init_func=init, interval=30, blit=True)
    try:
        plt.show()
    finally:
        try:
            ser.close()
        except Exception:
            pass

if __name__ == "__main__":
    main()
