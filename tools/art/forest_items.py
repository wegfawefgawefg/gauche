"""Readable 16px forest tools: small flat shapes, no noisy texture fill."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"
WOOD = "#a17c4b"
DARK = "#62503c"
IRON = "#a8afa0"
EDGE = "#d0d0b5"


def tool(name, paint):
    image = Image.new("RGBA", (16, 16))
    paint(ImageDraw.Draw(image))
    image.save(ROOT / (name + ".png"))


def rock(d):
    d.polygon([(3, 7), (6, 4), (11, 4), (14, 8), (12, 12), (4, 12), (2, 10)], fill="#8d9481")
    d.line([(4, 7), (7, 5), (10, 5)], fill="#bdc2a4")


def hatchet(d):
    d.line([(3, 14), (11, 3)], fill=WOOD, width=2)
    d.polygon([(8, 3), (12, 2), (14, 3), (14, 7), (10, 8), (9, 6)], fill=IRON)
    d.line([(14, 3), (14, 7), (12, 8)], fill=EDGE)


def spear(d):
    d.line([(2, 14), (12, 4)], fill=WOOD, width=1)
    d.polygon([(9, 5), (14, 1), (13, 6)], fill=IRON)
    d.line([(10, 5), (14, 1)], fill=EDGE)


def crossbow(d):
    d.line([(4, 13), (11, 4)], fill=WOOD, width=3)
    d.line([(3, 4), (5, 2), (12, 7), (13, 10)], fill=IRON, width=2)
    d.line([(3, 4), (8, 8), (13, 10)], fill="#d3c795")
    d.line([(8, 8), (13, 2)], fill=EDGE)


def blunder(d):
    d.polygon([(1, 12), (4, 14), (7, 10), (5, 8)], fill=WOOD)
    d.polygon([(5, 8), (10, 3), (12, 3), (15, 6), (10, 9), (8, 11)], fill=IRON)
    d.line([(11, 3), (14, 6)], fill="#434a42", width=2)
    d.line([(6, 8), (10, 4)], fill=EDGE)


def maul(d):
    d.line([(3, 14), (11, 4)], fill=WOOD, width=2)
    d.polygon([(6, 2), (9, 1), (15, 6), (12, 10), (5, 5)], fill="#90724d")
    d.line([(6, 2), (12, 7), (15, 6)], fill="#c1a371")


def rake(d):
    d.line([(2, 14), (11, 5)], fill=WOOD, width=1)
    d.line([(7, 2), (14, 9)], fill=IRON, width=2)
    for x, y in [(7, 2), (10, 5), (13, 8)]:
        d.line([(x, y), (x-2, y+2)], fill=IRON)


def knife(d):
    d.line([(4, 13), (7, 9)], fill=DARK, width=3)
    d.polygon([(6, 8), (12, 2), (12, 8), (8, 11)], fill="#929984")
    d.line([(6, 8), (12, 2)], fill=EDGE)


if __name__ == "__main__":
    for name, paint in [("throwing_rock", rock), ("hatchet", hatchet),
                        ("hunting_spear", spear), ("crossbow", crossbow),
                        ("blunderbuss", blunder), ("wooden_maul", maul),
                        ("rake", rake), ("flint_knife", knife)]:
        tool(name, paint)
