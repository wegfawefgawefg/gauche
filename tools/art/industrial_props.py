"""Sparse top-down steel partitions, mining bins and four loose materials."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((0,5,15,11),fill='#333b36')
p.rectangle((1,6,14,10),fill=(0,0,0,0))
for x in (0,4,8,12):
 p.line((x,6,x+4,10),fill='#747d6c');p.line((x,10,x+4,6),fill='#5e6a5b')
p.line((0,5,15,5),fill='#929885');p.line((0,11,15,11),fill='#5c6355')
p.point((0,5),fill='#b7ae82');p.point((15,11),fill='#8d724f')
im.save(root/'grate_h.png');im.transpose(Image.Transpose.ROTATE_90).save(root/'grate_v.png')
for name,ore in [('scrap_bin',False),('ore_bin',True)]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((2,3,13,13),fill='#52594f');p.rectangle((3,4,12,10),fill='#242e2a')
 p.line((3,3,12,3),fill='#9a9b80');p.line((2,4,2,12),fill='#797f6e')
 p.rectangle((3,11,12,13),fill='#686d59');p.line((4,12,11,12),fill='#3e493f')
 if ore:
  p.polygon([(4,9),(4,7),(6,5),(8,7),(8,9)],fill='#686858')
  p.polygon([(8,9),(9,5),(11,6),(12,9)],fill='#898574');p.point((10,6),fill='#b2a164')
 else:
  p.line([(4,6),(6,5),(8,6),(7,8)],fill='#ae8261')
  p.line((4,9,10,9),fill='#838e7d');p.rectangle((9,5,11,7),outline='#b0ac8c')
 im.save(root/(name+'.png'))
for name in ['basalt_chip','ore_flake','steel_washer','tin_curl']:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 if name=='basalt_chip':
  p.polygon([(4,10),(5,5),(9,4),(12,8),(9,11)],fill='#62685a');p.line((5,5,9,4),fill='#8a8c72')
 elif name=='ore_flake':
  p.polygon([(3,8),(8,4),(12,7),(9,10)],fill='#8b8360');p.line((5,7,8,5),fill='#b1a370')
 elif name=='steel_washer':
  p.ellipse((4,4,11,11),fill='#8a9788');p.ellipse((6,6,9,9),fill=(0,0,0,0));p.line((5,11,10,11),fill='#566559')
 else:
  p.line([(4,5),(10,4),(12,6),(11,9),(8,10),(5,9)],fill='#9fa48d',width=2)
  p.line((5,10,8,11),fill='#626f60')
 im.save(root/('debris_'+name+'.png'))
