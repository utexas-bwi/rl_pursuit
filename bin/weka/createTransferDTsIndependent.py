#!/usr/bin/env python

import subprocess, os
from common import getUniqueStudents, getArch

studentFile = 'data/newStudents29.txt'

def main(targetDir,sourceDir,jobInd):
  students = getUniqueStudents(studentFile)
  i = -1
  for targetStudent in students:
    for sourceStudent in students:
      if (targetStudent == sourceStudent):
        continue
      i += 1
      if (jobInd is not None) and (i != jobInd):
        continue
      print targetStudent, sourceStudent
      cmd = ['bin/%s/boostIndependent' % getArch(),targetStudent,sourceStudent,targetDir,sourceDir]
      descFile = os.path.join(targetDir,'boostIndependent',targetStudent + '-' + sourceStudent + '.desc')
      subprocess.check_call(cmd,stdout=open(descFile,'w'))

def extractTree(inFile,outFile):
  with open(inFile,'r') as f:
    lines = f.readlines()
  startInd = [x.startswith('@relation') for x in lines].index(True)
  ignoreStart = lines.index('REPTree\n')
  ignoreEnd = ignoreStart + 3 # 3 lines later, including REPTree
  endInd = [x.startswith('Size of the tree : ') for x in lines].index(True) - 1
  with open(outFile,'w') as f:
    f.writelines(lines[startInd:ignoreStart])
    f.writelines(lines[ignoreEnd:endInd])

if __name__ == '__main__':
  import sys
  usage = 'Usage: createTransferTrees.py targetDir sourceDir [--only NUM]'
  args = sys.argv[1:]
  jobInd = None
  if ('-h' in args) or ('--help' in args):
    print usage
    sys.exit(0)
  if '--only' in args:
    ind = args.index('--only')
    jobInd = int(args[ind+1])
    args = args[:ind] + args[ind+2:]
  if len(args) != 2:
    print usage
    sys.exit(1)
  targetDir = args[0]
  sourceDir = args[1]
  main(targetDir,sourceDir,jobInd)

