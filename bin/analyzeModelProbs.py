#!/usr/bin/env python

import json, os
from scipy.interpolate import interp1d
from plotModelProbs import plot

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

def getAverageProbs(probList,normalized,length):
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
    prob = None
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
        vals[key][step] += p / numTrials
  return vals
    #for step in range(maxLength):
      #if step < len(probs):
        #prob = probs[step]
      #for key,p in prob.iteritems():
        #if len(prob) == 1:
          #p = 1.0
        #vals[key][step] += p / numTrials
  #return vals

def normalizeListLength(vals,length):
  xs = [x / float(len(vals) - 1) for x in range(len(vals))]
  f = interp1d(xs,vals,kind='linear')
  newXs = [x / float(length - 1) for x in range(length)]
  return f(newXs)

def getAverageProbsNormalizedLength(probList, length):
  keys = probList[0][0].keys()
  vals = {}
  for key in keys:
    vals[key] = []
    for probs in probList:
      tempVals = []
      for step,prob in enumerate(probs):
        if key not in prob:
          p = 0
        else:
          p = prob[key]
          if len(prob) == 1:
            p = 1.0 # in case we didn't set this correctly
        tempVals.append(p)
      tempVals = normalizeListLength(tempVals,length)
      for ind,p in enumerate(tempVals):
        if len(vals[key]) <= ind:
          vals[key].append([0,0])
        oldCount = vals[key][ind][1]
        vals[key][ind][1] += 1
        vals[key][ind][0] = (oldCount * vals[key][ind][0] + p) / vals[key][ind][1]
  return vals

def main(directory,normalized,outputFile,plotQ):
  probList = loadFiles(directory)
  avgProbs = getAverageProbs(probList,normalized,5)
  #if normalized:
    #avgProbs = getAverageProbsNormalizedLength(probList,5)
  #else:
    #avgProbs = getAverageProbs(probList)
  #for k,v in avgProbs.iteritems():
    #print '%s:' % k,
    #for x in v:
      #print '%4.2f' % x,
    #print ''
  
  if outputFile is not None:
    import cPickle as pickle
    with open(outputFile,'w') as f:
      pickle.dump(avgProbs,f,pickle.HIGHEST_PROTOCOL)
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
  assert(len(args) == 2)
  directory = args[0]
  outputFile = args[1]
  main(directory, normalized, outputFile, plotQ)

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  mainFromArgs(args)
