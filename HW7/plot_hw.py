import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("hw7_data.csv", delimiter=",")

a = data[:,0]
b = data[:,1]

t = np.arange(len(a))

plt.figure(figsize=(10,5))
plt.plot(t, a, label="sine")
plt.plot(t, b, label="triangle")
plt.xlabel("sample")
plt.ylabel("adc value")
plt.title("HW7 DAC output read by Pico ADC")
plt.legend()
plt.grid(True)
plt.show()
