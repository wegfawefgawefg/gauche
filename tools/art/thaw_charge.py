"""Three states of a small sealed copper heater packed into pale cloth."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name,core in [('thaw_charge','#53645d'),('thaw_charge_lit','#da9b55'),('thaw_charge_ready','#f1dc9b')]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 d.rectangle((4,4,11,12),fill='#a8b7ac');d.rectangle((5,3,10,3),fill='#687c76')
 d.line((3,6,3,10),fill='#63766e');d.rectangle((5,12,10,13),fill='#68756a')
 d.rectangle((6,5,9,10),fill='#765c41');d.rectangle((7,6,8,9),fill=core)
 d.line((4,8,5,8),fill='#d3d5b5');d.line((10,8,11,8),fill='#d3d5b5')
 d.line([(10,4),(12,3),(12,1)],fill='#a98961');d.point((12,1),fill=core)
 im.save(root/(name+'.png'))
