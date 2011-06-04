# -*- coding: utf-8 -*-

#pokazuje płaszczyznę z YCbCr taką jaką ja tu teraz wykorzystuję 
import PIL.Image

print PIL.Image.MODES
img = PIL.Image.new('YCbCr', (256, 256))
for x in range(256):
    for y in range(256):
        img.putpixel((x,y), (0, 256-x, y))

img.show()


