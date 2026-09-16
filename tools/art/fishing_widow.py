"""Original veiled fisher poses, barbed hook, smoked fish and discarded bones."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['fishing_widow','widow_windup','widow_reel','widow_untangle','widow_work_cast','widow_work_reel']:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    warn=name=='widow_windup';reel=name=='widow_reel';rest=name=='widow_untangle'
    # VEIL: Wide woven hat above a narrow slit; ragged skirt stays a single dark mass.
    p.polygon([(2,4),(6,1),(10,3),(11,5),(2,5)],fill='#8a8564')
    p.line([(3,4),(8,3),(10,4)],fill='#b0a382')
    p.polygon([(4,5),(9,5),(10,9),(8,10),(4,8)],fill='#52645d')
    p.line([(7,6),(9,6)],fill='#c1b7a1')
    p.polygon([(4,8),(8,8),(10,13),(8,14),(6,13),(3,14),(2,12)],fill='#42564e')
    p.line([(4,9),(3,12)],fill='#7c8370')
    p.line([(4,14),(3,15)],fill='#948f77');p.line([(8,14),(10,15)],fill='#948f77')
    if name.startswith('widow_work_'):
        p.line([(8,9),(11,10),(12,9)],fill='#c1b7a1')
        p.line([(10,11),(12,7),(15,5 if name=='widow_work_cast' else 8)],fill='#ad9670')
        p.point((11,10),fill='#b6a477')
        p.point((15,6 if name=='widow_work_cast' else 9),fill='#b78459')
    elif warn:
        p.line([(8,9),(11,7),(11,4)],fill='#c1b7a1')
        p.line([(10,8),(12,2),(14,1)],fill='#ad9670')
        p.line([(14,1),(15,3),(15,5)],fill='#82958b')
    elif reel:
        p.line([(7,9),(11,9),(12,8)],fill='#c1b7a1')
        p.line([(9,10),(12,6),(15,6)],fill='#ad9670')
    elif rest:
        p.line([(7,9),(10,11),(7,12)],fill='#c1b7a1')
        p.line([(10,12),(12,9),(11,7),(13,5)],fill='#ad9670')
        p.line([(11,10),(13,11),(12,13)],fill='#82958b')
    else:
        p.line([(8,9),(11,10)],fill='#c1b7a1')
        p.line([(10,12),(12,4),(14,3)],fill='#ad9670')
    im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(2,7),(10,7),(13,8),(13,10),(10,11),(9,9),(11,10)],fill='#bcb6a0')
p.point((13,7),fill='#dbcaa5');im.save(root/'widow_hook.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(2,7),(5,5),(10,5),(13,7),(13,9),(10,11),(5,10),(2,12),(2,9)],fill='#9f7350')
p.line([(5,6),(10,6),(12,7)],fill='#cfa471')
p.line([(6,9),(10,9)],fill='#6b5743');p.point((11,7),fill='#343d39')
im.save(root/'smoked_fish.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(4,8),(10,8)],fill='#b3b7a0')
p.line([(6,6),(6,10)],fill='#9da68e');p.line([(8,7),(8,9)],fill='#9da68e')
p.line([(10,8),(12,6)],fill='#b3b7a0');p.line([(10,8),(12,10)],fill='#b3b7a0')
im.save(root/'debris_fish_bone.png')
