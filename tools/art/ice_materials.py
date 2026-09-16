"""16px rock backing, cloudy/clear ice seams and compacted snow. No baked light."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


def make(name, base, polygons=(), lines=(), dots=()):
    image = Image.new("RGBA", (16, 16), base)
    draw = ImageDraw.Draw(image)
    for points, color in polygons:
        draw.polygon(points, fill=color)
    for points, color in lines:
        draw.line(points, fill=color)
    for point, color in dots:
        draw.point(point, fill=color)
    image.save(ROOT / f"{name}.png")


def generate():
    for variant in range(2):
        suffix = "ab"[variant]
        shift = variant * 2
        make(f"ice_rock_{suffix}", "#252e34",
             [([(0, 3+shift), (5, 2), (11, 5), (15, 3), (15, 6), (8, 8), (2, 6)], "#303b42"),
              ([(1, 12), (6, 10), (11, 12), (15, 10), (15, 13), (9, 15)], "#202930")],
             [([(0, 7+shift), (4, 8+shift), (8, 7)], "#182229"),
              ([(2, 2), (5, 1), (8, 2)], "#4b626b"),
              ([(11, 10), (14, 9)], "#41565e")])
        make(f"ice_clear_{suffix}", "#293f49",
             [([(2, 15), (10+shift, 1), (12+shift, 1), (4, 15)], "#35505b")],
             [([(0, 11-shift), (5, 10), (8, 6), (9, 2)], "#4d6b77"),
              ([(5, 10), (8, 12), (13, 12)], "#223740"),
              ([(11, 4+shift), (12, 3+shift)], "#78929b")],
             [((3, 4), "#5d7983"), ((12, 14), "#47616c")])
        make(f"ice_cloud_{suffix}", "#334852",
             [([(0, 3), (5, 2+shift), (10, 4), (15, 3), (15, 6), (9, 8), (3, 7)], "#3f5660"),
              ([(0, 12), (5, 10), (9, 12-shift), (15, 11), (15, 14), (6, 14)], "#394f59")],
             [([(3, 0), (4, 4), (2, 6)], "#536b76"),
              ([(12, 9), (10, 12), (11, 15)], "#263a44")],
             [((8, 4), "#607c86"), ((5, 12), "#56737c")])
        make(f"packed_snow_{suffix}", "#6b7c80", (),
             [([(2, 5+shift), (4, 4+shift), (7, 4+shift)], "#627579"),
              ([(10, 12-shift), (13, 12-shift)], "#728287")],
             [((12, 3), "#607176"), ((5, 12), "#76868a")])


if __name__ == "__main__":
    generate()
