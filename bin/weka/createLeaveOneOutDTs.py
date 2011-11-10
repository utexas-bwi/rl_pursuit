#!/usr/bin/env python

import os, sys, tempfile
from copy import copy
from createDT import main as createDT

def main(dataBasename,stayWeight,treeOptions):
  with open('data/students.txt','r') as f:
    students = list(set(f.read().split()))
  
  dataDir = os.path.join('data','dt-train',dataBasename)
  studentFiles = [os.path.join(dataDir,'%s.arff' % student) for student in students]
  # make sure all of the student files exist before starting
  for studentFile in studentFiles:
    if not(os.path.exists(studentFile)):
      print >>sys.stderr,'Student file missing: %s' % studentFile
      sys.exit(1)
  # read in all of the files
  lines = []
  header = None
  for studentFile in studentFiles:
    with open(studentFile,'r') as f:
      temp = f.readlines()
      ind = temp.index('@data\n') + 1 # get the line where the data starts
      if header is None:
        header = temp[:ind]
      lines.append(temp[ind:])
  # do it
  for i,student in enumerate(students):
    fd,filename = tempfile.mkstemp('.arff')
    f = os.fdopen(fd,'w')
    try:
      print '-------------------'
      print student
      print '-------------------'
      contents = header + sum(lines[:i] + lines[i+1:],[])
      f.writelines(contents)
      f.close()
      createDT(filename,'leaveOneOut-%s-%s'%(student,dataBasename),stayWeight,treeOptions)
    finally:
      f.close()
      os.remove(filename)


if __name__ == '__main__':
  usage = 'Usage: createLeaveOneOutDTs.py dataBasename [treeOptions ...]'
  args = sys.argv[1:]
  if len(args) < 1:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  dataBasename = args[0]
  stayWeight = None
  treeOptions = args[1:]
  main(dataBasename,stayWeight,treeOptions)
