#!/usr/bin/env python

import subprocess, os
from addARFFWeights import addARFFWeights

from common import getFilename,makeTemp,parseArgs,BIN_PATH,DESC,UNWEIGHTED,WEIGHTED

def wekaCommandPrefix():
  return ['java','-cp',os.path.join(BIN_PATH,'weka.jar')]

def removeTrialStep(inFile,outFile):
  cmd = wekaCommandPrefix() + ['weka.filters.unsupervised.attribute.Remove','-R','1-2','-i',inFile,'-o',outFile]
  subprocess.check_call(cmd)

def createTree(inFile,outFile,options):
  cmd = wekaCommandPrefix() + ['-Xmx2048m','weka.classifiers.trees.REPTree','-t',inFile,'-i'] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def extractTree(arffFile,inFile,outFile):
  with open(arffFile,'r') as f:
    lines = f.readlines()
  ind = lines.index('@data\n')
  prefix = lines[:ind+1]
  with open(inFile,'r') as f:
    lines = f.readlines()
  ind = lines.index('REPTree\n')
  startInd = ind + 3 # 3 lines later, including REPTree
  endInd = startInd + lines[startInd:].index('\n')
  with open(outFile,'w') as f:
    f.writelines(prefix+lines[startInd:endInd])

def weightTree(inFile,dataFile,outFile):
  if os.uname()[4] == 'x86_64':
    arch = '64'
  else:
    arch = '32'
  cmd = [os.path.join(BIN_PATH,'addWeights%s' % arch),inFile,dataFile]
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def buildDT(dataFile,outFile,options):
  if os.uname()[4] == 'x86_64':
    arch = '64'
  else:
    arch = '32'
  cmd = [os.path.join('bin','buildDT%s' % arch),dataFile] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def main(inFile,base,name,stayWeight=None,treeOptions=[],useWeka=False):
  descFile = getFilename(base,name,DESC)
  unweightedFile = getFilename(base,name,UNWEIGHTED)
  weightedFile = getFilename(base,name,WEIGHTED)

  # create the temporary files we need
  tmpData = makeTemp('.arff')
  try:
    print 'Removing trial and step features'
    removeTrialStep(inFile,tmpData)
    if (stayWeight is not None) and (abs(stayWeight - 1.0) > 0.0001):
      print 'Adding stay weights'
      addARFFWeights(tmpData,tmpData,stayWeight)
    if useWeka:
      print 'Running weka to create initial tree'
      createTree(tmpData,descFile,treeOptions)
      print 'Extracting tree from weka output'
      extractTree(tmpData,descFile,unweightedFile)
      print 'Adding class weights to tree'
      weightTree(unweightedFile,tmpData,weightedFile)
    else:
      print 'Running buildDT to create a weighted tree'
      buildDT(tmpData,weightedFile,treeOptions)
    print 'Done.'
  finally:
    os.remove(tmpData)

if __name__ == '__main__':
  usage = 'trainDT.py [options] inFile basename [-- treeOptions]'
  options,args,treeOptions = parseArgs(usage=usage,numArgs=2)
  inFile = args[0]
  basename = args[1]
  stayWeight = None

  main(inFile,basename,stayWeight,treeOptions,options.useWeka)
