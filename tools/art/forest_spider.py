"""Forest cave spiders: splayed legs, exposed fangs, native-size brood roles."""
from underworks_palette import *
for role,size in [('forest_spider',16),('forest_spider_young',10),('forest_spider_mother',28)]:
    for phase,suffix in enumerate(['','_tell','_bite']):
        im,p=canvas((size,size));c=size//2
        for side in [-1,1]:
            for leg in range(4):
                y=int(size*(.28+leg*.14));outer=c+side*(c-1)
                bend=y+(-3 if leg<2 else 2)
                if phase==1:bend-=2
                if phase==2:bend+=2
                p.line([(c+side*2,y),(outer-side*2,bend),(outer,min(size-1,bend+3))],fill='#877959',width=max(1,size//14))
                p.point((outer-side*2,bend),fill='#b09b74')
        p.ellipse((int(size*.28),int(size*.16),int(size*.75),int(size*.64)),fill='#554730')
        p.ellipse((int(size*.37),int(size*.22),int(size*.64),int(size*.49)),fill='#8a7045')
        p.ellipse((c-size//5,size//2,c+size//5,int(size*.8)),fill='#373a2b')
        eye='#e7bf6a' if phase==1 else '#c4aa76'
        for x in [c-2,c+1]:p.point((x,int(size*.62)),fill=eye)
        fang=int(size*.75);end=min(size-1,fang+(size//5 if phase==2 else 2))
        for side in [-1,1]:p.line((c+side*2,fang,c+side,end),fill='#d4ceb0')
        if role.endswith('mother'):
            p.line([(c-3,8),(c,5),(c+3,8),(c,11),(c-3,8)],fill='#b89159')
        save(im,role+suffix)
for v in range(3):
    im,p=canvas();center=(6+v,8-v)
    ends=[(1,1),(7,0),(14,2),(15,8),(13,14),(7,15),(1,13),(0,7)]
    for i,(x,y) in enumerate(ends):
        if (i+v)%5!=0:p.line((center[0],center[1],x,y),fill='#667060')
    for layer,scale in enumerate([.38,.7]):
        ring=[(int(center[0]+(x-center[0])*scale),int(center[1]+(y-center[1])*scale)) for x,y in ends]
        for i in range(8):
            if (i+layer+v)%4!=0:p.line([ring[i],ring[(i+1)%8]],fill='#9a9e87' if layer==0 else '#7c8673')
    save(im,'forest_web_'+str(v))
# Two hooked fangs close on the committed target, independently of the body pose.
for closed in (False,True):
    im,p=canvas()
    for side in (-1,1):
        x=7+side*(2 if closed else 5)
        p.line([(x-side*2,2),(x,4),(x,9),(7+side,12 if closed else 10)],fill='#e0d9b8',width=2)
        p.point((x,5),fill='#f0ead5')
    save(im,'forest_spider_fangs_closed' if closed else 'forest_spider_fangs')
