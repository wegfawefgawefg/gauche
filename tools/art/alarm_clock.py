"""Original quiet brass alarm clock poses and a loose escapement gear."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
for name, ringing, wound in [('alarm_clock', False, False),
                              ('alarm_clock_wound', False, True),
                              ('alarm_clock_ringing', True, True)]:
    im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
    p.line([(4,12),(3,14)], fill='#8c7546', width=2)
    p.line([(11,12),(12,14)], fill='#8c7546', width=2)
    p.ellipse((3,4,12,13), fill='#8c7546')
    p.ellipse((4,5,11,12), fill='#cec5a2')
    p.line([(8,6),(8,9),(10,9)], fill='#3c4238')
    p.rectangle((4,2,6,3), fill='#bca06a')
    p.rectangle((10,2,12,3), fill='#bca06a')
    p.point((8,3), fill='#dfc98b' if wound else '#786744')
    if ringing:
        p.line([(1,3),(0,5)], fill='#dfc98b')
        p.line([(14,3),(15,5)], fill='#dfc98b')
        p.point((8,1), fill='#dfc98b')
    im.save(root / (name + '.png'))
im = Image.new('RGBA', (16,16)); p = ImageDraw.Draw(im)
p.rectangle((6,5,8,9),fill='#8c7546'); p.rectangle((5,6,9,8),fill='#8c7546')
p.point((7,7),fill='#3c4238'); p.point((6,6),fill='#bca06a')
im.save(root / 'debris_clock_gear.png')
