"""Forest mixtures: a stoppered scent jar, bruised fruit and a tarred fuse pot."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT = Path(__file__).resolve().parents[2] / 'assets/graphics'

def canvas():
    im = Image.new('RGBA', (16, 16)); return im, ImageDraw.Draw(im)

im, d = canvas()
d.rectangle((6, 2, 9, 4), fill='#9a8055')
d.polygon([(5, 5), (10, 5), (12, 8), (11, 13), (4, 13), (3, 8)], fill='#697745')
d.line([(4, 8), (5, 6), (6, 6)], fill='#b1b778')
d.rectangle((5, 9, 10, 11), fill='#b3aa74')
d.line([(6, 10), (7, 9), (8, 10), (9, 9)], fill='#526035')
im.save(OUT / 'stink_bomb.png')
im, d = canvas()
d.line((8, 2, 7, 5), fill='#826243')
d.polygon([(8, 3), (12, 2), (10, 5)], fill='#87924c')
d.polygon([(4, 5), (7, 4), (9, 5), (12, 5), (14, 8), (12, 12), (9, 14), (4, 12), (2, 8)], fill='#887653')
d.polygon([(3, 7), (5, 5), (7, 5), (6, 9), (3, 9)], fill='#bd8660')
d.polygon([(9, 9), (13, 8), (12, 12), (9, 13), (7, 11)], fill='#4f5841')
d.point((11, 10), fill='#c4ba81'); d.point((9, 11), fill='#a8ad78')
im.save(OUT / 'rotten_fruit.png')
for lit in [False, True]:
    im, d = canvas()
    d.polygon([(5, 6), (10, 6), (13, 9), (12, 13), (10, 14), (4, 13), (2, 10)], fill='#554b41')
    d.polygon([(5, 7), (10, 7), (12, 10), (10, 13), (4, 12), (3, 10)], fill='#2d3330')
    d.line([(4, 9), (5, 8), (7, 8)], fill='#a59372')
    d.rectangle((5, 5, 10, 6), fill='#945340')
    d.line([(8, 4), (8, 2), (11, 2), (12, 1)], fill='#c3ab77')
    if lit:
        d.point((12, 1), fill='#ffdc7b');d.point((14, 2), fill='#e98035')
        d.point((11, 0), fill='#d59c48')
    im.save(OUT / ('pitch_bomb_lit.png' if lit else 'pitch_bomb.png'))
im, d = canvas()
d.polygon([(4, 2), (11, 2), (14, 5), (13, 12), (10, 14), (4, 13), (2, 10), (2, 5)], fill='#a0ae65')
d.line((4, 6, 6, 7), fill='#46513a');d.line((9, 7, 11, 6), fill='#46513a')
d.line([(5, 10), (7, 9), (9, 11), (11, 10)], fill='#536242')
im.save(OUT / 'status_nausea.png')
