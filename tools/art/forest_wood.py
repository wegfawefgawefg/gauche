"""Quiet top-down timber and rooted trunks; damage uses the shared crack overlay."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets'/'graphics'

im=Image.new('RGBA',(16,16),'#252d21');d=ImageDraw.Draw(im)
d.polygon([(1,3),(5,4),(5,1),(10,1),(11,5),(15,3),(13,8),(15,12),(11,11),(12,15),(7,13),(3,15),(4,10),(1,9)],fill='#544c30')
d.polygon([(5,3),(10,3),(12,7),(11,12),(7,13),(4,10),(4,6)],fill='#76603d')
d.line([(7,3),(6,6),(7,9),(6,11)],fill='#a0834e')
d.line([(10,5),(9,8),(10,11)],fill='#443d2c')
d.polygon([(0,2),(2,1),(5,2),(3,4),(0,4)],fill='#475b33')
d.polygon([(12,12),(15,10),(15,14),(13,15)],fill='#3c5130')
im.save(OUT/'forest_tree.png')

im=Image.new('RGBA',(16,16),'#51442f');d=ImageDraw.Draw(im)
# Continuous grain crosses the tile boundaries; no platformer-style bright top lip.
for x in [0,6,12]:
    d.line((x,0,x,15),fill='#383a2b')
d.line([(3,0),(2,4),(3,8),(3,15)],fill='#796342')
d.line([(9,0),(9,5),(8,8),(9,12),(9,15)],fill='#69583c')
d.line((14,1,14,5),fill='#87714a');d.line((14,10,14,15),fill='#74613f')
d.point((4,6),fill='#2b3226');d.point((10,12),fill='#2b3226')
im.save(OUT/'forest_timber.png')

im=Image.new('RGBA',(16,16),'#252d21');d=ImageDraw.Draw(im)
d.polygon([(4,8),(5,5),(10,5),(12,8),(11,11),(5,11)],fill='#655337')
d.polygon([(5,6),(10,6),(11,8),(9,10),(6,10),(4,8)],fill='#a28b59')
d.line([(6,7),(9,7),(10,8),(8,9),(6,8)],fill='#75643d')
d.line((2,11,5,10),fill='#4c4930');d.line((11,10,13,12),fill='#4c4930')
im.save(OUT/'tree_stump.png')

im=Image.new('RGBA',(16,16),'#252d21');d=ImageDraw.Draw(im)
d.polygon([(2,9),(7,6),(8,8),(3,11)],fill='#715c3d')
d.polygon([(9,4),(11,3),(13,9),(11,10)],fill='#8b7147')
d.line((6,12,10,13),fill='#544b32');d.point((3,4),fill='#51492f')
im.save(OUT/'timber_broken.png')
