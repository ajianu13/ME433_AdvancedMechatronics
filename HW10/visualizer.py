# pgzrun visualizer.py

import pgzrun
import serial
import threading

# Serial reader thread setup
ser = serial.Serial('COM3', 115200) 
btn = 0
ax = ay = az = 0.0

def serial_thread():
    global btn, ax, ay, az
    while True:
        try:
            line = ser.readline().decode().strip()
            parts = line.split(',')
            # Expected format is BTN,x,AX,x,AY,x,AZ,x
            btn = int(parts[1])
            ax = float(parts[3])
            ay = float(parts[5])
            az = float(parts[7])
        except:
            pass

threading.Thread(target=serial_thread, daemon=True).start()

# Pygame Zero drawing setup
WIDTH = 600
HEIGHT = 400

ball_x = WIDTH // 2
ball_y = HEIGHT // 2
ball_radius = 20

obstacles = [
    (200, 150, 200, 20),
    (100, 300, 150, 20),
    (350, 50, 150, 20)
]

def collides_with_obstacle(x, y):
    for ox, oy, ow, oh in obstacles:
        if x + ball_radius > ox and x - ball_radius < ox + ow:
            if y + ball_radius > oy and y - ball_radius < oy + oh:
                return True
    return False

def update():
    global ball_x, ball_y

    # IMU tilt controls the motion of the ball
    # ball_x += ax * 10
    # ball_y += ay * 10

    new_x = ball_x + ax * 10
    new_y = ball_y + ay * 10

    if not collides_with_obstacle(new_x, ball_y):
        ball_x = new_x

    if not collides_with_obstacle(ball_x, new_y):
        ball_y = new_y

    if ball_x < ball_radius:
        ball_x = ball_radius
    if ball_x > WIDTH - ball_radius:
        ball_x = WIDTH - ball_radius
    if ball_y < ball_radius:
        ball_y = ball_radius
    if ball_y > HEIGHT - ball_radius:
        ball_y = HEIGHT - ball_radius

    # Keep the ball inside the window
    ball_x = max(20, min(WIDTH - 20, ball_x))
    ball_y = max(20, min(HEIGHT - 20, ball_y))

def draw():
    screen.fill((30, 30, 30))

    # Button pressed means red, not pressed means blue
    if btn == 1:
        color = (255, 80, 80)   # red
    else:
        color = (0, 200, 255)   # blue

    for ox, oy, ow, oh in obstacles:
        screen.draw.filled_rect(Rect((ox, oy), (ow, oh)), (200, 200, 200))

    screen.draw.filled_circle((ball_x, ball_y), ball_radius, color)


pgzrun.go()

