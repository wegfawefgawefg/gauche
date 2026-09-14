"""Native 16px wrought-iron barrier and an understated grave opening."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


if __name__ == "__main__":
    gate = Image.new("RGBA", (16, 16))
    draw = ImageDraw.Draw(gate)
    for x in [2, 6, 10, 14]:
        draw.line([(x, 2), (x, 13)], fill="#747969", width=1)
        draw.point((x, 1), fill="#a1a38b")
    draw.line([(1, 5), (14, 5)], fill="#5c6658")
    draw.line([(1, 11), (14, 11)], fill="#5c6658")
    gate.save(ROOT / "encounter_gate.png")
    grave = Image.new("RGBA", (16, 16))
    draw = ImageDraw.Draw(grave)
    draw.polygon([(4, 4), (11, 4), (13, 7), (11, 11), (3, 11), (2, 7)], fill="#171a19")
    draw.line([(3, 5), (5, 3), (10, 3), (12, 5)], fill="#52574a")
    draw.line([(3, 12), (11, 12), (13, 10)], fill="#343e32")
    for p in [(6, 6), (10, 8)]:
        draw.point(p, fill="#9c759e")
    grave.save(ROOT / "grave_vent.png")
