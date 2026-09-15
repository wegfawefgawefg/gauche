"""Small slate pebble with a cut sound groove, using broad quiet shapes."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name,recorded in [('echo_pebble',False),('echo_pebble_ready',True)]:
    im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
    d.polygon([(3,7),(5,4),(10,3),(13,6),(12,11),(8,13),(3,11)],fill='#536d71')
    d.polygon([(4,6),(6,4),(10,4),(12,6),(8,7)],fill='#a2b2a5')
    d.line([(4,10),(8,12),(11,10)],fill='#2f484e')
    d.line([(7,6),(9,7),(9,9),(7,10)],fill='#d6b16b' if recorded else '#273f48',width=1)
    if recorded:d.point((6,8),fill='#f0d39a')
    im.save(root/(name+'.png'))
