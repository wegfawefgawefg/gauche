"""Copper spool, floor junction and reusable grounding stake; sparse 16px silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.ellipse((3, 3, 12, 12), fill='#48382c', outline='#b07a46')
d.ellipse((5, 5, 10, 10), fill='#7e542f', outline='#dda965')
d.ellipse((7, 7, 8, 8), fill='#282d2c')
d.line([(11, 11), (13, 13), (15, 12)], fill='#ca9151')
im.save(root / 'copper_wire.png')

im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.line([(5, 7), (7, 5), (10, 7), (8, 10), (5, 8)], fill='#473a2b', width=3)
d.line([(5, 7), (7, 6), (9, 7), (8, 9), (6, 8)], fill='#bd874d')
im.save(root / 'wire_junction.png')

for name, hot, spent in [('grounding_spike', False, False), ('spike_hot', True, True),
                         ('spike_spent', False, True)]:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    d.ellipse((4, 11, 11, 13), fill='#3e3b31')
    d.polygon([(6, 3), (9, 3), (9, 10), (7, 14), (6, 10)], fill='#c26c36' if hot else '#82867b')
    d.line([(6, 3), (6, 10)], fill='#f2b661' if hot else '#c6c6a9')
    d.rectangle((4, 2, 11, 4), fill='#756351')
    d.line([(4, 2), (10, 2)], fill='#cab284')
    d.line([(9, 7), (11, 8), (10, 10)], fill='#d29049')
    d.point((7, 4), fill='#ffdf9e' if hot else '#413e35' if spent else '#9fb6a5')
    if spent and not hot:
        d.rectangle((4, 2, 11, 4), fill='#3d3c34')
        d.line([(4, 2), (10, 2)], fill='#766e58')
        d.rectangle((6, 5, 8, 7), fill='#343c39')
    im.save(root / (name + '.png'))
