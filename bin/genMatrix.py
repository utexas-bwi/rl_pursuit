#!/usr/bin/env python

import numpy
from weka.common import getUniqueStudents
from printResults import loadResults

dirName = 'condor/matrix'
students = getUniqueStudents()
#models = list(students)
models = list(students) + ['gr','ta','gp','pd']
resultDir = '%s/results/%s/%s'
metric = numpy.mean
#metric = len
#metric = lambda x: numpy.sum(x == 501) / float(len(x))
#def metric(x):
  #if (numpy.sum(x == 501) == 0):
    #return numpy.mean(x)
  #else:
    #return numpy.inf

print 'Columns are planning model'
print ',' + ','.join(models)

for student in students:
  s = '%s,' % student
  for model in models:
    currResultDir = resultDir % (dirName,student,model)
    results = loadResults(currResultDir)
    s += '%s,' % metric(results)
  print s[:-1]
