import serial
import time
import matplotlib.pyplot as plt

# open serial port
ser = serial.Serial('COM5', 115200, timeout=0.5)
time.sleep(2)

# send command
ser.write(b'a')

idx = []
des = []
cur = []

while len(idx) < 400:
    line = ser.readline().decode(errors='ignore').strip()
    if line == "":
        continue
    parts = line.split()
    if len(parts) != 3:
        continue
    try:
        i = int(parts[0])
        d = int(parts[1])
        c = int(parts[2])
        idx.append(i)
        des.append(d)
        cur.append(c)
    except:
        pass

ser.close()

# convert raw current to mA
cur_ma = [x / 3.0 for x in cur]

# plot data
plt.plot(idx, des, label='desired')
plt.plot(idx, cur_ma, label='actual')
plt.xlabel('index')
plt.ylabel('current mA')
plt.legend()
plt.grid(True)
plt.show()

for i in range(400):
    print(idx[i], des[i], cur[i])
