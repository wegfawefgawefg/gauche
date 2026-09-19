"""Small editable 16px placeholders for C++-only Teeming content."""

from pathlib import Path

from PIL import Image, ImageDraw


OUT = Path(__file__).resolve().parents[1] / "assets" / "graphics"
INK = "#171d23"
IRON = "#93a5a6"
LIGHT = "#d5ded2"
GOLD = "#e7b744"
WOOD = "#895832"
RED = "#cf6447"
GREEN = "#83ca73"


def canvas():
    image = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    return image, ImageDraw.Draw(image)


def save(name, image):
    image.save(OUT / f"{name}.png")


image, draw = canvas()
draw.ellipse((2, 2, 13, 13), fill=IRON, outline=INK)
draw.ellipse((4, 4, 11, 11), fill="#637b80", outline=LIGHT)
draw.rectangle((7, 6, 8, 9), fill=GOLD)
save("buckler", image)

image, draw = canvas()
draw.rectangle((2, 5, 13, 8), fill=IRON, outline=INK)
draw.rectangle((4, 8, 7, 13), fill=WOOD, outline=INK)
draw.rectangle((11, 5, 13, 6), fill=LIGHT)
save("pistol", image)

image, draw = canvas()
draw.rectangle((1, 6, 14, 7), fill=IRON, outline=INK)
draw.line((3, 9, 12, 9), fill=WOOD, width=3)
draw.line((3, 9, 1, 11), fill=WOOD, width=2)
save("musket", image)

image, draw = canvas()
draw.arc((2, 1, 13, 14), 85, 275, fill=WOOD, width=2)
draw.line((7, 2, 7, 13), fill=LIGHT)
draw.line((3, 8, 12, 8), fill=IRON)
draw.polygon(((12, 6), (15, 8), (12, 10)), fill=GOLD)
save("bow", image)

image, draw = canvas()
draw.polygon(((13, 4), (15, 8), (13, 12)), fill=RED)
draw.rectangle((3, 5, 12, 11), fill="#7f9b77", outline=INK)
draw.rectangle((1, 7, 3, 9), fill=WOOD)
draw.line((5, 7, 10, 7), fill=LIGHT)
save("rocket_launcher", image)

image, draw = canvas()
draw.rectangle((2, 4, 13, 13), fill=WOOD, outline=INK)
for x in (4, 7, 10):
    draw.rectangle((x, 5, x + 1, 10), fill=GOLD)
save("ammo", image)

image, draw = canvas()
draw.ellipse((3, 5, 13, 15), fill=INK, outline=IRON)
draw.line((8, 5, 10, 2), fill=WOOD, width=2)
draw.line((10, 2, 12, 1), fill=GOLD, width=2)
save("bomb", image)

image, draw = canvas()
draw.ellipse((2, 2, 8, 8), outline=GOLD, width=2)
draw.line((7, 7, 13, 13), fill=GOLD, width=2)
draw.rectangle((11, 11, 13, 14), fill=GOLD)
save("key", image)

image, draw = canvas()
draw.rectangle((1, 0, 14, 15), fill=INK, outline=IRON)
draw.rectangle((3, 2, 12, 14), fill="#56656a", outline=LIGHT)
draw.line((5, 3, 5, 13), fill=INK)
draw.line((10, 3, 10, 13), fill=INK)
draw.rectangle((7, 8, 9, 10), fill=GOLD)
save("door", image)

image, draw = canvas()
draw.rectangle((1, 1, 14, 15), fill=INK, outline=GREEN)
draw.rectangle((3, 3, 12, 14), fill="#284840", outline="#c0e9a8")
draw.polygon(((5, 8), (9, 8), (9, 5), (13, 10), (9, 14), (9, 11), (5, 11)), fill=GREEN)
save("exit", image)

image, draw = canvas()
draw.ellipse((1, 4, 14, 14), fill="#663a35", outline=RED)
draw.ellipse((4, 5, 11, 12), fill=INK, outline="#b66c58")
draw.ellipse((5, 8, 6, 9), fill=RED)
draw.ellipse((9, 8, 10, 9), fill=RED)
save("spawner", image)


