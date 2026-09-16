"""Three-tile-wide log, ice and steel roofs, hand drawn in 16px sections.

The A/B/C bands join across a span. Horizontal ends expose the passage lip.
North/south logs use a closed far dome and a near arch, not rotated side holes.
"""
from underworks_palette import *

for family in ('log', 'ice', 'gantry'):
    for end in (False, True):
        for band in range(3):
            im, p = canvas()
            if family == 'log':
                color = ('#4b4930', '#686045', '#393d2b')[band]
                p.rectangle((0, 0, 15, 15), fill=color)
                for y, x in [(3, 1), (9, 6), (14, 3)]:
                    p.line([(0, y), (x, y), (x+2, y-1), (15, y-1)], fill='#83704d' if band == 1 else '#555437')
                if band == 0:
                    p.line((0, 0, 15, 0), fill='#263626')
                    p.rectangle((3, 1, 9, 2), fill='#4c6038')
                if band == 2:
                    p.line((0, 15, 15, 15), fill=INK)
                if end:
                    # One oval cut end across all three bands, with a real hollow.
                    top = -band*16
                    p.rectangle((0, 0, 10, 15), fill=(0, 0, 0, 0))
                    p.ellipse((0, top, 14, top+47), fill='#60543b')
                    p.ellipse((2, top+4, 12, top+43), fill='#786544')
                    p.ellipse((3, top+7, 11, top+40), fill=INK)
                    p.ellipse((4, top+9, 10, top+38), fill=(0, 0, 0, 0))
            elif family == 'ice':
                p.rectangle((0, 0, 15, 15), fill=('#557575', '#85a7a2', '#425e63')[band])
                p.line([(0, 5), (5, 4), (9, 7), (15, 6)], fill='#a6c2b6' if band != 2 else '#668686')
                p.line([(8, 0), (9, 4), (7, 7), (8, 12)], fill='#628582')
                if band == 0:
                    p.line((0, 0, 15, 0), fill='#c6d0bf')
                if band == 2:
                    p.line((0, 15, 15, 15), fill=INK)
                if end:
                    top = -band*16
                    p.rectangle((0, 0, 7, 15), fill=(0, 0, 0, 0))
                    p.ellipse((0, top, 11, top+47), fill='#c6d0bf')
                    p.ellipse((2, top+5, 10, top+42), fill=ICE)
                    p.ellipse((3, top+9, 8, top+38), fill=(0, 0, 0, 0))
            else:
                p.rectangle((0, 0, 15, 15), fill=INK)
                for x in range(0, 16, 4):
                    p.line((x, 1, x, 14), fill=IRON)
                    for y in range(2, 16, 4):
                        p.line((x+1, y, min(x+3, 15), y), fill=STEEL)
                if band != 1:
                    y = 1 if band == 0 else 14
                    p.line((0, y, 15, y), fill=BRASS, width=2)
                    for x in (3, 11):
                        p.point((x, y), fill=GOLD)
                if end:
                    p.rectangle((0, 0, 2, 15), fill=IRON)
                    p.line((0, 0, 0, 15), fill=STEEL)
                    bolts(p, [(1, 3), (1, 12)])
            save(im, f'roof_{family}{"_end" if end else ""}_{"abc"[band]}')

# Upright timber stays on the same 3-column footprint as its support walls.
# Draw the complete cap first so the arc joins exactly across tile boundaries.
for end in ('far', 'near'):
    im, p = canvas((48, 16))
    for band, color in enumerate(('#393d2b', '#686045', '#4b4930')):
        p.rectangle((band*16, 0, band*16+15, 15), fill=color)
    for x, bend in ((3, 2), (9, -1), (15, 1), (21, 2), (28, -1), (35, 1), (43, -2)):
        p.line([(x, 0), (x, 7), (x+bend, 10), (x+bend, 15)], fill='#83704d' if 16<=x<32 else '#555437')
    if end == 'far':
        mask, q = canvas((48, 16))
        q.ellipse((0, 0, 47, 25), fill='white')
        q.rectangle((0, 12, 47, 15), fill='white')
        p.arc((0, 0, 47, 25), 180, 360, fill=INK, width=2)
        p.arc((3, 2, 44, 26), 185, 350, fill='#83704d')
        im.putalpha(mask.getchannel('A'))
    else:
        # Down-facing open arch: its clear center ends on the walkable column.
        p.ellipse((2, 0, 45, 27), fill='#60543b')
        p.ellipse((6, 2, 41, 28), fill='#786544')
        p.ellipse((10, 3, 37, 29), fill=INK)
        p.ellipse((16, 6, 31, 26), fill=(0, 0, 0, 0))
        p.rectangle((16, 15, 31, 15), fill=(0, 0, 0, 0))
        p.line((1, 2, 1, 15), fill=INK)
        p.line((46, 2, 46, 15), fill=INK)
    for band in range(3):
        save(im.crop((band*16, 0, band*16+16, 16)), f'roof_log_{end}_{"abc"[band]}')

# Hollow timber has its own wood-bottomed passage, continuous through the lips.
im, p = canvas()
p.rectangle((0, 0, 15, 15), fill='#4b4230')
for y, bend in ((2, 1), (6, -1), (11, 1), (15, 0)):
    p.line([(0, y), (5, y), (8, y+bend), (15, y+bend)], fill='#60513a')
p.line((2, 8, 6, 8), fill='#38372a')
p.line((10, 4, 14, 4), fill='#716044')
save(im, 'log_floor')
