# -*- coding: utf-8 -*-


import cv, sys, numpy
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as patch
img = cv.LoadImageM(sys.argv[1])
imgh = cv.LoadImageM(sys.argv[1])
cv.CvtColor(img, img, cv.CV_BGR2RGB)
cv.CvtColor(imgh, imgh, cv.CV_BGR2HSV)

prcx0, prcy0, prcx1, prcy1 = [float(x) for x in sys.argv[2:2+4]]
#w, h = img.cols, img.rows
w, h = 1, 1
# posx0, posy0, posx1, posy1 = prcx0/w, prcy0/h, prcx1/w, prcy1/h


(vr, vg, vb, vh, vs, vv, vch, vl) = [[] for i in range(8)]

px, py = -1, -1

for i in range(0, 1000):
    j = i*0.001
    x = int((prcx0+(prcx1-prcx0)*j)*w)
    y = int((prcy0+(prcy1-prcy0)*j)*w)
    if (px, py) == (x, y):
        continue
    (r, g, b, tmp) = cv.Get2D(img, y, x)
    (h, s, v, tmp) = cv.Get2D(imgh, y, x)
    for (vec, ch) in ((vb, b), (vg, g), (vr, r), (vh, h), (vs, s), (vv, v)):
        vec.append(ch)
    vch.append(s*v/256)
    vl.append((r+g+b)/3)
    cv.Set2D(img, y, x, (255, 255, 255))
    px, py=x, y
    

imgn = numpy.asarray(img)
plt.imshow(imgn, interpolation='nearest')
fig = plt.figure()
ax = fig.add_subplot(2,1,1)
bx = fig.add_subplot(2,1,2)
k=range(0, len(vr))
ax.grid(True)
ax.plot(k, vr, color='r')
ax.plot(k, vg, color='g')
ax.plot(k, vb, color='b')

for (i, r, g, b) in zip(k, vr, vg, vb):
    ax.add_artist(patch.Rectangle((i, 0), 1, 10, color='#%.2x%.2x%.2x'%(r,g,b)))


for h in range(6):
    for step in range(60):
        dh = step/60.0
        col = {
            'r':[1, 1-dh, 0, 0, dh, 1],
            'g':[dh, 1, 1, 1-dh, 0, 0],
            'b':[0, 0, dh, 1, 1, 1-dh]
        }
        bx.add_artist(patch.Rectangle((0, (h*60+step)/2), 1, 1, color='#%.2x%.2x%.2x'%tuple(col[c][h]*255 for c in 'rgb')))

bx.plot(k, vh, color='#000000')
bx.plot(k, vch, color='r')
bx.plot(k, vl, color='#888800')
bx.grid(True)
plt.show()


