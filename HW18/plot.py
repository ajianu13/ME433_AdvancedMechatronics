import serial
import matplotlib.pyplot as plt

PORT = "COM5"     # sometimes COM3 with other cable
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)

print("Sending 'a' to start HW18 test...")
ser.write(b'a')        # sends a to STM
ser.flush()

idx = []
des = []
force = []

print("Collecting data...")

while len(idx) < 400:
    line = ser.readline().decode().strip()
    if not line:
        continue

    try:
        parts = line.split()
        i = int(parts[0])
        d = float(parts[1])
        f = float(parts[2])

        idx.append(i)
        des.append(d)
        force.append(f)

        print(i, d, f)

    except:
        pass

ser.close()

# Plotting
plt.figure(figsize=(10,6))

plt.subplot(2,1,1)
plt.plot(idx, des)
plt.title("Desired Current (mA)")
plt.grid(True)

plt.subplot(2,1,2)
plt.plot(idx, force)
plt.title("Measured Force (mN)")
plt.grid(True)

plt.tight_layout()
plt.show()
