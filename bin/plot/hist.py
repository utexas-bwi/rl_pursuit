#!/usr/bin/env python

import numpy
import matplotlib.pyplot as plt

def createBins(results,numBins,exponent=1.0):
  minVal = results.min()
  maxVal = results.max()
  minVal = minVal ** (1.0 / exponent)
  maxVal = maxVal ** (1.0 / exponent)
  bins = [minVal]
  val = minVal
  for i in range(numBins):
    val += float(maxVal-minVal) / numBins
    bins.append(val)
  bins = [x ** exponent for x in bins]
  bins[-1] += 1
  return bins

def readFile(filename):
  numSteps = []
  with open(filename,'r') as f:
    for line in f:
      job,steps = map(int,line.split(','))
      numSteps.append(steps)
  return numpy.array(numSteps)

def main(filenames):
  results = []
  for filename in filenames:
    results.append(readFile(filename))
  allResults = numpy.hstack(results)
  bins = createBins(allResults,10,3.0)
  plt.hist(numpy.transpose(numpy.vstack(results)),bins=bins,label=filenames)#,log=True)
  plt.legend()
  plt.show()

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  main(args)
