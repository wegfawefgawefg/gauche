"""Native 16px cold-weather weapons and short strokes. Neutral points right."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name in ('ice_axe','tusk_pike','ice_axe_arc','tusk_pike_thrust'):
    image=Image.new('RGBA',(16,16));p=ImageDraw.Draw(image)
    if name=='ice_axe':
        p.line([(2,11),(10,7)],fill='#514d42',width=3)
        p.line([(2,10),(10,6)],fill='#8f7861',width=2)
        p.line([(3,10),(5,9)],fill='#c3ad83')
        p.polygon([(9,4),(12,2),(14,3),(14,8),(12,11),(9,10)],fill='#708e99')
        p.line([(12,2),(14,3),(14,8),(12,11)],fill='#c7d9d6')
        p.line([(9,5),(11,4),(12,4)],fill='#99b7bc')
        p.line([(9,6),(7,4)],fill='#8aa6a9',width=2)
        p.point((7,3),fill='#c7d9d6')
        p.line((5,9,6,11),fill='#4b5f65')
    elif name=='tusk_pike':
        p.line([(1,10),(10,7)],fill='#5d5343',width=2)
        p.line([(1,9),(10,6)],fill='#9b8057')
        p.polygon([(8,5),(11,4),(15,6),(13,8),(10,9),(9,8),(12,7)],fill='#b8b897')
        p.line([(9,5),(11,4),(15,6)],fill='#e0dec0')
        p.line((8,6,9,8),fill='#65523e')
        p.line((6,7,7,9),fill='#c6a66b')
    elif name=='ice_axe_arc':
        p.line([(5,1),(9,3),(12,6),(12,9),(9,12),(5,14)],fill='#acc3bf')
        p.line([(8,3),(10,6),(10,10),(7,12)],fill='#607f87')
        p.line((13,5,14,7),fill='#d0d9ce')
    else:
        p.line((2,8,13,8),fill='#b8b89c')
        p.line([(10,5),(15,8),(10,10)],fill='#d7d3b4')
        p.line((4,6,8,6),fill='#6c8b90')
        p.line((1,10,6,10),fill='#6c8b90')
    image.save(root/(name+'.png'))
