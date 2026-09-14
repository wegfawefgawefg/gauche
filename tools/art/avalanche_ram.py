"""Broad wool, short legs and curled horns: four original 16px ram poses."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['avalanche_ram','ram_paw','ram_lunge','ram_stagger']:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    lunge=name=='ram_lunge';paw=name=='ram_paw';stagger=name=='ram_stagger'
    # WOOL: One rough silhouette, only a few folds in the interior.
    p.polygon([(2,6),(3,4),(7,3),(10,5),(11,9),(9,12),(3,12),(1,9)],fill='#b3b49c')
    p.polygon([(2,7),(4,5),(7,5),(8,7),(7,9),(3,10)],fill='#dad5b8')
    p.line([(3,10),(6,11),(9,10)],fill='#8a957f')
    p.line([(3,12),(2 if lunge else 3,14)],fill='#566860')
    p.line([(8,12),(10 if lunge else 8,14 if not paw else 12)],fill='#566860')
    y=2 if stagger else 1 if lunge else 0
    p.polygon([(9,5+y),(13,5+y),(14,8+y),(12,10+y),(9,9+y)],fill='#8c9b86')
    p.line([(12,8+y),(14,8+y)],fill='#d5d0ad')
    p.point((12,6+y),fill='#293f3d')
    # HORN: A dark centre keeps the curl visible at source scale.
    p.line([(9,6+y),(7,4+y),(8,2+y),(11,2+y),(12,4+y),(10,5+y)],fill='#bba478',width=2)
    p.point((10,3+y),fill='#5f6857')
    if paw:p.line([(8,14),(11,13),(13,13)],fill='#a5bbb0')
    if stagger:
        p.point((3,1),fill='#c0bb93');p.line([(13,1),(14,2)],fill='#c0bb93')
    im.save(root/(name+'.png'))
