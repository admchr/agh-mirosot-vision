# -*- coding: utf-8 -*-

import glob, os.path

import cv


try:
    os.mkdir('out_wh')
except:
    pass

files = glob.glob('img/*.png')


pts_wc = (
    (108,4),
    (306, 6),
    (607, 7),
    (590, 411),
    (302, 408),
    (111, 404)
)

pts_si = (
(39,9),
(474,11),
(488,367),
(36, 370)
)

for fname in files:
    img = cv.LoadImageM(fname)
    cv.Smooth(img, img, smoothtype=cv.CV_MEDIAN, param1=3, param2=3)
    
    imgA, imgB, imgC = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    cv.Split(img, imgA, imgB, imgC, None)
    
    pts = pts_wc if 'wc' in fname else pts_si
    
    r=g=b=0
    for (posx, posy) in pts:
        b+=cv.Get2D(imgA, posy, posx)[0]
        g+=cv.Get2D(imgB, posy, posx)[0]
        r+=cv.Get2D(imgC, posy, posx)[0]
    
    n = len(pts)
    r/=n
    g/=n
    b/=n
    cv.ConvertScale(imgA, imgA, 128/b)
    cv.ConvertScale(imgB, imgB, 128/g)
    cv.ConvertScale(imgC, imgC, 128/r)
    
    cv.Merge(imgA, imgB, imgC, None, img)
    
    print ('%4d %4d %4d %s' % (r, g, b, fname))
    
    
    cv.SaveImage('out_wh/'+os.path.basename(fname), img)
    