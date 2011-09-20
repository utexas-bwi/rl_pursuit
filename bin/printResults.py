#!/usr/bin/env python

import numpy, csv

def loadResults(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      iteration = row[0]
      stepsPerTrial = map(int,row[1:])
      numSteps.append(stepsPerTrial)
  numSteps = numpy.array(numSteps)
  return numSteps

def main(filenames):
  for filename in filenames:
    print filename
    numSteps = loadResults(filename)
    print numSteps.mean()

if __name__ == '__main__':
  import sys
  main(sys.argv[1:])
