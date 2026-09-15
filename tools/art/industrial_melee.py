"""Two compact, right-facing workshop hammers in the native flat palette."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,rubber in [('press_hammer',False),('rubber_mallet',True)]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((1,7,11,9),fill='#594331');p.line((2,7,10,7),fill='#a78756')
 p.rectangle((2,8,4,9),fill='#756d4d')
 if rubber:
  p.rectangle((10,3,14,12),fill='#303936');p.rectangle((10,3,13,4),fill='#697363')
  p.line((14,5,14,11),fill='#465147');p.rectangle((11,11,13,12),fill='#202b28')
  p.rectangle((10,7,11,8),fill='#a6a085')
 else:
  p.polygon([(9,2),(13,2),(15,4),(15,12),(9,12)],fill='#565d58')
  p.rectangle((9,2,13,4),fill='#b7b9a0');p.line((9,5,9,11),fill='#969b88')
  p.rectangle((10,10,14,12),fill='#353e3a');p.line((14,4,14,9),fill='#828d82')
 im.save(root/(name+'.png'))
