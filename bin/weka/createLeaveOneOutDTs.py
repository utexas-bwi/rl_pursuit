#!/usr/bin/env python

import os
from createDT import main as createDT
from common import parseArgs, getSelectedStudents, makeDirs, readStudents, makeTemp, writeData

def processStudent(base,dest,i,header,studentData,treeOptions,stayWeight):
  filename = makeTemp('.arff')
  try:
    writeData(header,studentData,filename,i)
    createDT(filename,base,dest,stayWeight,treeOptions)
  finally:
    os.remove(filename)

def main(base,suffix,stayWeight,treeOptions,students):
  makeDirs(base,False)
  header,studentData = readStudents(base,students)
  for i,student in enumerate(students):
    if len(suffix) > 0:
      if suffix[0] != '-':
        suffix = '-' + suffix
    dest = 'leaveOneOut-%s%s' % (student,suffix)
    print '-------------------'
    print student
    print '-------------------'
    processStudent(base,dest,i,header,studentData,treeOptions,stayWeight)

if __name__ == '__main__':
  usage = 'createLeaveOneOutDTs.py base [suffix] [-- treeOptions ...]'
  options,args,treeOptions = parseArgs(usage=usage,minArgs=1,maxArgs=2)
  base = args[0]
  if len(args) >= 2:
    suffix = args[1]
  else:
    suffix = ''
  stayWeight = None
  students = getSelectedStudents(includeStudents=options.includeStudents,excludeStudents=options.excludeStudents)
  main(base,suffix,stayWeight,treeOptions,students)
