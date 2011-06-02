# -*- coding: utf-8 -*-


import cv
import glob
import os.path

try:
    os.mkdir('out_th')
except:
    pass

files = glob.glob('out_wh/wc_robots*.png') 

bad = (255, 0, 0)

skip = 1

constr = [
#[{'h>':80, 'h<':110, 's>':200, 'v>':80}, bad],
[{'h>':90, 'h<':100, 's>':240, 'v>':100}, (255, 255, 0)],
#[{'h>':1, 'h<':30, 's>':200, 'v>':50}, bad],
[{'h>':8, 'h<':22, 's>':210, 'v>':60}, (0, 0, 255)],
[{'h>':110, 'h<':120, 's>':240, 'v>':100}, (255, 128, 0)],
]
for ctr in constr:
    ctr[0]=tuple(ctr[0].items())
constr = tuple([tuple(l) for l in constr])

for fname in files:
    img0 = cv.LoadImageM(fname)
    img = cv.LoadImageM(fname)

    cv.CvtColor(img0, img0, cv.CV_RGB2HSV)
    
    for y in range(0, img.rows, skip):
        for x in range(0, img.cols, skip):
            (h, s, v, tmp) = cv.Get2D(img0, y, x)
            for (c, color) in constr:
                ok=True
                for t, val in c:
                    if (
                        (t == 'h>' and h<=val) or
                        (t == 'h<' and h>=val) or
                        (t == 's>' and s<=val) or
                        (t == 's<' and s>=val) or
                        (t == 'v>' and v<=val) or
                        (t == 'v<' and v>=val)
                        ):
                        ok=False
                if ok:
                    cv.Set2D(img, y, x, (color[2], color[1], color[0]))

    cv.SaveImage('out_th/'+os.path.basename(fname), img)    
