"""Five clear silhouettes for forest medicines and questionable meals."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def canvas():
    image = Image.new('RGBA', (16, 16))
    return image, ImageDraw.Draw(image)

image,d = canvas()
d.polygon([(4,7),(11,7),(13,12),(10,14),(4,13),(2,10)], fill='#8b805c')
d.line((4,10,11,10), fill='#c8b988')
d.line((6,8,5,3), fill='#779750');d.line((9,8,11,2), fill='#779750')
d.polygon([(5,6),(2,3),(4,2),(6,4)], fill='#a0b775')
d.polygon([(8,6),(8,3),(11,2),(10,5)], fill='#92aa5d')
d.polygon([(10,7),(11,4),(14,4),(13,6)], fill='#526b3b')
image.save(OUT/'herb_bag.png')

image,d = canvas()
d.polygon([(4,1),(7,1),(9,14),(6,15)], fill='#a78b56')
d.polygon([(8,1),(10,1),(13,14),(10,15)], fill='#776141')
d.polygon([(3,4),(11,3),(12,6),(4,7)], fill='#d4cba6')
d.polygon([(5,10),(12,9),(13,12),(6,13)], fill='#c4bea0')
d.line((4,6,11,5), fill='#969079');d.line((6,12,12,11), fill='#969079')
image.save(OUT/'splint.png')

image,d = canvas()
d.line([(7,6),(5,3),(6,1)], fill='#647c43')
d.line([(8,5),(10,3),(13,3)], fill='#8e9f58')
d.polygon([(6,4),(10,5),(11,8),(9,10),(11,13),(8,12),(6,15),(6,10),(3,11),(5,8)], fill='#af9160')
d.line([(7,6),(8,8),(7,10)], fill='#d8bd80')
d.line([(10,8),(8,9)], fill='#6a5136')
image.save(OUT/'bitter_root.png')

image,d = canvas()
d.line([(6,4),(8,2),(11,3)], fill='#8f9f4d', width=2)
d.polygon([(5,4),(9,4),(10,7),(9,10),(6,13),(2,14),(5,10),(5,8)], fill='#a73530')
d.polygon([(6,5),(8,5),(9,7),(7,10),(5,12),(6,8)], fill='#de6340')
d.line((6,6,6,8), fill='#f6a15f')
image.save(OUT/'chili.png')

image,d = canvas()
d.polygon([(3,4),(6,2),(11,3),(14,6),(13,13),(4,14),(1,10),(1,6)], fill='#85664b')
d.polygon([(3,5),(6,4),(11,4),(12,7),(11,12),(4,12),(3,9)], fill='#c4a679')
d.polygon([(4,5),(6,5),(7,7),(5,8),(3,7)], fill='#777b63')
d.rectangle((9,9,11,11), fill='#797765')
d.point((6,10), fill='#746545');d.point((9,5), fill='#927957')
d.point((5,6), fill='#b2ae83')
image.save(OUT/'fungal_bread.png')
