"""Carved effigy face; the worn version opens its two pale eyes."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for awake in [False,True]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 d.polygon([(4,2),(11,2),(13,5),(11,12),(8,15),(4,12),(2,5)],fill='#796d53')
 d.polygon([(4,3),(8,2),(8,13),(5,11),(3,5)],fill='#a49974')
 d.line([(8,3),(9,8),(7,8)],fill='#c1b58d')
 d.rectangle((4,5,6,7),fill='#242d2a');d.rectangle((9,5,11,7),fill='#242d2a')
 if awake:
  d.line((4,6,6,6),fill='#e4e3be');d.line((9,6,11,6),fill='#e4e3be')
 d.line((6,11,9,11),fill='#303932');d.point((10,3),fill='#b7aa80')
 im.save(root/('mask_watching.png' if awake else 'effigy_mask.png'))
