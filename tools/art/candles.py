"""Small wax lamps and wound linen, using broad shapes and sparse highlights."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for lit in (False, True):
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    p.polygon([(4,13),(6,12),(6,7),(10,7),(10,12),(12,13),(11,14),(4,14)],fill='#bba77c')
    p.rectangle((6,8,8,12),fill='#dfc99a');p.line([(9,9),(9,11)],fill='#f0dcae')
    p.line([(8,5),(8,7)],fill='#554230')
    if lit:
        p.polygon([(8,1),(7,4),(6,5),(7,7),(9,7),(10,5),(9,3)],fill='#db793a')
        p.line([(8,4),(8,6)],fill='#ffdc80')
    im.save(root/('candle_lit.png' if lit else 'candle_stub.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((5,3,10,12),fill='#7c6243');p.rectangle((3,3,12,4),fill='#b19764')
p.rectangle((3,12,12,13),fill='#b19764');p.rectangle((4,5,11,10),fill='#b8ad8e')
p.line([(5,6),(10,6)],fill='#ded2b0');p.line([(5,9),(10,9)],fill='#827c62')
p.line([(11,9),(13,10),(13,14)],fill='#d0c4a0');im.save(root/'wick_spool.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(5,7),(8,6),(11,8),(9,10),(5,9)],fill='#c0ab80')
p.line([(6,7),(8,7)],fill='#e0c798');im.save(root/'debris_wax.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(5,9),(7,7),(10,8)],fill='#6b5c49');p.point((10,8),fill='#a27045')
im.save(root/'debris_charred_wick.png')
