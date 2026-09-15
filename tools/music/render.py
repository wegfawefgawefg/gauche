"""Render editable Python scores to quiet stereo loops; no audio device is opened.

python3 tools/music/render.py [--output assets/music/sketches] [--only hollow_bough]
Requires numpy, scipy, libfluidsynth, FluidR3_GM.sf2 and ffmpeg.
"""
from pathlib import Path
import argparse
import ctypes as ct
import ctypes.util
import json
import math
import subprocess
import tempfile

import numpy as np
from scipy import signal
from scipy.io import wavfile

from woodland import hollow_bough, fern_lanterns
from underworks import shift_below

RATE = 44100
ROOT = Path(__file__).resolve().parents[2]


class Instruments:
    """Only offline sample rendering; note notation and arrangement live in the scores."""
    def __init__(self, voices, soundfont):
        self.lib = ct.CDLL(ctypes.util.find_library("fluidsynth"))
        declarations = {
            "new_fluid_settings": (ct.c_void_p, []),
            "fluid_settings_setnum": (ct.c_int, [ct.c_void_p, ct.c_char_p, ct.c_double]),
            "fluid_settings_setint": (ct.c_int, [ct.c_void_p, ct.c_char_p, ct.c_int]),
            "new_fluid_synth": (ct.c_void_p, [ct.c_void_p]),
            "fluid_synth_sfload": (ct.c_int, [ct.c_void_p, ct.c_char_p, ct.c_int]),
            "fluid_synth_program_change": (ct.c_int, [ct.c_void_p, ct.c_int, ct.c_int]),
            "fluid_synth_cc": (ct.c_int, [ct.c_void_p, ct.c_int, ct.c_int, ct.c_int]),
            "fluid_synth_noteon": (ct.c_int, [ct.c_void_p, ct.c_int, ct.c_int, ct.c_int]),
            "fluid_synth_noteoff": (ct.c_int, [ct.c_void_p, ct.c_int, ct.c_int]),
            "fluid_synth_write_float": (ct.c_int, [ct.c_void_p, ct.c_int, ct.c_void_p,
                                                    ct.c_int, ct.c_int, ct.c_void_p, ct.c_int, ct.c_int]),
            "delete_fluid_synth": (None, [ct.c_void_p]),
            "delete_fluid_settings": (None, [ct.c_void_p]),
        }
        for name, (result, arguments) in declarations.items():
            function = getattr(self.lib, name)
            function.restype, function.argtypes = result, arguments
        self.settings = self.lib.new_fluid_settings()
        for key, value in [(b"synth.sample-rate", RATE), (b"synth.gain", .55)]:
            self.lib.fluid_settings_setnum(self.settings, key, value)
        for key, value in [(b"synth.reverb.active", 0), (b"synth.chorus.active", 0),
                           (b"synth.polyphony", 128), (b"synth.cpu-cores", 1)]:
            self.lib.fluid_settings_setint(self.settings, key, value)
        self.synth = self.lib.new_fluid_synth(self.settings)
        if not self.synth or self.lib.fluid_synth_sfload(self.synth, str(soundfont).encode(), 1) < 0:
            raise RuntimeError("Cannot load instrument source")
        for channel, (program, volume, pan) in voices.items():
            self.lib.fluid_synth_program_change(self.synth, channel, program)
            self.lib.fluid_synth_cc(self.synth, channel, 7, volume)
            self.lib.fluid_synth_cc(self.synth, channel, 10, pan)
            self.lib.fluid_synth_cc(self.synth, channel, 11, 90)

    def render(self, score):
        # Render an extra decay tail. The mix folds it across the loop seam below.
        result = np.zeros((round((score.duration + 8) * RATE), 2), dtype=np.float32)
        cursor = 0
        for seconds, kind, channel, key, value in sorted(score.events):
            end = min(len(result), round(seconds * RATE))
            if end > cursor:
                block = result[cursor:end]
                address = block.ctypes.data
                self.lib.fluid_synth_write_float(self.synth, len(block), address, 0, 2, address, 1, 2)
                cursor = end
            if kind == 0:
                self.lib.fluid_synth_noteoff(self.synth, channel, key)
            elif kind == 1:
                self.lib.fluid_synth_noteon(self.synth, channel, key, value)
            else:
                self.lib.fluid_synth_cc(self.synth, channel, key, value)
        block = result[cursor:]
        self.lib.fluid_synth_write_float(self.synth, len(block), block.ctypes.data, 0, 2, block.ctypes.data, 1, 2)
        self.lib.delete_fluid_synth(self.synth)
        self.lib.delete_fluid_settings(self.settings)
        return result


def band(data, low, high):
    return signal.sosfilt(signal.butter(2, [low, high], btype="bandpass", fs=RATE, output="sos"), data)


def percussion(kind, tone, rng):
    duration = {"frame": 1.3, "war": 2.8, "brush": .36, "metal": 5.2}[kind]
    t = np.arange(round(duration * RATE)) / RATE
    if kind == "brush":
        data = band(rng.normal(size=len(t)), 1000, 6200) * np.exp(-t * 15)
    else:
        data = np.zeros_like(t)
        ratios = [1, 1.47, 2.09, 2.65, 3.18] if kind != "metal" else [1, 1.414, 2.756, 4.07, 5.43]
        for n, ratio in enumerate(ratios):
            decay = (1.9 if kind == "war" else 3.8 if kind == "frame" else .7) * (1 + n * .40)
            phase = 2 * np.pi * (tone * ratio * t + (3 if kind != "metal" else .1) * (1 - np.exp(-t * 35)))
            data += np.sin(phase) * np.exp(-t * decay) / (1 + n * n * .7)
        if kind != "metal":
            data += band(rng.normal(size=len(t)), 180, 2000) * .23 * np.exp(-t * 35)
    data *= np.minimum(1, t / .004)
    data *= np.minimum(1, (duration - t) / .03)
    return data / max(.001, np.max(np.abs(data)))


