"""Folded threshold and flat in-world frames. 16px top-down silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw
out=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
d.polygon([(3,3),(8,1),(13,4),(13,12),(8,14),(3,11)],fill='#665d53')
d.polygon([(4,4),(8,2),(8,12),(4,10)],fill='#b4a887')
d.polygon([(9,3),(12,5),(12,11),(9,13)],fill='#7c919a')
d.line((8,3,8,12),fill='#343e43'); d.point((10,8),fill='#d7cfac')
im.save(out/'pocket_door.png')
for opened in (False,True):
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    d.rectangle((2,2,13,13),fill='#252b36')
    d.rectangle((3,3,12,12),outline='#7d8594',width=1)
    d.rectangle((5,5,10,10),fill='#142332' if opened else '#2f333b')
    if opened:
        d.line((4,3,11,3),fill='#a6cadd');d.line((4,12,11,12),fill='#86a9c0')
        d.line((3,5,3,10),fill='#718dba');d.line((12,5,12,10),fill='#718dba')
        d.point((6,6),fill='#698298');d.point((9,10),fill='#658296')
    else:
        d.line((5,5,10,10),fill='#636778');d.line((10,5,5,10),fill='#535c68')
    im.save(out/('pocket_threshold_open.png' if opened else 'pocket_threshold.png'))
