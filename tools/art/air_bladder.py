"""A tied hide bladder and its small inflated cargo sling."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
INK, HIDE, EDGE, CORD = '#343a37', '#8c8964', '#b9b18a', '#677a79'
image = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(image)
p.polygon([(6, 2), (10, 2), (10, 5), (13, 8), (12, 13), (9, 15), (4, 14), (2, 10), (3, 6), (6, 4)], fill=INK)
p.polygon([(6, 5), (9, 5), (11, 8), (10, 12), (7, 13), (4, 11), (4, 8)], fill=HIDE)
p.line([(5, 8), (6, 6), (8, 6)], fill=EDGE)
p.line([(6, 3), (10, 3)], fill=CORD, width=2)
p.point((10, 10), fill=EDGE)
image.save(ROOT / 'air_bladder.png')
image = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(image)
p.ellipse((0, 4, 15, 12), fill=INK)
p.ellipse((1, 5, 14, 11), fill=HIDE)
p.ellipse((4, 6, 11, 9), fill=(0, 0, 0, 0))
p.line([(2, 6), (4, 5)], fill=EDGE)
p.line([(11, 10), (13, 9)], fill=EDGE)
p.line([(5, 4), (5, 12)], fill=CORD)
p.line([(10, 4), (10, 12)], fill=CORD)
image.save(ROOT / 'item_float.png')
