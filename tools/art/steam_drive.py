"""One low steam engine: flywheel and striped housing, running/off/broken."""
from underworks_palette import *
for state in ('','_off','_broken'):
    im,p=canvas()
    p.rectangle((1,12,14,14),fill=INK)
    p.line((2,13,13,13),fill=IRON)
    p.rectangle((1,4,6,12),fill=INK)
    p.rectangle((2,5,5,11),fill=BRASS)
    p.line((2,5,2,10),fill=STEEL)
    p.rectangle((2,2,4,5),fill=INK)
    p.line((3,2,3,4),fill=STEEL)
    p.ellipse((5,3,14,12),fill=INK)
    p.ellipse((6,4,13,11),fill=IRON)
    p.ellipse((7,5,12,10),fill=INK)
    p.point((9,8),fill=LIGHT)
    p.rectangle((2,7,4,8),fill=GOLD if not state else DARK)
    if state=='_broken':
        p.rectangle((9,2,15,7),fill=(0,0,0,0))
        p.line((8,3,9,5),fill=RUST)
        p.line((11,11,14,13),fill=BRASS)
        p.line((4,4,6,2),fill=RUST)
    save(im,'steam_drive'+state)
