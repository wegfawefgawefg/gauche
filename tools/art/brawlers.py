"""Small readable combatants: fur hood/stone club and workcoat/iron pipe."""
from underworks_palette import *
for industrial in (False,True):
    for pose in ('','_raise','_swing'):
        im,p=canvas()
        coat='#656e70' if industrial else '#8b8a76'
        trim='#a5ae9e' if industrial else '#d0cbb4'
        skin='#a18766' if industrial else '#799782'
        lean=1 if pose=='_swing' else 0
        p.rectangle((4+lean,7,10+lean,12),fill=INK)
        p.rectangle((5+lean,7,9+lean,11),fill=coat)
        p.line((6+lean,8,6+lean,11),fill=trim)
        p.line((5+lean,12,9+lean,12),fill='#574e3a')
        for x in (4,9):
            p.rectangle((x,13,x+2,14),fill='#4b4334')
            p.line((x-1,15,x+2,15),fill=INK)
        if industrial:
            p.rectangle((5+lean,2,10+lean,6),fill=INK)
            p.rectangle((6+lean,3,10+lean,6),fill=skin)
            p.line((7+lean,6,10+lean,6),fill='#3c3931')
            p.rectangle((5+lean,1,10+lean,2),fill=BRASS)
            p.line((4+lean,3,11+lean,3),fill=GOLD)
        else:
            p.polygon([(5+lean,1),(9+lean,1),(11+lean,4),(10+lean,7),(4+lean,7),(3+lean,4)],fill=trim)
            p.rectangle((5+lean,3,9+lean,6),fill=INK)
            p.rectangle((6+lean,4,10+lean,6),fill=skin)
            p.point((11+lean,5),fill=skin)
        p.point((9+lean,4),fill='#e0dba7')
        p.line((3+lean,8,3+lean,10),fill=coat)
        p.point((3+lean,11),fill=skin)
        if pose=='_raise':
            p.line([(10,8),(12,6),(12,4)],fill=skin,width=2)
            a,b=(12,5),(14,0)
        elif pose=='_swing':
            p.line((10,8,14,9),fill=skin,width=2)
            a,b=(12,9),(15,10)
        else:
            p.line((11,8,12,10),fill=skin)
            a,b=(12,11),(14,6)
        p.line([a,b],fill=INK,width=3)
        p.line([a,b],fill=STEEL if industrial else WOOD,width=2)
        if industrial:
            p.point(b,fill=LIGHT)
        else:
            x,y=b;p.rectangle((max(0,x-1),max(0,y-1),min(15,x+1),min(15,y+1)),fill='#a2aca4')
        save(im,('pipe_guard' if industrial else 'frost_goblin')+pose)
