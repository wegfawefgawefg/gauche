"""Industrial workers and machines with distinct, hand-pixelled action poses.

Run directly with Python/Pillow. Nothing is traced, sampled or downscaled from
generated art. Feet share the old workers' baseline; masks/cargo explain roles.
"""
from underworks_palette import *


def welder(name, lowered):
    im, p = canvas()
    boots(p)
    p.polygon([(4, 7), (10, 7), (12, 13), (3, 13)], fill=WOOD)
    p.rectangle((5, 8, 9, 12), fill=RUST)
    p.line((5, 8, 5, 11), fill=CLOTH)
    p.rectangle((5, 3, 10, 7), fill=IRON if lowered else CLOTH)
    p.rectangle((6, 4 if lowered else 1, 10, 6 if lowered else 3), fill=INK)
    p.line((7, 5 if lowered else 2, 9, 5 if lowered else 2), fill=TEAL)
    p.line([(10, 8), (12, 9), (14, 7)], fill=STEEL, width=2)
    p.line([(12, 10), (14, 12), (12, 14)], fill=INK)
    p.point((15, 6), fill=ICE if lowered else BRASS)
    save(im, name)


def clerk(name, pose):
    im, p = canvas()
    boots(p)
    p.polygon([(5, 6), (10, 6), (12, 13), (3, 13)], fill=CLOTH)
    p.line((7, 7, 7, 12), fill=WOOD)
    p.rectangle((6, 2, 10, 6), fill='#b69c7b')
    p.rectangle((5, 1, 10, 2), fill=IRON)
    p.line((4, 3, 11, 3), fill=INK)
    p.point((7, 4), fill=INK)
    p.point((10, 4), fill=INK)
    p.rectangle((1, 7, 5, 11), fill=WOOD)
    p.rectangle((2, 7, 4, 10), fill=LIGHT)
    p.line((2, 8, 3, 8), fill=IRON)
    if pose == 'alarm':
        p.line([(11, 8), (13, 5), (12, 4)], fill=CLOTH, width=2)
        p.polygon([(12, 3), (14, 2), (15, 5), (12, 5)], fill=GOLD)
        p.point((14, 6), fill=BRASS)
    else:
        y = 11 if pose == 'stamp' else 6
        p.line([(11, 8), (13, y)], fill=CLOTH)
        p.line((13, y - 2, 13, y), fill=RUST, width=2)
        p.line((11, y + 1, 15, y + 1), fill=INK)
    save(im, name)


def kiln(name, state):
    im, p = canvas()
    p.line([(3, 11), (2, 14), (5, 14)], fill=IRON, width=2)
    p.line([(11, 11), (13, 14), (10, 14)], fill=IRON, width=2)
    p.rectangle((9, 0, 11, 4), fill=IRON)
    p.line((8, 0, 12, 0), fill=STEEL)
    p.polygon([(4, 3), (10, 3), (13, 7), (12, 12), (3, 12), (2, 7)], fill=WOOD)
    p.line([(3, 6), (4, 4), (9, 4)], fill=RUST)
    p.rectangle((4, 6, 10, 10), fill=INK)
    if state == 'open':
        p.rectangle((5, 7, 9, 9), fill=FIRE)
        p.line([(6, 9), (7, 7), (8, 9)], fill=GOLD)
        p.rectangle((11, 7, 13, 10), fill=IRON)
    else:
        p.rectangle((5, 7, 9, 10), fill=IRON)
        p.line((6, 8, 8, 8), fill=ICE if state == 'cold' else FIRE)
        p.line((6, 10, 8, 10), fill=ICE if state == 'cold' else RUST)
    bolts(p, [(3, 7), (11, 6), (11, 11)])
    save(im, name)


def thief():
    im, p = canvas()
    boots(p)
    p.polygon([(5, 5), (10, 5), (13, 12), (3, 12)], fill=IRON)
    p.rectangle((2, 6, 5, 11), fill=WOOD)  # big collection sack
    p.line((2, 6, 5, 6), fill=CLOTH)
    p.polygon([(5, 2), (10, 2), (12, 6), (4, 6)], fill=DARK)
    p.rectangle((6, 4, 10, 5), fill=INK)
    bolts(p, [(7, 4), (10, 4)])
    p.line([(11, 7), (13, 9), (11, 10)], fill=CLOTH, width=2)
    p.rectangle((10, 10, 13, 12), fill=BRASS)
    p.line((11, 10, 13, 10), fill=GOLD)
    save(im, 'mold_thief')


def singer():
    im, p = canvas()
    boots(p)
    p.polygon([(5, 5), (10, 5), (12, 10), (12, 13), (3, 13), (3, 9)], fill=WOOD)
    p.polygon([(5, 7), (10, 7), (11, 12), (5, 12)], fill=DARK)
    helmet(p)
    bolts(p, [(5, 6), (10, 6)])
    p.rectangle((6, 7, 9, 10), fill=BRASS)
    p.rectangle((7, 7, 8, 9), fill=INK)
    p.line((9, 10, 9, 12), fill=BRASS)
    p.line([(3, 9), (1, 10), (2, 12)], fill=WOOD, width=2)
    p.line([(12, 9), (14, 10), (13, 12)], fill=WOOD, width=2)
    save(im, 'tar_singer')


def shunter():
    im, p = canvas()
    boots(p)
    p.polygon([(4, 7), (10, 7), (11, 13), (3, 13)], fill=IRON)
    p.line((5, 7, 5, 12), fill=BRASS)
    p.line((9, 7, 9, 12), fill=BRASS)
    p.rectangle((5, 3, 10, 6), fill=WOOD)
    helmet(p)
    p.line((6, 5, 9, 5), fill=INK)
    p.point((9, 5), fill=LIGHT)
    p.line([(10, 8), (13, 8)], fill=CLOTH, width=2)
    p.line((13, 5, 13, 14), fill=STEEL)
    p.line([(13, 5), (15, 5), (15, 7)], fill=STEEL)
    save(im, 'rail_shunter')


welder('arc_welder', False)
welder('welder_mask', True)
for filename, pose in [('audit_clerk', 'idle'), ('clerk_stamp', 'stamp'), ('clerk_alarm', 'alarm')]:
    clerk(filename, pose)
for filename, state in [('walking_kiln', 'closed'), ('kiln_open', 'open'), ('kiln_cooled', 'cold')]:
    kiln(filename, state)
thief()
singer()
shunter()
