# -*- coding: utf-8 -*-

# Testuję, jak wyglądają te obrazki w palecie 
# YCbCr, a konkretnie jaki ma komponent Cb = niebieskość
# Y to jasność, Cr to czerwoność
# maluję negatywną niebieskość jako kolor czerwony 
# (bo w YCbCr negatywna niebieskość przy luminancji 0
# wygląda jak brzydki zielony)


import cv
import glob
import os.path

try:
    os.mkdir('out_ycrcb')
except:
    pass


for fname in glob.glob('img/*.png'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2YCrCb)
    h = img.rows
    w = img.cols
    
    imgY, imgCB, imgCR = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    cv.Split(img, imgY, imgCR, imgCB, None)
    
    for x in xrange(w):
        for y in xrange(h):
            px=cv.Get2D(img, y, x)[1]
                
            valb = max(px, 128)
            valy = min(px, 128)
            cv.Set2D(imgY, y, x, 0)
            cv.Set2D(imgCR, y, x, valb)
            cv.Set2D(imgCB, y, x, 256-valy)
            
    
    cv.Merge(imgY, imgCR, imgCB, None, img)
    cv.CvtColor(img, img, cv.CV_YCrCb2RGB)
    cv.SaveImage('out_ycrcb/'+os.path.basename(fname), img)