def terrain(name, seed, base, flecks):
    import random

    rng = random.Random(seed)
    image = Image.new("RGBA", (16, 16), base)
    draw = ImageDraw.Draw(image)
    for _ in range(28):
        x = rng.randrange(16)
        y = rng.randrange(16)
        draw.point((x, y), fill=rng.choice(flecks))
    save(name, image)


terrain("forest_floor_a", 1, "#253027", ["#314434", "#394938", "#1d2922"])
terrain("forest_floor_b", 2, "#263128", ["#334937", "#405341", "#1a261e"])
terrain("forest_floor_c", 3, "#253028", ["#334b36", "#4a5942", "#1b2921"])
terrain("forest_grass", 4, "#2b3b2d", ["#477047", "#5a7850", "#203022"])
terrain("forest_ruin", 5, "#3a403b", ["#545c51", "#66705d", "#2a352c"])

image, draw = canvas()
draw.rectangle((0, 0, 15, 15), fill="#344039")
draw.rectangle((0, 0, 15, 2), fill="#697b5d")
draw.line((0, 8, 15, 8), fill="#242e28")
draw.line((6, 3, 6, 8), fill="#222d27")
draw.line((11, 9, 11, 15), fill="#232d27")
draw.line((1, 1, 1, 5), fill="#59764c")
draw.line((12, 2, 12, 6), fill="#54764a")
draw.point((13, 7), fill="#7e9569")
save("forest_wall", image)

image, draw = canvas()
draw.rectangle((0, 0, 15, 15), fill="#512421")
draw.polygon(((1, 2), (7, 0), (11, 4), (15, 3), (15, 11),
              (9, 15), (4, 12), (0, 14)), fill="#ca4d2d")
draw.line((0, 7, 7, 5, 15, 8), fill="#f6b44e", width=2)
draw.point((10, 11), fill="#ffe488")
save("lava_tile", image)

image, draw = canvas()
draw.rectangle((0, 0, 15, 15), fill="#416a88")
draw.polygon(((0, 0), (15, 0), (9, 8), (15, 15), (0, 15), (5, 9)),
             fill="#83bbce")
draw.line((1, 3, 14, 12), fill="#d4edf0")
draw.line((10, 1, 4, 14), fill="#b4e1e8")
save("ice_tile", image)


def creature(name, body, face, ears=()):
    image, draw = canvas()
    for ear in ears:
        draw.polygon(ear, fill=body, outline=INK)
    draw.ellipse((2, 4, 13, 14), fill=body, outline=INK)
    draw.rectangle((5, 8, 6, 9), fill=face)
    draw.rectangle((10, 8, 11, 9), fill=face)
    draw.point((8, 12), fill=face)
    save(name, image)


creature("bat", "#6d5b76", "#e8b65f",
         (((0, 5), (2, 2), (6, 7)), ((10, 7), (14, 2), (15, 5))))
creature("wolf", "#87918b", "#e9daac",
         (((2, 2), (6, 5), (3, 7)), ((13, 2), (10, 5), (13, 7))))
creature("bear", "#80543a", "#e7ca8d",
         (((2, 2), (5, 2), (5, 7)), ((11, 2), (14, 2), (11, 7))))
creature("bunny", "#b7b7a1", "#5b5960",
         (((4, 0), (6, 0), (6, 8)), ((10, 0), (12, 0), (10, 8))))
creature("ember", "#c85c35", "#ffe3a5",
         (((3, 6), (5, 0), (8, 5)), ((8, 5), (12, 1), (13, 8))))
creature("frost_bat", "#8bbbd3", "#e6f2f2",
         (((0, 5), (2, 2), (6, 7)), ((10, 7), (14, 2), (15, 5))))

image, draw = canvas()
draw.rectangle((3, 3, 12, 13), fill="#b6c5aa", outline=INK)
draw.rectangle((4, 2, 11, 4), fill="#6e8e68", outline=INK)
draw.ellipse((6, 7, 9, 10), fill="#5b7890")
draw.point((7, 8), fill="#dce8da")
save("sleep_meds", image)

image, draw = canvas()
draw.line((2, 13, 13, 2), fill=WOOD, width=3)
draw.point((2, 13), fill="#c29a65")
draw.point((13, 2), fill="#c29a65")
save("stick", image)

