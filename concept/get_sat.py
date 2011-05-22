# -*- coding: utf-8 -*-

# H S V -> 0 0 S

import cv
import PIL.Image
import glob
import os.path

for fname in glob.glob('img/*.png'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2HSV)
    
    imgH, imgS, imgV = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    cv.Split(img, imgH, imgS, imgV, None)
    cv.Set(imgH, 0)
    cv.Set(imgV, 0)
    cv.Merge(imgH, imgV, imgS, None, img)
    
    cv.CvtColor(img, img, cv.CV_HSV2RGB)
    
    cv.SaveImage('out/'+os.path.basename(fname), img)
