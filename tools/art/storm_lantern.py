"""Shielded tin lantern with wide and focused windows."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name,mode in [('storm_lantern',0),('lantern_open',1),('lantern_focused',2)]:
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    d.arc((4,1,10,7),180,360,fill='#a6a497',width=1)
    d.polygon([(4,5),(10,5),(12,7),(12,11),(10,13),(4,13)],fill='#4c5b59')
    d.rectangle((5,6,9,11),fill='#8b978a')
    d.line([(4,5),(9,5),(11,7)],fill='#c0bca0')
    d.rectangle((10,7,13,11),fill='#333f3c')
    if mode:
        d.rectangle((10,8,12,10),fill='#d5a960')
        d.rectangle((11,9,13,9) if mode==2 else (11,8,13,10),fill='#f2d999')
    else:
        d.line([(10,7),(12,8),(12,11)],fill='#788477')
    d.line([(4,13),(10,13)],fill='#9a8b68')
    d.point((6,8),fill='#414d48')
    im.save(root/(name+'.png'))
