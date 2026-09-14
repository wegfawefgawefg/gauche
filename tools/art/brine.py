"""Salt-green glass, a frozen folded-metal lunch tin and its loose lid."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
# FLASK: Broad fill and a salt mark distinguish it from water and cold flasks.
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((6,2,9,5),fill='#b2b7a3');p.rectangle((6,1,9,2),fill='#887757')
p.polygon([(5,5),(10,5),(13,10),(12,14),(3,14),(2,10)],fill='#758d81')
p.polygon([(4,8),(11,8),(12,11),(11,13),(4,13),(3,11)],fill='#a1ae80')
p.line([(4,7),(3,10)],fill='#d5d4b9');p.line([(6,10),(9,10)],fill='#e1debb')
p.point((8,11),fill='#e1debb');im.save(root/'brine_flask.png')
# TIN: Frost at the seam, folded metal sides, one recessed lid.
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((2,5,13,12),fill='#596e73');p.rectangle((3,3,12,9),fill='#93a8a7')
p.rectangle((4,4,11,7),fill='#6c8e8b');p.line([(2,9),(13,9),(12,12),(4,12)],fill='#a5b4a6')
p.line([(3,3),(6,3),(6,4)],fill='#d1d8c9');p.line([(9,8),(12,8),(12,6)],fill='#c0d1c4')
p.rectangle((7,5,9,5),fill='#b6b08e');im.save(root/'frozen_lunch_tin.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(4,6),(9,4),(12,6),(10,10),(5,11),(3,8)],fill='#718987')
p.line([(4,6),(9,5),(11,6)],fill='#adb6a4');p.line([(5,9),(8,8)],fill='#4e686a')
im.save(root/'debris_tin_lid.png')
