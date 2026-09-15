"""Sparse ice spider silhouettes, raised weaving legs and contiguous silk tiles."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name,pose in [('icicle_spider',0),('spider_weave',1),('spider_bite',2),('spider_rest',3)]:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    for flip in [-1,1]:
        for i in range(3):
            ox=7+flip*(4+i%2); oy=4+i*3
            if pose==1: oy-=2
            if pose==3: ox-=flip;oy+=1
            p.line([(7+flip*2,7+i),(ox,oy),(ox+flip,oy+2)],fill='#6b939b')
    p.polygon([(5,5),(7,3),(10,5),(10,8),(8,10),(5,8)],fill='#8fbbbe')
    p.polygon([(6,5),(7,4),(8,5),(7,7)],fill='#d3dccc')
    p.rectangle((6,9,9,11),fill='#637a80')
    p.point((6,10),fill='#e4d49b');p.point((9,10),fill='#e4d49b')
    end=14 if pose==2 else 12
    p.line([(6,11),(5,end)],fill='#ced7c5');p.line([(9,11),(10,end)],fill='#ced7c5')
    im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(0,8),(4,7),(8,8),(12,7),(15,8)],fill='#799999')
p.line([(4,7),(4,10)],fill='#adb9b1');p.line([(12,7),(12,9)],fill='#bcc6b9')
im.save(root/'spider_strand.png')
im.transpose(Image.Transpose.ROTATE_90).save(root/'spider_strand_v.png')
