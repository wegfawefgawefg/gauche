"""Small industrial wildlife, with shared anatomy across readable poses."""
from underworks_palette import *


def rat(name, pose):
    im, p = canvas()
    p.line([(4, 11), (1, 10), (1, 7), (3, 6)], fill=RUST)
    y = 7 if pose == 'dash' else 6
    p.polygon([(4, y), (9, y-1), (12, y+2), (12, 11), (4, 11)], fill=WOOD)
    if pose == 'inflate':
        p.ellipse((3, 3, 12, 13), fill=WOOD)
        p.arc((4, 4, 11, 12), 120, 240, fill=CLOTH)
        p.rectangle((6, 5, 9, 9), fill=RUST)
        p.line((6, 6, 9, 6), fill=GOLD)
    else:
        p.rectangle((5, y, 8, y+3), fill=RUST)
        p.line((5, y, 8, y), fill=BRASS)
    p.polygon([(10, 6), (12, 6), (15, 9), (14, 10), (10, 10)], fill=CLOTH)
    p.rectangle((10, 4, 11, 6), fill=RUST)
    p.point((13, 8), fill=INK)
    p.point((15, 9), fill=INK)
    p.line((5, 12, 4 if pose == 'dash' else 6, 13), fill=WOOD)
    p.line((10, 12, 12 if pose == 'dash' else 11, 13), fill=WOOD)
    save(im, name)


def crawler(name, charge):
    im, p = canvas()
    for x in (3, 6, 9, 12):
        p.line([(x, 7), (x-1, 4), (x+1, 3)], fill=STEEL)
        p.line([(x, 10), (x+1, 12), (x-1, 13)], fill=IRON)
    p.line([(1, 9), (4, 8), (7, 9), (10, 8), (13, 9)], fill=DARK, width=4)
    for x in (3, 6, 9):
        p.line((x, 7, x, 10), fill=GOLD if charge else RUST)
    p.rectangle((12, 7, 14, 10), fill=IRON)
    p.point((14, 7), fill=ICE if charge else LIGHT)
    if charge:
        p.line([(6, 6), (8, 4), (7, 3)], fill=ICE)
    save(im, name)


def moth(name, hot):
    im, p = canvas()
    for flip in (False, True):
        wing, q = canvas()
        q.polygon([(7, 6), (4, 2), (1, 3), (1, 7), (4, 9), (2, 11), (5, 13), (7, 10)], fill=RUST if hot else WOOD)
        q.line([(2, 4), (4, 5), (6, 8)], fill=FIRE if hot else BRASS)
        q.point((3, 6), fill=GOLD)
        q.line((4, 11, 5, 11), fill=FIRE if hot else CLOTH)
        if flip:
            wing = wing.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
        im.alpha_composite(wing)
    p = ImageDraw.Draw(im)
    p.line((7, 5, 7, 12), fill=INK, width=2)
    p.line((7, 6, 7, 9), fill=GOLD if hot else BRASS)
    p.line([(7, 4), (5, 2)], fill=STEEL)
    p.line([(8, 4), (10, 2)], fill=STEEL)
    save(im, name)


def snail(name, state):
    im, p = canvas()
    p.polygon([(1, 12), (3, 10), (11, 10), (14, 8), (14, 12), (12, 14), (2, 14)], fill=IRON)
    p.polygon([(2, 9), (3, 5), (6, 3), (10, 4), (12, 8), (11, 12), (3, 12)], fill=DARK)
    color = ICE if state == 'cold' else STEEL
    p.line([(3, 10), (4, 6), (7, 5), (9, 7), (9, 10), (6, 10), (6, 8)], fill=color)
    if state != 'tucked':
        p.line((13, 9, 13, 6), fill=IRON)
        p.point((13, 5), fill=LIGHT)
        p.point((15, 7), fill=LIGHT)
    if state == 'hot':
        p.line([(10, 5), (11, 7), (10, 9)], fill=FIRE)
        p.line((4, 13, 9, 13), fill=RUST)
    save(im, name)


def ash(name, awake):
    im, p = canvas()
    if awake:
        p.polygon([(7, 2), (12, 5), (10, 7), (13, 9), (9, 11), (10, 13), (5, 15), (6, 12), (2, 9), (4, 7), (3, 5)], fill=IRON)
        p.line([(5, 5), (9, 4), (10, 5)], fill=STEEL)
        p.line([(4, 9), (9, 8), (11, 9)], fill=STEEL)
        p.line((6, 12, 8, 12), fill=STEEL)
        p.point((6, 6), fill=GOLD)
        p.point((9, 6), fill=GOLD)
    else:
        p.polygon([(1, 12), (5, 9), (6, 6), (9, 7), (12, 11), (15, 13), (2, 14)], fill=IRON)
        p.line([(3, 12), (7, 11), (9, 12)], fill=STEEL)
        p.line((6, 8, 8, 8), fill=STEEL)
        p.point((11, 12), fill=DARK)
    save(im, name)


def yeti():
    im, p = canvas()
    # Broad hanging arms and small dark face; no detailed high-res fur.
    p.polygon([(5, 3), (10, 3), (12, 5), (14, 6), (15, 12), (12, 12), (11, 10),
               (11, 14), (8, 14), (8, 12), (6, 12), (6, 14), (3, 14), (4, 10), (2, 12), (0, 11), (1, 6), (4, 5)], fill='#b9c8bd')
    p.line([(2, 10), (3, 6), (5, 5)], fill='#e0e0c9')
    p.rectangle((5, 4, 10, 7), fill=IRON)
    p.point((6, 5), fill=LIGHT)
    p.point((9, 5), fill=LIGHT)
    p.line((6, 7, 9, 7), fill=WOOD)
    p.line([(4, 3), (3, 1), (5, 2)], fill=CLOTH)
    p.line([(11, 3), (12, 1), (10, 2)], fill=CLOTH)
    p.line((5, 9, 9, 9), fill=ICE)
    p.line((3, 15, 6, 15), fill=IRON)
    p.line((9, 15, 12, 15), fill=IRON)
    save(im, 'yeti')


for name, pose in [('pressure_rat', 'idle'), ('rat_inflate', 'inflate'), ('rat_dash', 'dash')]:
    rat(name, pose)
crawler('cable_crawler', False)
crawler('crawler_charge', True)
moth('furnace_moth', False)
moth('furnace_moth_hot', True)
for name, state in [('slag_snail', 'hot'), ('slag_snail_tucked', 'tucked'), ('slag_snail_cooled', 'cold')]:
    snail(name, state)
ash('ash_mound', False)
ash('ash_whirl', True)
yeti()
