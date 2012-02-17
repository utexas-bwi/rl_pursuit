#!/usr/bin/env python

import subprocess, os
from common import getUniqueStudents, getArch

studentFile = 'data/newStudents29.txt'

def main(targetDir,sourceDir,destDir,numTargetInstances,jobInd):
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
      cmd = ['bin/%s/boostIndependent' % getArch(),targetStudent,sourceStudent,targetDir,sourceDir,str(numTargetInstances)]
      descFile = os.path.join(destDir,'boostIndependent',targetStudent + '-' + sourceStudent + '.desc')
      subprocess.check_call(cmd,stdout=open(descFile,'w'))

if __name__ == '__main__':
  import sys
  usage = 'Usage: createTransferTrees.py targetDir sourceDir destDir numTargetInstances [--only NUM]'
  args = sys.argv[1:]
  jobInd = None
  if ('-h' in args) or ('--help' in args):
    print usage
    sys.exit(0)
  if '--only' in args:
    ind = args.index('--only')
    jobInd = int(args[ind+1])
    args = args[:ind] + args[ind+2:]
  if len(args) != 4:
    print usage
    sys.exit(1)
  targetDir = args[0]
  sourceDir = args[1]
  destDir = args[2]
  numTargetInstances = int(args[3])
  main(targetDir,sourceDir,destDir,numTargetInstances,jobInd)

