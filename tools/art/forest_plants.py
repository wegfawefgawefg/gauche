"""Small living roots and bark guardians. Broad shapes, quiet interiors."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

for coiled in [False, True]:
    im = Image.new('RGBA', (16,16)); d = ImageDraw.Draw(im)
    d.line([(1,14),(5,11),(7,12),(11,11),(14,14)], fill='#715739', width=2)
    d.line([(4,15),(8,11),(11,15)], fill='#927149', width=2)
    d.polygon([(5,11),(4,7),(6,4),(10,3),(13,6),(12,10),(9,12)], fill='#3b6339')
    d.polygon([(4,8),(2,5),(5,6),(7,3),(9,2),(10,5),(14,4),(12,8)], fill='#74994d')
    d.polygon([(7,5),(10,5),(12,7),(10,9),(7,9),(6,7)], fill='#2a3b28')
    if coiled:
        d.line([(6,7),(7,4),(11,4),(12,6),(10,8),(8,7)], fill='#d0b871', width=1)
        d.point((9,6), fill='#e3d296')
    else:
        d.rectangle((8,6,9,7), fill='#c3b982')
        d.point((10,7), fill='#89915d')
    im.save(OUT / ('root_turret_coiled.png' if coiled else 'root_turret.png'))

for swing in [False, True]:
    im = Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
    d.line([(5,10),(4,14),(2,15)], fill='#937748', width=2)
    d.line([(9,11),(11,14),(13,15)], fill='#765a3d', width=2)
    d.polygon([(4,5),(6,3),(10,3),(12,6),(11,11),(8,13),(4,10)], fill='#765a3d')
    d.polygon([(5,5),(8,4),(10,5),(9,10),(6,11),(4,9)], fill='#ae9058')
    d.line((7,7,7,10), fill='#5b4934')
    d.rectangle((5,5,6,5),fill='#e1cb87'); d.rectangle((9,5,10,5),fill='#e1cb87')
    d.polygon([(3,4),(1,1),(5,2),(6,0),(8,2),(11,0),(12,3),(14,2),(12,5)],fill='#4f733d')
    d.line([(3,7),(1,10),(3,11)], fill='#9a7b49', width=2)
    d.line([(11,8),(14,3 if swing else 9),(15,1 if swing else 7)],fill='#b09660',width=2)
    d.point((12,2 if swing else 10),fill='#d0bf82')
    im.save(OUT / ('bramble_guard_swing.png' if swing else 'bramble_guard.png'))
