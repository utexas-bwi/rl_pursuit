#!/usr/bin/env python

import subprocess, os
from addARFFWeights import addARFFWeights

from common import getFilename,makeTemp,parseArgs,makeDirs,BIN_PATH,DESC,UNWEIGHTED,WEIGHTED, getArch

def wekaCommandPrefix():
  return ['java','-cp',os.path.join(BIN_PATH,'weka.jar')]

def removeTrialStep(inFile,outFile,numInstances=None):
  cmd = wekaCommandPrefix() + ['weka.filters.unsupervised.attribute.Remove','-R','1-2','-i',inFile,'-o',outFile]
  subprocess.check_call(cmd)
  if numInstances is not None:
    with open(outFile,'r') as f:
      lines = f.readlines()
    ind = lines.index('@data\n')
    outLines = lines[:ind+1]
    ind += 1
    while lines[ind] == '\n':
      ind += 1
    outLines += lines[ind:ind+4*numInstances]
    #print ' '.join(outLines)
    with open(outFile,'w') as f:
      f.writelines(outLines)

def createTree(inFile,outFile,options):
  cmd = wekaCommandPrefix() + ['-Xmx2048m','weka.classifiers.trees.REPTree','-t',inFile,'-i'] + options
  #cmd = wekaCommandPrefix() + ['-Xmx4096m','weka.classifiers.trees.REPTree','-t',inFile,'-i'] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def extractTree(arffFile,inFile,outFile):
  with open(arffFile,'r') as f:
    lines = f.readlines()
  ind = lines.index('@data\n')
  prefix = lines[:ind+1]
  with open(inFile,'r') as f:
    lines = f.readlines()
  ind = lines.index('REPTree\n')
  startInd = ind + 2 # 3 lines later, including REPTree
  if lines[startInd] == '\n':
    startInd += 1
  endInd = startInd + lines[startInd:].index('\n')
  with open(outFile,'w') as f:
    f.writelines(prefix+lines[startInd:endInd])

def weightTree(inFile,dataFile,outFile):
  cmd = [os.path.join('bin',str(getArch()),'addWeights'),inFile,dataFile]
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def buildDT(dataFile,outFile,options):
  cmd = [os.path.join('bin',str(getArch()),'buildDT'),dataFile] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def main(inFile,base,name,stayWeight=None,treeOptions=[],useWeka=False,numInstances=None):
  descFile = getFilename(base,name,DESC)
  #unweightedFile = getFilename(base,name,UNWEIGHTED)
  weightedFile = getFilename(base,name,WEIGHTED)

  # create the temporary files we need
  tmpData = makeTemp('.arff')
  try:
    print 'Removing trial and step features'
    removeTrialStep(inFile,tmpData,numInstances)
    if (stayWeight is not None) and (abs(stayWeight - 1.0) > 0.0001):
      print 'Adding stay weights'
      addARFFWeights(tmpData,tmpData,stayWeight)
    if useWeka:
      print 'Running weka to create initial tree'
      createTree(tmpData,descFile,treeOptions)
      print 'Extracting tree from weka output'
      # NOTE: changed weka to output class distributions, no longer need to add my own weights
      extractTree(tmpData,descFile,weightedFile)
      #extractTree(tmpData,descFile,unweightedFile)
      #print 'Adding class weights to tree'
      #weightTree(unweightedFile,tmpData,weightedFile)
    else:
      print 'Running buildDT to create a weighted tree'
      buildDT(tmpData,weightedFile,treeOptions)
    print 'Done.'
  finally:
    os.remove(tmpData)

if __name__ == '__main__':
  usage = '%prog [options] inFile base name [-- treeOptions]'
  options,args,treeOptions = parseArgs(usage=usage,numArgs=3,studentOptions=False,numInstancesFlag=True)
  inFile = args[0]
  base = args[1]
  name = args[2]
  stayWeight = None

  makeDirs(base,printWarning=False)
  main(inFile,base,name,stayWeight,treeOptions,options.useWeka,options.numInstances)
