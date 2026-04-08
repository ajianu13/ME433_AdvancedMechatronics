import time      # Provides timing functions (like sleep) for the sweep speed
import board     # Provides access to the physical pin naming on the Pico 2 W
import pwmio     # Handles the Pulse Width Modulation hardware on the RP2350 chip
from adafruit_motor import servo  # Higher-level library to convert angles to PWM duty cycles

print("Starting the code")

# Setup PWM output on GPIO 16
# duty_cycle=0: Starts with the signal 'off' so the motor doesn't jump on power-up
# frequency=50: Standard servos expect a 50Hz signal (one pulse every 20 milliseconds)
pwm = pwmio.PWMOut(board.GP16, duty_cycle=0, frequency=50)

# Create a Servo object to allow setting of angle instead of duty cycles
# min_pulse: The pulse duration (in microseconds) for 0 degrees
# max_pulse: The pulse duration (in microseconds) for 180 degrees
# Use 500us and 2500us to match the range used in the previous C implementation
my_servo = servo.Servo(pwm, min_pulse=500, max_pulse=2500)

print("Starting Servo Sweep")

while True:
    # Sweep from 0 to 180 degrees
    # range(start, stop, step): 181 is used as the stop because range is exclusive
    for angle in range(0, 181, 5):  
        my_servo.angle = angle      # Command the servo to move to a specific degree
        time.sleep(0.01)            # 10ms delay to allow movement to happen
    
    # Sweep back
    for angle in range(180, -1, -5):
        my_servo.angle = angle
        time.sleep(0.01)            