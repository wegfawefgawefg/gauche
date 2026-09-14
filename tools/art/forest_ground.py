"""Calm 16px forest ground: continuous base color and a few deliberate shapes."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


def ground(name, marks, color=(37, 48, 39, 255)):
    image = Image.new("RGBA", (16, 16), color)
    pen = ImageDraw.Draw(image)
    for points, color in marks:
        pen.line(points, fill=color)
    image.save(ROOT / f"{name}.png")


if __name__ == "__main__":
    ground("forest_floor_a", [])
    ground("forest_floor_b", [([(4, 10), (6, 10)], "#29352b")])
    ground("forest_floor_c", [([(10, 5), (11, 5)], "#202b23")])
    # Grass clumps will be separate props, not a repeated mark in every floor tile.
    ground("forest_grass", [], (39, 53, 40, 255))
    ground("forest_ruin", [([(4, 10), (6, 10)], "#2b2e26"),
                           ([(11, 5), (12, 5)], "#24291f")], (36, 41, 34, 255))
