"""Native ice field pieces: brittle teeth, snow drifts and split/broad columns."""
from underworks_palette import *

for v,suffix in enumerate('abc'):
    im,p=canvas()
    p.ellipse((1,10,14,14),fill='#354d51')
    for x,h in (((3,7),(7,12),(12,6)),((2,5),(6,8),(11,11)),((4,12),(9,5),(13,8)))[v]:
        p.polygon([(x-2,12),(x,13-h),(x+2,12)],fill='#638a93')
        p.line((x,14-h,x-1,11),fill='#b4cdc4')
        p.point((x+1,12),fill='#85aeb0')
    save(im,'ice_spikes_'+suffix)
    im,p=canvas()
    bounds=((3,8,13,14),(1,6,14,14),(0,4,15,14))[v]
    p.ellipse(bounds,fill='#607976')
    p.ellipse((bounds[0]+1,bounds[1],bounds[2]-1,bounds[3]-2),fill='#a5b7aa')
    p.ellipse((bounds[0]+2,bounds[1],bounds[2]-3,bounds[3]-5),fill='#c4cdba')
    p.line((bounds[0]+3,12,bounds[2]-2,12),fill='#819994')
    save(im,'snow_pile_'+suffix)

for family in ('forked','broad'):
    for cracked in (False,True):
        im,p=canvas((16,48))
        outline=[(1,44),(2,19),(4,2),(7,16),(10,7),(13,17),(15,44)] if family=='forked' else [(1,44),(1,13),(4,3),(10,1),(14,9),(15,44)]
        p.polygon(outline,fill='#344e58')
        p.polygon([(3,42),(3,20),(5,6),(7,26),(8,42)],fill='#97b7af')
        p.polygon([(8,42),(8,24),(11,12 if family=='forked' else 4),(13,18),(13,42)],fill='#638a93')
        p.line([(4,39),(6,42),(12,42)],fill='#c0d1c4')
        p.line([(10,16),(11,32),(10,37)],fill='#afc8c0')
        if family=='broad':p.polygon([(5,7),(9,3),(8,18),(5,29)],fill='#b4cdc4')
        if cracked:p.line([(2,37),(8,33),(5,29),(11,23),(9,18)],fill='#253c43')
        save(im,'ice_pillar_'+family+('_cracked' if cracked else ''))
