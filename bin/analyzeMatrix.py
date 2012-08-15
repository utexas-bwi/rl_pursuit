#!/usr/bin/env python

import sys, csv, itertools, numpy, scipy

filename = sys.argv[1]
numStudentsToChoose = 4
useHC = True
evalFunc = numpy.mean
#evalFunc = numpy.max

def calcVal(results,current):
  vals = results[:,current]
  mins = numpy.min(vals,axis=1)
  return evalFunc(mins)

def calcBest(results,models):
  best = None
  bestVal = numpy.inf
  for current in itertools.combinations(range(len(models)),numStudentsToChoose):
    val = calcVal(results,current)
    if val < bestVal:
      best = current
      bestVal = val
  return bestVal,best,[models[x] for x in best]


reader = csv.reader(open(filename,'r'),delimiter=',')
reader.next() # ignore first line
header = reader.next()
origModels = header[1:]
students = origModels[:-4]
if useHC:
  models = list(origModels)
else:
  models = list(students)

results = numpy.zeros([len(students),len(models)])
numExpectedCols = len(models)
for i,row in enumerate(reader):
  if len(origModels) != len(row) - 1:
    print 'Bad Size:',len(origModels),len(row)-1
    sys.exit(2)
  for j,v in enumerate(row[1:len(models)+1]):
    results[i,j] = float(v)
  #results[row[0]] = {}
  #for k,v in zip(header[1:],row[1:]):
    #results[row[0]][k] = float(v)

#print results

bestVal,bestInds,bestModels = calcBest(results,models)
print 'Overall best: %s %s' % (bestVal,bestModels)
for i,student in enumerate(students):
  tempResults = scipy.delete(results,i,0)
  tempResults = scipy.delete(tempResults,i,1)
  tempModels = list(models)
  del tempModels[i]
  bestVal,bestInds,bestModels = calcBest(tempResults,tempModels)
  print 'Leave one out %s: %s %s' % (student,bestVal,bestModels)
  sys.stdout.flush()
