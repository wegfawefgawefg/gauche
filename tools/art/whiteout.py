"""Hooded weather drummer, glass snow globe and quiet weather-station metalwork."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['whiteout_drummer','drummer_left','drummer_right','drummer_rest']:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    # SILHOUETTE: Closed hood above a broad drum; sticks lift on alternating beats.
    p.polygon([(5,2),(9,1),(12,4),(11,7),(4,7),(3,4)],fill='#638284')
    p.polygon([(6,3),(9,3),(10,5),(8,6),(5,5)],fill='#273d48')
    p.line([(6,4),(8,4)],fill='#bdc4b1')
    p.polygon([(4,6),(11,6),(13,12),(10,13),(4,13),(2,11)],fill='#3f5966')
    p.line([(4,13),(3,15)],fill='#8d9b96');p.line([(10,13),(12,15)],fill='#8d9b96')
    p.rectangle((5,9,12,12),fill='#9b7657');p.line([(5,12),(12,12)],fill='#584d45')
    p.ellipse((4,7,13,10),fill='#b5bba7');p.line([(6,8),(11,8)],fill='#d0cbb5')
    if name=='drummer_left':
        p.line([(4,8),(1,6),(2,2)],fill='#bbc6ba');p.point((2,1),fill='#dfdac1')
        p.line([(11,6),(12,8)],fill='#94a997')
    elif name=='drummer_right':
        p.line([(11,6),(14,4),(14,1)],fill='#bbc6ba');p.point((14,0),fill='#dfdac1')
        p.line([(3,7),(6,8)],fill='#94a997')
    else:
        p.line([(3,7),(4,10)],fill='#94a997');p.line([(12,6),(14,10)],fill='#94a997')
        if name=='drummer_rest': p.line([(6,5),(8,5)],fill='#617371')
    im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.ellipse((3,1,13,12),fill='#657f8b')
p.ellipse((4,2,12,11),fill='#374e60')
p.line([(5,4),(5,3),(7,2)],fill='#b9cdcb')
p.polygon([(6,10),(8,5),(10,10)],fill='#aebfb6');p.point((10,4),fill='#c6cdbb')
p.rectangle((3,12,13,13),fill='#957c53');p.rectangle((2,14,14,14),fill='#5e5745')
im.save(root/'snow_globe.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(5,5),(10,7),(6,10)],fill='#b1c7c5');p.line([(6,6),(9,7)],fill='#dae0c7')
im.save(root/'debris_globe_glass.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(8,4),(8,14)],fill='#84958d');p.line([(5,14),(11,14)],fill='#536a70')
p.line([(2,5),(13,5)],fill='#b5ab84')
p.polygon([(1,5),(4,3),(4,7)],fill='#b5ab84')
p.polygon([(11,3),(14,3),(14,7),(11,7)],fill='#71857c')
p.line([(8,4),(8,1)],fill='#b5ab84');p.point((8,0),fill='#c6c7a6')
im.save(root/'weather_vane.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(5,6),(8,5),(10,7),(9,9),(7,9),(6,8)],fill='#b18b65')
p.point((5,7),fill='#d1ad80')
im.save(root/'debris_copper_curl.png')
