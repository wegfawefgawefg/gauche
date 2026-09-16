"""Hand-drawn 16px industrial gear and small surface/debris sprites."""
from underworks_palette import *


def tool(name):
    im, p = canvas()
    if name == 'arc_torch':
        p.rectangle((3, 9, 5, 13), fill=WOOD)
        p.line([(5, 10), (8, 7), (11, 7), (13, 5)], fill=STEEL, width=2)
        p.point((14, 4), fill=ICE)
        p.line([(3, 13), (1, 14), (1, 10)], fill=INK)
    elif name == 'pocket_drill':
        p.rectangle((3, 6, 9, 10), fill=BRASS)
        p.line((3, 6, 8, 6), fill=GOLD)
        p.rectangle((4, 10, 6, 13), fill=WOOD)
        p.line((10, 8, 15, 8), fill=STEEL)
        p.point((12, 7), fill=LIGHT)
        p.point((14, 9), fill=LIGHT)
    elif name == 'pocket_pump':
        p.rectangle((5, 5, 9, 12), fill=TEAL)
        p.line((6, 5, 6, 11), fill=STEEL)
        p.line((7, 2, 7, 5), fill=STEEL)
        p.line((4, 2, 10, 2), fill=WOOD, width=2)
        p.line([(9, 11), (12, 11), (12, 6), (14, 6)], fill=IRON)
    elif name in ('steam_lance', 'lance_hot'):
        p.rectangle((2, 7, 5, 9), fill=WOOD)
        p.line((5, 8, 13, 8), fill=STEEL, width=2)
        p.rectangle((7, 6, 10, 10), fill=BRASS)
        p.line((7, 7, 10, 7), fill=FIRE if name == 'lance_hot' else GOLD)
        p.line((14, 6, 14, 10), fill=IRON)
        p.line([(5, 10), (5, 12), (8, 12)], fill=TEAL)
    elif name == 'rail_switch_key':
        p.rectangle((3, 3, 7, 6), outline=STEEL)
        p.line((5, 6, 5, 13), fill=STEEL, width=2)
        p.line((5, 11, 9, 11), fill=BRASS)
        p.line((8, 11, 8, 13), fill=BRASS)
    elif name == 'mold_key':
        p.rectangle((3, 4, 6, 7), outline=GOLD)
        p.line((6, 7, 11, 12), fill=BRASS, width=2)
        p.line((10, 10, 12, 8), fill=BRASS)
    elif name == 'nozzle_elbow':
        p.line([(4, 12), (4, 6), (12, 6)], fill=IRON, width=5)
        p.line([(3, 11), (3, 5), (11, 5)], fill=STEEL)
        p.line((2, 12, 6, 12), fill=BRASS)
        p.line((12, 4, 12, 8), fill=BRASS)
    elif name == 'horseshoe_magnet':
        p.line([(4, 4), (4, 10), (6, 12), (9, 12), (11, 10), (11, 4)], fill=RUST, width=3)
        p.rectangle((3, 3, 5, 5), fill=STEEL)
        p.rectangle((10, 3, 12, 5), fill=STEEL)
    elif name == 'tension_spring':
        p.line([(4, 2), (10, 3), (4, 5), (10, 6), (4, 8), (10, 9), (4, 11), (10, 12), (7, 14)], fill=STEEL)
        for y in (3, 6, 9, 12):
            p.point((10, y), fill=LIGHT)
    elif name == 'chain_hook':
        for y in (2, 5, 8):
            p.rectangle((6, y, 8, y+3), outline=STEEL)
        p.line([(7, 10), (7, 13), (11, 13), (12, 10)], fill=STEEL, width=2)
    elif name == 'nail_board':
        p.polygon([(2, 11), (11, 3), (14, 6), (5, 14)], fill=WOOD)
        p.line((3, 11, 12, 4), fill=CLOTH)
        for x, y in [(6, 9), (10, 6)]:
            p.line((x, y, x, y-3), fill=STEEL)
            p.point((x+1, y-3), fill=LIGHT)
    elif name == 'hand_bellows':
        p.polygon([(3, 5), (6, 4), (11, 8), (7, 12), (3, 11)], fill=RUST)
        p.line([(3, 5), (10, 8), (4, 11)], fill=WOOD, width=2)
        p.line((11, 8, 14, 8), fill=STEEL, width=2)
        p.line((3, 4, 1, 2), fill=WOOD, width=2)
        p.line((4, 12, 2, 14), fill=WOOD, width=2)
    save(im, name)


def can(name, foam):
    im, p = canvas()
    handle(p, (5, 2, 9, 4))
    p.rectangle((3, 5, 11, 13), fill=CLOTH if foam else TEAL)
    p.line((3, 5, 3, 12), fill=LIGHT if foam else STEEL)
    p.rectangle((5, 7, 9, 11), fill=IRON)
    if foam:
        p.line((6, 8, 8, 8), fill=LIGHT)
        p.line((6, 10, 8, 10), fill=LIGHT)
    else:
        p.polygon([(7, 7), (9, 10), (7, 11), (6, 10)], fill=ICE)
    p.line([(11, 5), (13, 5), (14, 7)], fill=STEEL, width=2)
    save(im, name)


