"""Quiet 16px silhouettes: folded wool, an open broth cup, and a cold compress."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def canvas():
    image = Image.new('RGBA', (16, 16))
    return image, ImageDraw.Draw(image)

image, p = canvas()
p.polygon([(2, 4), (11, 2), (14, 5), (13, 12), (5, 14), (2, 11)], fill='#526264')
p.polygon([(3, 5), (10, 3), (12, 5), (11, 11), (5, 12), (3, 10)], fill='#bcc8c2')
p.line([(5, 6), (10, 5), (10, 9), (6, 10), (6, 8), (8, 8)], fill='#788d90')
p.line([(5, 13), (12, 11)], fill='#d3d8c4')
image.save(ROOT / 'wool_wrap.png')

image, p = canvas()
p.arc((10, 6, 15, 11), 250, 100, fill='#b59160', width=2)
p.polygon([(3, 6), (12, 6), (11, 13), (5, 14), (3, 11)], fill='#805333')
p.rectangle((5, 8, 10, 11), fill='#b59160')
p.ellipse((2, 4, 12, 8), fill='#d8bd84')
p.ellipse((4, 5, 10, 7), fill='#b4672f')
p.point((7, 6), fill='#dca851')
p.line([(6, 3), (5, 2), (6, 0)], fill='#9ca591')
p.line([(10, 3), (9, 2)], fill='#cad0b3')
image.save(ROOT / 'hot_broth.png')

image, p = canvas()
p.polygon([(2, 7), (7, 3), (13, 5), (14, 11), (9, 14), (3, 12)], fill='#617d87')
p.polygon([(3, 7), (7, 5), (12, 6), (12, 11), (8, 12), (4, 11)], fill='#a6bdbe')
p.polygon([(6, 6), (10, 6), (11, 9), (7, 10)], fill='#6ba3b7')
p.line([(7, 6), (9, 6)], fill='#d0e1d8')
p.line([(3, 9), (8, 12), (13, 9)], fill='#d0d3bb')
image.save(ROOT / 'ice_poultice.png')
