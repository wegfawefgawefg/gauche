"""A small brass-caged prism, with increasingly bright charge seams."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name in ['prism_bomb','prism_bomb_lit','prism_bomb_ready']:
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    p.polygon([(6,4),(10,4),(13,7),(13,11),(10,14),(5,14),(2,11),(2,7)],fill='#627280')
    p.polygon([(6,5),(10,5),(11,8),(8,12),(4,10),(4,7)],fill='#95a8b7')
    p.line([(6,5),(6,10),(8,12)],fill='#bdcbd0')
    p.line([(2,8),(12,8)],fill='#8b784f',width=2)
    p.line([(8,4),(8,13)],fill='#8b784f',width=2)
    p.rectangle((6,2,9,3),fill='#b39b69')
    if name!='prism_bomb':
        glow='#e0e5cf' if name=='prism_bomb_ready' else '#b9cfe3'
        p.line([(8,5),(8,12)],fill=glow)
        p.line([(4,8),(11,8)],fill=glow)
        p.point((8,1),fill=glow)
    im.save(root/(name+'.png'))
