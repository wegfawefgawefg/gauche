"""Forest fliers: long legs, a broad owl face, and a red-capped drilling bird."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

for fed in [False, True]:
    im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
    d.line([(3,13),(5,9),(8,8),(10,11),(12,13)], fill='#a5a18b')
    d.line([(2,10),(5,8),(9,8),(12,10)], fill='#a5a18b')
    d.polygon([(6,7),(2,3),(3,1),(7,5),(8,2),(10,2),(9,7)], fill='#b6c8b5')
    d.polygon([(3,9),(4,6),(8,6),(10,8),(7,10)], fill='#854534' if fed else '#5b6145')
    if fed: d.rectangle((4,7,6,9), fill='#b96045')
    d.rectangle((9,6,11,8),fill='#292c26');d.point((11,6),fill='#d0ba6e')
    d.line((12,7,15,5),fill='#bdb18c')
    im.save(OUT / ('mosquito_fed.png' if fed else 'mosquito.png'))

for flying in [False, True]:
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    if flying:
        d.polygon([(7,8),(3,2),(0,1),(1,8),(5,11),(8,10),(12,11),(15,7),(15,1),(12,3),(10,8)], fill='#907b5a')
        d.line([(1,3),(3,6),(5,8)],fill='#c3ac7d');d.line([(14,3),(12,6),(10,8)],fill='#c3ac7d')
    d.polygon([(5,7),(11,7),(12,11),(10,14),(6,14),(4,11)],fill='#705b41')
    d.polygon([(6,8),(10,8),(11,11),(9,13),(6,12)],fill='#ab9266')
    d.line((5,14,7,14),fill='#cbb276');d.line((9,14,11,14),fill='#cbb276')
    d.polygon([(3,2),(6,3),(8,2),(10,3),(13,2),(12,8),(8,10),(4,8)],fill='#b09b76')
    d.polygon([(4,4),(7,4),(8,6),(10,4),(12,4),(11,8),(8,9),(5,8)],fill='#ded0a3')
    d.rectangle((5,5,6,6),fill='#322e25');d.rectangle((10,5,11,6),fill='#322e25')
    d.point((8,7),fill='#b58442')
    im.save(OUT / ('owl_flying.png' if flying else 'owl.png'))

for drilling in [False, True]:
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    d.polygon([(2,13),(5,7),(9,5),(12,7),(11,11),(7,13)], fill='#41483d')
    d.polygon([(6,8),(10,7),(11,9),(9,12),(6,12)],fill='#c2c4a0')
    d.polygon([(3,11),(6,7),(8,7),(7,11)],fill='#64715b')
    d.line((6,13,5,15),fill='#957847');d.line((9,12,10,14),fill='#957847')
    d.rectangle((8,4,12,7),fill='#dedac0');d.rectangle((8,4,12,4),fill='#b74731')
    d.polygon([(8,4),(7,3),(11,2),(12,4)],fill='#c55636')
    d.point((11,5),fill='#252b23')
    d.polygon([(12,5),(15,5 if drilling else 4),(13,7)],fill='#d5b56d')
    if drilling: d.line((1,8,5,6),fill='#b5bca0')
    im.save(OUT / ('woodpecker_drilling.png' if drilling else 'woodpecker.png'))
