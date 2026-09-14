"""Brass diving helmet and dark canvas suit, with visible depth and attack poses."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
INK, BRASS, RIM, SUIT = '#253036', '#967647', '#c3a879', '#456069'


def diver(name, phase='idle'):
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    shift = 4 if phase in ('rise', 'sink', 'bell') else 0
    if phase not in ('sink', 'bell'):
        p.rectangle((4, 8+shift, 11, 13+shift), fill=INK)
        p.rectangle((5, 8+shift, 10, 12+shift), fill=SUIT)
        p.line([(5, 8+shift), (10, 8+shift)], fill=BRASS)
        p.line([(5, 13+shift), (3, 14+shift)], fill=INK, width=2)
        p.line([(10, 13+shift), (12, 14+shift)], fill=INK, width=2)
    p.ellipse((3, 1+shift, 12, 9+shift), fill=INK)
    p.ellipse((4, 2+shift, 11, 8+shift), fill=BRASS)
    p.line([(6, 2+shift), (9, 2+shift)], fill=RIM)
    p.rectangle((6, 4+shift, 10, 6+shift), fill=INK)
    p.line([(7, 4+shift), (9, 4+shift)], fill='#7ca6aa')
    p.point((5, 6+shift), fill=RIM)
    if phase == 'swing':
        p.line([(10, 9), (14, 5), (14, 2)], fill=SUIT, width=2)
        p.rectangle((12, 0, 15, 3), fill=BRASS)
        p.line([(12, 0), (15, 0)], fill=RIM)
    elif phase == 'idle':
        p.line([(11, 9), (13, 10)], fill=SUIT, width=2)
        p.rectangle((12, 11, 14, 13), fill=BRASS)
    if phase in ('rise', 'sink', 'bell'):
        p.line([(2, 13), (5, 14), (10, 14), (13, 13)], fill='#69939c')
    if phase == 'bell':
        p.line([(1, 6), (0, 8), (1, 10)], fill=RIM)
        p.line([(14, 6), (15, 8), (14, 10)], fill=RIM)
    image.save(ROOT / f'{name}.png')


if __name__ == '__main__':
    for name, phase in [('bell_diver', 'idle'), ('diver_bell', 'bell'),
                        ('diver_rise', 'rise'), ('diver_swing', 'swing'), ('diver_sink', 'sink')]:
        diver(name, phase)
    image = Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    p.ellipse((4, 8, 11, 11), outline='#537c8780')
    for box in [(5, 4, 7, 6), (9, 7, 11, 9), (8, 1, 9, 2)]:
        p.ellipse(box, outline='#9bbbc5cc')
    image.save(ROOT / 'diver_bubbles.png')
    image = Image.new('RGBA', (16, 16), '#405960')
    p = ImageDraw.Draw(image)
    p.polygon([(4, 3), (10, 2), (13, 5), (13, 10), (9, 13), (4, 12), (2, 9), (2, 5)], fill='#55747d')
    p.polygon([(4, 4), (10, 3), (12, 6), (12, 10), (8, 12), (4, 11), (3, 8)], fill='#17282f')
    p.line([(4, 3), (9, 2)], fill='#8ea5ac')
    p.line([(8, 12), (11, 11)], fill='#42666e')
    image.save(ROOT / 'ice_hole.png')
