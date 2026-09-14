"""Sparse folded cloth and a padded observatory case; 16px source assets."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name in ['black_felt', 'felt_cover', 'lens_case', 'debris_felt']:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    if name == 'black_felt':
        p.polygon([(2, 5), (11, 3), (14, 10), (4, 13)], fill='#333540')
        p.polygon([(2, 5), (11, 3), (13, 7), (4, 10)], fill='#53525b')
        p.line([(3, 6), (5, 10), (12, 8)], fill='#777078')
        p.line([(5, 12), (12, 10)], fill='#69606c')
    elif name == 'felt_cover':
        p.polygon([(3, 3), (7, 1), (11, 3), (13, 9), (14, 13), (10, 14), (7, 12), (3, 14), (1, 12)], fill='#333540')
        p.line([(3, 4), (7, 2), (10, 4)], fill='#79727a')
        p.line([(5, 5), (4, 11)], fill='#55535e')
        p.line([(9, 6), (11, 12)], fill='#4e4d57')
    elif name == 'lens_case':
        p.rounded_rectangle((1, 3, 14, 14), radius=1, fill='#645441')
        p.rectangle((2, 4, 13, 11), fill='#302f3a')
        p.line([(2, 4), (13, 4)], fill='#ae9367')
        p.polygon([(4, 6), (6, 5), (8, 7), (6, 10), (4, 9)], fill='#8cabc0')
        p.line([(5, 6), (6, 6)], fill='#d1d9d5')
        p.rectangle((10, 6, 11, 10), fill='#6b7c88')
        p.rectangle((6, 12, 8, 13), fill='#b69c6c')
    else:
        p.polygon([(4, 5), (10, 4), (11, 8), (8, 11), (5, 9)], fill='#65616f')
        p.line([(5, 6), (6, 9)], fill='#8e8390')
    im.save(root / (name + '.png'))
