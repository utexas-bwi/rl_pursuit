#!/usr/bin/env python

from createDT import main as createDT
from common import getUniqueStudents, getFilename, TRAIN

def main(basename,stayWeight,treeOptions,useWeka,studentInd):
  students = getUniqueStudents()
  
  for i,student in enumerate(students):
    if studentInd is not None:
      if i != studentInd:
        continue
    print '-------------------'
    print student
    print '-------------------'
    dataFile = getFilename(basename,student,TRAIN)
    createDT(dataFile,basename,'only-%s' % student,stayWeight,treeOptions,useWeka)

if __name__ == '__main__':
  import sys
  usage = 'Usage: createStudentDTs.py basename [--weka] [--only studentInd] [treeOptions ...]'
  args = sys.argv[1:]
  useWeka = False
  studentInd = None
  if '--weka' in args:
    args.remove('--weka')
    useWeka = True
  if '--only' in args:
    ind = args.index('--only')
    studentInd = int(args[ind+1])
    args = args[:ind] + args[ind+2:]
  if len(args) < 1:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  basename = args[0]
  stayWeight = None
  treeOptions = args[1:]
  main(basename ,stayWeight,treeOptions,useWeka,studentInd)
