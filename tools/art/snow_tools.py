"""Minimal snowbank creature, scoop, packed ball and buried-supply lump."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name in ['snow_burrower', 'snow_mound', 'snow_warn', 'snow_dive']:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    if name in ['snow_mound', 'snow_warn', 'snow_dive']:
        p.polygon([(1, 12), (3, 8), (6, 6), (10, 7), (14, 11), (14, 13), (3, 14)], fill='#40575b')
        p.polygon([(2, 11), (4, 8), (7, 7), (10, 8), (13, 11), (11, 12), (4, 12)], fill='#a3b7b6')
        p.line([(4, 8), (7, 7), (10, 8)], fill='#d0d5be')
        if name == 'snow_mound':
            p.line([(2, 14), (4, 15), (8, 15)], fill='#7b9798')
        elif name == 'snow_warn':
            p.rectangle((9, 7, 13, 10), fill='#2c3736')
            p.point((11, 7), fill='#dca66c'); p.point((13, 7), fill='#dca66c')
            p.line([(10, 10), (10, 11)], fill='#e5d8b2')
            p.line([(13, 10), (13, 11)], fill='#e5d8b2')
        else:
            p.rectangle((3, 7, 5, 9), fill='#736c56')
            p.line([(3, 6), (4, 4)], fill='#d0d5be')
    else:
        p.polygon([(2, 6), (5, 4), (10, 5), (13, 8), (13, 12), (5, 13), (2, 10)], fill='#423f35')
        p.polygon([(3, 6), (6, 5), (10, 6), (12, 9), (9, 11), (4, 10)], fill='#8c8570')
        p.line([(3, 6), (6, 5), (9, 6)], fill='#c1b698')
        p.rectangle((11, 7, 14, 10), fill='#726954')
        p.point((13, 7), fill='#e5bb79')
        p.line([(5, 12), (6, 14)], fill='#d0d5be'); p.line([(10, 12), (12, 14)], fill='#d0d5be')
    im.save(root / (name + '.png'))

im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.rectangle((1, 6, 3, 10), outline='#b69762')
p.line([(3, 8), (10, 8)], fill='#806342', width=2)
p.polygon([(10, 5), (14, 5), (15, 7), (15, 10), (11, 11), (9, 9)], fill='#57757b')
p.polygon([(11, 6), (14, 6), (14, 9), (11, 10)], fill='#a0b6b3')
p.line([(11, 6), (14, 6)], fill='#ced2b9')
im.save(root / 'snow_scoop.png')

im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.polygon([(4, 4), (9, 3), (12, 6), (12, 10), (8, 12), (4, 10), (3, 7)], fill='#7c989e')
p.polygon([(5, 4), (9, 4), (11, 6), (10, 9), (5, 10), (4, 7)], fill='#c7d1bc')
p.line([(5, 5), (8, 4)], fill='#e2e1c8')
im.save(root / 'snowball.png')

im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.polygon([(2, 11), (4, 7), (10, 6), (14, 10), (12, 13), (4, 13)], fill='#7c9698')
p.polygon([(4, 8), (9, 7), (12, 9), (11, 11), (4, 11)], fill='#b2c0b6')
p.rectangle((10, 10, 12, 12), fill='#856b4b'); p.point((11, 10), fill='#c4a978')
im.save(root / 'snow_cache.png')

im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.polygon([(5, 5), (10, 5), (12, 8), (9, 11), (5, 10), (4, 7)], fill='#789397')
p.polygon([(5, 5), (9, 5), (10, 7), (6, 8)], fill='#b0c0b6')
im.save(root / 'debris_snow_clump.png')
