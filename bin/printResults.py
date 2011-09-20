#!/usr/bin/env python

import numpy, csv

def loadResults(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      #iteration = row[0]
      stepsPerTrial = map(int,row[1:])
      numSteps.append(stepsPerTrial)
  numSteps = numpy.array(numSteps)
  return numSteps

def main(filenames,sameResult):
  if sameResult:
    numSteps = None
    for filename in filenames:
      res = loadResults(filename)
      if numSteps is None:
        numSteps = res
      else:
        numSteps = numpy.hstack((numSteps,res))
    print numSteps.size,numSteps.mean()
  else:
    for filename in filenames:
      print filename
      numSteps = loadResults(filename)
      print numSteps.size,numSteps.mean()

if __name__ == '__main__':
  import sys
  startInd = 1
  sameResult = False
  if sys.argv[1] in ['-h','--help']:
    print 'Usage: printResults.py [-a] result1.csv [result2.csv]'
    print '  -a treats all the result files as if they come from a single result'
    sys.exit(0)
  if sys.argv[1] == '-a':
    startInd = 2
    sameResult = True
  main(sys.argv[startInd:],sameResult)
