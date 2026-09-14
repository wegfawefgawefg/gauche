"""Blind tunnel hound: quiet slate coat, oversized pale listening ears."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for pose in ['echo_hound','echo_hound_trail','echo_hound_warn','echo_hound_recover']:
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    warn=pose.endswith('warn'); rest=pose.endswith('recover'); trail=pose.endswith('trail')
    p.polygon([(2,8),(1,5),(0,6),(1,11),(4,11)],fill='#879297')
    p.polygon([(3,7),(8,6),(11,8),(10,12),(3,12)],fill='#77858c')
    p.line([(3,7),(7,7),(9,8)],fill='#a3adae')
    p.polygon([(9,5),(12,5),(14,7),(14,10),(10,10),(8,8)],fill='#a5afad')
    # EARS: Fold forward during the bite; listening ears stand high above the back.
    if warn:
        p.polygon([(8,5),(5,3),(10,3),(11,6)],fill='#c5cbbd')
        p.polygon([(11,4),(14,2),(13,6)],fill='#e3dec5')
    elif rest:
        p.polygon([(8,6),(5,5),(9,3),(10,6)],fill='#bdc4b7')
        p.polygon([(11,6),(11,3),(13,5)],fill='#ccd1bf')
    else:
        p.polygon([(8,6),(7,0),(10,2),(11,6)],fill='#ccd1bf')
        p.polygon([(11,5),(12,0),(14,1),(13,6)],fill='#e0dfc8')
        p.line([(8,2),(9,4)],fill='#807d85')
        p.line([(13,1),(12,4)],fill='#969097')
    p.line([(11,7),(12,7)],fill='#626b75')
    p.point((14,7),fill='#343d48')
    p.line([(10,10),(13,10)],fill='#444953')
    if warn:
        p.rectangle((11,10,14,12),fill='#433c47')
        p.line([(12,11),(14,11)],fill='#d7ceac')
        p.point((13,12),fill='#d7ceac')
    legs=[(3,12,3,14),(8,12,9,14)] if trail else [(4,12,4,14),(9,12,9,14)]
    for line in legs: p.line(line,fill='#b0b8af')
    if rest: p.line([(9,12),(12,14)],fill='#84918f')
    im.save(root/(pose+'.png'))
