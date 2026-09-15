"""Sparse cold-room fittings. Decoration never implies new usable floor space."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT=Path(__file__).resolve().parents[2]/'assets/graphics'
metal='#697e83'; edge='#a5b6b3'; dark='#33494f'; wood='#686959'; brass='#aaa17a'


def make(name,draw):
    image=Image.new('RGBA',(16,16)); draw(ImageDraw.Draw(image)); image.save(ROOT/(name+'.png'))


def window(d):
    d.rectangle((2,1,13,12),fill=wood);d.rectangle((4,3,11,10),fill=dark)
    d.line([(7,2),(7,11)],fill=edge);d.line([(3,7),(12,7)],fill=metal)
    d.polygon([(8,3),(11,3),(8,6)],fill='#819d9f');d.line([(1,13),(14,13)],fill=edge)


def chain(d):
    d.rectangle((5,1,10,3),fill=wood)
    for x,y in [(7,3),(6,6),(7,9),(7,12)]: d.ellipse((x-1,y-1,x+2,y+2),outline=edge)


def hatch(d):
    d.ellipse((2,3,13,13),fill=dark,outline=metal)
    d.rectangle((5,7,10,9),outline=edge);d.point((4,5),fill=brass);d.point((11,11),fill=brass)


def pipe(d):
    d.line([(2,0),(2,5),(10,5),(10,8)],fill=metal,width=3)
    d.line([(3,0),(3,4),(11,4)],fill=edge)
    d.rectangle((0,2,4,3),fill=brass);d.line([(8,8),(12,8)],fill=edge)
    d.line([(10,10),(10,12)],fill='#71969d')


def wheel(d):
    d.ellipse((1,1,14,14),outline=wood,width=2)
    for a,b in [((2,7),(13,7)),((7,2),(7,13)),((3,3),(12,12)),((12,3),(3,12))]: d.line([a,b],fill=metal,width=2)
    d.rectangle((6,6,9,9),fill=brass)


def motor(d):
    d.ellipse((3,3,12,12),fill=dark,outline=metal,width=2)
    d.line([(7,4),(7,11)],fill=edge);d.line([(4,7),(11,7)],fill=edge)
    d.rectangle((6,6,9,9),fill=brass)


def flag(d):
    d.line([(4,1),(4,14)],fill=edge)
    d.polygon([(5,2),(13,3),(11,5),(13,7),(8,6),(5,7)],fill='#766b67')
    d.line([(6,3),(9,4)],fill='#af9e85');d.line([(2,14),(6,14)],fill=metal)


def drip(d):
    d.polygon([(3,0),(12,0),(10,4),(7,3),(6,6)],fill=metal)
    d.line([(7,4),(7,8)],fill='#a5c1c3');d.point((7,11),fill=edge)
    d.arc((4,12,10,15),0,180,fill='#688d98')


def chimney(d):
    d.rectangle((5,2,10,14),fill=dark);d.rectangle((6,3,9,13),fill=metal)
    d.line([(3,2),(12,2)],fill=edge,width=2);d.line([(4,12),(11,12)],fill=brass)


for name,draw in [('window',window),('chain',chain),('hatch',hatch),('pipe',pipe),('wheel',wheel),
                  ('motor',motor),('flag',flag),('drip',drip),('chimney',chimney)]: make('ice_'+name,draw)
