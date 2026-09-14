"""Eight quiet forest utility icons, with distinct silhouettes and limited palettes."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def bottle(name, color, wide=False):
    im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
    d.rectangle((6,1,9,3),fill='#8e6c45')
    d.polygon([(6,4),(9,4),(10,7),(13 if wide else 11,9),(12,14),(3,14),(2 if wide else 4,9),(5,7)],fill='#657d78')
    d.rectangle((5,9,10,13),fill=color)
    d.line((5,7,4,10),fill='#a9b8aa')
    im.save(ROOT/f'{name}.png')
for name,color,wide in [('oil_flask','#242335',False),('sap_jar','#b07737',True),('water_flask','#518899',False),('honey_pot','#dba546',True)]: bottle(name,color,wide)
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.line((5,14,9,6),fill='#8c6940',width=3)
d.polygon([(6,7),(5,4),(8,5),(8,0),(12,4),(12,7),(9,9)],fill='#cf742b')
d.polygon([(8,7),(8,4),(10,6),(10,8)],fill='#f5cc72');im.save(ROOT/'torch.png')
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.rectangle((4,6,10,14),fill='#87918a');d.rectangle((4,4,9,6),fill='#3e4643')
d.line((3,5,1,2,6,1),fill='#9da193',width=2);d.point((10,3),fill='#e9bf70');im.save(ROOT/'lighter.png')
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.polygon([(3,7),(6,5),(10,5),(13,9),(12,14),(4,14)],fill='#9b9471')
d.line((6,4,10,4),fill='#746048',width=2)
d.ellipse((6,8,10,11),fill='#b7aa93');d.rectangle((8,11,8,13),fill='#d1c6a4')
d.point((4,2),fill='#b9a2c1');d.point((11,1),fill='#a2b095');im.save(ROOT/'mushroom_spores.png')
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.ellipse((3,6,13,15),fill='#6c6c64');d.rectangle((6,4,10,7),fill='#8c8b7d')
d.line((8,4,6,2,8,0),fill='#a7a797');d.line((4,9,3,11),fill='#a3a69a');im.save(ROOT/'smoke_pot.png')
