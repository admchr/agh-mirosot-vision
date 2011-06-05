# -*- coding: utf-8 -*-

# sprawdzam, ile pikseli boiska nadaje się do samplowania

# H S V -> 0 0 V*value

import cv
import PIL.Image
import glob
import os.path
import sys

try:
    os.mkdir('out_v')
except:
    pass

try:
    mul = float(sys.argv[1])
except:
    raise Exception('usage: get_v_mul.py mnożnik')
    

for fname in glob.glob('img/*.*'):
    img = cv.LoadImageM(fname)
    
    cv.CvtColor(img, img, cv.CV_RGB2HSV)
    
    imgH, imgS, imgV = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    cv.Split(img, imgH, imgS, imgV, None)
    
    cv.Set(imgH, 0)
    cv.Set(imgS, 0)
    cv.ConvertScale(imgV, imgV, mul)
    cv.Merge(imgH, imgS, imgV, None, img)
    
    cv.CvtColor(img, img, cv.CV_HSV2RGB)
    
    cv.SaveImage('out_v/'+os.path.basename(fname), img)
