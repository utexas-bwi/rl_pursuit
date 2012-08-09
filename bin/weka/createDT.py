#!/usr/bin/env python

import subprocess, os, time
from addARFFWeights import addARFFWeights

from common import getFilename,makeTemp,parseArgs,makeDirs,BIN_PATH,DESC,UNWEIGHTED,WEIGHTED, getArch

def wekaCommandPrefix():
  return ['java','-cp',os.path.join(BIN_PATH,'weka.jar') + ':' + BIN_PATH]

def getSeed():
  return int(time.time() * 1000000) % 1000000

def resample(inFile,outFile,frac):
  cmd = wekaCommandPrefix() + ['weka.filters.unsupervised.instance.Resample','-Z',str(frac * 100),'-S',str(getSeed()),'-no-replacement','-i',inFile,'-o',outFile]
  subprocess.check_call(cmd)

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

def createTree(inFile,outFile,options,randomTree,featureFrac):
  if randomTree:
    #cmd = wekaCommandPrefix() + ['-Xmx2048m','weka.classifiers.trees.RandomTree','-t',inFile,'-i','-F',str(featureFrac),'-S',str(getSeed())] + options
    cmd = wekaCommandPrefix() + ['-Xmx2048m','REPRandomTree','-t',inFile,'-i','-F',str(featureFrac),'-S',str(getSeed())] + options
  else:
    #cmd = wekaCommandPrefix() + ['-Xmx2048m','weka.classifiers.trees.REPTree','-t',inFile,'-i','-S',str(getSeed())] + options
    cmd = wekaCommandPrefix() + ['-Xmx2048m','REPTree','-t',inFile,'-i','-S',str(getSeed())] + options # using my version of REPTree that outputs
  #cmd = wekaCommandPrefix() + ['-Xmx4096m','weka.classifiers.trees.REPTree','-t',inFile,'-i'] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def extractTree(arffFile,inFile,outFile):
  with open(arffFile,'r') as f:
    lines = f.readlines()
  ind = lines.index('@data\n')
  prefix = lines[:ind+1]
  with open(inFile,'r') as f:
    lines = f.readlines()
  try:
    ind = lines.index('REPTree\n')
  except ValueError:
    ind = lines.index('REPRandomTree\n')
  startInd = ind + 2 # 3 lines later, including REPTree
  if lines[startInd] == '\n':
    startInd += 1
  endInd = startInd + lines[startInd:].index('\n')
  with open(outFile,'w') as f:
    f.writelines(prefix+lines[startInd:endInd])

def weightTree(inFile,dataFile,outFile):
  cmd = [os.path.join('bin',str(getArch()),'addWeights'),inFile,dataFile]
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def buildDT(dataFile,outFile,options,randomTree):
  if randomTree:
    raise ValueError('cannot handle randomTrees here')
  cmd = [os.path.join('bin',str(getArch()),'buildDT'),dataFile] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def makeTree(data,useWeka,stayWeight,base,name,treeOptions,randomTree,featureFrac):
  descFile = getFilename(base,name,DESC)
  #unweightedFile = getFilename(base,name,UNWEIGHTED)
  weightedFile = getFilename(base,name,WEIGHTED)
  if (stayWeight is not None) and (abs(stayWeight - 1.0) > 0.0001):
    print 'Adding stay weights'
    addARFFWeights(data,data,stayWeight)
  if useWeka:
    print 'Running weka to create initial tree'
    createTree(data,descFile,treeOptions,randomTree,featureFrac)
    print 'Extracting tree from weka output'
    # NOTE: changed weka to output class distributions, no longer need to add my own weights
    #if randomTree:
      #extractTree(data,descFile,unweightedFile)
      #print 'Adding class weights to tree'
      #weightTree(unweightedFile,data,weightedFile)
    #else:
    extractTree(data,descFile,weightedFile)
  else:
    print 'Running buildDT to create a weighted tree'
    buildDT(data,weightedFile,treeOptions,randomTree)

def main(inFile,base,name,stayWeight=None,treeOptions=[],useWeka=False,numInstances=None,randomTree=False,numRandomTrees=10,featureFrac = 0.8,resampleFrac=0.5):

  # create the temporary files we need
  tmpData = makeTemp('.arff')
  removeFiles = [tmpData]
  try:
    print 'Removing trial and step features'
    removeTrialStep(inFile,tmpData,numInstances)
    if randomTree:
      tmpDataSampled = makeTemp('-sampled.arff')
      removeFiles.append(tmpDataSampled)
      for i in range(numRandomTrees):
        print '*** Random Tree %i' % i
        resample(tmpData,tmpDataSampled,resampleFrac)
        makeTree(tmpDataSampled,useWeka,stayWeight,base,name + '-%i' % i,treeOptions,randomTree,featureFrac)
    else:
      makeTree(tmpData,useWeka,stayWeight,base,name,treeOptions,randomTree,featureFrac)
    print 'Done.'
  finally:
    for f in removeFiles:
      os.remove(f)

if __name__ == '__main__':
  usage = '%prog [options] inFile base name [-- treeOptions]'
  options,args,treeOptions = parseArgs(usage=usage,numArgs=3,studentOptions=False,numInstancesFlag=True)
  inFile = args[0]
  base = args[1]
  name = args[2]
  stayWeight = None

  makeDirs(base,printWarning=False)
  main(inFile,base,name,stayWeight,treeOptions,options.useWeka,options.numInstances)
