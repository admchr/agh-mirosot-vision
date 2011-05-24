# -*- coding: utf-8 -*-


import cv
import glob
import os.path

try:
    os.mkdir('out_px')
except:
    pass

files = glob.glob('img/wc_robots*.png')

poss = (
(441, 47, 'różowy najwyższy '),
(436, 58, 'zielonkawy najwyższy '),
(544, 347, 'żółty prawy dół'),
(177, 364, 'żółty lewy dół'),
(327, 116, 'środek góra żółty'),
(184, 364, 'jasnożółty lewy dół'),
(433, 50, 'jasnożłóty najwyższy '),
(443, 55, 'jasnoniebieski najwyższy '),
(508, 80, 'jasnoniebieski górny prawy'),
(500, 82, 'niebieski górny prawy'),
(533, 351, 'niebieski prawy dół'),
(505, 72, 'niebieski góra prawy'),
(317, 120, 'środek góra niebieski'),
(184, 357, '(inny?) niebieski lewy dół'),
)

imgz={}
for fname in files:
    img0 = cv.LoadImageM(fname)
    img = cv.LoadImageM(fname)
    imgz[fname] = img
    cv.Smooth(img0, img, smoothtype=cv.CV_MEDIAN, param1=3, param2=3)

for (posx, posy, desc) in poss:
    print posx, posy, desc
    print ' r      g     b,     h     s     v'
    for fname in files:
        img = imgz[fname]
        # print fname
        
        imgA, imgB, imgC = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
        cv.CvtColor(img, img, cv.CV_RGB2HSV)
        
        cv.Split(img, imgA, imgB, imgC, None)
        
        h=cv.Get2D(imgA, posy, posx)[0]
        s=cv.Get2D(imgB, posy, posx)[0]
        v=cv.Get2D(imgC, posy, posx)[0]
        
        cv.Merge(imgA, imgB, imgC, None, img)
        
        cv.CvtColor(img, img, cv.CV_HSV2RGB)
        
        cv.Split(img, imgA, imgB, imgC, None)
        
        b=cv.Get2D(imgA, posy, posx)[0]
        g=cv.Get2D(imgB, posy, posx)[0]
        r=cv.Get2D(imgC, posy, posx)[0]
        
        cv.Set2D(imgA, posy, posx, 255)
        cv.Set2D(imgB, posy, posx, 255)
        cv.Set2D(imgC, posy, posx, 255)
        
        cv.Merge(imgA, imgB, imgC, None, img)
        
        print "%5d %5d %5d, %5d %5d %5d    %s"%(r, g, b, h, s, v, fname) 

for fname in files:
    cv.SaveImage('out_px/'+os.path.basename(fname), imgz[fname])
