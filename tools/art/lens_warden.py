"""A robed observatory operator and a fixed caged light source."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['lens_warden','warden_turn','warden_charge','warden_recover']:
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    p.polygon([(5,6),(10,6),(12,14),(3,14)],fill='#485462')
    p.polygon([(6,7),(9,7),(10,13),(5,13)],fill='#7a7d86')
    p.line([(7,7),(7,13)],fill='#ae9b70')
    p.rectangle((4,14,6,15),fill='#343d41'); p.rectangle((9,14,11,15),fill='#343d41')
    y=1 if name=='warden_recover' else 0
    p.polygon([(4,3+y),(6,1+y),(9,1+y),(11,4+y),(10,6+y),(4,6+y)],fill='#596c77')
    p.line([(6,2+y),(9,2+y)],fill='#b3b9ad')
    p.rectangle((6,4+y,11,5+y),fill='#bda878')
    p.rectangle((8,4+y,9,5+y),fill='#9dc9cc')
    if name=='warden_charge':
        p.line([(9,9),(12,5)],fill='#a6b5b5',width=2)
        p.line([(12,5),(13,2)],fill='#c4d0ca')
        p.point((13,1),fill='#badbe1')
    elif name=='warden_turn':
        p.line([(9,8),(13,8)],fill='#9caaaa',width=2)
        p.line([(12,6),(12,10)],fill='#c2ae79')
    else:
        p.line([(10,9),(12,12)],fill='#8b989c',width=2)
    im.save(root/(name+'.png'))
for name in ['beam_lamp','beam_lamp_charged']:
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    p.rectangle((4,12,11,14),fill='#656c70')
    p.line([(4,12),(11,12)],fill='#ac9a6b')
    p.polygon([(5,3),(10,3),(12,6),(11,11),(4,11),(3,6)],fill='#6f8492')
    p.rectangle((5,4,10,10),fill='#d6e3d5' if name.endswith('charged') else '#9dbcc6')
    p.line([(7,3),(7,11)],fill='#9d895c')
    p.line([(4,7),(11,7)],fill='#9d895c')
    p.rectangle((5,1,10,2),fill='#a18b5d')
    im.save(root/(name+'.png'))
