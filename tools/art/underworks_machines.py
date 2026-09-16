"""Native-pixel machinery: winches, clamps, freight and a tall anchored lamp."""
from underworks_palette import *


def hoist(name, magnet):
    im, p = canvas()
    # An anchored steel winch, not a humanoid carrying a mysterious crate.
    p.rectangle((2, 11, 13, 14), fill=IRON)
    p.line((2, 11, 13, 11), fill=STEEL)
    p.rectangle((3, 3, 5, 11), fill=BRASS)
    p.rectangle((10, 3, 12, 11), fill=BRASS)
    p.line((3, 2, 12, 2), fill=GOLD, width=2)
    p.rectangle((5, 5, 10, 9), fill=INK)
    for x in (6, 8, 10):
        p.line((x, 5, x, 9), fill=STEEL)
    p.line((1, 7, 4, 7), fill=STEEL)
    p.point((1, 8), fill=GOLD)
    bolts(p, [(3, 12), (12, 12)])
    if magnet:
        p.rectangle((6, 11, 9, 13), fill=RUST)
    else:
        p.line((6, 12, 9, 12), fill=GOLD)
    save(im, name)


def head():
    im, p = canvas()
    p.line((7, 1, 7, 4), fill=STEEL, width=2)
    p.rectangle((3, 4, 12, 7), fill=BRASS)
    p.line((3, 4, 12, 4), fill=GOLD)
    p.polygon([(3, 7), (5, 7), (5, 10), (7, 12), (5, 13), (2, 10)], fill=IRON)
    p.polygon([(10, 7), (12, 7), (13, 10), (10, 13), (8, 12), (10, 10)], fill=IRON)
    p.line((3, 8, 3, 10), fill=STEEL)
    p.line((12, 8, 12, 10), fill=STEEL)
    p.rectangle((6, 6, 9, 8), fill=RUST)
    save(im, 'crane_head')


def weight():
    im, p = canvas()
    p.rectangle((6, 1, 9, 4), outline=STEEL)
    p.rectangle((2, 4, 13, 13), fill=IRON)
    p.line((2, 4, 13, 4), fill=STEEL)
    p.line((2, 5, 2, 12), fill=STEEL)
    p.rectangle((3, 6, 12, 9), fill=INK)
    for x in (4, 8, 12):
        p.line((x, 6, x-2, 9), fill=BRASS, width=2)
    p.line((3, 13, 12, 13), fill=INK)
    bolts(p, [(4, 11), (11, 11)])
    save(im, 'weight_block')


def barricade(name, folded):
    im, p = canvas()
    p.line((3, 4, 3, 14), fill=IRON, width=2)
    p.line((12, 4, 12, 14), fill=IRON, width=2)
    p.rectangle((2, 5, 13, 11), fill=STEEL)
    p.rectangle((3, 6, 12, 10), fill=IRON)
    if folded:
        for x in (5, 8, 11):
            p.line((x, 6, x, 10), fill=INK)
    else:
        p.line((3, 7, 12, 7), fill=BRASS)
        for x in (5, 9):
            p.line((x, 6, x-2, 9), fill=INK)
    bolts(p, [(3, 5), (12, 5), (3, 11), (12, 11)])
    save(im, name)


def mold(name, opened):
    im, p = canvas()
    p.rectangle((1, 6, 14, 13), fill=IRON)
    p.rectangle((2, 7, 13, 12), fill=INK if opened else WOOD)
    p.rectangle((3, 2, 12, 5), fill=IRON)
    p.line((3, 2, 12, 2), fill=STEEL)
    if opened:
        p.line((5, 9, 10, 9), fill=RUST)
        p.rectangle((4, 8, 6, 10), outline=RUST)
    else:
        p.line((2, 8, 13, 8), fill=STEEL)
        p.rectangle((6, 6, 9, 11), fill=BRASS)
    bolts(p, [(2, 6), (13, 6), (2, 12), (13, 12)])
    save(im, name)


def pump():
    im, p = canvas()
    p.rectangle((2, 11, 13, 14), fill=IRON)
    p.ellipse((3, 4, 11, 12), fill=BRASS)
    p.ellipse((5, 6, 9, 10), fill=INK)
    p.line((7, 6, 7, 9), fill=STEEL)
    p.line((5, 8, 9, 8), fill=STEEL)
    p.line([(10, 7), (13, 7), (13, 4), (15, 4)], fill=STEEL, width=2)
    p.line([(3, 9), (1, 9), (1, 13)], fill=TEAL, width=2)
    p.rectangle((5, 1, 9, 3), outline=STEEL)
    save(im, 'emergency_pump')


