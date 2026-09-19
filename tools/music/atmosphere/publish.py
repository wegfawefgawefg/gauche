"""Build a local listening gallery, short comparison reel, and encoded-audio report."""
import html
import json
from pathlib import Path
import subprocess
import tempfile

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.io import wavfile

from render import OUTPUT, catalog, measure
from materials import RATE


def decode(path):
    result = subprocess.run(["ffmpeg","-v","error","-i",str(path),"-f","f32le",
                             "-ac","2","-ar",str(RATE),"-"],capture_output=True,check=True)
    return np.frombuffer(result.stdout,dtype=np.float32).reshape(-1,2)


def inspect(pieces):
    reports = []
    # Long studies in one image. Short cues have the same numerical checks below.
    fig,axes = plt.subplots(9,2,figsize=(15,20),layout="constrained")
    plotrow = 0
    for piece in pieces:
        stereo = decode(OUTPUT/(piece.slug+".ogg"))
        mono = stereo.mean(axis=1)
        blocksize = RATE//10
        blocks = mono[:len(mono)//blocksize*blocksize].reshape(-1,blocksize)
        rms = np.sqrt(np.mean(blocks**2,axis=1))
        peak = np.max(np.abs(blocks),axis=1)
        derivative = np.abs(np.diff(stereo,axis=0))
        db = 20*np.log10(np.maximum(rms,1e-10))
        # Negative space measured relative to the piece's loudest 100ms, not a
        # loudness gate which would silently discard its quietest passages.
        quiet = float(np.mean(db < np.max(db)-24)*100)
        report = dict(slug=piece.slug,seconds=len(stereo)/RATE,**measure(OUTPUT/(piece.slug+".ogg")),
                      finite=bool(np.isfinite(stereo).all()),peak=float(np.max(np.abs(stereo))),
                      boundary_step=float(np.max(np.abs(stereo[0]-stereo[-1]))),
                      internal_step_p99=float(np.quantile(derivative,.99)),
                      quiet_24db_below_loudest_percent=round(quiet,1),
                      last_100ms_peak=float(np.max(np.abs(stereo[-blocksize:]))))
        assert report["finite"] and report["peak"] < .9
        assert abs(report["seconds"]-piece.seconds) < .03
        if piece.loop:
            assert report["boundary_step"] < .002, report
            time = np.arange(len(rms))*.1
            axes[plotrow,0].fill_between(time,-peak,peak,color="#617f71",alpha=.7)
            axes[plotrow,0].plot(time,rms,color="#ba775d",linewidth=.8)
            axes[plotrow,0].set(title=piece.title,xlabel="Seconds",ylabel="Amplitude",ylim=(-.42,.42))
            f,t,power = signal.spectrogram(mono,RATE,nperseg=4096,noverlap=2048)
            axes[plotrow,1].pcolormesh(t,f,10*np.log10(np.maximum(power,1e-13)),
                                     shading="auto",vmin=-105,vmax=-38,cmap="magma")
            axes[plotrow,1].set(yscale="log",ylim=(30,12000),xlabel="Seconds",ylabel="Hz")
            plotrow += 1
        else:
            assert report["last_100ms_peak"] < .002, report
        reports.append(report)
        print(json.dumps(report),flush=True)
    fig.savefig(OUTPUT/"analysis.png",dpi=105)
    plt.close(fig)
    (OUTPUT/"encoded_analysis.json").write_text(json.dumps(reports,indent=2)+"\n")


def sampler(pieces):
    # Distinct useful excerpts, with explicit silence between them; no gain changes.
    starts = [7,13,12,12,8,8,35,8,7]
    blocks,markers = [],[]
    cursor = 0
    for index,piece in enumerate(pieces):
        seconds = 14 if piece.loop else 5
        start = starts[index] if piece.loop else 0
        data = decode(OUTPUT/(piece.slug+".ogg"))[round(start*RATE):round((start+seconds)*RATE)].copy()
        fade = round(.55*RATE)
        data[:fade] *= np.linspace(0,1,fade)[:,None]
        data[-fade:] *= np.linspace(1,0,fade)[:,None]
        markers.append(dict(seconds=cursor,name=f"{index+1:02} · {piece.title}",slug=piece.slug))
        blocks.extend([data,np.zeros((RATE,2),np.float32)])
        cursor += len(data)/RATE+1
    with tempfile.TemporaryDirectory() as tmp:
        source = Path(tmp)/"reel.wav"
        wavfile.write(source,RATE,np.concatenate(blocks).astype(np.float32))
        subprocess.run(["ffmpeg","-v","error","-y","-i",str(source),"-c:a","libvorbis",
                        "-q:a","6","-metadata","title=Teeming atmosphere pass 02 comparison reel",
                        str(OUTPUT/"comparison.ogg")],check=True)
    (OUTPUT/"comparison.json").write_text(json.dumps(dict(seconds=cursor,markers=markers),indent=2)+"\n")
    return markers


def marker_buttons(player,markers):
    return "".join(f'<button data-player="{player}" data-time="{marker["seconds"]}">'
                   f'{int(marker["seconds"])//60}:{int(marker["seconds"])%60:02} '
                   f'{html.escape(marker["name"])}</button>' for marker in markers)


def page(pieces,markers):
    cards = []
    for index,p in enumerate(pieces):
        category = "cues" if not p.loop else "title" if p.zone.startswith("TITLE") else "world"
        repeat = (f'<label><input type="checkbox" data-loop="{p.slug}"> Repeat loop</label>' if p.loop else "Ends cleanly · no loop")
        cards.append(f'''<article data-category="{category}" id="track-{p.slug}">
<p class="eyebrow">{index+1:02} / {html.escape(p.zone)} / {int(p.seconds)//60}:{int(p.seconds)%60:02}</p>
<h2>{html.escape(p.title)}</h2><p>{html.escape(p.description)}</p>
<audio id="{p.slug}" controls preload="metadata" src="{p.slug}.ogg"></audio>
<div class="markers">{marker_buttons(p.slug,p.markers)}</div>
<p class="compare">{html.escape(p.contrast)}</p>
<div class="links"><a href="{p.slug}.ogg" download>Download OGG</a>{repeat}</div>
<label class="notes">Your listening notes <textarea data-note="{p.slug}" rows="2"
placeholder="More like this? Less of a particular sound? A timestamp?"></textarea></label></article>''')
    content = '''<!doctype html><html lang="en"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Teeming — atmosphere studies, pass 02</title><style>
:root{color-scheme:dark;font:16px/1.6 system-ui,sans-serif;background:#101513;color:#e3dfcc}
body{max-width:1000px;margin:auto;padding:35px 24px 70px}h1{font:500 clamp(32px,5vw,52px)/1.1 Georgia,serif}
h2{font:500 28px/1.2 Georgia,serif;margin:8px 0 16px}p{max-width:77ch;color:#bfc8bc}
.eyebrow{color:#b9a181;font-size:12px;letter-spacing:.14em}article,.reel{background:#1b231f;
border-left:3px solid #8d7654;box-shadow:7px 8px #080c0a;padding:24px 28px;margin:28px 0}
.reel{border-color:#688f83}audio{width:100%;display:block;margin:20px 0}a{color:#d8b284}
button{background:#242f29;color:#d9d8c7;border:1px solid #4c5b4e;padding:8px 12px;font:inherit;
font-size:12px;cursor:pointer}button:hover,button:focus-visible{background:#36483d;border-color:#c6b186}
.markers,.links,nav{display:flex;flex-wrap:wrap;gap:8px}.links{font-size:13px;gap:24px;margin:18px 0}
.compare{font-size:14px;color:#d2b894}.notes{display:block;font-size:12px;color:#9bad9e}
textarea{box-sizing:border-box;display:block;width:100%;margin:7px 0;background:#111914;border:1px solid #455348;
color:#e0dcca;font:14px/1.5 system-ui;padding:10px;resize:vertical}footer{font-size:13px}
nav{position:sticky;top:0;background:#101513ef;padding:12px 0;z-index:1}nav button[aria-pressed=true]{border-color:#cfb48c}
[hidden]{display:none!important}
</style><header><p class="eyebrow">TEEMING / MUSIC WORKBENCH / PASS 02</p>
<h1>Space. Material. Something underneath.</h1>
<p>Nine longer studies and six short cues, written from scratch. More held resonance and empty space;
no sampled strings or lead-and-accompaniment score. These are directions to choose between, not a finished soundtrack.</p>
<p>The <a href="../sketches/index.html">original three drafts are preserved here</a>.
These previews haven't replaced the music in the game.</p></header>
<section class="reel"><p class="eyebrow">START HERE / 2:51 COMPARISON REEL</p>
<h2>A little of every direction</h2><p>Short excerpts at their original mix levels, separated by a pause.
Use the numbered buttons to identify a sound you like, then hear the full piece below.</p>
<audio id="comparison" controls preload="metadata" src="comparison.ogg"></audio><div class="markers">'''
    content += marker_buttons("comparison",markers)+'''</div></section>
<nav aria-label="Filter studies"><button data-filter="all" aria-pressed="true">Everything (15)</button>
<button data-filter="world" aria-pressed="false">Places & threat (7)</button>
<button data-filter="title" aria-pressed="false">Title / menu (2)</button>
<button data-filter="cues" aria-pressed="false">Short cues (6)</button></nav>'''
    content += "\n".join(cards)
    content += '''<footer><p>Original synthesis and compositions. No reference recordings or SoundFont samples used.
Editable sources: tools/music/atmosphere/. Masters preserve dynamics with a single gain adjustment.
Notes stay in this browser when local storage is available; nothing is uploaded.</p>
<button id="export-notes">Download listening notes</button></footer><script>
const players=[...document.querySelectorAll('audio')];
players.forEach(p=>p.addEventListener('play',()=>players.forEach(q=>{if(p!==q)q.pause()})));
document.querySelectorAll('[data-player]').forEach(b=>b.addEventListener('click',()=>{
const p=document.getElementById(b.dataset.player);p.currentTime=Number(b.dataset.time);p.play().catch(()=>{});
}));
document.querySelectorAll('[data-loop]').forEach(b=>b.addEventListener('change',()=>{
document.getElementById(b.dataset.loop).loop=b.checked;
}));
document.querySelectorAll('[data-filter]').forEach(b=>b.addEventListener('click',()=>{
document.querySelectorAll('[data-category]').forEach(card=>{
card.hidden=b.dataset.filter!=='all'&&card.dataset.category!==b.dataset.filter;
if(card.hidden)card.querySelector('audio').pause();
});
document.querySelectorAll('[data-filter]').forEach(q=>q.setAttribute('aria-pressed',String(q===b)));
}));
const notes=[...document.querySelectorAll('[data-note]')];
notes.forEach(n=>{try{n.value=(localStorage.getItem('teeming-music-02-'+n.dataset.note)??localStorage.getItem('gauche-music-02-'+n.dataset.note))||''}catch{}
n.addEventListener('input',()=>{try{localStorage.setItem('teeming-music-02-'+n.dataset.note,n.value)}catch{}})});
document.getElementById('export-notes').addEventListener('click',()=>{
const text=notes.filter(n=>n.value.trim()).map(n=>n.dataset.note+'\\n'+n.value).join('\\n\\n');
const url=URL.createObjectURL(new Blob(['Teeming music pass 02\\n\\n'+text],{type:'text/plain'}));
const a=document.createElement('a');a.href=url;a.download='teeming-music-listening-notes.txt';a.click();
setTimeout(()=>URL.revokeObjectURL(url),1000);
});</script></html>'''
    (OUTPUT/"index.html").write_text(content)


def main():
    pieces = catalog()
    inspect(pieces)
    markers = sampler(pieces)
    page(pieces,markers)


if __name__ == "__main__":
    main()
