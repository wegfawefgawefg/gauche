"""Authored canopy gaps, shared by the compiled light stencil and reference PNG."""
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]

# Open gaps are deliberately uneven; foliage occupies the quiet space between them.
ROWS = [
    "0000000000000000",
    "0000011000000000",
    "0000133100220000",
    "0000377302771000",
    "0011377202772000",
    "0137721001221000",
    "0277730010000000",
    "0133210177100000",
    "0010001377310000",
    "0000012777712200",
    "0001277773107710",
    "0013777730027710",
    "0012773100013100",
    "0001221000000000",
    "0000000000000000",
    "0000000000000000",
]

if __name__ == "__main__":
    pixels = [int(value) * 255 // 7 for row in ROWS for value in row]
    image = Image.new("L", (16, 16))
    image.putdata(pixels)
    (ROOT / "assets" / "lighting").mkdir(exist_ok=True)
    image.save(ROOT / "assets" / "lighting" / "forest_canopy.png")
    text = "#pragma once\n\n#include <array>\n#include <cstdint>\n\n"
    text += "// Generated from tools/art/canopy.py; edit the authored gaps there.\n"
    text += "inline constexpr std::array<std::uint8_t, 256> canopy_mask{\n"
    for y in range(16):
        text += "    " + ", ".join(str(v) for v in pixels[y*16:(y+1)*16]) + ",\n"
    text += "};\n"
    (ROOT / "src" / "lighting" / "canopy_mask.hpp").write_text(text)
