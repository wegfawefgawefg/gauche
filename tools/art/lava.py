"""Native pixel flow across an eight-tile pool patch, with wraparound edges."""
from pathlib import Path
import math
import random
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"
rng = random.Random(632)
islands = [(rng.randrange(128), rng.randrange(128), rng.randrange(3, 11),
            rng.randrange(2, 6), rng.random()*math.tau) for _ in range(56)]
for frame in range(8):
    image = Image.new("RGBA", (128, 128), "#963c20")
    pen = ImageDraw.Draw(image)
    for x, y, w, h, phase in islands:
        drift = round(2*math.sin(frame*math.tau/8+phase))
        swell = round(math.cos(frame*math.tau/8+phase))
        for ox in (-128, 0, 128):
            for oy in (-128, 0, 128):
                px, py = x+drift+ox, y+swell+oy
                pen.polygon([(px,py+1), (px+2,py), (px+w,py+1),
                             (px+w-1,py+h), (px+1,py+h+1)], fill="#80371f")
                pen.line([(px,py+1),(px+2,py),(px+w-2,py)], fill="#af4921")
                if w>6:
                    pen.line([(px+3,py+h+1),(px+w,py+h),(px+w+2,py+h)], fill="#b65224")
    image.save(ROOT / f"lava_flow_{frame}.png")

glob = Image.new("RGBA", (16, 16))
pen = ImageDraw.Draw(glob)
pen.polygon([(5,2),(10,2),(13,5),(14,10),(11,14),(5,14),(2,11),(2,6)], fill="#542b21")
pen.polygon([(6,3),(10,3),(12,6),(11,11),(8,13),(4,10),(3,7)], fill="#c55b24")
pen.polygon([(5,5),(8,3),(11,6),(9,8),(6,8),(6,11),(4,9)], fill="#f4a13e")
pen.line([(6,4),(8,4),(9,5)], fill="#ffe19a")
pen.line([(10,10),(12,9),(12,7)], fill="#773622")
glob.save(ROOT / "lava_glob.png")
