#!/usr/bin/env python

import subprocess, re, os

inTrainData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/AnatolyBroitman.arff'
inTestData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/DrorBanin.arff'
trainData = 'temp/classifierTestsTrain.arff'
testData = 'temp/classifierTestsTest.arff'
configName = 'temp/classifierTests.json'
numTargetTrainingInstances = 100

baseConfig = '''{
  "data": "%s",
  "type": "%s",
  "caching": false
  %s
}'''

tradaBoostConfig = '''
  "maxBoostingIterations": %s,
  "baseLearner": {
    "type": "%s",
    "caching": false
    %s
  }
'''

tests = [
  ['trada-20-nb','tradaboost',tradaBoostConfig % (20,'nb','')],
  #['trada-20-dt-5','tradaboost',tradaBoostConfig % (20,'dt',',"maxDepth": 5')],
  ['trada-100-nb','tradaboost',tradaBoostConfig % (100,'nb','')],
  #['trada-100-dt-5','tradaboost',tradaBoostConfig % (100,'dt',',"maxDepth": 5')],
  ['nb','nb',''],
  #['dt-inf','dt',''],
  #['dt-5','dt','"maxDepth": 5'],
  #['dt-10','dt','"maxDepth": 10'],
  ['weka-nb','weka','"options": "weka.classifiers.bayes.NaiveBayes"'],
  ['weka-j48','weka','"options": "weka.classifiers.trees.J48"']
]

from weka.createDT import removeTrialStep

print 'Removing Trial Step'
removeTrialStep(inTrainData,trainData)
removeTrialStep(inTestData,testData)

fracCorrect = []
numCorrect = []
for label,typeName,options in tests:
  print '*********************'
  print typeName,options
  print '*********************'
  if len(options) > 0:
    options = ',' + options
  config = baseConfig % (trainData,typeName,options)
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
  fracCorrect.append(re.findall('Frac\s*Correct:.*\(([.0-9]*)\)',output))
  numCorrect.append(re.findall('Num\s*Correct:.*\(([.0-9]*)\)',output))
  print fracCorrect
  print numCorrect

for (label,typeName,options),frac,num in zip(tests,fracCorrect,numCorrect):
  print label
  print '  Frac: %s' % frac
  print '  Num : %s' % num
