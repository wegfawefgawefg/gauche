"""Offline pass 02 renderer. Run with the documented NumPy/SciPy/Numba environment."""
import argparse
from dataclasses import asdict
import json
from pathlib import Path
import subprocess
import tempfile

import numpy as np
from scipy import signal
from scipy.io import wavfile

import forest
import titles
import zones
from materials import RATE, breath, chamber, friction, strike

ROOT = Path(__file__).resolve().parents[3]
OUTPUT = ROOT / "assets/music/atmosphere"


def measure(path):
    run = subprocess.run(["ffmpeg","-hide_banner","-i",str(path),"-af",
                          "loudnorm=I=-26:TP=-6:LRA=18:print_format=json","-f","null","-"],
                         capture_output=True,text=True,check=True)
    text = run.stderr
    report = json.loads(text[text.rfind("{"):text.rfind("}")+1])
    return {"lufs":float(report["input_i"]),"true_peak_dbtp":float(report["input_tp"]),
            "range_lu":float(report["input_lra"])}


def place(mix, mono, event, loop):
    start = round(event["at"] * RATE)
    pan = event["pan"]
    angle = (pan+1)*np.pi/4
    stereo = mono[:,None] * np.array([np.cos(angle),np.sin(angle)]) * event["gain"]
    count = min(len(stereo),len(mix)-start)
    mix[start:start+count] += stereo[:count]
    if loop and len(stereo) > count:
        mix[:len(stereo)-count] += stereo[count:]


def synthesize(piece):
    mix = np.zeros((round(piece.seconds*RATE),2))
    for index,event in enumerate(piece.events):
        # Per-event seeds: adding a later sound does not change every earlier timbre.
        rng = np.random.default_rng(piece.seed*1000+index)
        args = (event["duration"],event["hz"],rng)
        options = {key:value for key,value in event.items()
                   if key not in ("at","duration","kind","hz","gain","pan")}
        function = {"breath":breath,"strike":strike,"friction":friction}[event["kind"]]
        place(mix,function(*args,**options),event,piece.loop)
    lengths = np.array([1429,1789,2281,2671])
    # A pre-roll full cycle settles every feedback tail before capturing one period.
    source = np.tile(mix.mean(axis=1),2) if piece.loop else mix.mean(axis=1)
    wet = chamber(source,lengths,piece.room,.27)
    mix += wet[-len(mix):] * piece.wet
    hp = signal.butter(2,30,fs=RATE,btype="highpass",output="sos")
    if piece.loop:
        mix = signal.sosfilt(hp,np.tile(mix,(2,1)),axis=0)[-len(mix):]
    else:
        mix = signal.sosfilt(hp,mix,axis=0)
        # Cues are finite, not loops. Leave time for tails and a clean last half-second.
        fade = round(.5*RATE)
        mix[-fade:] *= np.linspace(1,0,fade)[:,None]**2
    assert np.isfinite(mix).all() and np.max(np.abs(mix)) > 1e-8
    return mix


def render(piece,output,masters=None):
    print(f"Rendering {piece.title} ({piece.seconds:g}s, {len(piece.events)} gestures)",flush=True)
    mix = synthesize(piece)
    output.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="gauche-music-") as temporary:
        source = Path(temporary)/"master.wav"
        wavfile.write(source,RATE,mix.astype(np.float32))
        before = measure(source)
        # A single gain. Loudnorm only measures; its processed stream is discarded.
        gain = min(piece.target-before["lufs"], -7-before["true_peak_dbtp"])
        mix *= 10**(gain/20)
        wavfile.write(source,RATE,mix.astype(np.float32))
        if masters:
            masters.mkdir(parents=True,exist_ok=True)
            wavfile.write(masters/(piece.slug+".wav"),RATE,mix.astype(np.float32))
        destination = output/(piece.slug+".ogg")
        subprocess.run(["ffmpeg","-v","error","-y","-i",str(source),"-c:a","libvorbis",
                        "-q:a","6","-metadata","title="+piece.title,"-metadata",
                        "artist=Gauche original atmosphere studies",str(destination)],check=True)
        encoded = measure(destination)
    report = asdict(piece)
    report["encoded"] = encoded
    report["fixed_gain_db"] = gain
    report["source_boundary_step"] = float(np.max(np.abs(mix[0]-mix[-1])))
    report["sample_rate"] = RATE
    # Store the score too, making feedback on a timestamp traceable to a gesture.
    (output/(piece.slug+".json")).write_text(json.dumps(report,indent=2)+"\n")
    print(f"  {encoded}",flush=True)


def catalog():
    return [*forest.pieces(),*zones.pieces(),*titles.pieces()]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--only",help="One exact track slug")
    parser.add_argument("--output",type=Path,default=OUTPUT)
    parser.add_argument("--masters",type=Path,help="Optional lossless float WAVs outside the repo")
    args = parser.parse_args()
    pieces = catalog()
    if args.only and args.only not in [piece.slug for piece in pieces]:
        parser.error("Unknown slug: "+args.only)
    for piece in pieces:
        if not args.only or args.only == piece.slug:
            render(piece,args.output,args.masters)


if __name__ == "__main__":
    main()
