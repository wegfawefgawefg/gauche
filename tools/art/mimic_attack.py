"""Hinged wooden jaws: existing 16px crate mimic, with four distinct bite poses."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets/graphics'
WOOD='#806645'; EDGE='#b19460'; DARK='#211c20'; TOOTH='#d8caa3'; LEG='#6f6952'

def pose(name,top,bottom,legs):
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    # Lid and base remain horizontal: a crate opening on a hinge, not a face blob.
    d.rectangle((2,top,13,bottom+2),fill=DARK)
    d.rectangle((2,top,13,top+3),fill=WOOD)
    d.line([(2,top),(13,top),(13,top+2)],fill=EDGE)
    d.line([(5,top+1),(5,top+2)],fill=EDGE);d.line([(10,top+1),(10,top+2)],fill=EDGE)
    d.rectangle((2,bottom,13,bottom+2),fill=WOOD)
    d.line([(3,bottom+1),(12,bottom+1)],fill=EDGE)
    for x in (3,7,11):
        d.polygon([(x,top+3),(x+2,top+3),(x+1,top+5)],fill=TOOTH)
        d.polygon([(x,bottom),(x+2,bottom),(x+1,bottom-1)],fill=TOOTH)
    if bottom-top>7:d.line([(6,bottom-1),(8,bottom-2),(10,bottom-1)],fill='#945953')
    if legs=='brace':
        d.line([(3,bottom+2),(1,14),(4,14)],fill=LEG);d.line([(12,bottom+2),(14,14),(11,14)],fill=LEG)
    elif legs=='snap':
        d.line([(3,bottom+2),(0,12),(2,13)],fill=LEG);d.line([(12,bottom+2),(15,12),(13,13)],fill=LEG)
    else:
        d.line([(3,bottom+2),(2,15),(5,15)],fill=LEG);d.line([(12,bottom+2),(13,15),(10,15)],fill=LEG)
    im.save(OUT/f'mimic_{name}.png')

def jaws(closed):
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    top,bottom=(5,10) if closed else (1,14)
    for y in (top,bottom):d.line([(2,y),(13,y)],fill=EDGE)
    for x in (3,7,11):
        d.polygon([(x,top+1),(x+2,top+1),(x+1,top+3)],fill=TOOTH)
        d.polygon([(x,bottom-1),(x+2,bottom-1),(x+1,bottom-3)],fill=TOOTH)
    if closed:
        for a,b in [((0,7),(1,7)),((14,7),(15,7)),((7,1),(7,2)),((7,13),(7,14))]:d.line([a,b],fill=EDGE)
    im.save(OUT/('mimic_jaws_closed.png' if closed else 'mimic_jaws.png'))

if __name__=='__main__':
    pose('open',2,10,'brace');pose('gape',0,11,'brace')
    pose('snap',6,11,'snap');pose('recover',6,12,'rest')
    jaws(False);jaws(True)
