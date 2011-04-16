
# Testuję, jak wyglądają te obrazki w palecie 
# YCbCr, a konkretnie jaki ma komponent Cb = niebieskość
# Y to jasność, Cr to czerwoność
# maluję negatywną niebieskość jako kolor czerwony 
# (bo w YCbCr negatywna niebieskość przy luminancji 0
# wygląda jak brzydki zielony)


import PIL.Image
import glob
import os.path

print PIL.Image.MODES
for fname in glob.glob('img/*.png'):
    img = PIL.Image.open(fname)
    img = img.convert('YCbCr')
    w = img.getbbox()[2]
    h = img.getbbox()[3]
    pd = img.load()
    for x in range(w):
        for y in range(h):
            px = pd[x, y]
            valb = max(px[1], 128)
            valy = min(px[1], 128)
            pd[x,y]=(0, valb, 256-valy)
            
    img = img.convert('RGB')
    img.save('out/'+os.path.basename(fname)+'.png')
