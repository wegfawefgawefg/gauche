"""A small iron guard with a caged amber lamp; transparent 16px silhouette."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[2] / 'assets/graphics'
im = Image.new('RGBA', (16, 16))
d = ImageDraw.Draw(im)
# Shield face: flat bevels, no noisy texture; handle points toward the wielder.
d.rectangle((1, 7, 5, 8), fill='#76614d')
d.polygon([(6, 2), (11, 2), (14, 5), (14, 10), (11, 13), (6, 13), (4, 10), (4, 5)], fill='#434c4b')
d.line([(6, 2), (11, 2), (14, 5)], fill='#98a18d')
d.line([(14, 6), (14, 10), (11, 13), (6, 13)], fill='#303736')
d.rectangle((7, 5, 11, 10), fill='#b87532')
d.rectangle((8, 6, 10, 9), fill='#efc471')
d.point((9, 7), fill='#fff0b0')
d.line((6, 4, 12, 4), fill='#827951')
d.line((6, 11, 12, 11), fill='#827951')
d.line((9, 5, 9, 10), fill='#736143')
im.save(OUT / 'shield_lantern.png')
