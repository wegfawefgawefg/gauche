"""Folded salted kelp and wet scraps, with broad shapes and sparse salt crystals."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
im = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(im)
p.polygon([(3,3),(7,2),(10,4),(13,3),(12,8),(14,11),(9,13),(5,12),(2,14),(3,9),(1,7)], fill='#425b44')
p.polygon([(5,3),(7,3),(8,7),(6,11),(4,12),(5,8)], fill='#788961')
p.polygon([(10,5),(12,4),(10,8),(12,11),(9,12),(8,9)], fill='#9a9b70')
p.line([(4,9),(10,7)], fill='#bba887')
for point in [(3,5),(6,4),(10,10)]:
    p.point(point, fill='#d1ceb3')
im.save(root / 'salted_kelp.png')
im = Image.new('RGBA', (16,16))
p = ImageDraw.Draw(im)
p.polygon([(6,5),(8,4),(7,7),(10,9),(8,11),(7,9),(5,8)], fill='#657650')
p.line([(6,6),(7,7),(6,8)], fill='#a4a376')
im.save(root / 'debris_kelp_scrap.png')
