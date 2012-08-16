#!/usr/bin/env python

import sys, csv, itertools, numpy, scipy
from optparse import OptionParser
from genMatrix import addMetricParser, getMetric
from multiprocessing import Pool

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

def calcBestWrapper(args):
  return calcBest(*args)

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
      print >>sys.stderr,'Bad Size:',len(origModels),len(row)-1
      sys.exit(2)
    for j,v in enumerate(row[1:len(models)+1]):
      results[i,j] = float(v)
  
  # get the arguments we want to call
  args = []
  args.append((results,models,opts.numStudentsToChoose,metric))
  for i,student in enumerate(students):
    tempResults = scipy.delete(results,i,0)
    tempResults = scipy.delete(tempResults,i,1)
    tempModels = list(models)
    del tempModels[i]
    args.append((tempResults,tempModels,opts.numStudentsToChoose,metric))
  if opts.multi:
    pool = Pool()
    res = pool.map(calcBestWrapper,args)
  else:
    res = map(calcBestWrapper,args)
  for student,(bestVal,bestInds,bestModels) in zip(['Overall'] + students,res):
    print '%s,%s' % (student,','.join(bestModels))

  #analysis = []
  #bestVal,bestInds,bestModels = calcBest(results,models,opts.numStudentsToChoose,metric)
  ##print 'Overall best: %s %s' % (bestVal,bestModels)
  #print 'Overall,%s' % (','.join(bestModels))
  #for i,student in enumerate(students):
    #tempResults = scipy.delete(results,i,0)
    #tempResults = scipy.delete(tempResults,i,1)
    #tempModels = list(models)
    #del tempModels[i]
    #bestVal,bestInds,bestModels = calcBest(tempResults,tempModels,opts.numStudentsToChoose,metric)
    ##print 'Leave one out %s: %s %s' % (student,bestVal,bestModels)
    #print '%s,%s' % (student,','.join(bestModels))
    #sys.stdout.flush()

if __name__ == '__main__':
  parser = OptionParser('%prog [options] inCSV')
  addMetricParser(parser,validMetrics)
  parser.add_option('--hc',action='store_true',dest='useHC',default=True)
  parser.add_option('--no-hc',action='store_false',dest='useHC',default=True)
  parser.add_option('-n','--numToChoose',action='store',dest='numStudentsToChoose',default=4,type='int')
  parser.add_option('--no-multi',action='store_false',dest='multi',default=True,help='Do not evaluate multithreaded')
  
  opts,args = parser.parse_args()
  assert(len(args) == 1),'Incorrect number of args, expected 1'
  filename = args[0]
  metric = getMetric(opts,validMetrics,passOpts=False)
  main(filename,metric,opts)
