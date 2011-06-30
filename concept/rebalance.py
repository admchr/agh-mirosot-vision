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
    (607, 12),
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

avg=False

for fname in files:
    img = cv.LoadImageM(fname)
    img0 = cv.LoadImageM(fname)
    cv.Smooth(img0, img0, smoothtype=cv.CV_MEDIAN, param1=5, param2=5)
    
    imgA, imgB, imgC = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    cv.Split(img, imgA, imgB, imgC, None)
    
    pts = pts_wc if 'wc' in fname else pts_si
    
    vals={}
    sr=sg=sb=0
    for (posx, posy) in pts:
        b=cv.Get2D(img0, posy, posx)[0]
        g=cv.Get2D(img0, posy, posx)[1]
        r=cv.Get2D(img0, posy, posx)[2]
        vals[(posx, posy)]=(r,g,b)
        sr+=r
        sg+=g
        sb+=b
    
    if avg:
        n = len(pts)
        sr/=n
        sg/=n
        sb/=n
        cv.ConvertScale(imgA, imgA, 128/sb)
        cv.ConvertScale(imgB, imgB, 128/sg)
        cv.ConvertScale(imgC, imgC, 128/sr)
    else:
        def d(p, q):
            s=0
            for i in [0,1]:
                s+=(p[i]-q[i])**2
            return s**0.5
        tile=32
        for y in range(0, img.rows-tile, tile):
            for x in range(0, img.cols-tile, tile):
                wsum=0
                r=g=b=0
                for p in pts:
                    val=vals[p]
                    weight=1.0/(d(p, (x+tile/2, y+tile/2))**2+1)
                    wsum+=weight
                    r+=weight*val[0]
                    g+=weight*val[1]
                    b+=weight*val[2]
                r/=wsum
                g/=wsum
                b/=wsum
                subA=cv.GetSubRect(imgA, (x, y, tile, tile))
                subB=cv.GetSubRect(imgB, (x, y, tile, tile))
                subC=cv.GetSubRect(imgC, (x, y, tile, tile))
                cv.ConvertScale(subA, subA, 128/b)
                cv.ConvertScale(subB, subB, 128/g)
                cv.ConvertScale(subC, subC, 128/r)
                
    cv.Merge(imgA, imgB, imgC, None, img)
    
    print ('%4d %4d %4d %s' % (r, g, b, fname))
    
    
    cv.SaveImage('out_wh/'+os.path.basename(fname), img)
    