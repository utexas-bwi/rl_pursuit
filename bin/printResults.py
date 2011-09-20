#!/usr/bin/env python

import numpy, csv

def main(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      iteration = row[0]
      stepsPerTrial = map(int,row[1:])
      numSteps.append(stepsPerTrial)
  numSteps = numpy.array(numSteps)
  print numpy.mean(numSteps)

if __name__ == '__main__':
  import sys
  main(sys.argv[1])