def small(name):
    im, p = canvas()
    if name == 'bolt_pouch':
        p.polygon([(4, 5), (10, 5), (12, 12), (9, 14), (3, 13), (2, 10)], fill=WOOD)
        p.line((4, 5, 10, 5), fill=CLOTH)
        for x, y in [(5, 8), (8, 9), (10, 7)]:
            p.line((x, y, x-1, y+3), fill=STEEL)
            p.line((x-1, y, x+1, y), fill=LIGHT)
    elif name in ('glow_slag', 'slag_cold'):
        p.polygon([(3, 11), (2, 8), (5, 4), (9, 3), (12, 7), (13, 11), (8, 13)], fill=DARK)
        p.line([(4, 8), (6, 5), (8, 5)], fill=STEEL)
        p.line([(9, 4), (8, 8), (11, 10), (9, 12)], fill=FIRE if name == 'glow_slag' else IRON, width=2)
        if name == 'glow_slag':
            p.point((8, 8), fill=GOLD)
    elif name == 'tar_flask':
        p.rectangle((6, 2, 9, 4), fill=WOOD)
        p.polygon([(5, 5), (10, 5), (12, 9), (11, 13), (4, 13), (3, 9)], fill=IRON)
        p.rectangle((5, 7, 10, 12), fill=INK)
        p.line((4, 7, 4, 10), fill=STEEL)
        p.line((6, 5, 9, 5), fill=BRASS)
    elif name == 'tar_gob':
        p.polygon([(3, 6), (6, 4), (9, 6), (12, 10), (10, 12), (5, 11), (3, 9)], fill=INK)
        p.line((5, 6, 7, 6), fill=WOOD)
        p.point((12, 13), fill=DARK)
    elif name == 'debris_bent_nail':
        p.line([(4, 5), (8, 8), (11, 8)], fill=STEEL)
        p.line((3, 6, 5, 4), fill=LIGHT)
    elif name == 'debris_chain_link':
        p.rectangle((5, 4, 9, 9), outline=STEEL)
        p.line((6, 10, 8, 10), fill=IRON)
    elif name == 'foam_cover':
        p.polygon([(0, 6), (3, 5), (3, 2), (7, 3), (10, 1), (12, 5), (15, 6), (13, 10), (15, 13), (9, 14), (5, 13), (1, 14), (2, 10)], fill='#aab4a3')
        for box in [(3, 6, 6, 9), (8, 4, 11, 7), (8, 10, 12, 12)]:
            p.arc(box, 180, 300, fill=LIGHT)
    save(im, name)


def footwear(name, pair):
    im, p = canvas()
    for x, y in ([(2, 3), (9, 5)] if pair else [(4, 4)]):
        p.rectangle((x, y, x+3, y+6), fill=WOOD if pair else IRON)
        p.rectangle((x, y+5, x+5, y+8), fill=INK if pair else STEEL)
        p.line((x, y, x+3, y), fill=BRASS)
        p.line((x, y+8, x+5, y+8), fill=IRON)
        if not pair:
            p.line((x+3, y+5, x+5, y+5), fill=LIGHT)
    save(im, name)


def lunch(name, empty):
    im, p = canvas()
    if empty:
        p.rectangle((3, 2, 12, 5), fill=IRON)
        p.rectangle((4, 3, 11, 4), fill=STEEL)
    else:
        handle(p, (6, 2, 9, 4))
    p.rectangle((2, 6, 13, 12), fill=TEAL)
    p.line((2, 6, 13, 6), fill=STEEL)
    if empty:
        p.rectangle((3, 7, 12, 10), fill=INK)
        p.line((4, 10, 11, 10), fill=IRON)
    else:
        p.line((3, 9, 12, 9), fill=IRON)
        p.rectangle((7, 8, 8, 10), fill=BRASS)
    save(im, name)


for name in ('arc_torch', 'pocket_drill', 'pocket_pump', 'steam_lance', 'lance_hot',
             'rail_switch_key', 'mold_key', 'nozzle_elbow', 'horseshoe_magnet',
             'tension_spring', 'chain_hook', 'nail_board', 'hand_bellows'):
    tool(name)
can('coolant_can', False)
can('emergency_foam', True)
for name in ('bolt_pouch', 'glow_slag', 'slag_cold', 'tar_flask', 'tar_gob',
             'debris_bent_nail', 'debris_chain_link', 'foam_cover'):
    small(name)
footwear('insulated_boots', True)
footwear('steel_toe', False)
lunch('lunch_tin', False)
lunch('lunch_empty', True)
