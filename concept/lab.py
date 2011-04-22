# -*- coding: utf-8 -*-


# L ightness
# a green-magenta
# b blue-yellow

import cv
import PIL.Image
import glob
import os.path

for fname in glob.glob('img/*.png'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2Lab)
    
    imgL, imgA, imgB = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    """for x in range(img.cols):
        for y in range(img.rows):
            px=img[x,y]
            
            L2=abs(px[2]-128)*2
            b2=255 if px[2]>128 else 0
            img[x,y]=(L2, 128, b2)
    """
    
    cv.Split(img, imgL, imgA, imgB, None)
    cv.Set(imgL, 128)
    cv.Set(imgA, 128)
    #greenyellow-bluemagenta
    #red-blue
    
    cv.Merge(imgB, imgL, imgA, None, img)
    
    cv.CvtColor(img, img, cv.CV_Lab2RGB)
    
    cv.SaveImage('out/'+os.path.basename(fname), img)
