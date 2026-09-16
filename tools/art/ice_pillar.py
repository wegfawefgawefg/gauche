"""Tall single-footprint ice column and fallen chunks; no generated bitmap source."""
from underworks_palette import *

for cracked in (False, True):
    im,p=canvas((16,48))
    p.polygon([(7,1),(11,6),(12,18),(14,40),(13,44),(3,45),(2,40),(4,17),(5,5)],fill='#344e58')
    p.polygon([(7,3),(10,7),(11,26),(12,42),(7,43),(5,30),(5,8)],fill='#779d9e')
    p.polygon([(7,3),(7,32),(5,39),(4,22),(6,6)],fill='#aac5bd')
    p.polygon([(10,9),(11,27),(12,40),(9,42),(9,18)],fill='#557d8a')
    p.line([(4,41),(7,43),(12,41)],fill='#b7cbc3')
    for x,y in ((6,10),(6,22),(8,34),(10,18)):
        p.line((x,y,x,y+2),fill='#c1d1cb')
    p.line([(4,16),(7,17),(10,15)],fill='#91b8b7')
    p.line([(5,29),(8,28),(11,30)],fill='#91b8b7')
    if cracked:
        p.line([(4,35),(8,33),(6,30),(10,25),(7,23),(9,19)],fill='#293f4c')
        p.line([(8,33),(11,36),(12,39)],fill='#d0d6c7')
        p.line([(6,30),(4,26)],fill='#d0d6c7')
    save(im,'ice_pillar_cracked' if cracked else 'ice_pillar')
im,p=canvas()
p.polygon([(1,7),(5,4),(13,5),(15,10),(12,13),(3,12)],fill='#314b58')
p.polygon([(2,7),(5,5),(12,6),(13,10),(5,10)],fill='#81a7aa')
p.line([(2,7),(6,8),(12,6)],fill='#c1d1cb')
p.line([(7,8),(8,11),(11,12)],fill='#486c7a')
save(im,'ice_rubble')
