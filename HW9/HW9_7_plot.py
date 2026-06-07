import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import firwin

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

def moving_average(x, N):
    y = []
    for i in range(len(x)):
        if i < N:
            y.append(np.mean(x[:i+1]))
        else:
            y.append(np.mean(x[i-N+1:i+1]))
    return np.array(y)

def iir_filter(x, A, B):
    y = []
    for i in range(len(x)):
        if i == 0:
            y.append(x[0])
        else:
            y.append(A * y[i-1] + B * x[i])
    return np.array(y)

def fir_filter(x, w):
    y = []
    L = len(w)
    for i in range(len(x)):
        s = 0.0
        for k in range(L):
            if i-k >= 0:
                s += w[k] * x[i-k]
        y.append(s)
    return np.array(y)


def choose_window(x, sample_rate):
    N = len(x)
    X = np.abs(np.fft.fft(x))
    f = np.fft.fftfreq(N, d=1.0/sample_rate)
    idx = np.where(f > 0)
    f = f[idx]
    X = X[idx]
    peak = np.argmax(X)
    target = X[peak]

    bestN = 1
    bestScore = None

    for Nw in [2,3,5,8,10,15,20,30,40,50,75,100]:
        xf = moving_average(x, Nw)
        XF = np.abs(np.fft.fft(xf))[idx]

        main = XF[peak] / target
        high = np.sum(XF[int(len(XF)*0.3):])

        td = np.mean(np.abs(x - xf))

        score = (1 - main) * 4 + high * 0.00001 + td

        if bestScore is None or score < bestScore:
            bestScore = score
            bestN = Nw

    return bestN

def compute_sample_rate(filename):
    t, x = load_csv(filename)
    total_time = t[-1] - t[0]
    fs = len(t) / total_time
    print(filename, "sample rate =", fs, "Hz")
    return fs

def compute_sample_rate(filename):
    t, _ = load_csv(filename)
    return len(t) / (t[-1] - t[0])

def design_fir_filter(filename, cutoff_hz, num_taps=201, window='blackman'):
    fs = compute_sample_rate(filename)
    nyq = fs / 2.0

    # Normalized cutoff
    cutoff_norm = cutoff_hz / nyq

    # Generate FIR weights
    w = firwin(num_taps, cutoff=cutoff_norm, window=window)

    # Print comma-separated weights
    # print("\nFIR weights (comma-separated):\n")
    # for val in w:
    #     print(f"{val},")

    return w

def plot_signal_and_fft(filename):
    # Load data
    t, x = load_csv(filename)

    # Compute sample rate
    total_time = t[-1] - t[0]
    sample_rate = len(t) / total_time

    # Choose FIR weights
    if filename == "sigA.csv":
        fc = 3
        bw = "N/A"
        window = "hamming"
        w = design_fir_filter("sigA.csv", cutoff_hz=fc, num_taps=701, window=window)

    elif filename == "sigB.csv":
        fc = 1
        bw = "N/A"
        window = "blackman"
        w = design_fir_filter("sigB.csv", cutoff_hz=fc, num_taps=301, window=window)

    elif filename == "sigC.csv":
        fc = 20
        bw = "N/A"
        window = "blackman"
        w = design_fir_filter("sigC.csv", cutoff_hz=fc, num_taps=101, window=window)

    elif filename == "sigD.csv":
        fc = 10
        bw = "N/A"
        window = "hamming"
        w = design_fir_filter("sigD.csv", cutoff_hz=fc, num_taps=26, window=window)



    # Filtered signal
    xf = fir_filter(x, w)

    # FFT
    N = len(x)
    X = np.fft.fft(x)
    freqs = np.fft.fftfreq(N, d=1.0/sample_rate)

    # Filtered FFT
    XF = np.fft.fft(xf)

    # Only keep positive frequencies
    idx = np.where(freqs >= 0)
    freqs = freqs[idx]
    X = np.abs(X[idx])
    XF = np.abs(XF[idx])

    # Plot
    plt.figure(figsize=(10,6))

    # Time-domain subplot
    plt.subplot(2,1,1)
    plt.plot(t, x, 'k-', label='unfiltered')
    plt.plot(t, xf, 'r-', label='filtered')
    plt.xlabel("Time [s]")
    plt.ylabel("Value")
    plt.title(f"{filename} — FIR LPF, taps={len(w)}, fc={fc}Hz, bw={bw}Hz, window={window}")
    plt.legend()

    # Frequency-domain subplot
    plt.subplot(2,1,2)
    plt.plot(freqs, X, 'k-', label='unfiltered FFT')
    plt.plot(freqs, XF, 'r-', label='filtered FFT')
    plt.xlabel("Frequency [Hz]")
    plt.ylabel("Magnitude")
    plt.title("FFT Unfiltered (black) Filtered (red)")
    plt.legend()

    plt.tight_layout()
    plt.show()


# compute_sample_rate("sigA.csv")
# compute_sample_rate("sigB.csv")
# compute_sample_rate("sigC.csv")
# compute_sample_rate("sigD.csv")
# sigA.csv sample rate = 10000.20000400008 Hz
# sigB.csv sample rate = 3300.2000121219467 Hz
# sigC.csv sample rate = 2500.1250062503127 Hz
# sigD.csv sample rate = 400.0833506980621 Hz

plot_signal_and_fft("sigA.csv")
plot_signal_and_fft("sigB.csv")
plot_signal_and_fft("sigC.csv")
plot_signal_and_fft("sigD.csv")
