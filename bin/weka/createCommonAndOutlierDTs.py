#!/usr/bin/env python

import os, sys, tempfile, multiprocessing
from copy import copy
from createDT import main as createDT

def process(dataBasename,name,lines,stayWeight,treeOptions,useWeka):
  fd,filename = tempfile.mkstemp('.arff')
  f = os.fdopen(fd,'w')
  try:
    print '-------------------'
    print name
    print '-------------------'
    f.writelines(lines)
    f.close()
    createDT(filename,'%s-%s'%(dataBasename,name),stayWeight=stayWeight,treeOptions=treeOptions,useWeka=True)
  finally:
    f.close()
    os.remove(filename)

def readFile(studentFile):
  with open(studentFile,'r') as f:
    temp = f.readlines()
  ind = temp.index('@data\n') + 1 # get the line where the data starts
  return temp[:ind],temp[ind:]

def main(dataBasename,stayWeight,outliers,treeOptions,useWeka):
  # get the students
  with open('data/students.txt','r') as f:
    students = list(set(f.read().split()))
  
  dataDir = os.path.join('data','dt-train',dataBasename)
  # get the filenames
  commonFiles = []
  outlierFiles = []
  for student in students:
    filename = os.path.join(dataDir,'%s.arff' % student)
    if student in outliers:
      outlierFiles.append(filename)
    else:
      commonFiles.append(filename)

  assert(len(outlierFiles) == len(outliers))

  # make sure all of the student files exist before starting
  for studentFile in outlierFiles + commonFiles:
    if not(os.path.exists(studentFile)):
      print >>sys.stderr,'Student file missing: %s' % studentFile
      sys.exit(1)
  
  # read in the files and create the trees
  commonData = []
  for filename in commonFiles:
    header,lines = readFile(filename)
    commonData.extend(lines)
  commonData = header + commonData
  process(dataBasename,'common',commonData,stayWeight,treeOptions,useWeka)

  for filename in outlierFiles:
    student = os.path.splitext(os.path.basename(filename))[0]
    header,lines = readFile(filename)
    process(dataBasename,'outlier-%s' % student,header+lines,stayWeight,treeOptions,useWeka)

if __name__ == '__main__':
  usage = 'Usage: createLeaveOneOutDTs.py dataBasename [outlier1 ...] [-- treeOptions ...]'
  args = sys.argv[1:]
  useWeka = False
  if '--weka' in args:
    args.remove('--weka')
    useWeka = True
  if len(args) < 1:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  try:
    ind = args.index('--')
  except:
    ind = len(args)

  dataBasename = args[0]
  stayWeight = None
  outliers = args[1:ind]
  treeOptions = args[ind+1:]
  main(dataBasename,stayWeight,outliers,treeOptions,useWeka)
