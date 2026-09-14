"""Small, quiet brass optics with readable slash and splitter silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name in ['lens_carbine', 'mirror_shard', 'crystal_lens', 'debris_mirror_chip', 'debris_crystal_splinter']:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    if name == 'lens_carbine':
        p.rectangle((1, 7, 10, 10), fill='#665c47')
        p.polygon([(2, 9), (5, 9), (4, 13), (1, 13)], fill='#7b6849')
        p.rectangle((6, 6, 12, 9), fill='#b09c67')
        p.rectangle((7, 7, 11, 8), fill='#424f56')
        p.rectangle((12, 5, 14, 10), fill='#526e7a')
        p.line([(13, 6), (13, 9)], fill='#b9d3c9')
        p.line([(5, 5), (8, 5)], fill='#9ea899')
    elif name == 'mirror_shard':
        p.polygon([(2, 12), (11, 3), (14, 3), (4, 14)], fill='#617b87')
        p.line([(3, 12), (12, 3)], fill='#ccd9cf', width=2)
        p.line([(5, 12), (13, 4)], fill='#94b4bd')
        p.rectangle((2, 13, 5, 14), fill='#9f895c')
        p.rectangle((11, 2, 14, 3), fill='#9f895c')
    elif name == 'crystal_lens':
        p.polygon([(7, 2), (13, 8), (8, 14), (2, 8)], fill='#687b96')
        p.polygon([(7, 3), (11, 8), (8, 12), (4, 8)], fill='#9baec1')
        p.line([(7, 4), (7, 8), (10, 8)], fill='#d1d9d5')
        p.line([(7, 8), (7, 11)], fill='#c1c9cb')
        p.rectangle((5, 13, 10, 14), fill='#8f7b58')
    elif name == 'debris_mirror_chip':
        p.polygon([(5, 6), (11, 4), (8, 11)], fill='#74929c')
        p.line([(5, 6), (10, 5)], fill='#b5c9c4')
    else:
        p.polygon([(7, 3), (10, 7), (6, 12), (5, 7)], fill='#7e87a7')
        p.line([(7, 4), (6, 9)], fill='#bbc8cc')
    im.save(root / (name + '.png'))
