#!/usr/bin/env python

import subprocess, re, os

inTrainData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/AnatolyBroitman.arff'
#inTrainData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/combined.arff'
blankData = 'data/dt/blank.arff'
inTestData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/DrorBanin.arff'
trainData = 'temp/classifierTestsTrain.arff'
testData = 'temp/classifierTestsTest.arff'
configName = 'temp/classifierTests.json'
numTargetTrainingInstances = 100

baseConfig = '''{
  "data": "%s",
  "type": "%s",
  "initialTrain": false,
  "caching": false
  %s
}'''

boostConfig = '''
  "maxBoostingIterations": %s,
  "baseLearner": {
    "type": "%s",
    "caching": false
    %s
  }
'''

trbaggConfig = boostConfig + ''',
  "fallbackLearner": {
    "type": "%s",
    "caching": false
    %s
  }
'''

tests = [
  #['ada-20-nb','adaboost',boostConfig % (20,'nb','')],
  #['ada-100-nb','adaboost',boostConfig % (100,'nb','')],
  #['trada-20-nb','tradaboost',boostConfig % (20,'nb','')],
  #['trbagg-20-nb','trbagg',trbaggConfig % (20,'nb','','dt',',"maxDepth": 1')],
  #['trbagg-100-nb','trbagg',trbaggConfig % (100,'nb','','dt',',"maxDepth": 1')],
  #['trbagg-1000-nb','trbagg',trbaggConfig % (1000,'nb','','dt',',"maxDepth": 1')],
  #['trbagg-10000-nb','trbagg',trbaggConfig % (10000,'nb','','dt',',"maxDepth": 1')],
  #['ada-20-dt-5','adaboost',boostConfig % (20,'dt',',"maxDepth": 5')],
  #['trada-20-dt-5','tradaboost',boostConfig % (20,'dt',',"maxDepth": 5')],
  #['trada-100-nb','tradaboost',boostConfig % (100,'nb','')],
  #['trada-100-dt-5','tradaboost',boostConfig % (100,'dt',',"maxDepth": 5')],
  ['nb','nb',''],
  ['svm','svm',''],
  #['dt-inf','dt',''],
  #['dt-5','dt','"maxDepth": 5'],
  #['dt-10','dt','"maxDepth": 10'],
  #['weka-nb','weka','"options": "weka.classifiers.bayes.NaiveBayes"'],
  #['weka-j48','weka','"options": "weka.classifiers.trees.J48"'],
]

from weka.createDT import removeTrialStep

print 'Removing Trial Step'
removeTrialStep(inTrainData,trainData)
removeTrialStep(inTestData,testData)

fracCorrect = {}
numCorrect = {}
#for useSourceData in [False,True]:
for useSourceData in [True]:
  for label,typeName,options in tests:
    print '*********************'
    print typeName,options
    print '*********************'
    if len(options) > 0:
      options = ',' + options
    if useSourceData:
      train = trainData
      label += '-withSource'
    else:
      train = blankData
      label += '-noSource'
    config = baseConfig % (train,typeName,options)
    with open(configName,'w') as f:
      f.write(config)
    cmd = ['bin/runClassifier32',configName,testData,str(numTargetTrainingInstances)]
    p = subprocess.Popen(cmd,stdout=subprocess.PIPE)
    output = ''
    while p.poll() is None:
      temp = p.stdout.readline()
      output += temp
      print temp,
    temp = p.stdout.readline()
    output += temp
    print temp,

    #output = p.communicate()[0]
    #print output
    fracCorrect[label] = (re.findall('Frac\s*Correct:.*\(([.0-9]*)\)',output)[0])
    numCorrect[label] = (re.findall('Num\s*Correct:.*\(([.0-9]*)\)',output)[0])
    print fracCorrect
    print numCorrect

labels = sorted(fracCorrect.keys())
for label in labels:
  print label
  print '  Frac: %s' % fracCorrect[label]
  print '  Num : %s' % numCorrect[label]
