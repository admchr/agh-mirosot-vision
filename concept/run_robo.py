import subprocess, os.path, glob, sys, PIL.Image



di = 'img'

patterns = (
(di+'/um*', 'config_um.txt'),
(di+'/wc*', 'config_wc.txt')
#(di+'/si*', 'config_si.txt'),
#(di+'/patches*')
)

try:
    os.mkdir('out_robo')
except:
    pass

subprocess.check_call(['make', 'driver'], cwd='../src')

for (pattern, config) in patterns:
    for fpath in glob.glob(pattern):
        args = ['../src/driver', fpath, config, 'tmp']
        subprocess.check_call(args)
        img = PIL.Image.open(fpath)
        w, h = img.getbbox()[2:4]
        
        files = glob.glob('tmp_*.png')
        files.sort()
        imgt = PIL.Image.new('RGB', (w, len(files)*h))
        for (i, outfile) in enumerate(files):
            try:
                imgo = PIL.Image.open(outfile)
                imgt.paste(imgo, (0, h*i))
            except IOError:
                raise Exception('could not open %s in %s'%(outfile, fpath))
            except IndexError:
                raise Exception('could not process %s in $s'%(outfile, fpath))
        fname = os.path.basename(fpath)
        fname_png = os.path.splitext(fname)[0]+'.png'
        imgt.save('out_robo/'+fname_png)
        