def place(mix, mono, seconds, gain, pan=0):
    start = round(seconds * RATE)
    count = min(len(mono), len(mix) - start)
    if count <= 0:
        return
    angle = (pan + 1) * np.pi / 4
    mix[start:start + count, 0] += mono[:count] * gain * np.cos(angle)
    mix[start:start + count, 1] += mono[:count] * gain * np.sin(angle)


def texture(duration, midi, rng):
    t = np.arange(round(duration * RATE)) / RATE
    frequency = 440 * 2 ** ((midi - 69) / 12)
    # Bowed-air bed: low harmonics with slow independent beating, not broadband space pads.
    data = np.zeros(len(t))
    for partial, weight in [(1, 1), (2, .31), (3, .13), (5, .04)]:
        phase = rng.uniform(0, 2 * np.pi)
        data += weight * np.sin(2 * np.pi * frequency * partial * t + .12 * np.sin(t * 2.1) + phase)
    data += band(rng.normal(size=len(t)), 350, 1500) * .07
    data *= np.sin(np.pi * np.minimum(t / duration, 1)) ** .8
    return data


def room(mix, seed):
    # Short early reflections plus frequency-damped diffuse tail, shared stereo room.
    rng = np.random.default_rng(seed)
    mono = mix.mean(axis=1)
    wet = np.zeros_like(mix)
    length = round(2.6 * RATE)
    t = np.arange(length) / RATE
    for channel in range(2):
        impulse = band(rng.normal(size=length), 160, 4600) * np.exp(-t * 3.5)
        impulse[:round(.042 * RATE)] = 0
        impulse /= max(.001, np.sqrt(np.sum(impulse ** 2)))
        wet[:, channel] = signal.fftconvolve(mono, impulse, mode="full")[:len(mono)] * .22
        for seconds, gain in [(.027, .13), (.049, .09), (.081, .055)]:
            delay = round((seconds + channel * .007) * RATE)
            wet[delay:, channel] += mix[:-delay, 1 - channel] * gain
    return mix + wet


def render(score, output, soundfont):
    print(f"Rendering {score.title}: {score.duration:.2f}s", flush=True)
    rng = np.random.default_rng(score.seed)
    mix = Instruments(score.voices, soundfont).render(score)
    for kind, seconds, gain, pan, tone in score.percussion:
        place(mix, percussion(kind, tone, rng), seconds, gain, pan)
    for seconds, duration, key, gain in score.textures:
        place(mix, texture(duration, key, rng), seconds, gain)
    mix = room(mix, score.seed)
    mix = signal.sosfilt(signal.butter(2, 28, btype="highpass", fs=RATE, output="sos"), mix, axis=0)
    count = round(score.duration * RATE)
    tail = mix[count:]
    mix = mix[:count].copy()
    mix[:len(tail)] += tail
    # One fixed gain per piece; no limiter/compressor pumping away its arrangement dynamics.
    mix *= .50 / max(.001, np.max(np.abs(mix)))
    output.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as temporary:
        source = Path(temporary) / "master.wav"
        wavfile.write(source, RATE, mix.astype(np.float32))
        measurement = subprocess.run(["ffmpeg", "-hide_banner", "-i", str(source), "-af",
            "loudnorm=I=-23:TP=-3:LRA=14:print_format=json", "-f", "null", "-"],
            capture_output=True, text=True, check=True).stderr
        metrics = json.loads(measurement[measurement.rfind("{"):measurement.rfind("}") + 1])
        gain_db = min(-23 - float(metrics["input_i"]), -3 - float(metrics["input_tp"]))
        mix *= 10 ** (gain_db / 20)
        wavfile.write(source, RATE, mix.astype(np.float32))
        path = output / (score.slug + ".ogg")
        subprocess.run(["ffmpeg", "-hide_banner", "-loglevel", "error", "-y", "-i", str(source),
                        "-c:a", "libvorbis", "-q:a", "6", "-metadata", "title=" + score.title,
                        "-metadata", "artist=Gauche original music sketches", str(path)], check=True)
    report = {"title": score.title, "description": score.description, "seconds": score.duration,
              "integrated_lufs_estimate": round(float(metrics["input_i"]) + gain_db, 2),
              "loudness_range_lu": float(metrics["input_lra"]),
              "true_peak_db_estimate": round(float(metrics["input_tp"]) + gain_db, 2),
              "loop_boundary_step": float(np.max(np.abs(mix[0] - mix[-1]))),
              "sections": [{"seconds": score.at(bar), "name": name} for bar, name in score.sections]}
    (output / (score.slug + ".json")).write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report), flush=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, default=ROOT / "assets/music/sketches")
    parser.add_argument("--soundfont", type=Path, default=Path("/usr/share/sounds/sf2/FluidR3_GM.sf2"))
    parser.add_argument("--only", default="")
    args = parser.parse_args()
    for score in (hollow_bough(), fern_lanterns(), shift_below()):
        if not args.only or score.slug == args.only:
            render(score, args.output, args.soundfont)


if __name__ == "__main__":
    main()
