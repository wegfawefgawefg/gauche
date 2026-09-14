"""Native 16px reservoir surfaces: quiet interiors, contours supplied by the renderer."""
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
    # SLATE: Match outer pixels so quiet neighboring tiles join without a seam.
    tile("ice_floor_a", "#303d46")
    tile("ice_floor_b", "#303d46", [([(5, 10), (7, 10)], "#33414a")])
    tile("ice_wall", "#28323a")
    tile("ice_ruin", "#333e45", [([(4, 9), (6, 8), (8, 8)], "#3c4850"),
                                    ([(11, 5), (12, 6)], "#29353d")])
    tile("snow", "#6b7c80")
    tile("reservoir_ice", "#405960", [([(4, 11), (7, 9), (11, 9)], "#466068")])
    # WATER: Two restrained ripples, no white checkerboard or directional tile ledges.
    tile("cold_shallow_a", "#2a414a", [([(4, 10), (7, 10)], "#314c56")])
    tile("cold_shallow_b", "#2a414a", [([(5, 9), (8, 9)], "#314c56")])
    tile("cold_water_a", "#1d2c39", [([(6, 6), (8, 6)], "#223441")])
    tile("cold_water_b", "#1d2c39", [([(7, 7), (9, 7)], "#223441")])
