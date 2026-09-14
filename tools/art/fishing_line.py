"""Original spool, tiny right-facing hook and loose line fiber; 16px."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((3,3,9,4),fill='#897353');p.rectangle((3,11,9,12),fill='#897353')
p.rectangle((4,5,8,10),fill='#adb2a0')
p.line([(4,6),(8,6)],fill='#6e807e');p.line([(4,8),(8,8)],fill='#6e807e')
p.line([(8,9),(11,10),(13,10),(14,8),(14,6),(13,7)],fill='#c3c6ad')
p.line([(3,3),(8,3)],fill='#b6a17c')
im.save(root/'fishing_line.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(3,7),(10,7),(12,8),(12,10),(10,11),(9,10),(9,9)],fill='#c3c6ad')
p.rectangle((4,6,5,8),fill='#926b50');p.point((9,9),fill='#e1ddbd')
im.save(root/'fishing_hook.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(4,6),(6,7),(8,6),(10,7),(10,9)],fill='#9aa99e')
p.point((6,8),fill='#697f80');im.save(root/'debris_rope_fiber.png')
