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
