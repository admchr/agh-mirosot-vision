# -*- coding: utf-8 -*-


import cv
import glob
import os.path

try:
    os.mkdir('out_px')
except:
    pass

files = glob.glob('img/wc_robots*.png')

col_blue = 'blue'
col_lblue = 'lblue'
col_yellow = 'yellow'
col_lyellow = 'lyellow'
col_red = 'red?'
col_green = 'green'
col_white = 'white'
col_unknown = '?'

color_mapping = {
    col_red : (255, 0, 0),
    col_green : (0, 255, 0),
    col_blue : (0, 0, 255),
    col_lblue : (128, 128, 255),
    col_yellow : (255, 255, 0),
    col_lyellow : (255, 255, 128),
    col_white : (255, 255, 255)
}


poss_l = (
((441, 47), col_red, 'różowy najwyższy '),
((436, 58), col_green, 'zielonkawy najwyższy '),
((544, 347), col_yellow,'żółty prawy dół'),
((177, 364), col_yellow, 'żółty lewy dół'),
((327, 116), col_yellow, 'środek góra żółty'),
((500, 82), col_yellow, 'górny prawy'),
((184, 364), col_lyellow, 'lewy dół'),
((433, 50), col_lyellow, 'najwyższy'),
((443, 55), col_lblue, 'najwyższy'),
((508, 80), col_lblue, 'górny prawy'),
((533, 351), col_blue, 'prawy dół'),
((505, 72), col_blue, 'góra prawy'),
((317, 120), col_blue, 'środek góra'),
((184, 357), col_blue, 'lewy dół'),
((108,4), col_white, 'góra lewy'),
((306, 6), col_white, 'góra'),
((607, 7), col_white, 'góra prawy'),
((590, 411), col_white, 'dół prawy'),
((302, 408), col_white, 'dół'),
((111, 404), col_white, 'dół lewy')
)
samples = {
    i: {'pos':poss_l[i][0], 'col':poss_l[i][1], 'desc':poss_l[i][2]} 
        for i in range(len(poss_l))
    }


data={}

imgz={}
for fname in files:
    img0 = cv.LoadImageM(fname)
    img = cv.LoadImageM(fname)
    imgz[fname] = img
    cv.Smooth(img0, img, smoothtype=cv.CV_MEDIAN, param1=3, param2=3)


for fname in files:
    img = imgz[fname]
    # print fname
    
    imgA, imgB, imgC = [cv.CreateMat(img.rows, img.cols, cv.CV_8UC1) for i in [1,2,3]]
    cv.CvtColor(img, img, cv.CV_RGB2HSV)
    
    cv.Split(img, imgA, imgB, imgC, None)
    
    for i, s in samples.iteritems():
        d = data[fname,i] = {}
        (posx, posy) = s['pos']
        d['h']=cv.Get2D(imgA, posy, posx)[0]
        d['s']=cv.Get2D(imgB, posy, posx)[0]
        d['v']=cv.Get2D(imgC, posy, posx)[0]
    
    cv.Merge(imgA, imgB, imgC, None, img)
    
    cv.CvtColor(img, img, cv.CV_HSV2RGB)
    
    cv.Split(img, imgA, imgB, imgC, None)
    
    for i, s in samples.iteritems():
        d = data[fname,i]
        (posx, posy) = s['pos']
        d['b']=cv.Get2D(imgA, posy, posx)[0]
        d['g']=cv.Get2D(imgB, posy, posx)[0]
        d['r']=cv.Get2D(imgC, posy, posx)[0]
        
        fill_name = s['col']
        if fill_name in color_mapping:
            fill = color_mapping[fill_name]
        else:
            fill = (255, 255, 255)
        
        cv.Set2D(imgA, posy, posx, fill[2])
        cv.Set2D(imgB, posy, posx, fill[1])
        cv.Set2D(imgC, posy, posx, fill[0])
        
    cv.Merge(imgA, imgB, imgC, None, img)
    

for fname in files:
    cv.SaveImage('out_px/'+os.path.basename(fname), imgz[fname])


for i, s in samples.iteritems():
    print "%s %s %s"%(s['pos'], s['col'], s['desc'])
    
    minimum = dict(data[files[0],i])
    maximum = dict(data[files[0],i])
    for fname in files:
        d = data[fname,i]
        for k in d:
            minimum[k]=min(d[k], minimum[k])
            maximum[k]=max(d[k], maximum[k])
    
    for d in (minimum, maximum):
        print "%5d %5d %5d, %5d %5d %5d"%(d['r'], d['g'], d['b'], d['h'], d['s'], d['v'])
    print ' r      g     b,     h     s     v' 
    for fname in files:
        d=data[fname,i]
        print "%5d %5d %5d, %5d %5d %5d    %s"%(d['r'], d['g'], d['b'], d['h'], d['s'], d['v'], fname[14:-3]) 

"""    print posx, posy, desc
    """