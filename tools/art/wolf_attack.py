"""Wolf bite poses and closing fangs at the existing 16-pixel scale."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT=Path(__file__).resolve().parents[2]/'assets/graphics'
FUR='#9c9c83'; SHADE='#6b7066'; DARK='#353a34'; TOOTH='#e6dfc4'; GUM='#92533f'

def pose(lunge):
    im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
    # Low haunches and braced forelegs stay recognizable as the existing wolf.
    d.polygon([(0,5),(2,7),(4,6),(7,6),(10,7),(12,6),(13,9),(11,12),(5,12),(2,10)],fill=FUR)
    d.polygon([(0,4),(0,8),(2,11),(4,11),(3,8)],fill=SHADE)
    d.line([(5,10),(4 if lunge else 3,14),(6,14)],fill=SHADE,width=2)
    d.line([(10,11),(13 if lunge else 10,14),(15 if lunge else 12,14)],fill=SHADE,width=2)
    if lunge:
        d.polygon([(9,6),(12,3),(12,6),(15,6),(15,8),(12,9),(15,11),(14,12),(10,10),(8,8)],fill=FUR)
        d.polygon([(12,8),(15,8),(15,10),(12,9)],fill=DARK)
        d.point((13,8),fill=TOOTH);d.point((14,10),fill=TOOTH)
        d.point((12,6),fill='#e7cc83');d.point((15,6),fill=DARK)
    else:
        d.polygon([(7,6),(8,3),(11,6),(13,6),(15,8),(15,10),(12,10),(14,12),(11,13),(8,10)],fill=FUR)
        d.polygon([(10,8),(15,9),(15,10),(12,10),(14,12),(10,11)],fill=DARK)
        d.line([(11,11),(13,12)],fill=GUM)
        for x in (11,14): d.line([(x,9),(x,10)],fill=TOOTH)
        d.point((11,7),fill='#e7cc83');d.point((15,8),fill=DARK)
        d.line([(3,6),(6,5),(8,6)],fill=SHADE)
    im.save(ROOT/('wolf_lunge.png' if lunge else 'wolf_crouch.png'))

def fangs(closed):
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    top,bottom=(6,9) if closed else (3,12)
    # A jaw bracket, not a claw slash: opposing curved rows close on the bite cell.
    d.line([(3,top+2),(5,top),(10,top),(13,top+2)],fill=TOOTH,width=1)
    d.line([(3,bottom-2),(5,bottom),(10,bottom),(13,bottom-2)],fill=TOOTH,width=1)
    for x in (5,10):
        d.line([(x,top),(x,top+2)],fill=TOOTH)
        d.line([(x+1,bottom),(x+1,bottom-2)],fill=TOOTH)
    if closed:
        for a,b in [((1,7),(2,7)),((14,7),(15,7)),((8,2),(8,3)),((8,12),(8,13))]:d.line([a,b],fill='#a8ad98')
    im.save(ROOT/('wolf_fangs_closed.png' if closed else 'wolf_fangs.png'))

if __name__=='__main__':
    pose(False);pose(True);fangs(False);fangs(True)
