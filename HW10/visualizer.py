import pgzrun
import serial
import threading

WIDTH = 800
HEIGHT = 400

circle_x = WIDTH // 2
circle_y = HEIGHT // 2

# ---- SERIAL SETUP ----
ser = serial.Serial('COM3', 115200)  # CHANGE THIS TO YOUR PORT
latest_value = 32768

def read_serial():
    global latest_value
    while True:
        try:
            line = ser.readline().decode().strip()
            if line.isdigit():
                latest_value = int(line)
        except:
            pass

threading.Thread(target=read_serial, daemon=True).start()

# ---- PGZ FUNCTIONS ----
def update():
    global circle_x
    # Map 0–65535 to 0–WIDTH
    circle_x = int((latest_value / 65535) * WIDTH)

def draw():
    screen.clear()
    screen.draw.filled_circle((circle_x, circle_y), 40, "cyan")
    screen.draw.text(f"Value: {latest_value}", (10, 10), color="white")

pgzrun.go()
