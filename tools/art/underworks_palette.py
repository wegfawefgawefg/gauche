"""Shared muted metal/cloth colors for hand-drawn native-resolution sprites."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
INK = '#242e2a'
DARK = '#3e493f'
IRON = '#566559'
STEEL = '#8a9788'
LIGHT = '#c4c6a7'
BRASS = '#9b783d'
GOLD = '#d3b061'
WOOD = '#72604f'
RUST = '#986045'
FIRE = '#cf793b'
TEAL = '#628d82'
ICE = '#91b9b6'
CLOTH = '#a9a184'


def canvas(size=(16, 16)):
    im = Image.new('RGBA', size)
    return im, ImageDraw.Draw(im)


def save(im, name):
    im.save(ROOT / (name + '.png'))


def bolts(p, points):
    for x, y in points:
        p.point((x, y), fill=LIGHT)


def boots(p):
    p.rectangle((4, 12, 6, 14), fill=WOOD)
    p.rectangle((9, 12, 11, 14), fill=WOOD)
    p.line((3, 15, 6, 15), fill=INK)
    p.line((9, 15, 12, 15), fill=INK)


def helmet(p):
    p.rectangle((5, 2, 10, 4), fill=BRASS)
    p.line((4, 4, 11, 4), fill=GOLD)
    p.line((7, 1, 8, 1), fill=GOLD)


def handle(p, box):
    p.rectangle(box, outline=STEEL)
