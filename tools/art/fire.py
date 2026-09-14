"""Small readable fire shapes, transparent pixels, and a spent campfire base."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


def flame(name, lean):
    image = Image.new("RGBA", (16, 16))
    pen = ImageDraw.Draw(image)
    pen.polygon([(5, 14), (2, 10), (4, 6), (5, 9), (8 + lean, 1),
                 (10, 7), (12, 5), (13, 10), (11, 14)], fill="#d65c29")
    pen.polygon([(6, 13), (4, 10), (8 + lean, 5), (9, 9), (11, 8),
                 (11, 12), (9, 14)], fill="#f5ae45")
    pen.polygon([(7, 13), (6, 11), (8, 8), (9, 11), (9, 13)], fill="#ffe5a2")
    image.save(ROOT / f"{name}.png")


def base(name, lit):
    image = Image.new("RGBA", (16, 16))
    pen = ImageDraw.Draw(image)
    pen.ellipse((2, 9, 13, 14), fill="#353534")
    pen.line((3, 13, 11, 9), fill="#62554a", width=2)
    pen.line((4, 9, 12, 13), fill="#453e39", width=2)
    for x, y in [(5, 12), (8, 10), (10, 12)]:
        pen.point((x, y), fill="#cb642d" if lit else "#81766a")
    image.save(ROOT / f"{name}.png")


if __name__ == "__main__":
    flame("flame_a", -1)
    flame("flame_b", 1)
    base("campfire", True)
    base("campfire_ash", False)
