"""Readable tiny arrow, drawn bow and lit bomb with deliberate empty space."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.line((2, 8, 12, 8), fill='#bb9963')
p.polygon([(11, 5), (15, 8), (11, 10)], fill='#b1b9ad')
p.line((1, 5, 4, 8, 1, 10), fill='#d3cbb0', width=2)
im.save(ROOT / 'arrow.png')
im = Image.open(ROOT / 'bomb.png').convert('RGBA'); p = ImageDraw.Draw(im)
p.line((8, 5, 10, 2, 12, 3), fill='#a07c4b')
p.point((12, 2), fill='#ffd67b'); p.point((13, 3), fill='#e77d36')
im.save(ROOT / 'bomb_lit.png')
im = Image.new('RGBA', (16, 16)); p = ImageDraw.Draw(im)
p.line((5, 1, 10, 3, 12, 7, 10, 12, 5, 14), fill='#a97e49', width=2)
p.line((5, 1, 2, 8, 5, 14), fill='#cfbc8e')
p.line((2, 8, 13, 8), fill='#bba16b'); p.polygon([(12, 6), (15, 8), (12, 9)], fill='#bac3b9')
im.save(ROOT / 'bow_drawn.png')
