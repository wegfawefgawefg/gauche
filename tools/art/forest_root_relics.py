"""Twisted root auger and an unusual paired seed, in the quiet forest palette."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets'/'graphics'
def canvas():
    image=Image.new('RGBA',(16,16))
    return image,ImageDraw.Draw(image)
image,d=canvas()
d.polygon([(1,5),(4,3),(7,5),(12,6),(15,8),(12,10),(7,11),(4,13),(1,11)],fill='#75603e')
d.line([(2,6),(5,5),(8,7),(10,6)],fill='#ae9862',width=2)
d.line([(3,11),(6,9),(8,10),(11,8),(13,8)],fill='#c3b883',width=2)
d.polygon([(3,4),(2,1),(5,2),(6,5)],fill='#6c8750')
d.point((14,8),fill='#e1dbb2')
image.save(OUT/'root_drill.png')
image,d=canvas()
d.polygon([(1,6),(3,4),(7,4),(10,5),(15,8),(10,11),(7,12),(3,12),(1,10)],fill='#81724a')
d.line([(2,6),(4,5),(6,8),(4,11),(7,10),(9,7),(11,7)],fill='#d4c78e',width=2)
d.line([(2,10),(4,8),(6,6),(8,6)],fill='#586642')
d.point((13,8),fill='#e5e1bf')
image.save(OUT/'drill_root.png')
image,d=canvas()
d.polygon([(2,8),(3,4),(6,3),(9,5),(7,10),(4,12)],fill='#8b719d')
d.polygon([(7,10),(9,5),(12,4),(14,7),(12,12),(9,13)],fill='#b5a66c')
d.line([(4,6),(6,5),(7,6)],fill='#cfb4db')
d.line([(10,8),(12,7),(12,9)],fill='#e4d79b')
d.line((7,11,9,14),fill='#657e45')
image.save(OUT/'swap_seed.png')
