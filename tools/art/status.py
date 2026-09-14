"""Compact status icons; names/timers carry the meaning as well as color."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"

if __name__ == "__main__":
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    d.line([(2,7),(7,7),(2,12),(7,12)], fill='#baa3dd', width=2)
    d.line([(9,2),(13,2),(9,6),(13,6)], fill='#baa3dd', width=1)
    im.save(ROOT/'status_sleep.png')
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    for x,y in [(4,6),(11,4),(9,12)]:
        d.line([(x-2,y),(x+2,y)],fill='#e7c65b');d.line([(x,y-2),(x,y+2)],fill='#e7c65b')
    im.save(ROOT/'status_stun.png')
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    for a,b in [((7,1),(7,14)),((1,4),(13,11)),((1,11),(13,4))]:
        d.line([a,b],fill='#79bfe6')
    for x,y in [(4,3),(10,3),(4,12),(10,12)]:d.point((x,y),fill='#c0e3ea')
    im.save(ROOT/'status_chill.png')
