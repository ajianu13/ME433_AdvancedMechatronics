import serial
import time
import matplotlib.pyplot as plt

PORT = 'COM6'
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)

angles = []
times = []

start = time.time()

print("Collecting data... rotate the magnet/arm")

while time.time() - start < 10:
    line = ser.readline().decode().strip()
    if not line:
        continue
    try:
        angle = float(line)
        angles.append(angle)
        times.append(time.time() - start)
    except:
        pass

ser.close()

plt.figure(figsize=(10,4))
plt.plot(times, angles, '-b')
plt.xlabel("Time (s)")
plt.ylabel("Angle (deg)")
plt.title("AS5600 Angle vs Time")
plt.grid(True)
plt.show()
