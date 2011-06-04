import PIL.Image
import glob
import os.path

print PIL.Image.MODES
for fname in glob.glob('img/*.bmp'):
    img = PIL.Image.open(fname)
    img.save('img/'+os.path.splitext(os.path.basename(fname))[0]+'.png')
