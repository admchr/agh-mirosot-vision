import subprocess, os.path, glob, sys

di = 'out_wh'

patterns = (
((33, 167), 9, di+'/[a-z]*'),
((108, 4), 3, di+'/wc*'),
((39, 9), 3, di+'/si*'),
((59, 177), 9, di+'/patches*')
)

try:
    os.mkdir('out_robo')
except:
    pass

subprocess.check_call(['make', 'main'], cwd='../src/')

for (center, radius, pattern) in patterns:
    for fname in glob.glob(pattern):
        args = ['../src/main', fname, 'out_robo/'+os.path.basename(fname), str(center[0]), str(center[1]), str(radius)]
        subprocess.Popen(args)
