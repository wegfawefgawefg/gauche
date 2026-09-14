"""Small slate seals, a woven fishing basket and its local scraps."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name in ['seal_thief', 'seal_carry', 'seal_eat', 'seal_bark', 'seal_rest']:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    # BODY: Broad taper, paired tail flippers and a small whiskered muzzle face right.
    p.polygon([(1,11),(4,10),(5,7),(8,5),(11,5),(13,7),(13,10),
               (11,12),(5,12),(2,14),(2,12),(0,12)], fill='#647a7b')
    p.polygon([(5,8),(8,6),(11,6),(12,8),(9,8),(7,10),(4,11)], fill='#8c9f99')
    p.polygon([(7,10),(10,10),(9,13),(7,14)], fill='#485e64')
    p.line([(5,12),(3,13)], fill='#485e64')
    p.rectangle((11,8,14,10), fill='#b4b7a2')
    p.point((14,8), fill='#303e48')
    p.point((11,7), fill='#242f37')
    p.line([(12,10),(15,11)], fill='#819491')
    if name == 'seal_bark':
        p.rectangle((12,9,15,11), fill='#353744')
        p.point((13,9), fill='#d1cfb2')
        p.line([(11,6),(13,6)], fill='#c8c8af')
    if name == 'seal_eat':
        p.line([(10,8),(11,9)], fill='#293c44')
        p.line([(12,11),(14,12)], fill='#b4b7a2')
    if name == 'seal_rest':
        p.line([(10,7),(12,7)], fill='#34474b')
    if name == 'seal_carry':
        p.line([(3,10),(1,10)], fill='#b0bcb1')
    im.save(root / (name+'.png'))

# CREEL: Sparse horizontal ribs, oval lid and a loop handle, no full checker weave.
im = Image.new('RGBA',(16,16)); p = ImageDraw.Draw(im)
p.line([(5,4),(5,1),(10,1),(11,4)], fill='#bba376')
p.polygon([(2,6),(4,4),(12,4),(14,6),(13,13),(4,14),(2,11)], fill='#826e50')
p.ellipse((2,3,14,8), fill='#ac9466')
p.ellipse((4,4,12,6), fill='#4e5547')
p.line([(3,8),(13,8)], fill='#c0a67a')
p.line([(4,11),(12,11)], fill='#ad9367')
p.line([(6,8),(6,13)], fill='#695c46')
p.line([(10,8),(10,13)], fill='#695c46')
p.rectangle((12,5,13,7), fill='#d2c8a3')
p.point((13,5), fill='#b76b50')
im.save(root/'fishing_creel.png')
im = Image.new('RGBA',(16,16)); p = ImageDraw.Draw(im)
p.line([(3,9),(5,7),(10,7),(12,8)], fill='#af9463')
p.line([(5,8),(10,8)], fill='#73664a')
p.point((12,9), fill='#af9463')
im.save(root/'debris_wicker.png')
im = Image.new('RGBA',(16,16)); p = ImageDraw.Draw(im)
p.rectangle((6,5,8,10), fill='#b9b59b')
p.rectangle((6,5,8,6), fill='#a86348')
p.line([(7,11),(9,12)], fill='#7b7860')
im.save(root/'debris_fishing_float.png')
