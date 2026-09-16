"""Open ice ribs: tapering feet, angular shoulders, a thin crown and icicles."""
from underworks_palette import *

im,p=canvas((16,64))
p.polygon([(1,61),(3,39),(6,16),(8,4),(13,2),(12,23),(14,44),(15,61),(9,63)],fill='#324d59')
p.polygon([(3,59),(5,37),(8,14),(10,5),(12,4),(10,30),(11,59)],fill='#719caa')
p.polygon([(4,56),(7,37),(9,12),(10,6),(9,37),(6,59)],fill='#c1d7d0')
p.polygon([(10,58),(11,32),(13,45),(13,60)],fill='#4f7487')
p.line([(4,61),(9,62),(13,60)],fill='#91b8ba')
p.line([(6,46),(10,43),(9,38)],fill='#426576')
save(im,'ice_arch_foot')
for name in ('crown','shoulder','crown_v'):
    im,p=canvas((16,24))
    if name=='shoulder':
        p.polygon([(0,9),(15,1),(15,10),(8,13),(1,19)],fill='#416675')
        p.polygon([(0,9),(15,1),(15,4),(2,13)],fill='#aecfc8')
        p.polygon([(4,13),(8,10),(7,22)],fill='#7ca9b2')
        p.line((7,13,7,19),fill='#bbd9d1')
    elif name=='crown':
        p.polygon([(0,1),(8,0),(15,1),(15,10),(10,8),(7,11),(0,10)],fill='#527e8e')
        p.line([(0,2),(8,1),(15,2)],fill='#c1dad2',width=2)
        p.polygon([(2,9),(5,9),(4,20)],fill='#9ec3c4')
        p.polygon([(10,9),(14,8),(12,15)],fill='#749fac')
        p.point((4,17),fill='#d0e0d6')
    else:
        p.polygon([(5,0),(11,0),(13,15),(10,17),(5,15),(3,8)],fill='#547f8d')
        p.line([(6,0),(5,8),(7,15)],fill='#bbd7cf',width=2)
        p.polygon([(10,12),(13,12),(12,22)],fill='#93bbbd')
    save(im,'ice_arch_'+name)
