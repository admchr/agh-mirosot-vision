import subprocess, os.path, glob, sys

fin = sys.argv[1]
fout = sys.argv[2]
ttype = sys.argv[3]

try:
    os.mkdir(fout)
except:
    pass

subprocess.check_call(['make', 'linearize'], cwd='../src/')

for fname in glob.glob(fin+'/*.*'):
    subprocess.Popen(['../src/linearize', fname, fout+'/'+os.path.basename(fname), ttype])
