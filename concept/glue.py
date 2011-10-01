# -*- coding: utf-8 -*-


import cv
import glob
import os.path
import sys

fl = sys.argv[1:]


print fl

try:
    os.mkdir('out')
except:
    pass
for fpath in glob.glob(fl[0]+'/*.*'):
    fname = os.path.basename(fpath)
    img = cv.LoadImageM(fpath)
    sz = cv.GetSize(img)
    w = sz[0]
    h = sz[1]
    
    tw = int(len(fl)**0.5+0.999)
    th = int(len(fl)*1.0/tw+0.999)
    imgt = cv.CreateMat(th*h, tw*w, cv.CV_8UC3)
    cv.SetZero(imgt)
    fname_png = os.path.splitext(fname)[0]+'.png'
    for i in range(len(fl)):
        for fn in [fname, fname_png]:
            try:
                img2 = cv.LoadImageM(fl[i]+'/'+fn)
                imgt_s = cv.GetSubRect(imgt, (w*(i%tw), h*(i//tw), w, h))
                cv.Copy(img2, imgt_s)
                break
            except IOError:
                # brak obrazka oznacza, że nie ma z czym sklejać
                pass

    cv.SaveImage('out/'+fname_png, imgt)