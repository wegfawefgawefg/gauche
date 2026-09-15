"""A squat furnace worker with coal sack and shovel; small flat 16px shapes."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,pose in [('stoker','idle'),('stoker_pack','pack'),('stoker_scoop','scoop'),('stoker_swing','swing')]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.polygon([(1,7),(4,6),(6,9),(5,13),(1,12)],fill='#66503c')
 p.rectangle((4,12,6,14),fill='#5c4435');p.rectangle((9,12,11,14),fill='#5c4435')
 p.polygon([(5,6),(10,6),(12,12),(4,12)],fill='#844d32')
 p.rectangle((6,2,10,6),fill='#715b45');p.rectangle((5,2,11,3),fill='#423d32')
 p.line([(6,1),(10,1)],fill='#8d704a');p.rectangle((7,4,10,4),fill='#edb664')
 p.rectangle((7,7,9,11),fill='#b38751');p.point((8,8),fill='#e6b66e')
 if pose=='pack':
  p.line([(10,8),(12,6),(11,5)],fill='#aa7b4f',width=2)
  p.rectangle((12,4,14,5),fill='#dc8139');p.point((13,4),fill='#f7d37c')
 elif pose=='scoop':
  p.line([(7,8),(3,9)],fill='#ab7c51',width=2)
  p.line([(12,8),(11,13)],fill='#73593a');p.rectangle((10,12,12,14),fill='#778077')
 elif pose=='swing':
  p.line([(11,8),(13,4)],fill='#aa7b4f',width=2)
  p.line([(13,8),(13,2)],fill='#76593c');p.rectangle((12,0,14,3),fill='#919788')
 else:
  p.line([(12,8),(13,13)],fill='#76593c');p.rectangle((12,12,14,14),fill='#858c7e')
 im.save(root/(name+'.png'))
for name,hot in [('coal_spit',True),('coal_spit_cold',False)]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 if hot:p.polygon([(1,5),(6,6),(3,9),(8,10),(6,12),(12,9)],fill='#b9512d')
 p.polygon([(8,5),(12,5),(14,7),(13,10),(9,11),(7,8)],fill='#dc853a' if hot else '#635e50')
 p.polygon([(10,6),(12,6),(13,8),(10,9)],fill='#f2cc70' if hot else '#a29b83')
 im.save(root/(name+'.png'))
