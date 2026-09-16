"""Native log cuts, splintered remains, and vertical-grain wall scars."""
from underworks_palette import *
base=Image.open(ROOT/'fallen_log.png').convert('RGBA')
for name,deep in [('fallen_log_bruised',False),('fallen_log_split',True)]:
    im=base.copy();p=ImageDraw.Draw(im)
    p.polygon([(5,5),(10,6),(8,8),(11,9),(6,10),(7,8)],fill='#b79b68')
    p.line([(6,5),(8,7),(7,9),(9,10)],fill='#2c2b22')
    if deep:
        p.polygon([(9,6),(15,5),(15,7),(11,8),(15,9),(15,11),(9,10)],fill='#bf9e65')
        p.line([(1,8),(5,7),(9,9),(14,8)],fill='#28281f')
        p.line([(8,10),(11,12),(13,11)],fill='#211f19')
    save(im,name)
im,p=canvas()
for points in [[(0,8),(6,7),(5,9),(9,10),(0,12)],[(10,6),(15,5),(15,10),(8,12),(11,9)],[(4,12),(10,13),(8,14),(2,14)]]:
    p.polygon(points,fill='#51452e')
    p.line(points[:2],fill='#a48754')
p.line([(1,9),(4,8),(3,10)],fill='#d0af77')
p.line([(11,7),(14,6),(13,8)],fill='#b79860')
save(im,'fallen_log_broken')
for name,deep in [('wood_bruised',False),('wood_split',True)]:
    im,p=canvas()
    p.polygon([(6,4),(9,6),(8,8),(10,10),(7,12),(7,8),(5,7)],fill='#a68a57')
    p.line([(6,4),(7,7),(6,9),(7,12)],fill='#24271f')
    p.line([(8,6),(8,8),(9,10)],fill='#d0b07a')
    if deep:
        p.polygon([(10,1),(12,5),(10,8),(12,12),(10,14),(9,10),(10,6)],fill='#9b7947')
        p.line([(11,2),(11,5),(9,8),(11,12),(10,14)],fill='#29271d')
        p.line([(4,8),(3,11),(5,14)],fill='#b59a69')
    save(im,name)
