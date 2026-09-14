"""Simple forest food icons: quiet shell, bright cooked yolk; no texture noise."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
im = Image.new('RGBA', (16,16)); p = ImageDraw.Draw(im)
p.polygon([(6,2),(9,2),(12,7),(12,11),(10,13),(5,13),(3,11),(3,7)], fill='#918768')
p.polygon([(6,2),(8,2),(10,6),(10,10),(8,12),(5,11),(4,9),(4,6)], fill='#ddd9ae')
p.line((6,4,5,7), fill='#f0ebce', width=1)
im.save(root/'egg.png')
im = Image.new('RGBA', (16,16)); p = ImageDraw.Draw(im)
p.polygon([(5,3),(10,3),(13,6),(14,10),(11,13),(4,13),(1,10),(2,6)], fill='#aaa47c')
p.polygon([(5,3),(10,4),(12,6),(12,10),(9,12),(4,11),(2,9),(3,5)], fill='#e6e0bc')
p.ellipse((5,5,10,10), fill='#d98c34'); p.rectangle((6,5,9,8), fill='#f2bf55')
im.save(root/'fried_egg.png')
