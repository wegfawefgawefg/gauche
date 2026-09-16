"""Old Growth: scarred broad bear, readable rear/paw/rush and collision poses."""
from underworks_palette import *
from PIL import Image

for phase in ('bear','rear','swipe','paw','rush','stagger'):
    im,p=canvas((32,32))
    edge='#302c25'; fur='#65543c'; lit='#91754d'; muzzle='#b09b73'
    low=phase in ('paw','rush','stagger')
    p.ellipse((2,12 if low else 7,25,28),fill=edge)
    p.ellipse((3,13 if low else 8,24,27),fill=fur)
    p.polygon([(8,13),(9,5 if not low else 11),(15,3 if not low else 9),(22,9 if not low else 14),(23,22)],fill=fur)
    hy=3 if phase=='rear' else 10 if not low else 16
    p.ellipse((17,hy,28,hy+12),fill=fur)
    for x in (18,25):
        p.ellipse((x,hy-1,x+4,hy+4),fill=edge)
        p.point((x+2,hy+1),fill=lit)
    p.rectangle((23,hy+7,31,hy+11),fill=muzzle)
    p.rectangle((29,hy+7,31,hy+9),fill=edge)
    p.point((26,hy+5),fill='#e4c888')
    p.line([(19,hy+5),(23,hy+8)],fill='#b6ab8f')
    for x in (6,20):
        foot=x+(3 if phase=='rush' else 0)
        p.rectangle((foot,25,foot+5,30),fill=fur)
        for n in (0,2,4):p.line([(foot+n,29),(foot+n+1,31)],fill='#c8bba0')
    if phase=='rear':
        p.polygon([(12,18),(8,7),(10,3),(14,7),(17,18)],fill=lit)
        for x in (8,10,12):p.line([(x,5),(x-1,1)],fill='#dbcfb0')
    elif phase=='swipe':
        p.polygon([(13,17),(20,16),(31,21),(30,26),(20,22)],fill=lit)
        for y in (21,23,25):p.line([(29,y),(31,y+1)],fill='#dbcfb0')
    else:p.line([(17,18),(19,26)],fill=edge,width=2)
    for x,y in ((4,13),(7,10),(10,8),(5,18)):
        if low:y+=4
        p.rectangle((x,y,x+3,y+1),fill='#67704b')
    p.line([(8,18),(10,23)],fill=lit)
    if phase=='stagger':
        p.line([(25,21),(28,24)],fill=edge);p.line([(28,21),(25,24)],fill=edge)
    save(im,'old_growth_'+phase)

# Snow follows the existing code-drawn crown, not a replacement silhouette.
tree=Image.open(ROOT/'tall_tree.png').convert('RGBA')
cap=Image.new('RGBA',tree.size)
for x in range(tree.width):
    for y in range(min(38,tree.height)):
        if tree.getpixel((x,y))[3]:
            for dy in range(1+(x//3)%3):
                if y+dy<tree.height and tree.getpixel((x,y+dy))[3]:
                    cap.putpixel((x,y+dy),(181,196,186,255) if dy==0 else (124,154,151,255))
            break
save(cap,'tall_tree_snow_cap')
