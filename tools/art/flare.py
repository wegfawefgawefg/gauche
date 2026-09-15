"""Red pyrotechnic tube, lit tip and soft spent cinder fragments."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name,lit in [('signal_flare',False),('flare_burning',True)]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 d.rectangle((3,7,11,9),fill='#a94e40');d.line([(3,7),(11,7)],fill='#d98665')
 d.rectangle((3,7,4,9),fill='#c5b89b');d.rectangle((10,7,11,9),fill='#d6c7a0')
 if lit:
  d.ellipse((10,5,15,11),fill='#c94736');d.ellipse((12,6,14,10),fill='#f49b64')
  d.rectangle((13,7,14,8),fill='#f4dfba');d.point((11,3),fill='#dc7952');d.point((15,4),fill='#ba513e')
 im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.polygon([(5,6),(9,5),(11,8),(8,10),(6,9)],fill='#857f74')
d.line([(5,6),(8,7),(10,6)],fill='#bd8065');d.point((7,9),fill='#c0afa0')
im.save(root/'debris_flare_cinder.png')
