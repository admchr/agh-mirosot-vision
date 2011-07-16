# -*- coding: utf-8 -*-

import glob, os.path, sys

import cv

t='m'

try:
    os.mkdir('out_fil')
except:
    pass

files = glob.glob('img/*.*')


for fname in files:
    
    img = cv.LoadImageM(fname)
    #imgB, imgG, imgR = [cv.CreateMat(img.rows, img.cols, cv.CV_8U) for i in range(3)]
    
    print 'ok'
    if t == 'c':
        cv.Split(img, imgB, imgG, imgR, None)
        for im in [imgR, imgG, imgB]:
            cv.Canny(im, im, 100, 200, 3)
        cv.Merge(imgB, imgG, imgR, None, img)
    
    elif t == 's':
        cv.Split(img, imgB, imgG, imgR, None)
        imT, imT2 = [cv.CreateMat(img.rows, img.cols, cv.CV_32F) for i in [1,2]]
        for im in [imgR, imgG, imgB]:
            sz = -1
            cv.Sobel(im, imT, 1, 0, sz)
            cv.Sobel(im, imT2, 0, 1, sz)
            cv.CartToPolar(imT, imT2, imT)
            cv.ConvertScale(imT, im, 0.1)
            cv.Merge(imgB, imgG, imgR, None, img)
        cv.Merge(imgB, imgG, imgR, None, img)
    elif t == 'm':
        print fname
        #img2 = cv.LoadImageM(fname)
        cv.PyrMeanShiftFiltering(img, img, 10, 20, 1)
    
    
    cv.SaveImage('out_fil/'+os.path.basename(fname), img)
    