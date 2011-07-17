# -*- coding: utf-8 -*-

import glob, os.path, sys

import cv

for fil in ['canny', 'sobel']:
    try:
        os.mkdir('out_'+fil)
    except:
        pass

    files = glob.glob('img/*.*')


    for fname in files:
        
        img = cv.LoadImageM(fname)
        imgB, imgG, imgR = [cv.CreateMat(img.rows, img.cols, cv.CV_8U) for i in range(3)]
        
        cv.Split(img, imgB, imgG, imgR, None)
        
        if fil == 'canny':
            for im in [imgR, imgG, imgB]:
                cv.Canny(im, im, 100, 200, 3)
            cv.Merge(imgB, imgG, imgR, None, img)
        
        elif fil == 'sobel':
            cv.Split(img, imgB, imgG, imgR, None)
            imT, imT2 = [cv.CreateMat(img.rows, img.cols, cv.CV_32F) for i in [1,2]]
            for im in [imgR, imgG, imgB]:
                sz = -1
                cv.Sobel(im, imT, 1, 0, sz)
                cv.Sobel(im, imT2, 0, 1, sz)
                cv.CartToPolar(imT, imT2, imT)
                # skala sobela
                cv.ConvertScale(imT, im, 0.1)
        
        cv.Merge(imgB, imgG, imgR, None, img)
        
        cv.SaveImage('out_'+fil+'/'+os.path.basename(fname), img)
