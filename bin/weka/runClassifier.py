#!/usr/bin/env python

import subprocess, sys
from common import getArch, baseExists, getFilename,TRAIN
from trainClassifier import parseArgs as parseTrainArgs

def parseArgs(args):
  from optparse import make_option
  option = make_option('--test',action='store',dest='numTest',type='int',default=None,help='num test instances')
  options,_ = parseTrainArgs(args,parserOptions=[option])
  
  assert(options.numTest is not None),'numTest unspecified'
  options.testBase = options.base % options.numTest
  if not(baseExists(options.testBase)):
    print >>sys.stderr,'Dir for numTest doesn\'t exist at:',options.testBase
    sys.exit(2)
  return options

def main(args = sys.argv[1:]):
  options = parseArgs(args)
  testFile = getFilename(options.testBase,options.student,TRAIN)
  cmd = ['bin/%s/runClassifier' % getArch(),options.saveConfigFilename,testFile,str(options.numTest),'--notrain']
  subprocess.check_call(cmd)

if __name__ == '__main__':
  main()
