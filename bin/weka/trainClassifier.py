#!/usr/bin/env python

import os, subprocess, sys, re
from common import getArch, getUniqueStudents, getFilename, baseExists, TRAIN, makeTemp

def getClassifier(c):
  # in format name, needs base learner, needs fallback learner
  KNOWN_CLASSIFIERS = [
    ['lsvm',False,False],
    ['nb',False,False],
    ['dt',False,False],
    ['dt-noweka',False,False],
    ['twostagetradaboost',True,False],
    ['trbagg',True,True],
    ['trbagg-partialLoad',True,True]
  ]

  KNOWN_CLASSIFIER_NAMES = [x[0] for x in KNOWN_CLASSIFIERS]
  try:
    ind = KNOWN_CLASSIFIER_NAMES.index(c)
    return KNOWN_CLASSIFIERS[ind]
  except:
    print >>sys.stderr,'Unknown classifier:',c
    sys.exit(2)

def getConfigFilename(classifier):
  return os.path.join('configs','learners','%s.json' % classifier)

def getSaveFilename(classifier,numTarget,numSource,student):
  return os.path.join('configs','learners','saved','%s-target%i-source%i-%s.txt' % (classifier,numTarget,numSource,student))

def combineConfigs(learner,baseLearner,fallbackLearner,saveConfigFilename,saveFile,student):
  with open(getConfigFilename(learner),'r') as f:
    content = f.read().strip()
  if learner == 'trbagg-partialLoad':
    # TODO
    content = content.replace('$(PARTIAL_FILENAME)','data/dt/studentsNew29-unperturbed-transfer/target10000-source50000/weighted/trBagg-%s.weka' % student)
  endInd = content.rfind('}')
  if content[endInd-1] == '\n':
    endInd -= 1
  res = content[:endInd]
  
  prefixList = ['"baseLearner":','"fallbackLearner":']
  configs = [baseLearner,fallbackLearner]
  for conf,prefix in zip(configs,prefixList):
    if conf is None:
      continue
    res += ',\n  ' + prefix + ' '
    with open(getConfigFilename(conf),'r') as f:
      for i,line in enumerate(f):
        res += ('  ' if i != 0 else '') + line
    res = res.strip()
  res += '\n}'

  filename = makeTemp()
  with open(filename,'w') as f:
    f.write(res)
  
  # for save config
  ind = res.find('{')
  ind += 1
  while res[ind] in ['\n','\r',' ','\t']:
    ind += 1
  filenameLine = '"filename": "%s",\n  ' % saveFile
  res = res[:ind] + filenameLine + res[ind:]
  res = re.sub('.*"partialFilename".*\n','',res)
  with open(saveConfigFilename,'w') as f:
    f.write(res)

  return filename

def parseArgs(args,parserOptions=[],numAdditionalArgs=0,additionalArgsString=''):
  from optparse import OptionParser
  parser = OptionParser('%prog [options] classifier student ' + additionalArgsString)
  parser.add_option('-b','--baseLearner',action='store',dest='baseLearner',type='str',default=None,help='use the classifier as the base learner')
  parser.add_option('-f','--fallbackLearner',action='store',dest='fallbackLearner',type='str',default=None,help='use the classifier file as the fallback learner')
  #parser.add_option('--student',action='store',dest='student',type='str',default=None)
  parser.add_option('-s','--source',action='store',dest='numSource',type='int',default=None,help='num source instances')
  parser.add_option('-t','--target',action='store',dest='numTarget',type='int',default=None,help='num target instances')
  parser.add_option('--no-source',action='store_false',dest='useSource',default=True,help='don\'t use the source data, but use the name')
  for option in parserOptions:
    parser.add_option(option)
  options,args = parser.parse_args(args)

  numExpectedArgs = 2
  if len(args) != numExpectedArgs + numAdditionalArgs:
    print >>sys.stderr,'Incorrect number of arguments expected %i but got %i' % (numExpectedArgs,len(args))
    parser.parse_args(['--help'])
    sys.exit(1)
  
  classifierType,student = args[:numExpectedArgs]
  # num training should be an int, and the filename should exist
  options.base = 'studentsNew29-unperturbed-%i'
  assert(options.numTarget is not None),'numTarget unspecified'
  options.targetBase = options.base % options.numTarget
  if not(baseExists(options.targetBase)):
    print >>sys.stderr,'Dir for numTarget doesn\'t exist at:',options.targetBase
    sys.exit(2)
  assert(options.numSource is not None),'numSource unspecified'
  if options.numSource != 0:
    options.sourceBase = options.base % options.numSource
    if not(baseExists(options.sourceBase)):
      print >>sys.stderr,'Dir for numSource doesn\'t exist at:',options.sourceBase
      sys.exit(2)
  # get students and check provided student
  students = getUniqueStudents()
  try:
    ind = int(student)
    student = students[ind]
  except:
    if student not in students:
      print >>sys.stderr,'Unknown student:',student
      sys.exit(2)
  options.student = student
  options.otherStudents = list(students)
  options.otherStudents.remove(student)
  # check provided classifiers
  options.classifier = getClassifier(classifierType)
  if (options.classifier[1] == True) and (options.baseLearner is None):
    print >>sys.stderr,'Missing base learner:',classifierType
    sys.exit(1)
  if (options.classifier[2] == True) and (options.fallbackLearner is None):
    print >>sys.stderr,'Missing fallback learner:',classifierType
    sys.exit(1)
  if options.baseLearner is not None:
    temp = getClassifier(options.baseLearner)
    assert((not temp[1]) and (not temp[2]))
    if options.baseLearner == 'lsvm':
      options.baseLearner += '-base'
  if options.fallbackLearner is not None:
    temp = getClassifier(options.fallbackLearner)
    assert((not temp[1]) and (not temp[2]))
    if options.fallbackLearner == 'lsvm':
      options.fallbackLearner += '-base'
  # get name
  name = options.classifier[0]
  if name == 'trbagg-partialLoad':
    name = 'trbagg'
  if options.baseLearner is not None:
    name += '_base' + options.baseLearner
  if options.fallbackLearner is not None:
    name += '_fb' + options.fallbackLearner
  options.name = name
  options.saveName = name + '-target%i-source%i' % (options.numTarget,options.numSource)

  # get the arguments
  options.saveConfigFilename = getConfigFilename('saved/' + options.saveName + '-' + student)

  return options,args[numExpectedArgs:]

def main(args = sys.argv[1:]):
  options,_ = parseArgs(args)
  
  try:
    saveFile = getSaveFilename(options.name,options.numTarget,options.numSource,options.student)
    filename = combineConfigs(options.classifier[0],options.baseLearner,options.fallbackLearner,options.saveConfigFilename,saveFile,options.student)
    targetData = getFilename(options.targetBase,options.student,TRAIN)
    if (options.numSource == 0) or not(options.useSource):
      sourceData = []
    else:
      sourceData = [getFilename(options.sourceBase,s,TRAIN) for s in options.otherStudents]
    # run the cmd
    print 'save config will be at',options.saveConfigFilename
    cmd = ['bin/%s/trainClassifier' % getArch(),filename,saveFile,targetData] + sourceData
    subprocess.check_call(cmd)
  finally:
    if filename is not None:
      os.remove(filename)

if __name__ == '__main__':
  main()
