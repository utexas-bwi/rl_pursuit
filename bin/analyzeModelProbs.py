#!/usr/bin/env python

import json, os
from scipy.interpolate import interp1d
from plotModelProbs import plot

ANALYSIS_TYPE_REGULAR = 0
ANALYSIS_TYPE_CONVERGENCE = 1
ANALYSIS_TYPE_REMOVAL = 2

def loadFile(filename):
  probs = []
  with open(filename,'r') as f:
    for line in f:
      probs.append(json.loads(line))
  return probs

def getFiles(directory):
  return getFileDict(directory).values()

def getFilesSorted(directory):
  fileDict = getFileDict(directory)
  keys = sorted(fileDict)
  return [fileDict[k] for k in keys]

def getFileDict(directory):
  files = os.listdir(directory)
  fileDict = {}
  for f in files:
    base,ext = os.path.splitext(f)
    try:
      jobNum = int(base)
      path = os.path.join(directory,f)
      fileDict[jobNum] = path
    except ValueError:
      pass
  return fileDict

def loadFiles(directory):
  files = getFiles(directory)
  probList = []
  for f in files:
    probList.append(loadFile(f))
  return probList

def getAverageProbs(probList,normalized,length,analysisType):
  EPS = 0.05
  if normalized:
    maxLength = length
  else:
    maxLength = 0
    for probs in probList:
      maxLength = max(maxLength,len(probs))
  keys = probList[0][0].keys()
  vals = {}
  for key in keys:
    vals[key] = [0 for i in range(maxLength)]
  numTrials = float(len(probList))
  for probs in probList:
    for key in keys:
      tempProb = []
      for step in range(len(probs)):
        if key in probs[step]:
          if len(probs[step]) == 1:
            p = 1.0
          else:
            p = probs[step][key]
        else:
          p = 0.0
        tempProb.append(p)
      if normalized:
        tempProb = normalizeListLength(tempProb,maxLength)
      else:
        while len(tempProb) < maxLength:
          tempProb.append(tempProb[-1])

      for step,p in enumerate(tempProb):
        if analysisType == ANALYSIS_TYPE_CONVERGENCE:
          if p > 1.0 - EPS:
            p = 1.0
          else:
            p = 0.0
        elif analysisType == ANALYSIS_TYPE_REMOVAL:
          if p < EPS:
            p = 1.0
          else:
            p = 0.0
        vals[key][step] += p / numTrials
  return vals

def normalizeListLength(vals,length):
  xs = [x / float(len(vals) - 1) for x in range(len(vals))]
  f = interp1d(xs,vals,kind='linear')
  newXs = [x / float(length - 1) for x in range(length)]
  return f(newXs)

def main(directory,normalized,outputFile,plotQ,convergence):
  print 'loading files'
  probList = loadFiles(directory)
  print 'averaging'
  avgProbs = getAverageProbs(probList,normalized,1000,convergence)
  #for k,v in avgProbs.iteritems():
    #print '%s:' % k,
    #for x in v:
      #print '%4.2f' % x,
    #print ''
  print 'outputing'
  if outputFile is not None:
    import cPickle as pickle
    with open(outputFile,'w') as f:
      pickle.dump(avgProbs,f,pickle.HIGHEST_PROTOCOL)
  print 'plotting'
  if plotQ:
    plot(avgProbs)
  

def mainFromArgs(args):
  normalized = False
  if '--norm' in args:
    args.remove('--norm')
    normalized = True
  plotQ = False
  if '--plot' in args:
    args.remove('--plot')
    plotQ = True
  analysisType = ANALYSIS_TYPE_REGULAR
  if '--conv' in args:
    args.remove('--conv')
    analysisType = ANALYSIS_TYPE_CONVERGENCE
  if '--rm' in args:
    args.remove('--rm')
    analysisType = ANALYSIS_TYPE_REMOVAL
  assert(len(args) == 2)
  directory = args[0]
  outputFile = args[1]
  main(directory, normalized, outputFile, plotQ,analysisType)

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  mainFromArgs(args)
