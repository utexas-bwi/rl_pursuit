#!/usr/bin/env python

import subprocess, time, multiprocessing, os
from createDT import main as createDT

def main(numTrials,stayWeight,treeOptions):
  with open('data/aamas11students.txt','r') as f:
    students = set(f.read().split())

  dataDir = os.path.join('data','dt-train',str(numTrials))
  for student in students:
    print '-------------------'
    print student
    print '-------------------'
    dataFile = os.path.join(dataDir,'%s.arff'%student)
    createDT(dataFile,'%s-%i'%(student,numTrials),stayWeight,treeOptions)

if __name__ == '__main__':
  import sys
  usage = 'Usage: createStudentDTs.py numTrials [treeOptions ...]'
  args = sys.argv[1:]
  if len(args) < 1:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  numTrials = int(args[0])
  stayWeight = None
  treeOptions = args[1:]
  main(numTrials,stayWeight,treeOptions)
