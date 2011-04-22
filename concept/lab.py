# -*- coding: utf-8 -*-

# zamienia wskaźnik b* z L*a*b* na jasność; jedyny w miarę sensowny sposób
# wizualizacji
# L a b -> b 128 128


# L ightness
# a green-magenta
# b blue-yellow
# ( w praktyce to nie wygląda aż tak ładnie)
# + Lab się potem może nie skonwertować na RGB! (inny gamut)
import cv
import PIL.Image
import glob
import os.path

for fname in glob.glob('img/*.png'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2Lab)
    
    imgL, imgA, imgB = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    cv.Split(img, imgL, imgA, imgB, None)
    cv.Set(imgL, 128)
    cv.Set(imgA, 128)
    
    cv.Merge(imgB, imgL, imgA, None, img)
    cv.CvtColor(img, img, cv.CV_Lab2RGB)
    
    cv.SaveImage('out/'+os.path.basename(fname), img)
