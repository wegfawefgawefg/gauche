"""A small three-coin pile, with empty space around the pickup silhouette."""
from pathlib import Path
from PIL import Image, ImageDraw


if __name__ == "__main__":
    image = Image.new("RGBA", (16, 16))
    draw = ImageDraw.Draw(image)
    for x, y in [(3, 8), (8, 7), (6, 4)]:
        draw.ellipse((x, y, x + 5, y + 4), fill="#80562d")
        draw.ellipse((x, y, x + 5, y + 3), fill="#c7a451")
        draw.line([(x + 1, y), (x + 3, y)], fill="#ead497")
    image.save(Path(__file__).resolve().parents[2] / "assets/graphics/coins.png")
