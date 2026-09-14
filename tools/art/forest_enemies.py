"""Small forest silhouettes, flat colors and empty space; all sprites face right."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / "assets" / "graphics"


def canvas():
    image = Image.new("RGBA", (16, 16))
    return image, ImageDraw.Draw(image)


def boar():
    im, d = canvas()
    d.polygon([(2, 6), (5, 4), (10, 4), (12, 6), (14, 7), (14, 11), (4, 12), (1, 10)], fill="#745444")
    d.rectangle((4, 11, 5, 14), fill="#4b3831")
    d.rectangle((10, 11, 11, 14), fill="#4b3831")
    d.polygon([(4, 5), (5, 2), (6, 4), (7, 2), (8, 4), (9, 2), (10, 5)], fill="#493830")
    d.polygon([(12, 11), (15, 8), (14, 12)], fill="#ddd0a2")
    d.point((11, 7), fill="#f0bc67")
    d.rectangle((14, 8, 15, 10), fill="#a17157")
    im.save(ROOT / "boar.png")


def snail(closed):
    im, d = canvas()
    d.polygon([(1, 12), (4, 10), (12, 10), (15, 12), (13, 14), (2, 14)], fill="#818758")
    d.polygon([(3, 10), (2, 7), (4, 3), (8, 2), (12, 5), (12, 11)], fill="#a19165")
    d.line([(4, 9), (4, 6), (7, 4), (10, 6), (10, 9), (7, 10), (6, 8), (8, 7)], fill="#514d39")
    for pts in [[(2, 5), (0, 4), (2, 8)], [(5, 3), (5, 0), (7, 2)], [(10, 3), (13, 1), (12, 5)]]:
        d.polygon(pts, fill="#c0b680")
    if not closed:
        d.line([(13, 11), (13, 7), (14, 6)], fill="#b3b879")
        d.point((14, 6), fill="#ecdfb2")
    else:
        d.line([(2, 13), (13, 13)], fill="#c0b680")
    im.save(ROOT / ("thorn_snail_closed.png" if closed else "thorn_snail.png"))


def moth():
    im, d = canvas()
    d.polygon([(7, 6), (2, 1), (0, 3), (2, 8), (5, 9), (2, 13), (5, 14), (8, 9)], fill="#c2b888")
    d.polygon([(8, 6), (13, 1), (15, 3), (13, 8), (10, 9), (13, 13), (10, 14), (7, 9)], fill="#c2b888")
    d.rectangle((7, 4, 8, 12), fill="#615e49")
    d.rectangle((7, 4, 8, 5), fill="#eee1a7")
    d.point((3, 5), fill="#79734e")
    d.point((12, 5), fill="#79734e")
    im.save(ROOT / "lantern_moth.png")


def toad(swollen):
    im, d = canvas()
    d.polygon([(2, 10), (0, 13), (5, 13), (5, 10)], fill="#666d46")
    d.polygon([(11, 10), (15, 13), (10, 13)], fill="#666d46")
    d.ellipse((2, 3 if swollen else 6, 13, 12), fill="#9c9d61")
    d.rectangle((10, 5, 13, 8), fill="#b4b16e")
    d.point((12, 6), fill="#201f1e")
    d.line([(11, 10), (14, 10)], fill="#494b34")
    if swollen:
        d.ellipse((4, 4, 10, 11), fill="#c1b873")
        d.point((6, 2), fill="#ded49b")
    im.save(ROOT / ("spore_toad_swollen.png" if swollen else "spore_toad.png"))


def mimic():
    im, d = canvas()
    d.rectangle((2, 3, 13, 12), fill="#806645")
    d.rectangle((3, 4, 12, 11), outline="#b19460")
    d.polygon([(2, 7), (13, 5), (14, 11), (2, 12)], fill="#211c20")
    for x in [4, 7, 10]:
        d.polygon([(x, 7), (x+2, 7), (x+1, 9)], fill="#d8caa3")
        d.point((x+1, 11), fill="#d8caa3")
    d.line([(3, 12), (1, 14), (4, 14)], fill="#6f6952")
    d.line([(11, 12), (14, 14), (11, 14)], fill="#6f6952")
    im.save(ROOT / "crate_mimic.png")



def wolf():
    im, d = canvas()
    d.polygon([(1, 6), (4, 8), (6, 6), (11, 6), (13, 3), (14, 7), (15, 8), (15, 10), (11, 11), (5, 11), (2, 9)], fill="#9c9c83")
    d.polygon([(2, 6), (1, 3), (0, 5), (1, 9), (4, 10)], fill="#6b7066")
    d.rectangle((5, 10, 6, 14), fill="#6b7066")
    d.rectangle((10, 10, 11, 14), fill="#6b7066")
    d.point((13, 7), fill="#e7cc83")
    d.point((15, 8), fill="#353a34")
    im.save(ROOT / "wolf.png")


def bear():
    im, d = canvas()
    d.polygon([(1, 7), (3, 4), (9, 4), (11, 6), (14, 5), (15, 8), (15, 10), (11, 12), (3, 12), (1, 10)], fill="#8d6a45")
    d.rectangle((3, 11, 5, 14), fill="#604933")
    d.rectangle((10, 11, 12, 14), fill="#604933")
    d.rectangle((11, 3, 13, 5), fill="#604933")
    d.rectangle((13, 8, 15, 10), fill="#b79a6b")
    d.point((12, 6), fill="#ead7a8")
    d.point((15, 8), fill="#382f29")
    im.save(ROOT / "bear.png")


def bat():
    im, d = canvas()
    d.polygon([(7, 6), (3, 3), (0, 2), (1, 8), (4, 7), (5, 10), (7, 9)], fill="#938399")
    d.polygon([(8, 6), (12, 3), (15, 2), (14, 8), (11, 7), (10, 10), (8, 9)], fill="#938399")
    d.polygon([(6, 3), (7, 5), (9, 3), (10, 8), (8, 12), (6, 9)], fill="#655b70")
    d.point((7, 6), fill="#e2c894")
    d.point((9, 6), fill="#e2c894")
    im.save(ROOT / "bat.png")

if __name__ == "__main__":
    wolf()
    bear()
    bat()
    boar()
    snail(False)
    snail(True)
    moth()
    toad(False)
    toad(True)
    mimic()
