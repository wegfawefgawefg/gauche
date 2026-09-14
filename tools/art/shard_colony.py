"""Three states of a rooted blue crystal, without hiding the damaged body."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['shard_node','shard_charged','shard_dim']:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    lit=name=='shard_charged';dim=name=='shard_dim'
    p.polygon([(3,13),(6,11),(11,12),(13,14),(4,14)],fill='#47595b')
    p.polygon([(7,1),(11,6),(10,11),(7,13),(4,9),(4,6)],fill='#466773' if dim else '#70999f')
    p.polygon([(7,1),(7,11),(4,9),(4,6)],fill='#647c7b' if dim else '#a3c1b4')
    p.polygon([(7,2),(10,6),(8,7)],fill='#cbe5cb' if lit else '#8faea6')
    p.line([(7,4),(6,7),(8,9),(7,11)],fill='#e7f7d7' if lit else '#2e4a55')
    p.polygon([(12,8),(13,11),(11,13),(10,11)],fill='#95c8cb' if lit else '#507278')
    if lit: p.point((3,4),fill='#d1eee4');p.point((12,3),fill='#a7dbdd')
    im.save(root/(name+'.png'))
