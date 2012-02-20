#!/usr/bin/env python

from createDT import main as createDT
from common import getUniqueStudents, getFilename, TRAIN, parseArgs

def main(basename,dataDir,stayWeight,treeOptions,useWeka,studentInd,numInstances):
  students = getUniqueStudents()
  
  for i,student in enumerate(students):
    if studentInd is not None:
      if i != studentInd:
        continue
    print '-------------------'
    print student
    print '-------------------'
    dataFile = getFilename(dataDir,student,TRAIN)
    createDT(dataFile,basename,'only-%s' % student,stayWeight,treeOptions,useWeka,numInstances)

if __name__ == '__main__':
  from optparse import make_option
  opts = []
  opts.append(make_option('--only',action='store',dest='studentInd',default=None,help='only run for specified student',metavar='NUM',type='int'))
  opts.append(make_option('--dataDir',action='store',dest='dataDir',default=None,help='data dir',metavar='DIR'))

  usage = 'Usage: createStudentDTs.py [options] baseDir [-- treeOptions ...]'
  options,args,treeOptions = parseArgs(usage=usage,numArgs=1,studentOptions=False,options=opts,numInstancesFlag=True)
  baseDir = args[0]
  if options.dataDir is None:
    options.dataDir = baseDir
  stayWeight = None
  main(baseDir,options.dataDir,stayWeight,treeOptions,options.useWeka,options.studentInd,options.numInstances)
