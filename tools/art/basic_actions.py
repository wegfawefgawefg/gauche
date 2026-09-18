"""Permanent actions and augments: 16px silhouettes, shared warm bone/iron palette."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFilter
OUT = Path(__file__).resolve().parents[2] / 'assets/graphics'
INK='#292a27'; BONE='#e4d2a1'; SHADE='#ac8155'; IRON='#667a80'; LIGHT='#b9cbd0'; GOLD='#d4a747'; RED='#b85848'; GREEN='#89a768'
made=[]
def new():
    im=Image.new('RGBA',(16,16));return im,ImageDraw.Draw(im)
def save(name,im):
    # One-pixel dark silhouette, with nearest-neighbor edges like the existing art.
    edge=im.getchannel('A').filter(ImageFilter.MaxFilter(3))
    base=Image.new('RGBA',im.size,INK);base.putalpha(edge);base.alpha_composite(im)
    base.save(OUT/(name+'.png'));made.append((name,base))
def arrow(d,a,b,color=LIGHT):
    d.line((a,b),fill=color,width=2)
    dx=1 if b[0]>a[0] else -1 if b[0]<a[0] else 0
    dy=1 if b[1]>a[1] else -1 if b[1]<a[1] else 0
    d.line([(b[0]-dx*3-dy*2,b[1]-dy*3+dx*2),b,(b[0]-dx*3+dy*2,b[1]-dy*3-dx*2)],fill=color)
def hand(d,x=2,y=3,color=BONE):
    d.polygon([(x,y+4),(x+3,y+3),(x+4,y),(x+8,y),(x+10,y+3),(x+10,y+7),(x+7,y+9),(x+2,y+8)],fill=color)
    d.line((x+4,y+2,x+9,y+2),fill=SHADE)
    d.line((x+4,y+5,x+8,y+5),fill=SHADE)
    d.rectangle((x,y+5,x+3,y+7),fill=SHADE)
# Open fingers read differently from the existing closed fist.
im,d=new();d.polygon([(4,13),(4,8),(2,5),(3,4),(6,7),(6,2),(7,2),(7,7),(8,1),(9,1),(9,7),(10,2),(11,2),(11,8),(12,4),(13,4),(13,10),(10,13)],fill=BONE);d.line((6,10,10,10),fill=SHADE);save('slap',im)
im,d=new();d.ellipse((5,2,13,10),fill=IRON);d.ellipse((7,3,12,8),fill=LIGHT);d.ellipse((8,4,11,7),fill=IRON);d.line((5,9,2,14),fill=SHADE,width=3);save('parry_pan',im)
im,d=new();d.polygon([(3,9),(6,9),(6,12),(10,12),(11,14),(3,14)],fill=BONE);d.arc((3,1,14,13),180,315,fill=LIGHT,width=2);d.polygon([(10,1),(14,3),(10,4)],fill=LIGHT);save('jump',im)
im,d=new();d.polygon([(1,7),(4,5),(7,5),(7,7),(4,7),(4,10),(7,11),(6,13),(2,11)],fill=BONE);d.polygon([(14,7),(11,5),(8,5),(8,7),(11,7),(11,10),(8,11),(9,13),(13,11)],fill=SHADE);d.rectangle((6,7,9,10),fill=IRON);d.line((7,7,8,10),fill=LIGHT);save('grapple',im)
im,d=new();d.rectangle((2,7,7,10),fill=SHADE);d.rectangle((7,4,9,12),fill=BONE);d.line((10,8,14,8),fill=LIGHT,width=2);d.line([(12,5),(14,8),(12,11)],fill=LIGHT);save('shove',im)
im,d=new();d.polygon([(3,2),(8,2),(8,8),(12,8),(14,11),(14,13),(3,13)],fill='#6d4e38');d.rectangle((4,3,7,8),fill=SHADE);d.line((3,13,14,13),fill=LIGHT);d.line((9,9,11,11),fill=BONE);save('kick',im)
im,d=new();d.polygon([(12,2),(14,5),(7,12),(3,10),(3,4),(6,4),(6,8)],fill=BONE);d.line([(5,5),(5,10),(7,10)],fill=SHADE);d.line([(1,8),(1,13),(5,14)],fill=RED);save('elbow',im)
im,d=new();hand(d,color=GOLD);d.line((3,4,1,2),fill=BONE);d.line((8,2,8,0),fill=BONE);d.line((13,4,15,2),fill=BONE);d.polygon([(9,4),(6,8),(9,8),(7,12),(12,6),(9,6)],fill=BONE);save('god_fist',im)
im,d=new();d.line([(8,9),(7,11),(9,13),(8,15)],fill=BONE);d.ellipse((3,1,12,9),fill=RED);d.ellipse((4,2,7,5),fill='#e29e78');d.polygon([(7,9),(9,9),(10,11),(6,11)],fill=RED);save('balloon',im)
im,d=new();d.polygon([(3,1),(12,1),(13,3),(13,12),(10,14),(4,14),(2,11),(2,3)],fill=IRON);d.rectangle((4,3,11,11),fill='#445056');d.rectangle((5,4,10,5),fill=LIGHT);d.line((4,13,11,13),fill=GOLD,width=2)
for x,y in [(3,3),(12,3),(3,11),(12,11)]:d.point((x,y),fill=LIGHT)
save('crush_shield',im)
# Passive icons have individual forms rather than a repeated colored badge.
im,d=new();d.line((6,2,6,13),fill=IRON,width=2);d.line((10,2,10,13),fill=IRON,width=2);arrow(d,(1,8),(14,8),GOLD);save('power_pierce',im)
im,d=new();d.polygon([(5,2),(12,2),(12,13),(5,13)],fill=IRON);d.line([(3,4),(7,8),(3,12)],fill=GOLD,width=2);d.line((10,3,10,11),fill=LIGHT);save('power_reflect',im)
im,d=new();d.line((2,13,13,13),fill=SHADE,width=2);d.polygon([(4,11),(3,7),(7,9),(8,2),(12,7),(12,11),(8,13)],fill=RED);d.polygon([(6,11),(8,6),(10,10),(8,12)],fill=GOLD);save('power_hearth',im)
im,d=new();d.polygon([(7,2),(10,2),(9,7),(13,10),(12,13),(6,12),(3,9),(6,8)],fill=BONE);d.line((2,4,5,4),fill=LIGHT);d.line((1,7,4,7),fill=LIGHT);d.line((1,12,4,12),fill=LIGHT);save('power_speed',im)
im,d=new();d.polygon([(2,12),(2,9),(5,8),(6,5),(8,5),(8,9),(11,8),(11,5),(9,4),(9,2),(13,2),(14,4),(14,10),(11,13),(6,14)],fill=SHADE);d.line([(3,11),(6,10),(9,11),(12,9)],fill=BONE,width=2);save('power_strong',im)
im,d=new();hand(d,3,3);d.line((1,4,4,4),fill=LIGHT);d.line((0,7,3,7),fill=LIGHT);d.line((1,11,3,11),fill=LIGHT);save('power_quick',im)
im,d=new();d.polygon([(2,5),(4,2),(6,2),(8,4),(10,2),(12,2),(14,5),(13,9),(8,14),(3,9)],fill=RED);d.line((4,4,5,4),fill='#eca782',width=2);d.line((10,9,14,9),fill=BONE);d.line((12,7,12,11),fill=BONE);save('power_health',im)
im,d=new();d.ellipse((8,1,11,4),fill=BONE);d.line([(9,5),(7,8),(10,12)],fill=BONE,width=2);d.line([(7,8),(3,12)],fill=BONE,width=2);arrow(d,(1,4),(6,4),RED);save('power_dodge',im)
im,d=new();d.ellipse((2,2,13,13),outline=GREEN,width=2);d.line((7,5,7,10),fill=BONE,width=2);d.line((5,7,10,7),fill=BONE,width=2);d.rectangle((10,1,14,5),fill=INK);d.polygon([(11,1),(14,5),(9,5)],fill=GREEN);save('power_regen',im)
im,d=new();d.line((3,12,12,3),fill=LIGHT,width=3);d.line((3,9,6,12),fill=SHADE,width=2);d.line((1,14,4,11),fill=SHADE,width=2);d.line((11,7,14,10),fill=RED);d.line((11,10,14,7),fill=RED);save('power_crit',im)
im,d=new();d.polygon([(8,1),(9,5),(14,3),(11,7),(15,9),(10,10),(12,15),(8,12),(4,15),(5,10),(1,9),(5,7),(2,3),(7,5)],fill=RED);d.polygon([(8,4),(10,8),(8,12),(6,8)],fill=BONE);save('power_crit_power',im)
im,d=new();d.polygon([(5,2),(6,4),(10,4),(11,2),(14,5),(12,8),(11,8),(12,14),(4,14),(5,8),(3,8),(1,5)],fill=IRON);d.line((7,5,7,12),fill=LIGHT);d.line((5,11,11,11),fill=LIGHT);save('power_armor',im)
im,d=new();d.line((3,13,11,5),fill=IRON,width=3);d.polygon([(8,5),(8,2),(11,1),(10,4),(12,5),(14,2),(14,6),(11,8)],fill=LIGHT);d.ellipse((1,11,5,15),outline=LIGHT);save('power_technical',im)
im,d=new();d.rectangle((3,4,12,13),fill=BONE);d.rectangle((5,2,10,4),outline=SHADE);d.rectangle((6,6,9,11),fill=RED);d.rectangle((5,7,10,9),fill=RED);save('power_medic',im)
im,d=new();d.ellipse((2,2,13,13),fill=BONE);d.line((3,6,12,10),fill=SHADE);d.line((3,10,12,6),fill=SHADE);d.rectangle((6,6,9,9),fill=RED);save('power_reuse',im)
im,d=new();d.ellipse((2,6,10,14),fill=GOLD);d.ellipse((4,8,8,12),outline=BONE);d.line((10,11,14,3),fill=SHADE,width=2);d.line((9,2,14,5),fill=LIGHT,width=2);save('power_gold',im)
im,d=new();hand(d,1,1);d.line([(10,14),(14,14),(14,10)],fill=GOLD);d.line([(1,4),(1,1),(4,1)],fill=GOLD);save('power_big',im)
im,d=new();d.arc((1,1,14,14),195,340,fill=LIGHT,width=2);d.arc((3,3,12,12),195,340,fill=IRON);d.line((7,11,10,6),fill=SHADE,width=2);d.polygon([(7,5),(10,3),(13,6),(10,8)],fill=BONE);save('power_sweep',im)
im,d=new();d.polygon([(2,5),(13,5),(10,9),(10,12),(13,14),(3,14),(6,12),(6,9),(2,7)],fill=IRON);d.line((2,5,13,5),fill=LIGHT,width=2);d.line((6,12,10,12),fill=LIGHT);save('power_iron',im)
im,d=new();d.ellipse((1,2,7,8),fill=BONE);d.ellipse((5,1,11,8),fill=BONE);d.ellipse((9,2,15,8),fill=BONE);d.rectangle((4,6,12,13),fill=BONE);d.rectangle((4,11,12,13),fill=SHADE);d.line((7,7,7,9),fill=LIGHT);save('power_chef',im)
if __name__=='__main__':
    sheet=Image.new('RGB',(8*112,4*100),'#303b35');draw=ImageDraw.Draw(sheet)
    for i,(name,im) in enumerate(made):
        x=(i%8)*112;y=(i//8)*100;sheet.paste(im.resize((64,64),Image.Resampling.NEAREST),(x+20,y),im.resize((64,64),Image.Resampling.NEAREST));draw.text((x+2,y+66),name,fill='#e4d2a1')
    sheet.save('/tmp/teeming-action-icons.png')
