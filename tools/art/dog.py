"""Scruffy brown dog: full body, braced warning and extended snapping muzzle.

Keep the old port dog.png intact; gameplay uses these native 16px poses.
"""
from underworks_palette import *
FUR='#a7834e'; SHADE='#695439'; PALE='#c4ad78'; NOSE='#292f29'
for pose in ('body','crouch','bite'):
    im,p=canvas();low=pose=='crouch';snap=pose=='bite'
    back=7 if low else 6
    p.line([(0,5),(1,8),(4,9)],fill=SHADE,width=2)
    p.rectangle((3,back,11,11),fill=FUR)
    p.rectangle((3,back,6,back+2),fill=SHADE)
    p.line([(4,10),(3 if low else 4,14),(5,14)],fill=SHADE,width=2)
    p.line([(9,10),(12 if snap else 10,14),(14 if snap else 11,14)],fill=FUR,width=2)
    p.line((6,11,9,11),fill=PALE)
    hx=10 if snap else 9;hy=5 if low else 3
    p.rectangle((hx,hy,hx+3,hy+5),fill=FUR)
    p.line([(hx,hy),(hx-1,hy+2),(hx,hy+5)],fill=SHADE,width=2)
    p.rectangle((hx+2,hy+3,15,hy+5),fill=PALE)
    p.point((hx+2,hy+1),fill=NOSE);p.point((15,hy+3),fill=NOSE)
    if low or snap:
        p.rectangle((hx+2,hy+4,15,hy+5),fill=NOSE)
        p.point((hx+2,hy+4),fill=LIGHT);p.point((14,hy+5),fill=LIGHT)
        p.line((hx+2,hy+6,14,hy+6),fill=FUR)
    if low:
        p.line([(4,6),(6,5),(8,6)],fill=SHADE)
    save(im,'dog_'+pose)
