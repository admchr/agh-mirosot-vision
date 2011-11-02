# -*- coding: utf-8 -*-

import glob, os.path, sys

import cv


try:
    os.mkdir('out_med')
except:
    pass
try:
    os.mkdir('out_sub')
except:
    pass

files = glob.glob('img/*.*')

med=int(sys.argv[1])

for fname in files:
    img = cv.LoadImageM(fname)
    img_med = cv.LoadImageM(fname)
    cv.Smooth(img_med, img_med, smoothtype=cv.CV_MEDIAN, param1=med, param2=med)
    cv.SaveImage('out_med/'+os.path.basename(fname), img_med)
    
    cv.Sub(img, img_med, img)
    cv.SaveImage('out_sub/'+os.path.basename(fname), img)
