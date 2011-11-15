#!/usr/bin/env python

import subprocess
import tempfile, os
from quickSetup import main as quickSetup

def main(args,removeConfig,additionalArgs):
  fd,filename = tempfile.mkstemp(dir='configs/quick',suffix='.json')
  try:
    print filename
    os.close(fd)
    quickSetup(args+[filename])
    cmd = ['bin/main'] + additionalArgs + [filename]
    print ' '.join(cmd)
    subprocess.check_call(cmd)
  finally:
    if removeConfig:
      os.remove(filename)

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  additionalArgs = []
  removeConfig = True
  if '--no-remove' in args:
    removeConfig = False
    args.remove('--no-remove')
  if '---' in args:
    ind = args.index('---')
    additionalArgs = args[ind + 1:]
    args = args[:ind]
  main(args,removeConfig,additionalArgs)
