"""A carved bone whistle and the small call mark shown above a distracted wolf."""
from pathlib import Path
from PIL import Image, ImageDraw
out = Path(__file__).resolve().parents[2] / 'assets/graphics'
im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.line([(3, 9), (2, 12), (5, 14), (8, 13)], fill='#7a6042')
d.polygon([(3, 6), (12, 6), (14, 7), (14, 10), (11, 11), (3, 10)], fill='#b3ac85')
d.line((4, 6, 12, 6), fill='#e0d9af')
d.line((4, 10, 11, 10), fill='#716e54')
d.rectangle((6, 7, 7, 8), fill='#393f35')
d.rectangle((10, 7, 11, 8), fill='#393f35')
d.rectangle((1, 7, 3, 9), fill='#c5bc94')
im.save(out / 'wolf_whistle.png')
im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.line([(6, 10), (6, 3), (11, 2), (11, 9)], fill='#d7d8a7', width=2)
d.rectangle((3, 9, 6, 11), fill='#d7d8a7')
d.rectangle((8, 8, 11, 10), fill='#b3c394')
im.save(out / 'wolf_called.png')
