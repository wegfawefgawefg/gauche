"""Quiet 16px underworks tiles; exposed wall edges are supplied by the renderer."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


def tile(name, base, marks=()):
    image = Image.new("RGBA", (16, 16), base)
    pen = ImageDraw.Draw(image)
    for points, color in marks:
        pen.line(points, fill=color)
    image.save(ROOT / f"{name}.png")


if __name__ == "__main__":
    # MASS: No ledge, border, random grit or repeated metal plate around every cell.
    tile("industrial_floor_a", "#302d2b")
    tile("industrial_floor_b", "#302d2b", [([(5, 10), (7, 10)], "#35312e")])
    tile("industrial_wall", "#27272a")
    tile("industrial_ruin", "#302e2d", [([(6, 9), (8, 8), (9, 8)], "#393432")])
    # HEAT: Most molten cells are uninterrupted color. Isolated quiet patches
    # breathe slowly, sharing identical boundary pixels with the plain tile.
    tile("industrial_lava", "#963c20")
    tile("industrial_lava_a", "#963c20", [([(5, 9), (7, 8), (10, 8)], "#a34523")])
    tile("industrial_lava_b", "#963c20", [([(5, 9), (7, 8), (10, 8)], "#ab4a26")])
