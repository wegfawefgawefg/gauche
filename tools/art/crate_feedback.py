"""Damage stages for the original 16px crate; keep its palette and silhouette."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
base = Image.open(ROOT / 'crate.png').convert('RGBA')
for name in ('bruised', 'splintered', 'broken'):
    image = base.copy() if name != 'broken' else Image.new('RGBA', (16, 16))
    p = ImageDraw.Draw(image)
    dark, edge, wood = '#514632', '#b29966', '#886d48'
    if name != 'broken':
        p.line([(10, 5), (8, 7), (9, 9), (7, 10), (8, 12)], fill=dark)
        p.point((10, 9), fill=edge)
        p.line((3, 11, 5, 11), fill=dark)
    if name == 'splintered':
        p.polygon([(8, 6), (11, 7), (10, 10), (7, 12), (6, 10)], fill='#332e26')
        p.line([(7, 6), (8, 8), (6, 11)], fill=edge)
        p.line((10, 11, 12, 12), fill=edge)
        p.line((13, 7, 13, 10), fill=(0, 0, 0, 0))
        p.point((12, 9), fill=edge)
    if name == 'broken':
        p.polygon([(3, 11), (10, 10), (14, 13), (10, 15), (2, 14)], fill=dark)
        p.line([(2, 11), (6, 13), (10, 12)], fill=wood, width=2)
        p.line((4, 10, 10, 14), fill=edge)
        p.line((8, 10, 13, 12), fill=wood, width=2)
        p.point((12, 11), fill=edge)
        p.line((2, 14, 5, 14), fill=edge)
        p.line((7, 14, 8, 13), fill='#878876')
    image.save(ROOT / f'crate_{name}.png')
