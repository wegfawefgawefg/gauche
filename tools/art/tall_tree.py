"""Native-pixel oak with one planted foot, cut/char poses and connected timber."""
from underworks_palette import *

for pose in ('tree', 'cut', 'char'):
    im,p=canvas((32,64))
    p.polygon([(13,18),(19,19),(20,54),(24,60),(19,61),(16,58),(10,61),(7,59),(12,54)],fill='#393728')
    p.polygon([(14,25),(17,23),(17,54),(19,59),(15,56),(11,59),(14,51)],fill='#7a6646')
    p.line([(18,30),(19,52),(21,57)],fill='#534c32')
    p.line([(14,41),(15,45),(13,51)],fill='#b19460')
    for a,b,c in (((14,36),(6,29),(3,18)),((18,33),(24,26),(27,15)),((14,22),(9,16),(11,6))):
        p.line([a,b,c],fill='#51472e',width=3)
    for box in ((7,2,22,15),(1,10,15,26),(16,9,29,24),(5,18,25,31)):
        p.ellipse(box,fill='#233a26');x,y,xx,yy=box
        p.arc((x+1,y+1,xx-2,yy-2),185,300,fill='#456039',width=2)
        p.line((x+4,y+3,x+7,y+3),fill='#567243')
    if pose=='cut':
        p.polygon([(12,51),(18,53),(12,55)],fill='#d0b07a')
        p.line((13,52,17,53),fill='#292a21')
    if pose=='char':
        pixels=im.load()
        for y in range(64):
            for x in range(32):
                r,g,b,a=pixels[x,y]
                if a: pixels[x,y]=(r//3+9,g//4+6,b//4+4,a)
        for y in (28,38,47,54):p.line((14,y,16,y+2),fill='#a45b27')
    save(im, 'tall_tree' if pose=='tree' else f'tall_tree_{pose}')
im,p=canvas();p.polygon([(3,12),(5,6),(10,5),(12,10),(15,12),(10,14),(7,11),(2,14)],fill='#4b412e')
p.ellipse((4,4,11,9),fill='#a88b5a');p.ellipse((6,5,9,7),outline='#695235');save(im,'tall_tree_stump')
im,p=canvas();p.rectangle((0,5,15,12),fill='#3d3928');p.rectangle((0,5,15,9),fill='#756044')
p.line([(0,6),(5,6),(7,5),(15,5)],fill='#a38a56');p.line([(0,10),(4,10),(6,9),(15,9)],fill='#51472e')
p.line((0,12,15,12),fill=INK);save(im,'fallen_log')
