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
for fpath in glob.glob(fl[0]+'/*.*'):
    fname = os.path.basename(fpath)
    img = PIL.Image.open(fpath)
    w = img.getbbox()[2]
    h = img.getbbox()[3]
    
    tw = int(len(fl)**0.5+0.999)
    th = int(len(fl)*1.0/tw+0.999)
    imgt = PIL.Image.new('RGB', (tw*w, th*h))
    fname_png = os.path.splitext(fname)[0]+'.png'
    for i in range(len(fl)):
        for fn in [fname, fname_png]:
            try:
                img2 = PIL.Image.open(fl[i]+'/'+fn)
                imgt.paste(img2, (w*(i%tw), h*(i//tw)))
                break
            except IOError:
                # brak obrazka oznacza, że nie ma z czym sklejać
                pass

    imgt.save('out/'+fname_png)