"""Small woodland tools and planted props, with quiet silhouettes and few colors."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def canvas():
    image = Image.new('RGBA', (16, 16))
    return image, ImageDraw.Draw(image)

image, d = canvas()
d.polygon([(2,8),(6,5),(12,8),(11,12),(7,14),(3,12)], fill='#735638')
d.line([(3,10),(7,12),(11,10)], fill='#b19163')
for x,y in [(5,7),(8,6),(11,7)]:
    d.polygon([(x-1,y+1),(x,y-4),(x+2,y-5),(x+1,y),(x+1,y+2)], fill='#d1c39c')
    d.point((x+1,y-3), fill='#f1e9c9')
image.save(OUT/'digging_claws.png')

image,d = canvas()
d.rectangle((5,2,10,4), fill='#847451')
d.polygon([(5,5),(10,5),(12,8),(12,13),(3,13),(3,8)], fill='#a37636')
d.rectangle((4,8,11,12), fill='#c49444')
d.line((5,7,5,11), fill='#f1cf72')
d.line((7,10,9,10), fill='#594833')
image.save(OUT/'resin_glue.png')

image,d = canvas()
d.polygon([(5,3),(10,3),(9,6),(12,9),(11,14),(4,14),(3,9),(6,6)], fill='#a68c60')
d.line((5,6,10,6), fill='#584b35')
d.polygon([(6,9),(9,8),(10,10),(8,12),(6,11)], fill='#6c793d')
d.point((7,9), fill='#c6cc81')
image.save(OUT/'seed_bag.png')

image,d = canvas()
d.line((8,9,8,14), fill='#87924b')
d.polygon([(7,11),(3,8),(3,6),(6,7),(8,10),(12,6),(14,6),(12,10),(9,12)], fill='#638046')
d.polygon([(5,7),(5,4),(7,2),(10,3),(12,5),(10,8),(7,9)], fill='#a5b65c')
d.polygon([(7,4),(9,4),(10,6),(8,7),(6,6)], fill='#efdb8b')
image.save(OUT/'lantern_seed.png')

for name, tall in [('shoot',False),('shoot_tall',True)]:
    image,d = canvas()
    d.line((8,8 if tall else 11,8,14), fill='#78934f')
    d.polygon([(8,12),(3,10),(2,7 if tall else 9),(6,9),(8,11)], fill='#88a45a')
    d.polygon([(8,11),(10,7 if tall else 9),(14,6 if tall else 9),(12,10),(9,12)], fill='#a6b568')
    d.line((5,14,11,14), fill='#69563a')
    image.save(OUT/f'{name}.png')

image,d = canvas()
d.polygon([(1,11),(2,6),(5,5),(6,1),(10,2),(10,5),(14,6),(15,11),(12,14),(3,14)], fill='#465d34')
d.polygon([(3,6),(6,6),(7,3),(10,6),(13,7),(12,11),(8,10),(4,12),(2,10)], fill='#6a8347')
d.line([(7,13),(8,9),(5,6)], fill='#967f50')
d.line([(8,9),(11,7)], fill='#967f50')
d.line([(7,13),(11,14)], fill='#584a33')
image.save(OUT/'root_cover.png')

image,d = canvas()
d.line([(7,14),(8,10),(8,5)], fill='#81984e')
d.polygon([(8,11),(4,8),(2,8),(4,11),(7,12)], fill='#586f3c')
d.polygon([(8,12),(11,9),(14,8),(13,11),(9,13)], fill='#879b50')
d.polygon([(4,5),(6,2),(10,2),(12,5),(10,8),(6,8)], fill='#a7b75d')
d.polygon([(6,4),(10,4),(10,6),(8,7),(6,6)], fill='#f4e8a6')
d.line((5,14,11,14), fill='#65543b')
image.save(OUT/'lantern_plant.png')
