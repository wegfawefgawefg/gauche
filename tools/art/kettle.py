"""Hand-sized iron kettle; blue water mark and warm lid distinguish its contents."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name, mark in [('steam_kettle', '#4b5756'), ('kettle_full', '#89bbbe'), ('kettle_hot', '#e4b565')]:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    p.line([(3,8),(2,5),(3,2),(8,1),(10,3),(10,5)],fill='#73807b',width=2)
    p.polygon([(3,6),(10,6),(12,8),(12,12),(10,14),(4,14),(2,12),(2,9)],fill='#75857d')
    p.polygon([(10,8),(12,7),(13,4),(15,4),(14,9),(11,11)],fill='#a1aaa0')
    p.rectangle((4,5,10,6),fill='#b3b7a1');p.point((7,4),fill=mark)
    p.line([(3,10),(4,12),(10,12)],fill='#515f59')
    p.rectangle((5,8,8,10),fill=mark)
    p.point((4,7),fill='#bec1a7')
    if name=='kettle_hot':
        p.line([(11,3),(10,2),(11,0)],fill='#c5c5a7')
    im.save(root/(name+'.png'))
