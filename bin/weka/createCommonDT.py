#!/usr/bin/env python

import os
from createDT import main as createDT
from common import parseArgs, getSelectedStudents, makeDirs, readStudents, makeTemp, writeData

def main(base,suffix,stayWeight,treeOptions,students,useWeka):
  makeDirs(base,False)
  header,studentData = readStudents(base,students)
  filename = makeTemp('.arff')
  try:
    writeData(header,studentData,filename)
    if len(suffix) > 0:
      if suffix[0] != '-':
        suffix = '-' + suffix
    dest = 'common%s' % (suffix)
    createDT(filename,base,dest,stayWeight,treeOptions,useWeka)
  finally:
    os.remove(filename)

if __name__ == '__main__':
  usage = '%prog [options] base [suffix] [-- treeOptions ...]'
  options,args,treeOptions = parseArgs(usage=usage,minArgs=1,maxArgs=2,studentOptions=False)
  base = args[0]
  if len(args) >= 2:
    suffix = args[1]
  else:
    suffix = ''
  stayWeight = None
  students = getSelectedStudents()
  main(base,suffix,stayWeight,treeOptions,students,options.useWeka)
