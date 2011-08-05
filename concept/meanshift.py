# -*- coding: utf-8 -*-

import glob, os, os.path, sys, multiprocessing

import cv

t='m'

try:
    os.mkdir('out_meanshift')
except:
    pass

files = glob.glob('out_wh/*.*')


def f(fn, to):
    # po co to kombinowanie? okazuje się, że implementacja mean shift 
    # psuje okoliczną pamięć (taki bug) i wywala interpreter przy zamknięciu
    img = cv.LoadImageM(fname)
    img2 = cv.LoadImageM(fname)
    cv.PyrMeanShiftFiltering(img2, img, 5, 20, termcrit=(cv.CV_TERMCRIT_ITER+cv.CV_TERMCRIT_EPS, 10, 1))
    cv.SaveImage(to, img)
    os.kill(os.getpid(), 9)

for fname in files:
    outf = 'out_meanshift/'+os.path.splitext(os.path.basename(fname))[0]+'.png'
    p = multiprocessing.Process(target=f, args=(fname, outf))
    p.start()
#   ponawianie nie daje żadnego nowego efektu
#    p.join()
#    p = multiprocessing.Process(target=f, args=(outf, outf))
#    p.start()
    
