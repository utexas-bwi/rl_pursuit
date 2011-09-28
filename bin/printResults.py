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
      try:
        numSteps = numpy.vstack((numSteps,res))
      except:
        pass
  return numSteps

def printResults(episodeLengths,label):
  #for i in range(len(episodeLengths)):
    #for j in range(len(episodeLengths[i])):
      #if episodeLengths[i][j] > 1000:
        #episodeLengths[i][j] = 1000
  print '-----------------------------------'
  print label
  if episodeLengths is None:
    print 'Num episodes = ',0
    return
  print 'Num episodes = ',len(episodeLengths)
  print 'mean=',numpy.mean(episodeLengths)
  print 'means=',numpy.mean(episodeLengths,0)
  print 'median=',numpy.median(episodeLengths)
  print 'std=',numpy.std(episodeLengths)
  print 'min,max=',numpy.min(episodeLengths),numpy.max(episodeLengths)

def main(paths):
  for path in paths:
    numSteps = loadResults(path)
    printResults(numSteps,path)
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
