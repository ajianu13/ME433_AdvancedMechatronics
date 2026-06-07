import csv
import numpy as np
import matplotlib.pyplot as plt

def load_csv(filename):
    t = []
    x = []

    with open(filename) as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 2:
                continue
            t.append(float(row[0]))
            x.append(float(row[1]))

    return np.array(t), np.array(x)

def plot_signal_and_fft(filename):
    # Load data
    t, x = load_csv(filename)

    # Compute sample rate
    total_time = t[-1] - t[0]
    sample_rate = len(t) / total_time

    # FFT
    N = len(x)
    X = np.fft.fft(x)
    freqs = np.fft.fftfreq(N, d=1.0/sample_rate)

    # Only keep positive frequencies
    idx = np.where(freqs >= 0)
    freqs = freqs[idx]
    X = np.abs(X[idx])

    # Plot
    plt.figure(figsize=(10,6))

    # Time-domain subplot
    plt.subplot(2,1,1)
    plt.plot(t, x, 'b-')
    plt.xlabel("Time [s]")
    plt.ylabel("Value")
    plt.title(f"{filename} — Signal vs Time\nSample Rate = {sample_rate:.2f} Hz")

    # Frequency-domain subplot
    plt.subplot(2,1,2)
    plt.plot(freqs, X, 'r-')
    plt.xlabel("Frequency [Hz]")
    plt.ylabel("Magnitude")
    plt.title("FFT Magnitude Spectrum")

    plt.tight_layout()
    plt.show()

# Run all of the files
plot_signal_and_fft("sigA.csv")
plot_signal_and_fft("sigB.csv")
plot_signal_and_fft("sigC.csv")
plot_signal_and_fft("sigD.csv")
