"""Top-down forest wall mass. Its exposed contour is drawn from neighboring tiles."""
from pathlib import Path
from PIL import Image


if __name__ == "__main__":
    root = Path(__file__).resolve().parents[2] / "assets" / "graphics"
    # NEGATIVE SPACE: No ledge, brick course or per-tile bevel to repeat across a wall.
    Image.new("RGBA", (16, 16), (43, 49, 41, 255)).save(root / "forest_wall.png")
