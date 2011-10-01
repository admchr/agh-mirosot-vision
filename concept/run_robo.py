import subprocess, os.path, glob, sys, cv, threading



di = 'img'

patterns = (
(di+'/um*', 'config_um.txt'),
(di+'/wc*', 'config_wc.txt'),
(di+'/si*', 'config_si.txt'),
(di+'/patches*', 'config_pa.txt')
)

try:
    os.mkdir('out_robo')
except:
    pass
try:
    os.mkdir('out_robo_split')
except:
    pass

subprocess.check_call(['make', 'driver'], cwd='../src')


def run_for_type(pattern, config):
    stdin = []
    for fpath in glob.glob(pattern):
        fname = os.path.basename(fpath)
        fname_noext = os.path.splitext(fname)[0]
        fname_out = 'out_robo_split/' + fname_noext
        stdin.append([fpath, config, fname_out])
    
    proc = subprocess.Popen(['../src/driver'], stdin=subprocess.PIPE)
    stdin_str = ('%d\n'%len(stdin))+'\n'.join([' '.join(line) for line in stdin])
    
    proc.communicate(stdin_str)
    
    for (fpath, config, fname_out) in stdin:
        img = cv.LoadImageM(fpath)
        w, h = cv.GetSize(img)
        
        files = glob.glob('%s_*.png'%fname_out)
        files.sort()
        imgt = cv.CreateMat(len(files)*h, w, cv.CV_8UC3)
        for (i, outfile) in enumerate(files):
            try:
                imgo = cv.LoadImageM(outfile)
                imgt_frag = cv.GetSubRect(imgt, (0, h*i, w, h))
                cv.Copy(imgo, imgt_frag)
            except IOError:
                raise Exception('could not open %s in %s'%(outfile, fpath))
            except IndexError:
                raise Exception('could not process %s in $s'%(outfile, fpath))
        
        fname = os.path.basename(fpath)
        fname_noext = os.path.splitext(fname)[0]
        cv.SaveImage('out_robo/%s.png' % fname_noext, imgt)
        
for (pattern, config) in patterns:
    t = threading.Thread(target=run_for_type, args=(pattern, config))
    t.start()
    