image, draw = canvas()
draw.rectangle((1, 5, 14, 8), fill=IRON, outline=INK)
draw.rectangle((1, 8, 6, 10), fill=WOOD, outline=INK)
draw.rectangle((4, 10, 6, 14), fill=WOOD, outline=INK)
draw.rectangle((12, 4, 14, 9), fill=LIGHT)
save("shotgun", image)

image, draw = canvas()
draw.rectangle((2, 5, 14, 8), fill=IRON, outline=INK)
draw.rectangle((5, 8, 8, 13), fill="#687971", outline=INK)
draw.rectangle((3, 8, 4, 11), fill=WOOD)
draw.line((11, 5, 11, 8), fill=GOLD)
save("smg", image)

image, draw = canvas()
draw.arc((1, 3, 14, 14), 0, 180, fill=IRON, width=2)
draw.arc((2, 4, 13, 13), 180, 360, fill=IRON, width=2)
for x in (3, 6, 9, 12):
    draw.polygon(((x, 5), (x + 1, 8), (x + 2, 5)), fill=LIGHT)
save("bear_trap", image)

image, draw = canvas()
draw.ellipse((2, 4, 13, 13), fill="#5e6d64", outline=INK)
draw.ellipse((4, 5, 11, 11), fill="#8a9c84", outline=IRON)
draw.ellipse((7, 7, 9, 9), fill=RED)
save("mine", image)

image, draw = canvas()
draw.rectangle((2, 2, 13, 13), fill="#4d5d5a", outline=INK)
draw.rectangle((4, 4, 11, 11), fill="#829c85", outline=LIGHT)
draw.line((8, 4, 8, 9), fill=RED, width=2)
draw.ellipse((6, 9, 10, 12), fill=GOLD)
save("switch", image)

image, draw = canvas()
draw.line((3, 14, 10, 4), fill=WOOD, width=2)
draw.line((3, 13, 8, 6), fill="#b48a51", width=1)
draw.polygon([(2, 5), (4, 2), (7, 1), (10, 2), (13, 5), (14, 9),
              (11, 6), (8, 4), (5, 3)], fill=IRON)
draw.line([(4, 2), (7, 1), (10, 2), (12, 4)], fill=LIGHT, width=1)
draw.point((9, 4), fill=INK)
save("pickaxe", image)

image, draw = canvas()
draw.ellipse((2, 4, 13, 13), fill="#b45c59", outline=INK)
draw.ellipse((5, 5, 11, 10), fill="#e58d82")
draw.point((8, 8), fill="#f4b6a6")
save("raw_meat", image)

image, draw = canvas()
draw.ellipse((2, 4, 13, 13), fill="#8e4a2d", outline=INK)
draw.ellipse((5, 5, 11, 10), fill="#b9743b")
draw.point((8, 8), fill="#e5ac61")
save("cooked_meat", image)

image, draw = canvas()
draw.line((2, 12, 13, 12), fill=WOOD, width=3)
draw.line((4, 14, 11, 8), fill=WOOD, width=2)
draw.polygon(((4, 9), (7, 2), (9, 7), (11, 2), (13, 10),
              (9, 12)), fill="#eb9235", outline=RED)
draw.polygon(((7, 10), (8, 5), (10, 10)), fill="#f9d06b")
save("campfire", image)

image, draw = canvas()
draw.rectangle((1, 7, 14, 14), fill="#624c35", outline=INK)
draw.ellipse((3, 3, 12, 13), fill="#4c5941", outline="#9aaa73")
draw.ellipse((5, 6, 10, 12), fill=INK, outline="#936443")
draw.line((2, 8, 5, 4), fill=WOOD, width=2)
draw.line((10, 4, 13, 8), fill=WOOD, width=2)
save("den", image)

image, draw = canvas()
draw.rectangle((1, 1, 14, 14), fill="#5f6670", outline=INK)
draw.rectangle((3, 3, 12, 12), fill="#9a9b94", outline=LIGHT)
for x in (4, 7, 10):
    draw.polygon(((x, 5), (x + 2, 5), (x + 1, 11)), fill="#5b3431")
save("crusher", image)
