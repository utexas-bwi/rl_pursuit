#!/usr/bin/env python

import os
from createDT import main as createDT

def main(basename,stayWeight,treeOptions):
  with open('data/students.txt','r') as f:
    students = set(f.read().split())

  dataDir = os.path.join('data','dt-train',basename)
  for student in students:
    print '-------------------'
    print student
    print '-------------------'
    dataFile = os.path.join(dataDir,'%s.arff'%student)
    createDT(dataFile,'%s-%s'%(basename,student),stayWeight,treeOptions)

if __name__ == '__main__':
  import sys
  usage = 'Usage: createStudentDTs.py basename [treeOptions ...]'
  args = sys.argv[1:]
  if len(args) < 1:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  basename = args[0]
  stayWeight = None
  treeOptions = args[1:]
  main(basename ,stayWeight,treeOptions)
