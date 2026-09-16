"""Bear silhouettes at native scale: cub, broad adults and a scarred old bear."""
from underworks_palette import *

for name,n,body,edge in (
    ('bear_cub',12,'#98724d','#543e2b'),
    ('bear_mother',20,'#806046','#402f24'),
    ('bear_father',24,'#624b37','#35291f'),
    ('bear_old',28,'#53483c','#2d2923')):
    im,p=canvas((n,n))
    def box(a,b,c,d): return tuple(round(v*n/16) for v in (a,b,c,d))
    p.ellipse(box(1,3,13,13),fill=edge)
    p.ellipse(box(2,3,12,12),fill=body)
    p.ellipse(box(7,2,14,10),fill=body)
    p.ellipse(box(8,1,10,4),fill=edge)
    p.ellipse(box(9,2,10,3),fill='#98724d')
    p.ellipse(box(12,2,14,4),fill=edge)
    p.rectangle(box(11,7,15,9),fill='#b0976b')
    p.rectangle(box(14,7,15,8),fill='#302b22')
    p.rectangle(box(12,5,12.5,5.5),fill='#e1d0a0')
    p.rectangle(box(3,11,5,14),fill=body)
    p.rectangle(box(10,10,12,14),fill=body)
    p.line(box(2,14,5,14),fill=edge,width=max(1,n//16))
    p.line(box(10,14,13,14),fill=edge,width=max(1,n//16))
    p.line(box(4,4,7,3),fill='#a3865b' if name=='bear_cub' else '#8a7252')
    if name in ('bear_father','bear_old'):
        p.polygon([(round(x*n/16),round(y*n/16)) for x,y in ((5,3),(7,1),(10,2),(10,4))],fill=body)
        for x in (10,12): p.point((round(x*n/16),round(14*n/16)),fill='#b9ad8e')
    if name=='bear_old':
        p.line(box(10,4,12,6),fill='#baac92',width=1)
        p.line(box(3,6,4,8),fill='#927f66',width=1)
        p.point((round(14*n/16),round(9*n/16)),fill='#ded2b4')
    save(im,name)
