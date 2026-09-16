"""Single-tile floating supports: notched lily leaf and a bark-covered drift log."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets/graphics'
leaf = Image.new('RGBA', (16, 16))
d = ImageDraw.Draw(leaf)
d.line([(1,12),(4,14),(11,14),(14,12)], fill='#497779')
d.polygon([(2,4),(5,2),(11,2),(14,5),(14,10),(11,13),(4,13),(1,10),(1,6)], fill='#263e25')
d.polygon([(3,4),(6,3),(11,3),(13,6),(13,10),(10,12),(4,11),(2,8)], fill='#69824a')
d.polygon([(4,4),(10,3),(12,5),(8,6),(4,9),(2,7)], fill='#8c9b59')
d.polygon([(8,8),(14,9),(14,12)], fill=(0,0,0,0))
d.line([(3,5),(8,8),(5,11)], fill='#465d34')
d.line([(10,4),(8,8)], fill='#465d34')
leaf.save(root / 'river_lily.png')

log = Image.new('RGBA', (16, 16))
d = ImageDraw.Draw(log)
d.line([(0,11),(3,13),(12,13),(15,11)], fill='#497779')
d.rectangle((2,4,13,11), fill='#302a20')
d.rectangle((3,5,12,10), fill='#735331')
d.line([(4,5),(8,5),(9,6),(12,6)], fill='#9b7545')
d.line([(3,8),(7,8),(8,9),(12,9)], fill='#483b27')
for x in (1,12):
    d.ellipse((x,4,x+2,11), fill='#4a3826')
    d.line((x+1,5,x+1,10), fill='#b49459')
d.line([(6,5),(7,2),(9,2)], fill='#51432a', width=2)
log.save(root / 'river_log.png')
