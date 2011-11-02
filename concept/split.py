# -*- coding: utf-8 -*-

# H S V -> H 255 255

import cv
import glob
import os.path
import sys

for chan in ('mh', 'h', 's', 'v', 'r', 'g', 'b', 'ch'):
    
    hsv = chan in ('mh', 'h', 's', 'v', 'ch')
    dname = 'out_'+chan+'/'
    
    
    try:
        os.mkdir(dname)
    except:
        pass

    for fname in glob.glob(sys.argv[1]+'/*.*'):
        img = cv.LoadImageM(fname)
        
        if hsv:
            cv.CvtColor(img, img, cv.CV_BGR2HSV)
        
        imgH, imgS, imgV = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
        
        cv.Split(img, imgH, imgS, imgV, None)
        
        if chan == 'h':
            (imgV, imgS, imgH) = (imgH, imgS, imgV)
        elif chan == 's':
            (imgV, imgS, imgH) = (imgS, imgH, imgV)
        elif chan == 'v':
            (imgV, imgS, imgH) = (imgV, imgH, imgS)
        elif chan == 'r':
            (imgV, imgS, imgH) = (imgV, imgH, imgS)
        elif chan == 'g':
            (imgV, imgS, imgH) = (imgS, imgH, imgV)
        elif chan == 'b':
            (imgV, imgS, imgH) = (imgH, imgS, imgV)
        elif chan == 'ch':
            cv.Mul(imgS, imgV, imgV, 1/256.0)
            (imgH, imgS, imgV) = (imgH, imgS, imgV)
        
        if chan != 'mh':
            cv.Set(imgH, 0)
            cv.Set(imgS, 0)
        else:
            cv.Set(imgS, 200)
            cv.Set(imgV, 200)
        
        cv.Merge(imgH, imgS, imgV, None, img)
        
        cv.CvtColor(img, img, cv.CV_HSV2BGR)
        
        cv.SaveImage(dname+os.path.basename(fname), img)
     
