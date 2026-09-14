"""A small foil-wrapped chemical warmer; the torn orange core identifies its use."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
image = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(image)
p.polygon([(4, 1), (11, 1), (13, 3), (13, 12), (11, 15), (4, 15), (2, 12), (2, 4)], fill='#303b3b')
p.rectangle((4, 3, 11, 12), fill='#a1aba0')
p.line([(5, 3), (10, 3)], fill='#d2d5ba')
p.line([(4, 12), (10, 12)], fill='#717f78')
p.polygon([(4, 6), (7, 5), (11, 6), (10, 10), (5, 10)], fill='#9f542f')
p.line([(6, 7), (9, 7), (8, 9)], fill='#e7b06a')
p.point((6, 13), fill='#b1b6a3')
image.save(ROOT / 'heat_capsule.png')
