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

def plot_signal_and_fft(filename):
    # Load data
    t, x = load_csv(filename)

    # Compute sample rate
    total_time = t[-1] - t[0]
    sample_rate = len(t) / total_time

    # Choose moving average window
    # Nw = choose_window(x, sample_rate)
    # if filename == "sigA.csv":
    #     Nw = 400
    # elif filename == "sigB.csv":
    #     Nw = 225
    # elif filename == "sigC.csv":
    #     Nw = 5
    # elif filename == "sigD.csv":
    #     Nw = 20

    # Choose IIR weights
    if filename == "sigA.csv":
        A = 0.998
        B = 1.00 - A
    elif filename == "sigB.csv":
        A = 0.991
        B = 1.00 - A
    elif filename == "sigC.csv":
        A = 0.70
        B = 1.00 - A
    elif filename == "sigD.csv":
        A = 0.92
        B = 1.00 - A

            

    # Filtered signal
    # xf = moving_average(x, Nw)
    xf = iir_filter(x, A, B)

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
    plt.plot(t, x, 'k-')
    plt.plot(t, xf, 'r-')
    plt.xlabel("Time [s]")
    plt.ylabel("Value")
    # plt.title(f"{filename} — Moving Average = {Nw}")
    plt.title(f"{filename} — A = {A}  B = {B}")


    # Frequency-domain subplot
    plt.subplot(2,1,2)
    plt.plot(freqs, X, 'k-')
    plt.plot(freqs, XF, 'r-')
    plt.xlabel("Frequency [Hz]")
    plt.ylabel("Magnitude")
    plt.title("FFT Unfiltered (black) Filtered (red)")

    plt.tight_layout()
    plt.show()

plot_signal_and_fft("sigA.csv")
plot_signal_and_fft("sigB.csv")
plot_signal_and_fft("sigC.csv")
plot_signal_and_fft("sigD.csv")
