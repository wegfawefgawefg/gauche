"""Sparse straw-and-cloth ward, kept readable as a small 16px silhouette."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT = Path(__file__).resolve().parents[2] / 'assets/graphics'
for planted in [False, True]:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    if planted:
        d.line((8, 7, 8, 15), fill='#7c694d', width=2)
        d.line((1, 7, 14, 7), fill='#a58b58')
        d.polygon([(5, 6), (10, 6), (12, 11), (9, 10), (8, 12), (5, 11), (4, 9)], fill='#686b53')
        d.line((7, 7, 7, 10), fill='#929274')
        d.point((3, 8), fill='#c2ad73'); d.point((12, 8), fill='#c2ad73')
        d.rectangle((6, 3, 10, 5), fill='#b39c6d')
        d.point((7, 4), fill='#4a4b3b'); d.point((9, 4), fill='#4a4b3b')
        d.rectangle((6, 1, 10, 2), fill='#766145'); d.line((4, 3, 12, 3), fill='#8f7952')
        d.line((6, 12, 5, 13), fill='#b49b60'); d.line((10, 11, 11, 13), fill='#b49b60')
    else:
        d.line((2, 4, 13, 13), fill='#8b7550', width=2)
        d.polygon([(3, 4), (10, 4), (13, 9), (11, 12), (6, 11), (4, 8)], fill='#767961')
        d.line((6, 5, 11, 10), fill='#9b9975')
        d.line((5, 8, 10, 6), fill='#c2ae77')
        d.rectangle((2, 2, 7, 4), fill='#a18a5b')
        d.line((1, 5, 8, 5), fill='#796448')
    im.save(OUT / ('scarecrow.png' if planted else 'scarecrow_bundle.png'))

# Loose straw has its own pale dry color and the light-material wind response.
im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.line([(4, 11), (7, 7), (9, 3)], fill='#b8a166')
d.line((6, 12, 10, 6), fill='#89784d')
d.point((8, 5), fill='#d1bd83')
im.save(OUT / 'debris_straw.png')

# A tied sack figure differs from the broad-armed, hat-wearing scarecrow.
for standing in [False, True]:
    im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
    if standing:
        d.rectangle((5, 1, 10, 5), fill='#c7b787')
        d.line((6, 1, 9, 1), fill='#e0cca0')
        d.point((6, 3), fill='#554e3b'); d.point((9, 3), fill='#554e3b')
        d.polygon([(5, 6), (10, 6), (12, 10), (10, 11), (5, 11), (3, 10)], fill='#ab9566')
        d.line((5, 7, 10, 7), fill='#a55240')
        d.line((6, 8, 6, 10), fill='#d2bd89')
        d.rectangle((5, 11, 6, 14), fill='#8d794d')
        d.rectangle((9, 11, 10, 14), fill='#8d794d')
        d.point((4, 14), fill='#baa371'); d.point((11, 14), fill='#baa371')
    else:
        d.ellipse((3, 3, 12, 12), fill='#a28c5d')
        d.line((5, 4, 9, 4), fill='#d4c08d')
        d.line((6, 6, 6, 11), fill='#c4ad79')
        d.line((3, 8, 12, 8), fill='#9d5341')
        d.line((6, 2, 9, 2), fill='#c6b17b')
        d.line((5, 13, 10, 13), fill='#8e7a50')
    im.save(OUT / ('straw_decoy.png' if standing else 'straw_decoy_bundle.png'))
