"""Upright parallax trees. Optional --previews writes fully filled canopy studies."""
from pathlib import Path
import argparse
import random
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'

def tree(name):
    im = Image.new('RGBA', (48, 64))
    p = ImageDraw.Draw(im)
    # Branches stay exposed below/between crowns, with a warm bark highlight.
    p.polygon([(20,63),(21,41),(17,27),(23,21),(27,41),(28,63)], fill='#372719')
    p.line((24,62,23,36,22,20), fill='#71502e', width=3)
    p.line((23,47,10,34,6,22), fill='#513920', width=4)
    p.line((25,42,37,29,41,14), fill='#68482a', width=3)
    if name == 'oak':
        boxes = [(12,2,33,19), (3,12,23,31), (27,9,45,28),
                 (17,16,36,33), (0,27,17,42), (30,29,47,43), (10,23,27,37)]
        for n, box in enumerate(boxes):
            x,y,r,b = box
            p.ellipse(box, fill=['#304c28','#385a2d','#2b4727'][n%3])
            p.arc((x+2,y+2,r-2,b-3), 195, 295, fill='#4a6b33', width=2)
            p.line((x+5,b-2,r-4,b-2), fill='#263e22', width=2)
        p.line((23,49,18,37,14,34), fill='#785333', width=2)
        p.line((24,44,30,37,34,35), fill='#5e4228', width=2)
    else:
        for y, radius in [(12,7),(23,11),(35,16),(47,21)]:
            p.polygon([(24,y-11),(24-radius,y+4),(17,y+3),(24,y+7),
                       (31,y+3),(24+radius,y+4)], fill='#274637')
            p.line((24,y-9,19,y-1,23,y-2), fill='#456344', width=2)
            p.line((24-radius+4,y+4,24,y+6,24+radius-4,y+4), fill='#203b2d')
            p.line((23,y+7,28,y+8), fill='#63492e', width=2)
    return im


def preview(folder, trees):
    folder.mkdir(parents=True, exist_ok=True)
    # All three studies fill the frame. The game applies its opening separately.
    for index, name in enumerate(['mixed-crowns','oak-branches','pine-grove']):
        rng = random.Random(832+index)
        im = Image.new('RGBA',(320,180),'#101b16')
        placements=[]
        for y in range(-42,230,30):
            for x in range(-32,365,29):
                species=rng.choice(['oak','pine']) if index==0 else ('oak' if index==1 else 'pine')
                size=rng.randrange(42,68)
                sprite=trees[species].resize((size,round(size*4/3)),Image.Resampling.NEAREST)
                if rng.randrange(2): sprite=sprite.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
                placements.append((y+rng.randrange(-9,10), x+rng.randrange(-10,11), sprite))
        for y,x,sprite in sorted(placements,key=lambda p:p[0]): im.alpha_composite(sprite,(x,y-sprite.height))
        im.resize((1280,720),Image.Resampling.NEAREST).save(folder/f'{name}.png')

if __name__ == '__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--previews',type=Path)
    args=parser.parse_args()
    trees={name:tree(name) for name in ['oak','pine']}
    for name,im in trees.items(): im.save(ROOT/f'canopy_{name}.png')
    if args.previews: preview(args.previews,trees)
