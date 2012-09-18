#!/usr/bin/env python

import sys, re, os, numpy
from trainClassifier import mainOptions as trainClassifierMain
from trainClassifier import parseArgs

def main(args=sys.argv[1:]):
  args = ['twostagetradaboost-partial'] + args
  combine = False
  if '--combine' in args:
    args.remove('--combine')
    combine = True
    args = args + ['--ignorePartialMax']

  options,_ = parseArgs(args)
  directory = os.path.join('configs/learners/saved/twostage-partial',options.baseLearner,options.student)
  pathBase = os.path.join(directory,'%i.txt')
  if combine:
    print options.student,options.partialMax
    bestT = None
    bestError = numpy.inf
    for t in range(options.partialMax):
      path = pathBase % t
      with open(path,'r') as f:
        error = float(f.read().strip())
      if error < bestError:
        bestError = error
        bestT = t
    options.partialInd = bestT
    print 'bestT:',bestT
    trainClassifierMain(options)
  else:
    args = args + ['--no-save','--catchOutput']
    options,_ = parseArgs(args)
    output,error = trainClassifierMain(options)
    res = re.findall('BEST T: (\d+)\nBEST ERROR: ([.0-9]+)',output)
    bestT = int(res[0][0])
    bestError = float(res[0][1])
    
    try:
      os.makedirs(directory)
    except:
      pass
    filename = pathBase % bestT
    with open(filename,'w') as f:
      f.write('%f\n'%bestError)

if __name__ == '__main__':
  main()
