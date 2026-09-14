"""Quiet top-down shallows and a small wall-fed spring; no all-over grain."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"
for frame in range(2):
    image = Image.new("RGBA", (16, 16), "#273f40")
    pen = ImageDraw.Draw(image)
    pen.line((4+frame*3, 10, 6+frame*3, 10), fill="#314c4a")
    image.save(ROOT / f"shallow_water_{'ab'[frame]}.png")
    pen.rectangle((5, 0, 10, 3), fill="#333c33")
    pen.rectangle((6, 0, 9, 2), fill="#101d1c")
    pen.line((7, 1, 7, 8+frame), fill="#6d9890", width=2)
    pen.line((6, 9, 10, 9), fill="#76998f")
    pen.point((5+frame, 7), fill="#557a75")
    image.save(ROOT / f"spring_{'ab'[frame]}.png")
