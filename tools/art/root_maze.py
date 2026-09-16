"""Connected, native-resolution roots. E/S/W/N sockets follow actual terrain."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for mask in range(16):
    im=Image.new('RGBA',(16,16),'#252d21');p=ImageDraw.Draw(im)
    ends=[(16,8),(8,16),(-1,8),(8,-1)]
    degree=mask.bit_count()
    if mask==15:
        p.rectangle((0,0,15,15),fill='#594a32')
        p.line([(3,0),(2,5),(4,10),(3,16)],fill='#75613e')
        p.line([(10,0),(9,6),(11,12),(10,16)],fill='#433b29')
        p.line([(14,0),(13,8),(14,16)],fill='#685638')
    else:
        for i,end in enumerate(ends):
            if not mask & (1<<i): continue
            p.line([(8,8),end],fill='#332e23',width=13)
            p.line([(7,7),end],fill='#594a32',width=10)
            p.line([(6,6),end],fill='#75613e',width=3)
            p.line([(9,10),end],fill='#433b29',width=2)
        if degree<2:
            p.ellipse((3,3,12,12),fill='#594a32')
            p.arc((4,4,11,11),20,305,fill='#887047',width=1)
            p.line([(9,5),(6,7),(7,10)],fill='#413b29')
        else:
            # Grain bends through junctions; don't stamp a knot on every tile.
            p.line([(6,5),(7,8),(5,11)],fill='#75613e')
            p.line([(10,5),(9,8),(11,10)],fill='#433b29')
    if not mask&1:p.line([(12,10),(14,12)],fill='#45452c')
    if not mask&8:p.point((13,2),fill='#445132')
    im.save(OUT/f'root_{mask}.png')
im=Image.new('RGBA',(16,16),'#252d21');p=ImageDraw.Draw(im)
p.polygon([(0,5),(4,6),(6,10),(2,11),(0,10)],fill='#685237')
p.polygon([(11,4),(15,5),(15,10),(12,11),(10,8)],fill='#574a33')
p.line([(3,6),(5,9),(3,10)],fill='#b0955d')
p.line([(11,5),(10,8),(12,10)],fill='#a18753')
p.line([(5,12),(9,13)],fill='#6c5b3c');p.point((8,5),fill='#8b7046')
im.save(OUT/'root_cut.png')
