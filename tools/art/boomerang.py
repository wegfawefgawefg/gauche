"""A bent hardwood returning tool, with a pale grip; transparent 16px sprite."""
from pathlib import Path
from PIL import Image, ImageDraw
image = Image.new('RGBA', (16, 16))
draw = ImageDraw.Draw(image)
draw.polygon([(2,2),(5,2),(13,7),(14,9),(13,11),(5,14),(2,14),(3,11),(9,8),(3,5)], fill='#795431')
draw.line([(3,3),(5,3),(12,8),(12,9),(4,13),(3,13)], fill='#b39359', width=2)
draw.line([(8,6),(7,7)], fill='#d8c991', width=2)
draw.line([(8,10),(9,11)], fill='#d8c991', width=2)
image.save(Path(__file__).resolve().parents[2] / 'assets/graphics/boomerang.png')
