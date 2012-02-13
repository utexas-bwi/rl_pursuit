#!/usr/bin/env python

import subprocess, os
from common import getUniqueStudents, getArch

studentFile = 'data/newStudents29.txt'
sourceData = 4000

def main(targetDir,sourceDir,prefix,studentInd):
  students = getUniqueStudents(studentFile)
  for i,student in enumerate(students):
    if (studentInd is not None) and (i != studentInd):
      continue
    print '-------------------'
    print student
    print '-------------------'
    cmd = ['bin/%s/boostTest' % getArch(),student,studentFile,targetDir,sourceDir,str(sourceData)]
    descFile = os.path.join(targetDir,'desc',prefix + '-' + student + '.desc')
    resultFile = os.path.join(targetDir,'weighted',prefix + '-' + student + '.weka')
    subprocess.check_call(cmd,stdout=open(descFile,'w'))
    extractTree(descFile,resultFile)

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
  usage = 'Usage: createTransferTrees.py targetDir sourceDir prefix [--only NUM]'
  args = sys.argv[1:]
  studentInd = None
  if ('-h' in args) or ('--help' in args):
    print usage
    sys.exit(0)
  if '--only' in args:
    ind = args.index('--only')
    studentInd = int(args[ind+1])
    args = args[:ind] + args[ind+2:]
  if len(args) != 3:
    print usage
    sys.exit(1)
  targetDir = args[0]
  sourceDir = args[1]
  prefix = args[2]
  main(targetDir,sourceDir,prefix,studentInd)