def cart():
    im, p = canvas()
    for x in (2, 11):
        p.rectangle((x, 4, x+2, 6), fill=INK)
        p.rectangle((x, 12, x+2, 14), fill=INK)
        p.point((x+1, 13), fill=STEEL)
    p.rectangle((3, 3, 12, 12), fill=IRON)
    p.rectangle((4, 4, 11, 10), fill=WOOD)
    for y in (5, 8, 11):
        p.line((4, y, 11, y), fill=DARK)
    p.line((3, 3, 12, 3), fill=STEEL)
    p.line((3, 3, 3, 12), fill=STEEL)
    p.line((1, 8, 3, 8), fill=STEEL)
    p.line((12, 8, 14, 8), fill=STEEL)
    save(im, 'rail_cart')


def points():
    im, p = canvas()
    for x in (2, 6, 10, 14):
        p.line((x, 4, x, 12), fill=WOOD, width=2)
    for y in (5, 11):
        p.line((0, y, 15, y), fill=STEEL)
    p.line([(5, 11), (7, 8), (7, 0)], fill=BRASS)
    p.line([(10, 11), (12, 8), (12, 0)], fill=STEEL)
    p.rectangle((1, 0, 3, 2), fill=RUST)
    p.line((2, 2, 5, 5), fill=IRON)
    save(im, 'rail_points')


def cage():
    im, p = canvas()
    p.rectangle((2, 3, 13, 13), fill=DARK)
    p.rectangle((3, 4, 12, 12), fill=INK)
    p.line((4, 11, 10, 11), fill=BRASS)
    for x in (4, 7, 10):
        p.line((x, 4, x, 12), fill=STEEL)
    p.line((2, 3, 13, 3), fill=STEEL)
    p.line((2, 13, 13, 13), fill=IRON)
    p.rectangle((11, 7, 13, 9), fill=GOLD)
    p.point((12, 8), fill=INK)
    save(im, 'pay_cage')


def lamp():
    im, p = canvas((16, 48))
    p.rectangle((5, 43, 10, 46), fill=IRON)
    p.line((5, 43, 10, 43), fill=STEEL)
    p.line((7, 8, 7, 43), fill=IRON, width=2)
    p.line((7, 11, 7, 40), fill=STEEL)
    p.line([(7, 10), (7, 4), (11, 4)], fill=IRON, width=2)
    p.polygon([(9, 4), (13, 4), (15, 7), (8, 7)], fill=IRON)
    p.line((10, 7, 13, 7), fill=GOLD)
    p.line((11, 8, 12, 8), fill=LIGHT)
    p.rectangle((6, 30, 9, 34), fill=BRASS)
    p.point((7, 32), fill=INK)
    save(im, 'streetlamp')


def reactor():
    im, p = canvas((32, 32))
    p.rectangle((4, 25, 27, 30), fill=IRON)
    p.line((4, 26, 27, 26), fill=STEEL)
    p.rectangle((7, 7, 24, 25), fill=IRON)
    p.polygon([(7, 7), (10, 3), (21, 3), (24, 7)], fill=STEEL)
    p.rectangle((10, 8, 21, 23), fill=INK)
    for x in (12, 16, 20):
        p.line((x, 9, x, 22), fill='#74965e', width=2)
        p.line((x, 10, x, 18), fill='#b4c887')
    for y in (8, 23):
        p.line((9, y, 22, y), fill=BRASS, width=2)
    p.line([(7, 12), (3, 12), (3, 24), (7, 24)], fill=TEAL, width=2)
    p.line([(24, 11), (28, 11), (28, 21), (25, 21)], fill=RUST, width=2)
    p.rectangle((12, 26, 19, 29), fill=DARK)
    p.line((16, 28, 18, 25), fill=STEEL)
    p.rectangle((17, 24, 19, 25), fill=RUST)
    bolts(p, [(8, 9), (23, 9), (8, 23), (23, 23), (6, 28), (25, 28)])
    save(im, 'reactor_core')
    flame, q = canvas()
    q.polygon([(3, 14), (1, 10), (3, 7), (4, 10), (6, 6), (7, 1), (10, 5), (10, 9),
               (13, 5), (14, 10), (12, 14)], fill='#628054')
    q.polygon([(5, 13), (5, 10), (8, 5), (9, 11), (11, 9), (11, 13)], fill='#a2b56d')
    q.line([(7, 13), (8, 10), (9, 13)], fill='#d5cf8b', width=2)
    save(flame, 'reactor_flame')


hoist('counterweight', False)
hoist('magnet_crane', True)
head()
weight()
barricade('barricade_section', False)
barricade('folding_barricade', True)
mold('mold_open', True)
mold('mold_closed', False)
pump()
cart()
points()
cage()
lamp()
reactor()
