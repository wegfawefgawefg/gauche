"""Three-tile ground seams and small native 16px steam/molten release poses."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT=Path(__file__).resolve().parents[2]/"assets"/"graphics"
seam=[(0,8),(3,8),(5,6),(8,7),(10,10),(12,8),(15,8)]
for name,edge,core in [("crack","#51453a","#111516"),("glow","#733c24","#e5b364"),("cooled","#465654","#192322")]:
    im=Image.new("RGBA",(16,16));p=ImageDraw.Draw(im)
    p.line([(x,y+1) for x,y in seam],fill=edge,width=3)
    p.line(seam,fill=core)
    p.line([(5,7),(5,4),(7,3)],fill=edge);p.line([(10,9),(9,12),(6,13)],fill=edge)
    im.save(ROOT/f"fissure_{name}.png")
for frame in range(2):
    im=Image.new("RGBA",(16,32));p=ImageDraw.Draw(im)
    for i,y in enumerate((28,22,16,10,4)):
        x=7+((i*2+frame)%5)-2
        w=2+i//2
        p.polygon([(x-w,y-1),(x-w+1,y-4),(x+1,y-5),(x+w,y-3),
                   (x+w+1,y),(x+1,y+3),(x-w+1,y+2)],fill=(157,177,175,150-i*19))
        p.polygon([(x-1,y-3),(x+1,y-3),(x+2,y-1),(x,y+1),(x-2,y)],
                  fill=(204,214,204,185-i*24))
        p.point((x-w-1,y+1),fill=(182,201,191,85))
    p.line([(7,31),(7,26),(9,22)],fill=(215,221,208,220),width=2)
    im.save(ROOT/f"fissure_steam_{'ab'[frame]}.png")
    im=Image.new("RGBA",(16,32));p=ImageDraw.Draw(im)
    p.polygon([(4,31),(5,26),(8,22),(9,15),(11,21),(10,27),(12,31)],fill="#b54d23")
    p.line([(7,31),(8,26),(9,22)],fill="#f2a03c",width=2)
    for x,y in ((3+frame,17),(10-frame,8),(5+frame,3),(13-frame,22)):
        p.rectangle((x,y,x+1,y+2),fill="#e98d2c");p.point((x,y),fill="#ffd06b")
    im.save(ROOT/f"fissure_lava_{'ab'[frame]}.png")
