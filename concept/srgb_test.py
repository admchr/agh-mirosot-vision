# -*- coding: utf-8 -*-

#pokazuje płaszczyznę z YCbCr taką jaką ja tu teraz wykorzystuję 
import PIL.Image

print PIL.Image.MODES

def put_gr(sx, sy, tx, ty, mul):
    (stepx, stepy, mulx, muly) = (2*mul, 2*mul, mul, mul)
    
    for x in range(sx, sx+tx, stepx):
        for y in range(sy, sy+ty, stepy):
            for mx in range(mulx):
                for my in range(muly):
                    img.putpixel((x+mx, y+my), (255, 255, 255))

mul=50
s=500

img = PIL.Image.new('RGB', (2*s, 2*s))

for (posx, posy) in ((0,0),(0, s),(s, 0),(s,s)):
    if posy != 0:
        put_gr(posx, posy, s, s, mul)
    if posx != 0:
        put_gr(posx, posy + mul, s, s, mul)
    if posx != 0:
        put_gr(posx + mul, posy , s, s, mul)
    #put_gr(posx + mul, posy + mul, s, s, mul)

img.save("ch%d.png"%mul) 
