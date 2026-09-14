"""A masked skater in a short fur coat, with visible blades and a tied grit pouch."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"
OUTLINE = "#182932"
COAT = "#506477"
FUR = "#a2afb0"
STEEL = "#7f999f"
MASK = "#c0cecd"


def skater(name, lean, push=False):
    image = Image.new("RGBA", (16, 16))
    p = ImageDraw.Draw(image)
    # BODY: Lean comes from the drawing, not a displaced gameplay position.
    p.polygon([(4, 6), (9, 5), (12, 10), (10, 12), (4, 11)], fill=OUTLINE)
    p.polygon([(5, 6), (9, 6), (11, 10), (8, 11), (5, 10)], fill=COAT)
    p.line([(4, 6), (7, 5), (10, 6)], fill=FUR, width=2)
    p.rectangle((7 + lean, 1, 11 + lean, 5), fill=OUTLINE)
    p.rectangle((8 + lean, 2, 11 + lean, 4), fill=MASK)
    p.point((10 + lean, 3), fill=OUTLINE)
    p.line([(6, 11), (5 if push else 6, 13)], fill=OUTLINE, width=2)
    p.line([(9, 11), (12 if push else 10, 13)], fill=OUTLINE, width=2)
    p.line([(2 if push else 4, 14), (7, 14)], fill=STEEL)
    p.line([(9, 14), (14 if push else 12, 14)], fill=STEEL)
    p.line([(10, 7), (13, 8)], fill=FUR)
    p.line([(3, 7), (1, 8 if push else 6)], fill="#6e8996")
    image.save(ROOT / f"{name}.png")


if __name__ == "__main__":
    skater("rime_skater", 0)
    skater("rime_skater_push", -1, True)
    skater("rime_skater_glide", 1)
    image = Image.new("RGBA", (16, 16))
    p = ImageDraw.Draw(image)
    p.polygon([(5, 4), (10, 4), (13, 10), (11, 14), (4, 14), (2, 10)], fill="#30332d")
    p.polygon([(5, 5), (10, 5), (11, 9), (10, 12), (5, 12), (4, 9)], fill="#85724b")
    p.line([(5, 4), (10, 4)], fill="#c2af7b", width=2)
    for point in [(6, 8), (9, 9), (7, 11)]:
        p.point(point, fill="#cebd91")
    image.save(ROOT / "grit_pouch.png")
