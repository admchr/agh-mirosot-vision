# -*- coding: utf-8 -*-

import cv
h=20
w=180
img = cv.CreateMat(h, w, cv.CV_8UC3)
for y in range(h):
    for x in range(w):
        v=255
        
        if (y > 10) and (x%60==0):
            v=0
        if (y > 15) and (x%30==0):
            v=0
        cv.Set2D(img, y, x, (x, 255, v))

cv.CvtColor(img, img, cv.CV_HSV2BGR)
cv.SaveImage("rainbow.png", img)