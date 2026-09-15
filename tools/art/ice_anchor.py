"""Small steel ice screw and rope reel, with quiet ground placement silhouette."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for placed in [False,True]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 d.ellipse((2,6,10,13),fill='#333b3a');d.arc((2,5,11,13),0,320,fill='#a08b66',width=2)
 d.arc((4,7,9,11),10,310,fill='#796a52',width=1)
 if placed:
  d.polygon([(8,7),(13,6),(15,9),(10,11),(7,10)],fill='#485354')
  d.line((10,3,10,10),fill='#c0d3cf',width=2);d.line((7,3,13,3),fill='#7f9f9d',width=2)
  d.line((9,12,13,11),fill='#263a3b')
 else:
  d.line((10,2,13,11),fill='#afc9c8',width=2)
  d.line((8,3,13,1),fill='#d1d9c9',width=2)
  d.line((10,7,13,6),fill='#526d70');d.line((11,10,14,9),fill='#526d70')
 im.save(root/('anchor_point.png' if placed else 'ice_anchor.png'))
