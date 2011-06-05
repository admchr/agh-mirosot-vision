# -*- coding: utf-8 -*-

# H S V -> H 255 255

import cv
import glob
import os.path

try:
    os.mkdir('out_h')
except:
    pass

for fname in glob.glob('img/*.*'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2HSV)
    
    imgH, imgS, imgV = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    cv.Split(img, imgH, imgS, imgV, None)
    
    cv.Set(imgS, 200)
    cv.Set(imgV, 200)
    
    cv.Merge(imgH, imgS, imgV, None, img)
    
    cv.CvtColor(img, img, cv.CV_HSV2RGB)
    
    cv.SaveImage('out_h/'+os.path.basename(fname), img)
