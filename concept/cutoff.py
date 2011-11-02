# -*- coding: utf-8 -*-

# zeruje wartości na wszystkich kanałach przekraczających pewną wartość


import cv
import glob
import os.path
import sys

cutoff = int(sys.argv[1])
dir = sys.argv[2] or 'img'

thr = cv.CV_THRESH_TOZERO_INV # if cutoff>129 else cv.CV_THRESH_BINARY

try:
    os.mkdir('out_cut')
except:
    pass

for fname in glob.glob('%s/*.*'%dir):
    img = cv.LoadImageM(fname)
    
    
    imgB, imgG, imgR = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    
    cv.Split(img, imgB, imgG, imgR, None)
    
    for imgs in [imgR, imgG, imgB]:
        cv.Threshold(imgs, imgs, cutoff, 255, thr)
    
    cv.Merge(imgB, imgG, imgR, None, img)
    
    
    cv.SaveImage('out_cut/'+os.path.basename(fname), img)
