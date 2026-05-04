from machine import ADC, Pin
import time
import sys

pot = ADC(26)   # GP26 = ADC0

while True:
    value = pot.read_u16()  # 0–65535
    sys.stdout.write(str(value) + "\n")
    sys.stdout.flush()
    time.sleep(0.02)  # 50 Hz update
