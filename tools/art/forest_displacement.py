"""Small horn and hook silhouettes, facing right like the other held equipment."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets'/'graphics'
def canvas():
    image=Image.new('RGBA',(16,16))
    return image,ImageDraw.Draw(image)
image,d=canvas()
d.polygon([(2,4),(5,4),(5,8),(7,10),(9,9),(11,5),(13,3),(15,4),(15,10),(12,10),(9,13),(5,13),(2,10)],fill='#92774c')
d.line([(3,5),(3,9),(6,11),(8,11),(11,7)],fill='#d3b97b',width=2)
d.line((14,4,14,10),fill='#dfcb9b')
d.line((15,5,15,9),fill='#433d2e')
image.save(OUT/'hunting_horn.png')
image,d=canvas()
d.ellipse((1,6,9,13),outline='#8f794f',width=2)
d.line([(6,9),(11,7),(13,8)],fill='#b09b69')
d.line([(8,7),(10,3),(13,2),(15,4),(15,7),(13,9)],fill='#a6aba4',width=2)
d.line((13,7,13,9),fill='#d4d7c1')
image.save(OUT/'rope_hook.png')
image,d=canvas()
d.line((2,8,10,8),fill='#82714d')
d.line([(8,8),(9,4),(12,3),(14,5),(14,8),(11,11)],fill='#b4b9ad',width=2)
d.point((12,4),fill='#ece9d4')
image.save(OUT/'hook_head.png')
