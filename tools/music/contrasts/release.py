"""Small pass 03 gallery and comparison reel; verifies actual exported samples."""
import html
import json
from pathlib import Path
import subprocess
import tempfile

import numpy as np
from scipy.io import wavfile

from build import OUTPUT,pieces
from materials import RATE
from publish import decode
from render import measure


def main():
    cards,reports,excerpts,buttons=[],[],[],[]
    for index,p in enumerate(pieces()):
        data=decode(OUTPUT/(p.slug+".ogg"))
        report=dict(slug=p.slug,seconds=len(data)/RATE,**measure(OUTPUT/(p.slug+".ogg")),
                    finite=bool(np.isfinite(data).all()),peak=float(np.max(np.abs(data))),
                    boundary_step=float(np.max(np.abs(data[0]-data[-1]))))
        assert report["finite"] and report["peak"]<.7 and report["boundary_step"]<.002
        assert abs(report["seconds"]-p.seconds)<.03
        reports.append(report)
        start=[52,1,1,1,30][index]
        clip=data[start*RATE:(start+20)*RATE].copy()
        fade=round(.4*RATE)
        clip[:fade]*=np.linspace(0,1,fade)[:,None]
        clip[-fade:]*=np.linspace(1,0,fade)[:,None]
        excerpts.extend([clip,np.zeros((RATE,2),np.float32)])
        buttons.append(f'<button data-id="reel" data-time="{index*21}">{index+1:02} · {html.escape(p.title)}</button>')
        markers="".join(f'<button data-id="{p.slug}" data-time="{m["seconds"]}">'
                        f'{int(m["seconds"])//60}:{int(m["seconds"])%60:02} {html.escape(m["name"])}</button>' for m in p.markers)
        cards.append(f'''<article><p class="tag">{index+1:02} / {p.zone} / {int(p.seconds)//60}:{int(p.seconds)%60:02}</p>
<h2>{html.escape(p.title)}</h2><p>{html.escape(p.description)}</p>
<audio id="{p.slug}" controls preload="metadata" src="{p.slug}.ogg"></audio>
<div class="markers">{markers}</div><p class="question">{html.escape(p.contrast)}</p>
<p><a href="{p.slug}.ogg" download>Download</a> · <label><input type="checkbox" data-loop="{p.slug}"> Repeat</label></p></article>''')
    with tempfile.TemporaryDirectory() as directory:
        source=Path(directory)/"reel.wav"
        wavfile.write(source,RATE,np.concatenate(excerpts))
        subprocess.run(["ffmpeg","-v","error","-y","-i",str(source),"-c:a","libvorbis",
                        "-q:a","6",str(OUTPUT/"comparison.ogg")],check=True)
    (OUTPUT/"encoded_analysis.json").write_text(json.dumps(reports,indent=2)+"\n")
    page='''<!doctype html><html lang="en"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Teeming · distinct palettes</title>
<style>:root{color-scheme:dark;font:16px/1.6 system-ui;background:#111714;color:#dedccb}
body{max-width:980px;margin:auto;padding:30px 24px 70px}h1{font:46px/1.15 Georgia}h2{font:29px Georgia}
article{background:#1b2520;padding:24px 28px;margin:28px 0;border-left:3px solid #937b57}
.tag{font-size:12px;letter-spacing:.1em;color:#bcab89}p{max-width:77ch}audio{width:100%;margin:15px 0}
a{color:#d4b583}.question{color:#baa990}.markers{display:flex;gap:7px;flex-wrap:wrap}
button{background:#2b382f;color:#ded8c3;border:1px solid #5c6a57;padding:9px;cursor:pointer}
button:hover,button:focus-visible{border-color:#c9b081}.approved{border-color:#8da985}</style>
<p class="tag">TEEMING / MUSIC PASS 03</p><h1>One pipe song. Different worlds.</h1>
<p>The last batch was one sound spread across too many pieces. Its shop cue is approved;
the pipe palette now belongs to one Ice candidate. Four shorter studies use different dominant instruments and rhythms.</p>
<article><h2>1:45 comparison reel</h2><p>Twenty seconds of each new direction, at its original mix level.</p>
<audio id="reel" controls preload="metadata" src="comparison.ogg"></audio><div class="markers">'''
    page+="".join(buttons)+"</div></article>"+"".join(cards)
    page+='''<article class="approved"><p class="tag">APPROVED / PRESERVED UNCHANGED</p>
<h2>Shop arrives — wheels and a crooked bell</h2><p>Selected by the user for the shop arrival. Playback integration is still pending.</p>
<audio controls preload="metadata" src="../atmosphere/shop_arrives.ogg"></audio></article>
<p>These new tracks await listening feedback. Earlier drafts remain in the
<a href="../atmosphere/index.html">pipe-study gallery</a> and
<a href="../sketches/index.html">first-sketch gallery</a>.</p>
<script>const players=[...document.querySelectorAll('audio')];
players.forEach(p=>p.addEventListener('play',()=>players.forEach(q=>{if(q!==p)q.pause()})));
document.querySelectorAll('[data-id]').forEach(b=>b.addEventListener('click',()=>{
const p=document.getElementById(b.dataset.id);p.currentTime=Number(b.dataset.time);p.play().catch(()=>{});}));
document.querySelectorAll('[data-loop]').forEach(b=>b.addEventListener('change',()=>{
document.getElementById(b.dataset.loop).loop=b.checked;}));</script></html>'''
    (OUTPUT/"index.html").write_text(page)
    print(json.dumps(reports,indent=2))


if __name__=="__main__":
    main()
