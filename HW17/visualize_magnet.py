import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

PORT = 'COM6'
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)

fig, ax = plt.subplots(figsize=(5,5))
ax.set_xlim(-1.2, 1.2)
ax.set_ylim(-1.2, 1.2)
ax.set_aspect('equal')
ax.set_title("Magnet Orientation")

circle = plt.Circle((0,0), 1.0, fill=False, linestyle='--')
ax.add_patch(circle)

north_line, = ax.plot([], [], lw=4, color='red')
south_point, = ax.plot([], [], 'bo', markersize=10)

north_text = ax.text(0, 0, "", fontsize=12, color='red', ha='center')
south_text = ax.text(0, 0, "", fontsize=12, color='blue', ha='center')

last_angle = None
flip_warning_shown = False

def update(frame):
    global last_angle, flip_warning_shown

    line = ser.readline().decode().strip()
    try:
        angle_deg = float(line)
    except:
        return north_line

    theta = np.radians(angle_deg)

    xN = np.cos(theta)
    yN = np.sin(theta)

    xS = -xN
    yS = -yN

    north_line.set_data([0, xN], [0, yN])
    south_point.set_data([xS], [yS])

    north_text.set_position((xN * 1.1, yN * 1.1))
    north_text.set_text("N")

    south_text.set_position((xS * 1.1, yS * 1.1))
    south_text.set_text("S")

    if last_angle is not None:
        delta = angle_deg - last_angle
        if delta < -5 and not flip_warning_shown:
            print("Warning: Angle decreases when rotating counterclockwise.")
            print("The magnet may be flipped. Turn it over if needed.")
            flip_warning_shown = True

    last_angle = angle_deg
    return north_line

ani = animation.FuncAnimation(fig, update, interval=10)
plt.show()