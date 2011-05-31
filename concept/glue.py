# -*- coding: utf-8 -*-


import PIL.Image
import glob
import os.path
import sys

fl = sys.argv[1:]


print fl

try:
    os.mkdir('out')
except:
    pass
for fpath in glob.glob(fl[0]+'/*.png'):
    fname = os.path.basename(fpath)
    img = PIL.Image.open(fpath)
    w = img.getbbox()[2]
    h = img.getbbox()[3]
    
    tw = int(len(fl)**0.5+0.999)
    th = int(len(fl)*1.0/tw+0.999)
    try:
        imgt = PIL.Image.new('RGB', (tw*w, th*h))
        for i in range(len(fl)):
            img2 = PIL.Image.open(fl[i]+'/'+fname)
            imgt.paste(img2, (w*(i%tw), h*(i//tw)))
        imgt.save('out/'+fname)
    except IOError:
        # brak obrazka oznacza, że niwe ma z czym sklejać
        pass
