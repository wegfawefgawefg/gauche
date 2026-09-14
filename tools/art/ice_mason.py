"""A fur-hooded quarry worker: broad mitts, a bronze chisel, restrained cold colors."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def mason(name, pose):
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    p.rectangle((4, 12, 6, 14), fill='#655744')
    p.rectangle((9, 12, 11, 14), fill='#655744')
    p.polygon([(4, 7), (11, 7), (12, 12), (3, 12)], fill='#566e7d')
    p.line([(5, 8), (5, 11)], fill='#829f9e')
    p.rectangle((4, 2, 11, 7), fill='#a6b5ae')
    p.rectangle((6, 3, 10, 6), fill='#493f3c')
    p.line([(7, 4), (9, 4)], fill='#d3c69e')
    p.line([(5, 7), (10, 7)], fill='#c4cbb5')
    if pose=='cut':
        p.line([(11, 9), (14, 7), (12, 3)], fill='#b79963', width=2)
        p.line([(11, 2), (14, 2)], fill='#b8cecc')
        p.rectangle((2, 8, 4, 10), fill='#a79570')
    elif pose=='jab':
        p.line([(10, 8), (14, 8)], fill='#a79570', width=2)
        p.line([(13, 7), (15, 7)], fill='#d4ceaa')
    elif pose=='build':
        p.rectangle((2, 9, 4, 11), fill='#b79963')
        p.rectangle((11, 9, 14, 11), fill='#b79963')
    elif pose=='carry':
        p.rectangle((2, 8, 4, 10), fill='#b79963')
        p.rectangle((11, 8, 13, 10), fill='#b79963')
    else:
        p.rectangle((2, 8, 3, 10), fill='#b79963')
        p.line([(12, 8), (12, 12)], fill='#b79963')
        p.point((12, 13), fill='#c4cbb5')
    image.save(ROOT/(name+'.png'))

for name, pose in [('ice_mason','idle'), ('mason_carry','carry'), ('mason_cut','cut'),
                    ('mason_build','build'), ('mason_jab','jab')]: mason(name, pose)
for thaw in [False, True]:
    image=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(image)
    p.polygon([(2, 4), (11, 2), (14, 5), (13, 12), (4, 14), (1, 11)], fill='#395b6c')
    p.polygon([(3, 5), (10, 3), (12, 5), (5, 7)], fill='#b7cecc')
    p.polygon([(3, 6), (6, 8), (12, 6), (11, 11), (5, 12), (3, 10)], fill='#749ca9')
    p.line([(4, 7), (4, 10)], fill='#9cbdc3')
    if thaw:
        p.line([(9, 5), (7, 8), (9, 10), (8, 13)], fill='#395b6c')
        p.point((3, 14), fill='#9cbdc3');p.point((13, 13), fill='#9cbdc3')
    image.save(ROOT/('ice_block_thaw.png' if thaw else 'ice_block.png'))
image=Image.new('RGBA',(16,16));p=ImageDraw.Draw(image)
p.polygon([(4, 5), (10, 4), (12, 8), (6, 11)], fill='#6b94a4')
p.line([(4, 5), (10, 4), (9, 7)], fill='#c0d5d2')
image.save(ROOT/'debris_ice_chip.png')
