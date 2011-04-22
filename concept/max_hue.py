# -*- coding: utf-8 -*-

import cv
import PIL.Image
import glob
import os.path

for fname in glob.glob('img/*.png'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2HSV)
    
    imgH, imgS, imgV = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    cv.Split(img, imgH, imgS, imgV, None)
    
    cv.Threshold(imgS, imgS, 0, 255, cv.CV_THRESH_BINARY)
    cv.Threshold(imgV, imgV, 0, 255, cv.CV_THRESH_BINARY)
    
    cv.Merge(imgH, imgS, imgV, None, img)
    
    cv.CvtColor(img, img, cv.CV_HSV2RGB)
    
    cv.SaveImage('out/'+os.path.basename(fname), img)
