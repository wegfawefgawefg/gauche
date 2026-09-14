"""Observatory keeper: slate plate, brass visor and an edge-facing mirror shield."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name in ['mirror_knight', 'knight_raise', 'knight_guard', 'knight_swing', 'knight_recover']:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    slump = 1 if name == 'knight_recover' else 0
    # BODY: Quiet silhouette; the separate shield carries the directional information.
    p.polygon([(4, 7), (10, 7), (12, 12), (9, 14), (3, 12)], fill='#354650')
    p.rectangle((4, 8, 9, 11), fill='#71868a')
    p.line([(4, 8), (9, 8)], fill='#adb6ac')
    p.rectangle((4, 12, 5, 14), fill='#657780'); p.rectangle((8, 12, 9, 14), fill='#657780')
    p.rectangle((3, 14, 5, 15), fill='#434a46'); p.rectangle((8, 14, 10, 15), fill='#434a46')
    p.polygon([(4, 3+slump), (7, 1+slump), (10, 3+slump), (10, 6+slump), (4, 6+slump)], fill='#5b707e')
    p.line([(5, 3+slump), (8, 2+slump), (9, 3+slump)], fill='#bdc6bb')
    p.rectangle((5, 4+slump, 10, 5+slump), fill='#293944')
    p.line([(6, 4+slump), (9, 4+slump)], fill='#b69d67')
    p.rectangle((3, 8, 4, 10), fill='#a18b5a')
    if name == 'knight_swing':
        p.line([(9, 8), (12, 5)], fill='#9daaa8', width=2)
        p.line([(12, 5), (14, 1)], fill='#d1d5bf')
        p.line([(11, 4), (14, 6)], fill='#a88e56')
    elif name == 'knight_raise':
        p.line([(9, 8), (12, 6)], fill='#8fa1a3', width=2)
    elif name == 'knight_guard':
        p.rectangle((9, 8, 11, 9), fill='#acb8ae')
    else:
        p.line([(10, 9), (12, 13)], fill='#91a5a7', width=2)
        p.line([(11, 12), (13, 10)], fill='#9d8656')
    im.save(root/(name+'.png'))
for name in ['knight_shield', 'knight_shield_lit']:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    p.polygon([(4, 2), (10, 2), (13, 5), (13, 11), (10, 14), (4, 14), (2, 11), (2, 5)], fill='#887a56')
    p.polygon([(5, 3), (10, 3), (12, 6), (12, 10), (9, 13), (5, 13), (3, 10), (3, 6)], fill='#627e8d')
    p.polygon([(5, 4), (9, 4), (11, 6), (10, 9), (6, 11), (4, 9)], fill='#a8c3c4' if name.endswith('lit') else '#7e979d')
    p.line([(9, 4), (5, 9)], fill='#d4e2d5' if name.endswith('lit') else '#a4b7b2')
    # FRONT EDGE: Bright lip points right before renderer rotation.
    p.line([(12, 5), (12, 11)], fill='#d9d4ac' if name.endswith('lit') else '#a29060')
    im.save(root/(name+'.png'))
