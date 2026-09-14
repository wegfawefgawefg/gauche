"""Small forest silhouettes: paper hive, striped wasp, sack goblin and carrion crow."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

for state in ['nest', 'nest_stirring', 'nest_empty']:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    d.line([(7, 0), (8, 3)], fill='#66563a', width=2)
    d.polygon([(6, 2), (10, 3), (13, 6), (13, 10), (10, 14), (6, 14), (3, 11), (2, 7), (4, 4)], fill='#a39472')
    for y, x, end in [(5, 5, 10), (8, 3, 12), (11, 4, 11)]:
        d.line([(x, y), (7, y+1), (end, y)], fill='#706b53')
    d.rectangle((7, 9, 9, 11), fill='#373b2c')
    if state == 'nest_stirring':
        d.point((8, 10), fill='#e5ba58'); d.line((1, 3, 2, 1), fill='#b5c0a4'); d.line((14, 10, 15, 8), fill='#b5c0a4')
    if state == 'nest_empty':
        d.polygon([(7, 7), (10, 8), (10, 11), (8, 13), (5, 10)], fill='#424332')
        d.line((5, 5, 9, 4), fill='#d0bea0')
    im.save(OUT / f'wasp_{state}.png')

for sting in [False, True]:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    d.polygon([(7, 6), (3, 2), (1, 3), (3, 6), (7, 8), (8, 4), (10, 2), (12, 4), (10, 8)], fill='#bed0b8')
    d.line([(4, 10), (3, 13), (6, 12)], fill='#b2a479')
    d.line([(9, 10), (10, 13), (12, 12)], fill='#b2a479')
    d.polygon([(1, 9), (4, 7), (8, 8), (9, 10), (5, 12)], fill='#d4a647')
    d.line((4, 8, 5, 11), fill='#45432d', width=2)
    d.rectangle((8, 7, 11, 10), fill='#524a30')
    d.point((11, 7), fill='#edd36c')
    d.line((11, 8, 14, 6), fill='#a39770')
    if sting:
        d.line((12, 10, 15, 10), fill='#e0c58c')
        d.rectangle((9, 9, 12, 10), fill='#d7a04b')
    im.save(OUT / ('wasp_sting.png' if sting else 'wasp.png'))

for knife in [False, True]:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    d.polygon([(1, 7), (5, 5), (8, 9), (6, 13), (2, 12)], fill='#877044')
    d.line((2, 8, 5, 7), fill='#b39764')
    d.polygon([(6, 7), (10, 7), (11, 12), (5, 12)], fill='#735042')
    d.line((6, 12, 5, 15), fill='#8c9b58', width=2)
    d.line((9, 12, 11, 15), fill='#8c9b58', width=2)
    d.polygon([(5, 3), (2, 2), (4, 5), (6, 7), (10, 6), (14, 3), (10, 3), (9, 1), (6, 1)], fill='#99a85d')
    d.line((6, 5, 10, 5), fill='#464d32'); d.point((9, 3), fill='#e4c882')
    d.rectangle((10, 8, 12, 9), fill='#a0ac61')
    if knife:
        d.polygon([(12, 8), (15, 5), (15, 8), (13, 10)], fill='#c7c5a6')
        d.point((13, 9), fill='#806e49')
    im.save(OUT / ('forager_goblin_knife.png' if knife else 'forager_goblin.png'))

for snatch in [False, True]:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    d.polygon([(1, 12), (5, 7), (7, 6), (12, 7), (11, 11), (6, 13)], fill='#394953')
    d.polygon([(3, 10), (6, 5), (8, 7), (6, 11)], fill='#5a6870')
    d.line((6, 13, 5, 15), fill='#88846b'); d.line((10, 12, 11, 14), fill='#88846b')
    d.rectangle((8, 4, 11, 7), fill='#434d55')
    d.point((10, 5), fill='#e3d9ac')
    d.polygon([(12, 5), (15, 6), (12, 7)], fill='#aba789')
    if snatch:
        d.polygon([(4, 7), (1, 2), (3, 2), (7, 6)], fill='#768285')
        d.line((13, 8, 15, 9), fill='#bfb28a')
    im.save(OUT / ('carrion_crow_snatch.png' if snatch else 'carrion_crow.png'))
