"""Minimal top-down tree silhouettes: broad quiet crowns and sparse branches."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
for name in ['oak', 'pine']:
    im = Image.new('RGBA', (48, 48)); p = ImageDraw.Draw(im)
    p.line((23, 43, 23, 23, 11, 11), fill='#151f17', width=3)
    p.line((23, 25, 36, 10), fill='#151f17', width=2)
    if name == 'oak':
        for box in [(6, 5, 25, 22), (20, 2, 39, 20), (1, 19, 24, 38),
                    (17, 13, 44, 35), (18, 26, 37, 44)]:
            p.ellipse(box, fill='#18291d')
            x0,y0,x1,y1 = box
            p.arc((x0+2,y0+2,x1-2,y1-3), 200, 275, fill='#1e3021', width=2)
    else:
        for y, radius in [(9, 8), (17, 13), (26, 17), (36, 19)]:
            p.polygon([(24,y-8), (24-radius,y+5), (20,y+2), (24,y+7),
                       (29,y+2), (24+radius,y+5)], fill='#162b25')
            p.line((24,y-6,18,y+1), fill='#1d342a', width=2)
    im.save(ROOT / f'canopy_{name}.png')
