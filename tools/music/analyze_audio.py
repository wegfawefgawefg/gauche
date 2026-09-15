"""Inspect actual encoded previews: loudness, stereo peaks, waveform and spectrum."""
from pathlib import Path
import json
import subprocess

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

ROOT = Path(__file__).resolve().parents[2] / "assets/music/sketches"


def main():
    files = [ROOT / (slug + ".ogg") for slug in ("hollow_bough", "fern_lanterns", "shift_below")]
    figure, axes = plt.subplots(3, 2, figsize=(14, 9), layout="constrained")
    reports = []
    for row, source in enumerate(files):
        measured = subprocess.run(["ffmpeg", "-hide_banner", "-i", str(source), "-af",
            "loudnorm=I=-23:TP=-3:LRA=14:print_format=json", "-f", "null", "-"],
            capture_output=True, text=True, check=True).stderr
        values = json.loads(measured[measured.rfind("{"):measured.rfind("}") + 1])
        raw = subprocess.run(["ffmpeg", "-v", "error", "-i", str(source), "-f", "f32le",
                              "-ac", "2", "-ar", "22050", "-"], capture_output=True, check=True).stdout
        stereo = np.frombuffer(raw, np.float32).reshape(-1, 2)
        mono = stereo.mean(axis=1)
        count = len(mono) // 2205
        blocks = mono[:count*2205].reshape(count, 2205)
        times = np.arange(count) * .1
        peak = np.max(np.abs(blocks), axis=1)
        rms = np.sqrt(np.mean(blocks**2, axis=1))
        axes[row, 0].fill_between(times, -peak, peak, color="#8daf91", alpha=.65)
        axes[row, 0].plot(times, rms, color="#9b533d", linewidth=.9)
        axes[row, 0].set(title=source.stem + " — peak / RMS", xlabel="Seconds", ylabel="Amplitude", ylim=(-.6, .6))
        freq, time, power = signal.spectrogram(mono, 22050, nperseg=2048, noverlap=1024)
        db = 10 * np.log10(np.maximum(power, 1e-12))
        axes[row, 1].pcolormesh(time, freq, db, shading="auto", vmin=-100, vmax=-35, cmap="magma")
        axes[row, 1].set(yscale="log", ylim=(35, 10000), xlabel="Seconds", ylabel="Hz", title="Time / frequency energy")
        report = {"file": source.name, "duration": len(stereo)/22050,
                  "integrated_lufs": float(values["input_i"]),
                  "true_peak_dbtp": float(values["input_tp"]),
                  "loudness_range_lu": float(values["input_lra"]),
                  "decoded_peak": float(np.max(np.abs(stereo))),
                  "decoded_boundary_step": float(np.max(np.abs(stereo[0]-stereo[-1]))),
                  "finite": bool(np.isfinite(stereo).all())}
        reports.append(report)
        print(json.dumps(report), flush=True)
    figure.savefig(ROOT / "analysis.png", dpi=130)
    (ROOT / "encoded_analysis.json").write_text(json.dumps(reports, indent=2) + "\n")


if __name__ == "__main__":
    main()
