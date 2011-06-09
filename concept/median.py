# -*- coding: utf-8 -*-

import glob, os.path

import cv


try:
    os.mkdir('out_med')
except:
    pass

files = glob.glob('img/*.*')

med=16*5+1

for fname in files:
    img0 = cv.LoadImageM(fname)
    cv.Smooth(img0, img0, smoothtype=cv.CV_MEDIAN, param1=med, param2=med)
    cv.SaveImage('out_med/'+os.path.basename(fname), img0)
