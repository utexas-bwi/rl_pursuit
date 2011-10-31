#!/usr/bin/env python

'''
File: statTestResults.py
Author: Samuel Barrett
Description: runs a paired students t test on the means of 2 results
Created:  2011-10-31
Modified: 2011-10-31
'''

import numpy, csv
from scipy import stats

def studentTTest(mu1,sd1,n1,mu2,sd2,n2):
  sxx = ((sd1 ** 2) / float(n1) + (sd2 ** 2) / float(n2)) ** 0.5
  t = (mu1 - mu2) / sxx
  
  dof = (sxx ** 2) / ((((sd1 ** 2) / n1) ** 2) / (n1 - 1.0) + (((sd2 ** 2) / n2) ** 2) / (n2 - 1.0))
  #print sxx,t,dof
  #print stats.t.cdf(t,dof)

  p = stats.t.cdf(t,dof)
  if p > 0.5:
    p = 1 - p
  return p

def pairedStudentsTTest(data1,data2):
  vals = numpy.array([d1-d2 for d1,d2 in zip(data1,data2)])
  mu = vals.mean()
  stdev = vals.std()
  n = len(vals)
  adjn = n - 1
  t = mu / (stdev / (adjn ** 0.5))
  if t > 0:
    t = -t
  dof = n - 1
  p = stats.t.cdf(t,dof)
  return 2*p

def loadResultsFromFile(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      iteration = row[0]
      assert(int(iteration) == len(numSteps)),'Missing or out of order results'
      stepsPerTrial = map(int,row[1:])
      numSteps.append(stepsPerTrial)
  numSteps = numpy.array(numSteps)
  return numSteps

def main(args):
  assert(len(args) == 2),'Incorrect number of arguments'
  resultsPath1 = args[0]
  resultsPath2 = args[1]
  a = loadResultsFromFile(resultsPath1)
  b = loadResultsFromFile(resultsPath2)
  
  print 'Num Episodes:',len(a)
  print 'prob equal = %f' % pairedStudentsTTest(a,b)
  if a.mean() < b.mean():
    print '%s is smaller' % args[0]
  else:
    print '%s is smaller' % args[1]
  print 'Mean of %s is %f' % (args[0],a.mean())
  print 'Mean of %s is %f' % (args[1],b.mean())

if __name__ == '__main__':
  import sys
  main(sys.argv[1:])

