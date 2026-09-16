"""Small Forest residents and grown houses; muted native-pixel sprites."""
from underworks_palette import *
for role,name in enumerate(['stick','bow','crossbow','rider']):
    for phase,suffix in enumerate(['','_walk','_tell','_hit']):
        im,p=canvas();dy=-1 if phase==2 else 0
        if role==3:
            im,p=canvas((24,24))
            for x in (5,11,17):
                p.line([(x,16),(x-3,20),(x-4,23)],fill='#ad915a')
                p.line([(x,14),(x-2,11),(x-3,10)],fill='#8e764b')
            p.ellipse((2,10,11,18),fill='#695037');p.ellipse((10,14,16,20),fill='#8a643d')
            p.ellipse((15,15,22,21),fill='#947449');p.point((21,17),fill='#ddd2a5')
            p.line([(20,16),(23,13)],fill='#b8a374');p.line([(21,20),(23,22)],fill='#b8a374');dy=2
        else:
            p.rectangle((5,12,7,14 if phase!=1 else 15),fill=WOOD);p.rectangle((9,12,11,15 if phase!=1 else 14),fill=WOOD)
        p.rectangle((5,9+dy,10,12+dy),fill=['#61765c','#697578','#807451','#876552'][role])
        p.ellipse((5,5+dy,11,10+dy),fill='#c7b28a');p.polygon([(5,9+dy),(11,8+dy),(8,12+dy)],fill='#ddd6b7')
        p.polygon([(3,6+dy),(7,1+dy),(9,0+dy),(10,4+dy),(13,6+dy)],fill=['#936b4a','#697b68','#897355','#ac7e45'][role]);p.line([(4,6+dy),(12,6+dy)],fill='#c09b64')
        p.point((10,7+dy),fill=INK)
        if role in (0,3):p.line([(12,6 if phase==2 else 9),(14 if phase==3 else 12,15)],fill='#bda57a',width=2)
        elif role==1:
            p.line([(13,5),(15,9),(13,14)],fill='#b8975e');p.line([(13,5),(13 if phase!=2 else 10,9),(13,14)],fill='#dad3b5')
        else:
            y=7 if phase==2 else 10
            p.line([(10,y),(15,y)],fill='#ae986f',width=2);p.line([(13,y-3),(13,y+3)],fill='#827f6a')
        save(im,'gnome_'+name+suffix)
colors=[('#895950','#bb8070'),('#527d7b','#83a49a'),('#97834e','#c2ad6b')]
for variant,(dark,bright) in enumerate(colors):
    tail=['','_blue','_ochre'][variant]
    for closed in (False,True):
        im,p=canvas((32,32))
        p.ellipse((7,25,25,31),fill='#42432f');p.polygon([(10,13),(22,13),(25,28),(7,28)],fill='#b1a47f')
        p.line([(10,17),(9,26),(11,28)],fill='#7b7353',width=2)
        p.polygon([(2,15),(4,9),(10,4),(19,2),(27,7),(30,15),(25,18),(7,18)],fill=dark)
        p.line([(4,10),(11,5),(19,3),(25,7)],fill=bright,width=2)
        for x,y in [(8,11),(14,7),(23,11),(19,14)]:p.rectangle((x,y,x+2,y+1),fill='#d4c6a0')
        p.rounded_rectangle((13,20,20,28),radius=3,fill='#393c30' if not closed else '#73583f')
        if closed:p.line([(16,21),(16,28)],fill='#a1875b');p.point((19,25),fill='#dac987')
        p.rectangle((22,19,25,22),fill='#544f38');p.point((23,20),fill='#dbc083')
        save(im,'gnome_house'+tail+('_closed' if closed else ''))
    im,p=canvas((24,40))
    p.polygon([(10,13),(14,13),(15,34),(18,39),(7,39),(9,32)],fill='#b8ad8b')
    p.line([(11,18),(11,36),(9,38)],fill='#e0d4b3')
    p.polygon([(0,13),(2,8),(7,3),(13,0),(20,5),(23,12),(20,16),(4,16)],fill=dark)
    p.line([(2,9),(7,4),(13,1),(19,5)],fill=bright,width=2)
    p.line([(4,16),(11,19),(20,16)],fill='#7c7760')
    for x,y in [(5,9),(10,5),(15,10),(18,7)]:p.rectangle((x,y,x+2,y+1),fill='#d8cba3')
    save(im,'tall_mushroom'+tail)
