import subprocess, os.path, glob, sys

try:
    fin = sys.argv[1]
    fout = sys.argv[2]
    ttype = sys.argv[3]
except:
    print 'usage: dir_in dir_out lin|delin'
    sys.exit(0)

try:
    os.mkdir(fout)
except:
    pass

subprocess.check_call(['make', 'linearize'])

for fname in glob.glob(fin+'/*.*'):
    subprocess.Popen(['./linearize', fname, fout+'/'+os.path.basename(fname), ttype])
