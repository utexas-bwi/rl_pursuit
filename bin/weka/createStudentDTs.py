#!/usr/bin/env python

from createDT import main as createDT
from common import getUniqueStudents, getFilename, TRAIN, parseArgs

def main(basename,dataDir,stayWeight,treeOptions,options):
  students = getUniqueStudents()
  
  for i,student in enumerate(students):
    if options.studentInd is not None:
      if i != options.studentInd:
        continue
    print '-------------------'
    print student
    print '-------------------'
    dataFile = getFilename(dataDir,student,TRAIN)
    createDT(dataFile,basename,'only-%s' % student,stayWeight,treeOptions,options.useWeka,options.numInstances,options.randomTree,options.numRandomTrees,options.featureFrac,options.resampleFrac)

if __name__ == '__main__':
  from optparse import make_option
  opts = []
  opts.append(make_option('--only',action='store',dest='studentInd',default=None,help='only run for specified student',metavar='NUM',type='int'))
  opts.append(make_option('--dataDir',action='store',dest='dataDir',default=None,help='data dir',metavar='DIR'))

  opts.append(make_option('--random',action='store_true',dest='randomTree',default=False))
  opts.append(make_option('--featureFrac',action='store',dest='featureFrac',default=0.8,type='float'))
  opts.append(make_option('--numRandomTrees',action='store',dest='numRandomTrees',default=10,type='int'))
  opts.append(make_option('--resampleFrac',action='store',dest='resampleFrac',default=0.5,type='float'))

  usage = 'Usage: createStudentDTs.py [options] baseDir [-- treeOptions ...]'
  options,args,treeOptions = parseArgs(usage=usage,numArgs=1,studentOptions=False,options=opts,numInstancesFlag=True)
  baseDir = args[0]
  if options.dataDir is None:
    options.dataDir = baseDir
  stayWeight = None
  main(baseDir,options.dataDir,stayWeight,treeOptions,options)
