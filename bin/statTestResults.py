#!/usr/bin/env python

'''
File: statTestResults.py
Author: Samuel Barrett
Description: runs a paired students t test on the means of 2 results
Created:  2011-10-31
Modified: 2011-10-31
'''

import numpy, csv, sys
from scipy import stats
'''
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
'''


def loadResultsFromFile(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      iteration = row[0]
      assert(int(iteration) == len(numSteps)),'Missing or out of order results'
      stepsPerTrial = map(int,row[1:])
      numSteps.append(stepsPerTrial)
  numSteps = numpy.array(numSteps)[:,0]
  return numSteps

def main(args):
  if len(args) != 2:
    print >>sys.stderr,'Incorrect number of arguments'
    print >>sys.stderr,'Expected %i, but got %i' % (2,len(args))
    sys.exit(1)
  resultsPath1 = args[0]
  resultsPath2 = args[1]
  a = loadResultsFromFile(resultsPath1)
  b = loadResultsFromFile(resultsPath2)
  #diffs = a - b
  print 'Num Episodes:',len(a)
  print 'Mean of %s is %f' % (resultsPath1,a.mean())
  print 'Mean of %s is %f' % (resultsPath2,b.mean())
  z,p = stats.wilcoxon(a,b)
  print 'Wilcoxon z-statistic: %g' % z
  print 'Wilcoxon p-value: %g' % p
  t,p = stats.ttest_rel(a,b)
  print 'TTest t-statistic: %g' % t
  print 'TTest p-value: %g' % p
  #if diffs.mean() < 0:
    #print '%s is smaller' % resultsPath1
  #else:
    #print '%s is smaller' % resultsPath2

if __name__ == '__main__':
  main(sys.argv[1:])

