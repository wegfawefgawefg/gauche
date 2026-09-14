"""Forest clutter with distinct silhouettes and little interior noise."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


def draw(name):
    image = Image.new("RGBA", (16, 16))
    p = ImageDraw.Draw(image)
    if name == "leaves":
        for x, y, color in [(3, 9, "#8e713d"), (10, 11, "#706638"), (8, 6, "#986646")]:
            p.polygon([(x, y-2), (x+3, y), (x+1, y+2), (x-1, y)], fill=color)
    elif name == "twigs":
        p.line([(3, 12), (10, 7), (12, 7)], fill="#8b7655")
        p.line([(7, 9), (7, 6)], fill="#8b7655")
        p.line([(7, 13), (12, 11)], fill="#655b47")
    elif name in ("fern", "tall_grass"):
        p.line([(8, 14), (8, 3)], fill="#7e9258")
        if name == "fern":
            for y, width in [(5, 2), (8, 4), (11, 5)]:
                p.line([(8-width, y-2), (8-width, y), (8, y+2), (8+width, y),
                        (8+width, y-2)], fill="#547549", width=2)
        else:
            p.polygon([(7, 14), (3, 5), (7, 10), (10, 2), (10, 10),
                       (14, 7), (11, 14)], fill="#677e43")
    elif name == "puffball":
        p.rectangle((7, 10, 9, 14), fill="#968865")
        p.polygon([(3, 10), (3, 7), (6, 4), (10, 4), (13, 7), (13, 10)], fill="#baae7b")
        p.line((5, 8, 10, 8), fill="#ddd1a0")
        p.point((9, 6), fill="#776c50")
    elif name == "rotten_log":
        p.polygon([(2, 6), (11, 4), (14, 7), (14, 12), (4, 14), (2, 11)], fill="#665242")
        p.polygon([(11, 4), (14, 7), (14, 12), (11, 10)], fill="#a08455")
        p.line((4, 8, 10, 6), fill="#3f3930", width=2)
        p.line((4, 12, 9, 11), fill="#3f3930")
        p.line((3, 6, 9, 5), fill="#576441", width=2)
    elif name == "crate":
        p.rectangle((2, 4, 13, 14), fill="#886d48", outline="#b29966")
        p.line((6, 5, 6, 13), fill="#514632")
        p.line((10, 5, 10, 13), fill="#514632")
        p.line((3, 5, 12, 13), fill="#b29966", width=2)
    elif name == "nest":
        p.ellipse((2, 7, 13, 14), fill="#806747")
        p.ellipse((4, 8, 11, 12), fill="#493f32")
        p.ellipse((6, 9, 8, 12), fill="#c4b790")
        p.line((3, 12, 6, 14), fill="#a88c5d")
    elif name == "clay_pot":
        p.polygon([(5, 4), (10, 4), (10, 6), (13, 9), (12, 13), (10, 14),
                   (5, 14), (3, 12), (3, 9), (5, 6)], fill="#a16c4c")
        p.line((5, 4, 10, 4), fill="#d49c67", width=2)
        p.line((5, 9, 5, 12), fill="#c38b5b")
    image.save(ROOT / f"{name}.png")


if __name__ == "__main__":
    for name in ["leaves", "twigs", "fern", "tall_grass", "puffball", "rotten_log",
                 "crate", "nest", "clay_pot"]:
        draw(name)
