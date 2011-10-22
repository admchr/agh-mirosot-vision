import subprocess, os.path, glob, sys, cv, threading



di = 'img'

patterns = (
(di+'/um*', 'config_um.txt'),
(di+'/wc*', 'config_wc.txt'),
(di+'/si*', 'config_si.txt'),
(di+'/patches*', 'config_pa.txt')
)

def md(d):

    try:
        os.mkdir(d)
    except:
        pass
md('out_robo')
for i in range(6):
    md('out_robo_%d'%i)
md('out_robo_split')

subprocess.check_call(['make', 'driver'], cwd='../src')


def run_for_type(pattern, config):
    stdin = []
    for fpath in glob.glob(pattern):
        fname = os.path.basename(fpath)
        fname_noext = os.path.splitext(fname)[0]
        fname_out = 'out_robo_split/' + fname
        stdin.append([fpath, fname_out])
    f = open('/dev/null', 'r')
    proc = subprocess.Popen(['../src/driver'], stdin=subprocess.PIPE, stdout=f, stderr=f)
    stdin_str = ('%d %s\n'%(len(stdin), config))+'\n'.join([' '.join(line) for line in stdin])
    
    proc.communicate(stdin_str)
      
    for (fpath, fname_out) in stdin:
        img = cv.LoadImageM(fpath)
        w, h = cv.GetSize(img)
        
        fname = os.path.basename(fpath)
        fname_noext = os.path.splitext(fname)[0]
        
        files = glob.glob('%s_*.png'%fname_out)
        files.sort()
        imgt = cv.CreateMat(len(files)*h, w, cv.CV_8UC3)
        for (i, outfile) in enumerate(files):
            try:
                imgo = cv.LoadImageM(outfile)
                imgt_frag = cv.GetSubRect(imgt, (0, h*i, w, h))
                
                cv.Copy(imgo, imgt_frag)
                
                os.rename(outfile, 'out_robo_%s/%s.png'%(outfile.split('_')[-1][0],fname_noext))
            except IOError:
                raise Exception('could not open %s in %s'%(outfile, fpath))
            except IndexError:
                raise Exception('could not process %s in $s'%(outfile, fpath))
        
        cv.SaveImage('out_robo/%s.png' % fname_noext, imgt)
        
for (pattern, config) in patterns:
    #t = threading.Thread(target=run_for_type, args=(pattern, config))
    #t.start()
    run_for_type(pattern, config)
    