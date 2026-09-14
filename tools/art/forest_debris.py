"""Twenty distinct forest scrap silhouettes; the game keeps their movement local."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"

# SILHOUETTES: Large deliberate pixels survive when scraps render below tile size.
SHAPES = {
    "oak_leaf": ("#a27d40", [(3, 7), (6, 5), (5, 2), (9, 4), (12, 3), (11, 7),
                            (14, 10), (10, 10), (8, 14), (6, 10), (2, 10)]),
    "birch_leaf": ("#b5a658", [(3, 6), (7, 2), (12, 5), (11, 10), (7, 13), (3, 9)]),
    "pine_needle": ("#647851", [(4, 2), (7, 11), (8, 3), (9, 12), (13, 5), (10, 14), (6, 13)]),
    "twig": ("#8c795c", [(2, 12), (7, 8), (7, 3), (9, 3), (9, 7), (13, 4),
                         (14, 6), (9, 10), (3, 14)]),
    "bark": ("#66503b", [(3, 3), (9, 2), (12, 5), (10, 12), (5, 14), (4, 8)]),
    "wood_chip": ("#b79b65", [(2, 9), (11, 3), (14, 5), (6, 13)]),
    "root": ("#817151", [(3, 2), (6, 3), (6, 7), (10, 8), (13, 13), (10, 14),
                         (8, 10), (4, 8)]),
    "fern_leaf": ("#627e48", [(2, 5), (7, 7), (7, 2), (10, 3), (10, 7), (14, 5),
                              (13, 9), (9, 11), (7, 14), (5, 10), (2, 9)]),
    "grass_blade": ("#849757", [(4, 2), (8, 5), (10, 14), (7, 14), (6, 7)]),
    "mushroom_cap": ("#b79d76", [(2, 9), (3, 5), (7, 3), (12, 5), (14, 10), (9, 11)]),
    "mushroom_stem": ("#d4c3a1", [(6, 3), (10, 4), (10, 12), (7, 14), (5, 11)]),
    "spore": ("#c9bc75", [(5, 6), (7, 4), (10, 6), (9, 10), (6, 11), (4, 9)]),
    "acorn": ("#967044", [(4, 5), (11, 5), (12, 9), (8, 14), (5, 11)]),
    "seed_husk": ("#b9a071", [(3, 4), (8, 2), (12, 7), (11, 12), (7, 10), (4, 8)]),
    "feather": ("#b4b3a0", [(2, 9), (5, 3), (9, 2), (12, 4), (11, 8), (8, 10),
                            (5, 11), (3, 14)]),
    "bone_chip": ("#c2b590", [(2, 5), (5, 3), (8, 5), (12, 6), (14, 10),
                              (11, 12), (8, 9), (5, 8), (3, 9)]),
    "pottery": ("#b67b54", [(2, 5), (13, 3), (10, 12), (5, 10)]),
    "cloth": ("#879080", [(3, 3), (12, 4), (11, 7), (13, 12), (8, 11), (4, 14), (2, 8)]),
    "brass_case": ("#ba9b50", [(5, 3), (10, 3), (10, 12), (11, 12), (11, 14),
                              (4, 14), (4, 12), (5, 12)]),
    "stone_chip": ("#929081", [(2, 8), (5, 3), (10, 4), (13, 9), (8, 13), (4, 12)]),
}


if __name__ == "__main__":
    for name, (color, points) in SHAPES.items():
        image = Image.new("RGBA", (16, 16))
        pen = ImageDraw.Draw(image)
        pen.polygon(points, fill=color)
        if name in ("oak_leaf", "birch_leaf", "fern_leaf", "feather"):
            pen.line((7, 6, 7, 12), fill="#697153")
        elif name == "acorn":
            pen.rectangle((4, 4, 11, 6), fill="#645540")
        image.save(ROOT / f"debris_{name}.png")
