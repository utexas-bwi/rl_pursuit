#!/usr/bin/env python

import numpy
from printResults import loadAndProcessResults, parseArgs

def main(paths,options):
  assert(len(paths) == 2)
  results = loadAndProcessResults(paths,options)
  assert(set(paths) == set(results.keys()))
  
  # get results in trial order
  for k,[trials,numSteps] in results.iteritems():
    inds = numpy.argsort(trials)
    results[k] = [trials[inds],numSteps[inds]]
  
  trials1,numSteps1 = results[paths[0]]
  trials2,numSteps2 = results[paths[1]]
  assert((trials1 == trials2).all())
  diffs = numSteps1 - numSteps2
  if options.abs:
    diffs = numpy.abs(diffs)
  if options.fracDiff:
    diffs = diffs / (0.5 * (numSteps1 + numSteps2))
  print 'trial steps1 steps2  diff'
  print '-------------------------'
  for i in range(options.num):
    ind = numpy.argmax(diffs)
    print '%4i  %5i  %5i  %5s' % (trials1[ind],numSteps1[ind,0],numSteps2[ind,0],diffs[ind,0])
    diffs[ind] = -999999

  return 0

if __name__ == '__main__':
  import sys
  from optparse import make_option
  
  parserOptions = []
  parserOptions.append(make_option('-n','--number',action='store',dest='num',default=5,type='int',help='Number of top diffs to output'))
  parserOptions.append(make_option('--no-abs',action='store_false',dest='abs',default=True,help='Turn off using the absolute value, looks for diffs where first is bigger than second'))
  parserOptions.append(make_option('--fracDiff',action='store_true',dest='fracDiff',default=False,help='Order diffs by frac of diff to run length'))

  options,args = parseArgs(sys.argv[1:],parserOptions)
  sys.exit(main(args,options))
