#!/usr/bin/env python

import numpy, csv, os

def loadResults(path):
  if os.path.isfile(path):
    return loadResultsFromFile(path)
  else:
    res = loadResultsFromDirectory(path)
    if res is None:
      res = loadResultsFromDirectory(os.path.join(path,'results'))
    return res

def loadResultsFromFile(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      #iteration = row[0]
      stepsPerTrial = map(int,row[1:])
      numSteps.append(stepsPerTrial)
  numSteps = numpy.array(numSteps)
  return numSteps

def validResultFile(filename):
  _,ext =  os.path.splitext(filename)
  return ext == '.csv'

def loadResultsFromDirectory(directory):
  tempFilenames = [os.path.join(directory,x) for x in os.listdir(directory)]
  filenames = filter(validResultFile,tempFilenames)
  if len(filenames) == 0:
    return None
  return loadResultsFromFileSet(filenames)

def loadResultsFromFileSet(filenames):
  numSteps = None
  for filename in filenames:
    res = loadResults(filename)
    if numSteps is None:
      numSteps = res
    else:
      numSteps = numpy.hstack((numSteps,res))
  return numSteps

def main(paths):
  for path in paths:
    print path
    numSteps = loadResults(path)
    print numSteps.size,numSteps.mean()
  #for filenameList in filenames:
    #filenameList = flatten(map(getFilenames,filenameList))
    #numSteps = loadResultsFromFileSet(filenames)
    #print numSteps.size,numSteps.mean()
  #else:
    #for filename in filenames:
      #print filename
      #numSteps = loadResults(filename)
      #print numSteps.size,numSteps.mean()

if __name__ == '__main__':
  import sys
  startInd = 1
  sameResult = False
  if sys.argv[1] in ['-h','--help']:
    print 'Usage: printResults.py result1.csv [result2.csv]'
    print 'NOTE: can take directories or files'
    #print '  -a treats all the result files as if they come from a single result'
    sys.exit(0)
  #if sys.argv[1] == '-a':
    #startInd = 2
    #sameResult = True

  main(sys.argv[startInd:])
