#!/usr/bin/env python

import numpy
from weka.common import getUniqueStudents
from printResults import loadResults
from optparse import OptionParser

def mean(_,x):
  return numpy.mean(x)

def meanPenalized(opts,x):
  return numpy.mean(x) + opts.penalty * numpy.sum(x == opts.maxLength)

def meanInf(opts,x):
  if (numpy.sum(x == opts.maxLength) == 0):
    return numpy.mean(x)
  else:
    return numpy.inf

def fracFinished(opts,x):
  return 1.0 - numpy.sum(x == opts.maxLength) / float(len(x))

validMetrics = [mean,meanPenalized,meanInf,fracFinished]

def main(metric):
  dirName = 'condor/matrix'
  students = getUniqueStudents()
  #models = list(students)
  models = list(students) + ['gr','ta','gp','pd']
  resultDir = '%s/results/%s/%s'

  print 'Columns are planning model'
  print ',' + ','.join(models)

  for student in students:
    s = '%s,' % student
    for model in models:
      currResultDir = resultDir % (dirName,student,model)
      results = loadResults(currResultDir)
      s += '%s,' % metric(results)
    print s[:-1]

def addMetricParser(parser,validMetrics):
  validMetricNames = [x.__name__ for x in validMetrics]
  parser.add_option('-m','--metric',action='store',dest='metric',type='str',default='mean',help='metric to evaluate runs, options are: %s' % ' '.join(validMetricNames))

def getMetric(opts,validMetrics,passOpts=True):
  validMetricNames = [x.__name__ for x in validMetrics]
  assert(opts.metric in validMetricNames),'Invalid metric: %s, allowed set: %s' % (opts.metric,' '.join(validMetricNames))
  metricInd = validMetricNames.index(opts.metric)
  if passOpts:
    metric = lambda x: validMetrics[metricInd](opts,x)
  else:
    metric = validMetrics[metricInd]
  return metric

if __name__ == '__main__':
  parser = OptionParser('%prog [options]')
  addMetricParser(parser,validMetrics)
  parser.add_option('--maxLength',action='store',dest='maxLength',type='int',default=501,help='max length of an episode')
  parser.add_option('--penalty',action='store',dest='penalty',type='int',default=1000,help='penalty for an unfinished episode')

  opts,args = parser.parse_args()
  assert(len(args) == 0),'Incorrect number of args'
  metric = getMetric(opts,validMetrics)
  main(metric)
