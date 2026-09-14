"""Folded cave bat, puffed throat tell, and broad flight wings; sparse cold colors."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
INK, WING, FUR, LIGHT = '#253743', '#567580', '#8fa9af', '#d6e7df'


def bat(name, flying=False, inhale=False):
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    if flying:
        for points in [[(6, 7), (1, 2), (0, 9), (3, 8), (4, 11), (7, 9)],
                       [(9, 7), (14, 2), (15, 9), (12, 8), (11, 11), (8, 9)]]:
            p.polygon(points, fill=INK)
        p.polygon([(5, 7), (2, 4), (2, 7), (5, 9)], fill=WING)
        p.polygon([(10, 7), (13, 4), (13, 7), (10, 9)], fill=WING)
    else:
        p.polygon([(5, 4), (3, 11), (6, 14), (8, 10), (9, 14), (12, 11), (10, 4)], fill=INK)
        p.line([(5, 7), (4, 11), (6, 13)], fill=WING)
        p.line([(10, 7), (11, 11), (9, 13)], fill=WING)
    p.rectangle((6, 5, 9, 11), fill=FUR)
    p.polygon([(5, 5), (5, 1), (7, 4), (8, 4), (10, 1), (10, 6)], fill=INK)
    p.rectangle((6, 4, 9, 7), fill=FUR)
    p.point((6, 5), fill=LIGHT)
    p.point((9, 5), fill=LIGHT)
    p.rectangle((7, 6, 8, 7), fill=INK)
    if inhale:
        p.ellipse((5, 8, 10, 12), fill=LIGHT)
        p.rectangle((7, 7, 8, 9), fill=INK)
    image.save(ROOT / f'{name}.png')


if __name__ == '__main__':
    bat('frost_bat')
    bat('frost_bat_inhale', inhale=True)
    bat('frost_bat_flying', flying=True)
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    p.polygon([(2, 6), (7, 4), (10, 5), (13, 7), (12, 11), (8, 12), (3, 10), (5, 8)], fill='#789ca6b0')
    p.polygon([(6, 6), (10, 6), (12, 8), (10, 10), (7, 10), (8, 8)], fill='#cae0dfdd')
    for xy in [(1, 4), (3, 12), (14, 5), (14, 10)]:
        p.point(xy, fill=LIGHT)
    image.save(ROOT / 'frost_puff.png')
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    p.polygon([(1, 7), (5, 5), (15, 7), (5, 10)], fill='#5d8292')
    p.polygon([(2, 7), (6, 6), (14, 7), (5, 8)], fill='#c7e0df')
    p.line([(4, 9), (9, 8)], fill='#91b9c0')
    image.save(ROOT / 'ice_needle.png')
