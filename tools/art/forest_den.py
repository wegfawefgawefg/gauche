"""Native 16px leaf beds and gnawed bones for ordinary bear hollows."""
from underworks_palette import *

im, p = canvas()
p.ellipse((0, 4, 15, 14), fill='#343526')
p.ellipse((1, 4, 14, 13), fill='#66583b')
p.ellipse((3, 5, 12, 11), fill='#49432d')
for x,y in ((1,8),(3,5),(6,4),(10,4),(13,7),(12,11),(8,13),(4,12),(2,10)):
    p.line((x,y,min(15,x+2),y-1), fill='#8a764b')
p.line((5,8,10,9), fill='#363a2b')
save(im, 'bear_bed')

im, p = canvas()
p.ellipse((2, 9, 14, 14), fill='#30332b')
for a,b in (((3,11),(10,6)),((6,9),(12,12)),((3,13),(9,12))):
    p.line((*a,*b), fill='#aba286', width=2)
    for x,y in (a,b):
        p.point((x-1,y),fill='#d0c3a0')
        p.point((x,y+1),fill='#d0c3a0')
p.rectangle((2,5,6,8),fill='#b5aa8a')
p.rectangle((3,4,5,4),fill='#c8bd9d')
p.point((3,6),fill='#363a2b')
p.point((5,6),fill='#363a2b')
p.point((4,8),fill='#363a2b')
save(im, 'bone_pile')
