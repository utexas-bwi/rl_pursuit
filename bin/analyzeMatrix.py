#!/usr/bin/env python

import sys, csv, itertools, numpy, scipy
from optparse import OptionParser
from genMatrix import addMetricParser, getMetric

validMetrics = [numpy.mean,numpy.max]

def calcVal(results,current,metric):
  vals = results[:,current]
  mins = numpy.min(vals,axis=1)
  return metric(mins)

def calcBest(results,models,numStudentsToChoose,metric):
  best = None
  bestVal = numpy.inf
  for current in itertools.combinations(range(len(models)),numStudentsToChoose):
    val = calcVal(results,current,metric)
    if val < bestVal:
      best = current
      bestVal = val
  return bestVal,best,[models[x] for x in best]

def main(filename,metric,opts):
  reader = csv.reader(open(filename,'r'),delimiter=',')
  reader.next() # ignore first line
  header = reader.next()
  origModels = header[1:]
  students = origModels[:-4]
  if opts.useHC:
    models = list(origModels)
  else:
    models = list(students)

  results = numpy.zeros([len(students),len(models)])
  for i,row in enumerate(reader):
    if len(origModels) != len(row) - 1:
      print 'Bad Size:',len(origModels),len(row)-1
      sys.exit(2)
    for j,v in enumerate(row[1:len(models)+1]):
      results[i,j] = float(v)

  bestVal,bestInds,bestModels = calcBest(results,models,opts.numStudentsToChoose,metric)
  #print 'Overall best: %s %s' % (bestVal,bestModels)
  print 'Overall,%s' % (','.join(bestModels))
  for i,student in enumerate(students):
    tempResults = scipy.delete(results,i,0)
    tempResults = scipy.delete(tempResults,i,1)
    tempModels = list(models)
    del tempModels[i]
    bestVal,bestInds,bestModels = calcBest(tempResults,tempModels,opts.numStudentsToChoose,metric)
    #print 'Leave one out %s: %s %s' % (student,bestVal,bestModels)
    print '%s,%s' % (student,','.join(bestModels))
    sys.stdout.flush()

if __name__ == '__main__':
  parser = OptionParser('%prog [options] inCSV')
  addMetricParser(parser,validMetrics)
  parser.add_option('--hc',action='store_true',dest='useHC',default=True)
  parser.add_option('--no-hc',action='store_false',dest='useHC',default=True)
  parser.add_option('-n','--numToChoose',action='store',dest='numStudentsToChoose',default=4,type='int')
  
  opts,args = parser.parse_args()
  assert(len(args) == 1),'Incorrect number of args, expected 1'
  filename = args[0]
  metric = getMetric(opts,validMetrics,passOpts=False)
  main(filename,metric,opts)
