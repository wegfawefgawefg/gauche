"""Make a self-contained local listening page, with relative audio asset links."""
from pathlib import Path
import html
import json

ROOT = Path(__file__).resolve().parents[2] / "assets/music/sketches"
SLUGS = ["hollow_bough", "fern_lanterns", "shift_below"]


def main():
    cards = []
    for index, slug in enumerate(SLUGS):
        info = json.loads((ROOT / (slug + ".json")).read_text())
        markers = "".join(f'<button data-player="{slug}" data-time="{section["seconds"]:.3f}">'
                          f'{int(section["seconds"])//60}:{int(section["seconds"])%60:02} '
                          f'{html.escape(section["name"])}</button>' for section in info["sections"])
        cards.append(f'''<article>
<p class="eyebrow">{index+1:02} / {'FOREST' if index<2 else 'INDUSTRY'} / FIRST SKETCH</p>
<h2>{html.escape(info['title'])}</h2><p>{html.escape(info['description'])}</p>
<audio id="{slug}" controls preload="metadata" src="{slug}.ogg"></audio>
<div class="markers">{markers}</div>
<p class="links"><a href="{slug}.ogg" download>Download OGG</a>
<label><input type="checkbox" data-loop="{slug}"> Repeat to hear the loop</label></p></article>''')
    page = '''<!doctype html><html lang="en"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Teeming — first music sketches</title><style>
:root{color-scheme:dark;font:17px/1.55 system-ui,sans-serif;background:#101612;color:#e4e0cc}
body{max-width:920px;margin:0 auto;padding:36px 24px 70px}header{margin-bottom:34px}
h1{font:600 clamp(30px,5vw,48px)/1.1 Georgia,serif;margin:12px 0 20px}
h2{font:500 28px/1.2 Georgia,serif;margin:6px 0 12px}.eyebrow{color:#c5a776;letter-spacing:.14em;font-size:12px}
article{background:#1a231e;border-left:3px solid #8d6c43;padding:24px 28px;margin:24px 0;box-shadow:7px 8px #080d0a}
p{max-width:74ch;color:#c6cbbb}audio{display:block;width:100%;margin:22px 0 16px}
button{font:inherit;font-size:12px;color:#d6c5a8;background:#222c24;border:1px solid #465442;padding:7px 10px;cursor:pointer}
button:hover,button:focus-visible{border-color:#c5a776;background:#344335}.markers{display:flex;gap:7px;flex-wrap:wrap}
a{color:#e4ba80}.links{display:flex;gap:24px;flex-wrap:wrap;font-size:13px}footer{font-size:13px;color:#a1ad9c}
</style><header><p class="eyebrow">TEEMING / MUSIC WORKBENCH / PASS 01</p>
<h1>Roots, lanterns, and the shift below.</h1>
<p>Three original directions to listen through. These are shorter arrangement sketches,
not the finished biome soundtrack. They are mixed quietly and retain their swells and rests.</p>
<p>Each has section buttons for jumping around. Nothing plays automatically.</p></header>'''
    page += "\n".join(cards)
    page += '''<footer>Original Teeming scores. Cello, woodwinds, horns and other sampled instruments
from FluidR3 (MIT), layered with custom synthesis. Editable sources: tools/music/.<br>
Useful feedback: which tune stays with you; which instruments feel right or wrong;
which passages should breathe, grow, or disappear.</footer><script>
const players=[...document.querySelectorAll('audio')];
players.forEach(player=>player.addEventListener('play',()=>players.forEach(other=>{if(other!==player)other.pause()})));
document.querySelectorAll('[data-player]').forEach(button=>button.addEventListener('click',()=>{
 const player=document.getElementById(button.dataset.player);player.currentTime=Number(button.dataset.time);player.play();
}));
document.querySelectorAll('[data-loop]').forEach(box=>box.addEventListener('change',()=>{
 document.getElementById(box.dataset.loop).loop=box.checked;
}));</script></html>'''
    (ROOT / "index.html").write_text(page)


if __name__ == "__main__":
    main()
