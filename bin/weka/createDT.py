#!/usr/bin/env python

import subprocess, os
from addARFFWeights import addARFFWeights

BASE_PATH=os.path.join('bin','weka')

def makeTemp(*args,**kwargs):
  import tempfile
  fd,temp = tempfile.mkstemp(*args,**kwargs)
  os.close(fd)
  return temp

def wekaCommandPrefix():
  return ['java','-cp',os.path.join(BASE_PATH,'weka.jar')]

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
  cmd = [os.path.join(BASE_PATH,'addWeights%s' % arch),inFile,dataFile]
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def buildDT(dataFile,outFile,options):
  if os.uname()[4] == 'x86_64':
    arch = '64'
  else:
    arch = '32'
  cmd = [os.path.join('bin','buildDT%s' % arch),dataFile] + options
  subprocess.check_call(cmd,stdout=open(outFile,'w'))

def main(inFile,basename,stayWeight=None,treeOptions=[],useWeka=False):
  descFile = os.path.join('data','dt','desc','%s.desc' % basename)
  unweightedFile = os.path.join('data','dt','unweighted','%s.weka' % basename)
  weightedFile = os.path.join('data','dt','weighted','%s.weka' % basename)

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
  import sys
  args = sys.argv[1:]
  usage = 'Usage trainDT.py inFile basename [--weka] [treeOptions ...]'
  useWeka = False
  if '--weka' in args:
    args.remove('--weka')
    useWeka = True
  if len(args) < 2:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
    
  inFile = args[0]
  basename = args[1]
  stayWeight = None
  treeOptions = args[2:]

  main(inFile,basename,stayWeight,treeOptions,useWeka)
