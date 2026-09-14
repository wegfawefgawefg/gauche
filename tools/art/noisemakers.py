"""Small brass bell and red paper firecracker. Transparent, quiet 16px silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT = Path(__file__).resolve().parents[2] / 'assets/graphics'

def canvas():
    im = Image.new('RGBA', (16, 16)); return im, ImageDraw.Draw(im)

im, d = canvas()
d.rectangle((7, 1, 8, 5), fill='#866d4a')
d.point((7, 1), fill='#c5a777')
d.polygon([(5, 5), (10, 5), (11, 10), (13, 12), (2, 12), (4, 10)], fill='#b09350')
d.line([(5, 6), (5, 9), (4, 11)], fill='#e0c579')
d.line((3, 13, 12, 13), fill='#6e5937')
d.rectangle((7, 13, 8, 14), fill='#c8ad68')
im.save(OUT / 'hand_bell.png')
for lit in [False, True]:
    im, d = canvas()
    d.rectangle((5, 6, 9, 13), fill='#a94035')
    d.line((5, 6, 5, 12), fill='#d06b48')
    d.line((9, 6, 9, 13), fill='#6f342c')
    d.line((5, 7, 8, 7), fill='#d9bd75')
    d.line((5, 12, 8, 12), fill='#d9bd75')
    d.line([(7, 5), (7, 3), (10, 3), (11, 1)], fill='#b3a28a')
    if lit:
        d.point((11, 1), fill='#fff0a5'); d.point((13, 2), fill='#eaac54')
        d.point((10, 0), fill='#ed7735'); d.point((12, 4), fill='#d59644')
    im.save(OUT / ('firecracker_lit.png' if lit else 'firecracker.png'))
