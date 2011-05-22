# -*- coding: utf-8 -*-

# sprawdzam, ile pikseli boiska nadaje się do samplowania

# H S V -> 0 0 V*value

import cv
import PIL.Image
import glob
import os.path

files = glob.glob('img/wc_robots*.png')

poss = (
(433, 50, 'żłóty najwyższy '),
(436, 58, 'zielonkawy najwyższy '),
(441, 47, 'różowy najwyższy '),
(443, 55, 'jasnoniebieski najwyższy ')
)

imgz={}
for fname in files:
    imgz[fname] = cv.LoadImageM(fname)

for (posx, posy, desc) in poss:
    print desc
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
        
        r=cv.Get2D(imgA, posy, posx)[0]
        g=cv.Get2D(imgB, posy, posx)[0]
        b=cv.Get2D(imgC, posy, posx)[0]
        
        cv.Set2D(imgA, posy, posx, 255)
        cv.Set2D(imgB, posy, posx, 255)
        cv.Set2D(imgC, posy, posx, 255)
        
        cv.Merge(imgA, imgB, imgC, None, img)
        
        print "%5.1f %5.1f %5.1f, %5.1f %5.1f %5.1f    %s"%(r, g, b, h, s, v, fname) 

for fname in files:
    cv.SaveImage('out/'+os.path.basename(fname), imgz[fname])
