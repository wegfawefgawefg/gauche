"""A split acorn with a cold lightning core. Restrained 16px silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw
out = Path(__file__).resolve().parents[2] / 'assets/graphics'
for lit in (False, True):
    im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
    d.polygon([(4,7),(11,7),(11,11),(8,14),(6,13),(4,10)],fill='#8b9dad')
    d.line([(5,8),(5,10),(7,12)],fill='#beced1')
    d.polygon([(3,5),(5,3),(10,3),(12,5),(12,7),(3,7)],fill='#917c50')
    d.line((5,3,10,3),fill='#c2aa70')
    d.line((7,1,8,3),fill='#817450')
    d.line([(8,8),(7,10),(9,10),(8,12)],fill='#e4f0df')
    if lit:
        d.line([(13,4),(15,3),(14,6)],fill='#a5d7eb')
        d.line([(2,8),(0,9),(2,11)],fill='#d2e6dd')
        d.point((11,13),fill='#accbe0')
    im.save(out / ('thunder_acorn_lit.png' if lit else 'thunder_acorn.png'))
