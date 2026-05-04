import numpy as np
import matplotlib.pyplot as plt

# ---------- Basic helpers ----------

def load_csv(filename):
    """
    Load CSV with two columns: time, value.
    Returns time (1D np.array), signal (1D np.array).
    """
    data = np.loadtxt(filename, delimiter=',')
    t = data[:, 0]
    x = data[:, 1]
    return t, x

def compute_sample_rate(t):
    """
    Sample rate = number of points / total time.
    """
    total_time = t[-1] - t[0]
    fs = len(t) / total_time
    return fs

def compute_fft(x, fs):
    """
    Compute single-sided FFT magnitude and frequency axis.
    """
    N = len(x)
    # Real FFT (only positive frequencies)
    X = np.fft.rfft(x)
    freqs = np.fft.rfftfreq(N, d=1.0/fs)
    mag = np.abs(X)
    return freqs, mag

# ---------- Filters ----------

def moving_average_filter(x, window_size):
    """
    Simple moving average (FIR) implemented with a loop,
    as described in the assignment.
    """
    N = len(x)
    y = np.zeros(N)
    # Assume samples before index 0 are 0
    for i in range(N):
        start = max(0, i - window_size + 1)
        window = x[start:i+1]
        # If we are at the beginning, the effective window is smaller
        y[i] = np.sum(window) / window_size
    return y

def iir_filter(x, A, B):
    """
    First-order IIR: y[i] = A * y[i-1] + B * x[i]
    with A + B = 1.
    """
    N = len(x)
    y = np.zeros(N)
    y[0] = x[0]  # or 0, depending on how you want to start
    for i in range(1, N):
        y[i] = A * y[i-1] + B * x[i]
    return y

def fir_filter(x, weights):
    """
    General FIR filter: y[n] = sum_k w[k] * x[n-k]
    Implemented with a loop.
    """
    N = len(x)
    M = len(weights)
    y = np.zeros(N)
    for n in range(N):
        acc = 0.0
        for k in range(M):
            idx = n - k
            if idx >= 0:
                acc += weights[k] * x[idx]
        y[n] = acc
    return y

# ---------- Plotting helpers ----------

def plot_time_and_fft(t, x_orig, x_filt, fs, title_time, title_fft):
    """
    Make a figure with:
      - Top: time-domain original (black) and filtered (red)
      - Bottom: FFT magnitude original (black) and filtered (red)
    """
    freqs_orig, mag_orig = compute_fft(x_orig, fs)
    freqs_filt, mag_filt = compute_fft(x_filt, fs)

    fig, axs = plt.subplots(2, 1, figsize=(10, 6))

    # Time-domain
    axs[0].plot(t, x_orig, 'k', label='Original')
    axs[0].plot(t, x_filt, 'r', label='Filtered')
    axs[0].set_xlabel('Time [s]')
    axs[0].set_ylabel('Amplitude')
    axs[0].set_title(title_time)
    axs[0].legend()
    axs[0].grid(True)

    # Frequency-domain
    axs[1].plot(freqs_orig, mag_orig, 'k', label='Original FFT')
    axs[1].plot(freqs_filt, mag_filt, 'r', label='Filtered FFT')
    axs[1].set_xlabel('Frequency [Hz]')
    axs[1].set_ylabel('Magnitude')
    axs[1].set_title(title_fft)
    axs[1].legend()
    axs[1].grid(True)

    plt.tight_layout()
    plt.show()

# ---------- Main workflow ----------

def process_file(filename, maf_window, iir_A, iir_B, fir_weights):
    print(f"\nProcessing {filename}")
    t, x = load_csv(filename)
    fs = compute_sample_rate(t)
    print(f"  Sample rate ≈ {fs:.2f} Hz, N = {len(x)}")

    # --- Moving Average Filter ---
    x_maf = moving_average_filter(x, maf_window)
    plot_time_and_fft(
        t,
        x,
        x_maf,
        fs,
        title_time=f"{filename} - Moving Average (window={maf_window})",
        title_fft=f"{filename} - FFT (MAF window={maf_window})"
    )

    # --- IIR Filter ---
    x_iir = iir_filter(x, iir_A, iir_B)
    plot_time_and_fft(
        t,
        x,
        x_iir,
        fs,
        title_time=f"{filename} - IIR (A={iir_A}, B={iir_B})",
        title_fft=f"{filename} - FFT (IIR A={iir_A}, B={iir_B})"
    )

    # --- FIR Filter with designed weights ---
    if fir_weights is not None and len(fir_weights) > 0:
        x_fir = fir_filter(x, fir_weights)
        plot_time_and_fft(
            t,
            x,
            x_fir,
            fs,
            title_time=f"{filename} - FIR (taps={len(fir_weights)})",
            title_fft=f"{filename} - FFT (FIR taps={len(fir_weights)})"
        )
    else:
        print("  FIR weights not provided for this file; skipping FIR plot.")

def main():
    # You will tune these by eye for each signal.
    # Start with some guesses, then adjust.
    params_maf = {
        "sigA.csv": 20,
        "sigB.csv": 20,
        "sigC.csv": 5,
        "sigD.csv": 20,
    }

    params_iir = {
        # A + B must equal 1
        "sigA.csv": (0.9, 0.1),
        "sigB.csv": (0.9, 0.1),
        "sigC.csv": (0.8, 0.2),
        "sigD.csv": (0.9, 0.1),
    }

    # FIR weights: paste real coefficients from the online FIR designer.
    # Example dummy low-pass-like weights (replace with real ones):
    fir_weights_A = np.array([
        0.02, 0.05, 0.1, 0.16, 0.2, 0.16, 0.1, 0.05, 0.02
    ])
    fir_weights_B = fir_weights_A  # you can make them different per file
    fir_weights_C = fir_weights_A
    fir_weights_D = fir_weights_A

    params_fir = {
        "sigA.csv": fir_weights_A,
        "sigB.csv": fir_weights_B,
        "sigC.csv": fir_weights_C,
        "sigD.csv": fir_weights_D,
    }

    files = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]

    for fname in files:
        maf_window = params_maf[fname]
        A, B = params_iir[fname]
        fir_w = params_fir[fname]
        process_file(fname, maf_window, A, B, fir_w)

if __name__ == "__main__":
    main()
