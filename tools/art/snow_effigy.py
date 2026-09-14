"""Four sparse snow-statue and exposed twig-frame poses, facing right."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['snow_effigy','effigy_strike','effigy_frame','effigy_frame_strike']:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    frame='frame' in name;strike='strike' in name
    # FRAME: Long forked sticks remain after the broad snow covering melts.
    p.line([(6,4),(7,10),(4,14)],fill='#8b805d',width=2)
    p.line([(7,9),(10,14)],fill='#8b805d')
    p.line([(7,7),(3,6),(2,3)],fill='#8b805d')
    p.line([(7,7),(12,5 if strike else 8),(14,4 if strike else 7)],fill='#ac9b6d')
    if not frame:
        p.polygon([(5,7),(9,7),(10,9),(10,12),(8,13),(3,12),(3,10)],fill='#a7b6a8')
        p.polygon([(4,9),(6,8),(8,9),(8,11),(4,11)],fill='#d0d6bd')
    p.polygon([(5,2),(9,2),(11,4),(10,6),(6,6),(4,4)],fill='#887b57' if frame else '#c6d1b8')
    p.line([(5,2),(4,0)],fill='#8b805d');p.line([(8,2),(10,0)],fill='#8b805d')
    eyes='#f1bc61' if strike else '#37493f'
    p.point((8,3),fill=eyes);p.point((10,4),fill=eyes)
    p.line([(8,5),(9,5)],fill='#675b3c')
    im.save(root/(name+'.png'))
