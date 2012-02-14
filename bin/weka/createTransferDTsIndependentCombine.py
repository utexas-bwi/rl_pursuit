#!/usr/bin/env python

import subprocess, os
from common import getUniqueStudents, getArch

studentFile = 'data/newStudents29.txt'

def main(targetDir,prefix,jobInd):
  TOP_NUM = 10
  students = getUniqueStudents(studentFile)
  i = -1
  for targetStudent in students:
    i += 1
    if (jobInd is not None) and (i != jobInd):
      continue
    orderedVals = []
    orderedStudents = []
    for sourceStudent in students:
      if (targetStudent == sourceStudent):
        continue
      descFile = os.path.join(targetDir,'boostIndependent',targetStudent + '-' + sourceStudent + '.desc')
      with open(descFile,'r') as f:
        line = f.readlines()[-1].strip()
      val,student = line.split(' ')
      val = float(val)
      assert(student == sourceStudent)
      if val > 1e-10:
        i = 0
        for i,v in enumerate(orderedVals):
          if val > v:
            break
        else:
          i = len(orderedVals)
        orderedVals.insert(i,val)
        orderedStudents.insert(i,sourceStudent)
    print targetStudent
    print '  ',orderedVals
    print '  ',orderedStudents
    print '  ',len(orderedVals)
    print '\n'
    outFile = os.path.join(targetDir,'desc',prefix + '-' + targetStudent + '.desc')
    if os.path.exists(outFile):
      print >>sys.stderr,'FILE EXISTS:',outFile
      sys.exit(1)
    with open(outFile,'w') as f:
      for v,s in zip(orderedVals[:TOP_NUM],orderedStudents[:TOP_NUM]):
        f.write('%s %s\n' % (v,s))
      f.write('TwoStageTrAdaBoost\n')

      #subprocess.check_call(cmd,stdout=open(descFile,'w'))

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
  usage = 'Usage: createTransferTrees.py targetDir prefix [--only NUM]'
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
  prefix = args[1]
  main(targetDir,prefix,jobInd)

