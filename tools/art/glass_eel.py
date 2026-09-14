"""Spare translucent river eels: a visible spine and charged organs, no texture noise."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
spine = [(1, 10), (3, 11), (6, 10), (7, 7), (10, 6), (13, 7)]
for name, body, edge, organ in [
    ('glass_eel', '#527876', '#9aac9e', '#aaa66b'),
    ('eel_charge', '#659ba8', '#badacb', '#f0e5a5'),
    ('eel_spent', '#455e65', '#748d89', '#6d7758'),
    ('eel_stranded', '#626861', '#a9aa8b', '#918662'),
]:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    p.line(spine, fill='#26383c', width=5)
    p.line(spine, fill=body, width=3)
    p.line(spine, fill=edge, width=1)
    p.rectangle((12, 5, 14, 8), fill=body)
    p.point((14, 6), fill='#ede7b5'); p.point((15, 8), fill='#26383c')
    for xy in [(4, 11), (7, 8), (10, 6)]: p.point(xy, fill=organ)
    if name == 'eel_charge':
        p.line([(3, 6), (4, 5), (3, 3), (5, 2)], fill='#b1dbe0')
        p.line([(11, 10), (12, 11), (11, 13), (13, 14)], fill='#b1dbe0')
        for x, y in [(4, 11), (7, 8), (10, 6)]:
            p.rectangle((x, y, x+1, y+1), fill=organ)
    im.save(root / (name + '.png'))

# BATTERY: Preserved eel organs in a small green jar with two exposed copper prongs.
im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.rectangle((3, 5, 10, 12), fill='#253a39')
p.rectangle((4, 6, 9, 11), fill='#567968')
p.line([(5, 10), (7, 9), (6, 8), (8, 7)], fill='#d2c881', width=2)
p.line([(4, 6), (4, 9)], fill='#a8bd9c')
p.rectangle((3, 4, 10, 5), fill='#5b5550')
p.line([(10, 6), (13, 6), (13, 4)], fill='#c4965b')
p.line([(10, 10), (13, 10), (13, 9)], fill='#c4965b')
im.save(root / 'eel_battery.png')
