"""Five readable heat-feeder poses, each native 16px with an exposed amber belly."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def leech(name, top, bottom, fed=False, latch=False, spent=False):
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    p.polygon([(1, 7), (4, top+1), (8, top), (12, top+1), (14, 6),
               (14, 10), (11, bottom), (5, bottom), (2, 10)], fill='#233d43')
    p.ellipse((3, top+1, 12, bottom-1), fill='#6f9690' if not spent else '#58736f')
    p.line([(4, top+3), (7, top+2), (10, top+2)], fill='#acc1a9')
    if fed:
        p.ellipse((5, top+3, 11, bottom-2), fill='#bd813f')
        p.line([(7, top+4), (9, top+4)], fill='#e6c27c')
    for x in [4, 7, 10]:
        p.line([(x, bottom-2), (x+1, bottom)], fill='#395852')
    p.rectangle((12, 6, 15 if latch else 14, 9), fill='#bcc8a4')
    p.line([(13, 7), (15 if latch else 14, 7)], fill='#384a46')
    p.point((11, 5 if not spent else 7), fill='#dfe0ba')
    image.save(ROOT / (name+'.png'))

leech('steam_leech', 5, 11)
leech('leech_latch', 4, 11, latch=True)
leech('leech_feed', 3, 12, fed=True, latch=True)
leech('leech_swell', 1, 14, fed=True, latch=True)
leech('leech_spent', 7, 12, spent=True)


# STEAM: A warm pale puff, distinct from blue frost breath.
image = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(image)
p.polygon([(2, 8), (4, 4), (9, 2), (13, 5), (14, 9), (11, 13), (5, 12)], fill=(170, 175, 159, 60))
p.polygon([(4, 8), (6, 5), (10, 4), (12, 7), (11, 10), (7, 11)], fill=(212, 210, 185, 110))
p.line([(6, 7), (9, 6), (10, 8)], fill=(236, 227, 199, 130), width=2)
image.save(ROOT / 'steam_puff.png')
