import numpy as np
import matplotlib.pyplot as plt

# spring curve
def spring_curve(x):
    return x

# bump curve
def bump_curve(x):
    return np.exp(-(x * x * x * x))

# detent curve
def detent_curve(x):
    return np.tanh(x * x * x)

# toggle curve
def toggle_curve(x):
    left = np.tanh((x - 0.2) * 12)
    right = np.tanh((x + 0.2) * 12)
    return left - right

# dip curve
def dip_curve(x):
    return -np.exp(-(x * x * x * x))

# keep values in range
def norm(v):
    return v / np.max(np.abs(v))

# displacement range
x = np.linspace(-1, 1, 1000)

# curves
spring = norm(spring_curve(x))
bump = norm(bump_curve(x))
detent = norm(detent_curve(x))
toggle = norm(toggle_curve(x))
dip = norm(dip_curve(x))

# make plot
plt.figure(figsize=(10, 6))
plt.plot(x, spring, label="spring")
plt.plot(x, bump, label="bump")
plt.plot(x, detent, label="detent")
plt.plot(x, toggle, label="toggle")
plt.plot(x, dip, label="dip")

# labels
plt.title("force displacement curves")
plt.xlabel("displacement")
plt.ylabel("force")

plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
