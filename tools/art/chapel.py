"""Native 16px chapel furnishings: worn timber, faded cloth, pottery and wax."""
from underworks_palette import *

im, p = canvas()
p.rectangle((1, 4, 14, 12), fill=INK)
p.rectangle((2, 5, 13, 8), fill='#756449')
p.line((2, 5, 13, 5), fill='#a58c5c')
p.rectangle((3, 10, 12, 12), fill='#8b754e')
p.line((3, 11, 12, 11), fill='#b19864')
for x in (1, 13):
    p.rectangle((x, 7, x+1, 14), fill='#574937')
    p.point((x, 7), fill='#b19864')
for x in (5, 10):
    p.line((x, 6, x, 8), fill='#5b503c')
save(im, 'chapel_pew')

im, p = canvas()
for x, y in ((2, 7), (6, 10), (10, 5)):
    p.polygon([(x, y), (x+4, y+2), (x+3, y+4), (x, y+2)], fill='#756449')
    p.line((x, y, x+3, y+2), fill='#b19864')
p.point((4, 13), fill=IRON)
p.point((12, 11), fill=IRON)
save(im, 'chapel_pew_broken')

im, p = canvas()
p.rectangle((2, 5, 13, 14), fill=INK)
p.rectangle((3, 8, 5, 13), fill='#67573f')
p.rectangle((10, 8, 12, 13), fill='#67573f')
p.rectangle((1, 4, 14, 7), fill='#927b52')
p.rectangle((5, 5, 10, 12), fill='#747c85')
p.line((6, 6, 6, 11), fill='#a9adb0')
p.line((5, 12, 10, 12), fill=BRASS)
# A small closed book, not an invented light source.
p.rectangle((6, 2, 10, 4), fill='#66473b')
p.line((7, 4, 10, 4), fill=CLOTH)
save(im, 'chapel_altar')

im, p = canvas()
p.rectangle((3, 0, 12, 15), fill='#4e555d')
p.line((3, 0, 3, 15), fill='#858073')
p.line((12, 0, 12, 15), fill='#858073')
p.line((4, 0, 4, 15), fill='#6b7077')
p.line((11, 0, 11, 15), fill='#6b7077')
p.polygon([(8, 5), (10, 8), (8, 10), (6, 8)], outline='#a29974')
p.point((3, 9), fill=(0, 0, 0, 0))
p.point((10, 13), fill=(0, 0, 0, 0))
save(im, 'chapel_runner')

im, p = canvas()
p.ellipse((3, 5, 12, 14), fill=INK)
p.ellipse((4, 5, 11, 13), fill='#7b8383')
p.rectangle((5, 3, 10, 6), fill='#a2aaa4')
p.line((6, 3, 9, 3), fill=INK)
p.line((5, 9, 10, 9), fill=BRASS)
p.line((5, 6, 5, 8), fill='#c0c2ad')
p.line((9, 11, 10, 12), fill='#565c60')
save(im, 'chapel_urn')

im, p = canvas()
for box in ((3, 10, 7, 12), (8, 8, 10, 10), (10, 12, 12, 13)):
    p.ellipse(box, fill='#a9a186')
p.line((5, 8, 5, 11), fill='#d4c8a2')
p.point((9, 9), fill='#d4c8a2')
save(im, 'chapel_wax')
