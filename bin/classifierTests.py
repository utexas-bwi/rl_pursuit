#!/usr/bin/env python

import subprocess, re, sys
from weka.common import getArch

#inTrainData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/AnatolyBroitman.arff'
#inTrainData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/combined.arff'
inTrainData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/combined-DrorBanin.arff'
blankData = 'data/dt/blank.arff'
inTestData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/DrorBanin.arff'
#inTestData = 'data/dt/ra-20x20-centerPrey-myActionHistory-5000/train/AnatolyBroitman.arff'
trainData = 'temp/classifierTestsTrain.arff'
testData = 'temp/classifierTestsTest.arff'
configName = 'temp/classifierTests%s.json'
numTargetTrainingInstancesList = [0,10,50,100,500]

if len(sys.argv) >= 2:
  specifiedNum = int(sys.argv[1])
  configName = configName % specifiedNum
else:
  specifiedNum = None
  configName = configName % ''

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

twostagetradaConfig = '''
  "maxBoostingIterations": %i,
  "folds": %i,
  "baseLearner": {
    "type": "adaboostprime",
    "caching": false,
    "maxBoostingIterations" : %i,
    "baseLearner": {
      "type": "%s",
      "caching": false
      %s
    }
  }
'''

tests = [
  ['weka-nb','weka','"options": "weka.classifiers.bayes.NaiveBayes"'],
  ['weka-j48','weka','"options": "weka.classifiers.trees.J48"'],
  ['nb','nb',''],
  ['svm','svm',''],
  ['lsvm','lsvm',''],
  ['ada-20-nb','adaboost',boostConfig % (20,'nb','')],
  ['ada-20-svm','adaboost',boostConfig % (20,'svm','')],
  ['ada-20-lsvm','adaboost',boostConfig % (20,'lsvm','')],
  ['trada-20-nb','tradaboost',boostConfig % (20,'nb','')],
  ['trada-20-svm','tradaboost',boostConfig % (20,'svm','')],
  ['trada-20-lsvm','tradaboost',boostConfig % (20,'lsvm','')],
  ['trbagg-200-nb','trbagg',trbaggConfig % (200,'nb','','dt',',"maxDepth": 1')],
  ['trbagg-200-svm','trbagg',trbaggConfig % (200,'svm','','dt',',"maxDepth": 1')],
  ['trbagg-200-lsvm','trbagg',trbaggConfig % (200,'lsvm','','dt',',"maxDepth": 1')],
  ['twostagetrada-20-nb','twostagetradaboost',boostConfig % (20,'nb','')],
  ['twostagetrada-20-svm','twostagetradaboost',boostConfig % (20,'svm','')],
  ['twostagetrada-20-lsvm','twostagetradaboost',boostConfig % (20,'lsvm','')],
  ['twostagetrada-20-adaboostprime-20-lsvm','twostagetradaboost',twostagetradaConfig % (20,5,20,'lsvm','')],
  ['twostagetrada-20-adaboostprime-20-nb','twostagetradaboost',twostagetradaConfig % (20,5,20,'nb','')],


  #['ada-100-nb','adaboost',boostConfig % (100,'nb','')],
  #['trada-20-nb','tradaboost',boostConfig % (20,'nb','')],
  #['ada-20-svm','adaboost',boostConfig % (20,'svm','')],
  #['trada-20-svm','tradaboost',boostConfig % (20,'svm','')],
  #['trada-20-lsvm','tradaboost',boostConfig % (20,'lsvm','')],
  #['trbagg-20-nb','trbagg',trbaggConfig % (20,'nb','','dt',',"maxDepth": 1')],
  #['trbagg-200-svm','trbagg',trbaggConfig % (200,'svm','','dt',',"maxDepth": 1')],
  #['trbagg-1000-dt','trbagg',trbaggConfig % (1000,'dt',',"maxDepth": 5','dt',',"maxDepth": 1')],
  #['trbagg-100-nb','trbagg',trbaggConfig % (100,'nb','','dt',',"maxDepth": 1')],
  #['trbagg-1000-nb','trbagg',trbaggConfig % (1000,'nb','','dt',',"maxDepth": 1')],
  #['trbagg-10000-nb','trbagg',trbaggConfig % (10000,'nb','','dt',',"maxDepth": 1')],
  #['ada-20-dt-5','adaboost',boostConfig % (20,'dt',',"maxDepth": 5')],
  #['trada-20-dt-5','tradaboost',boostConfig % (20,'dt',',"maxDepth": 5')],
  #['trada-100-nb','tradaboost',boostConfig % (100,'nb','')],
  #['trada-100-dt-5','tradaboost',boostConfig % (100,'dt',',"maxDepth": 5')],
  #['trada-20-lsvm','tradaboost',boostConfig % (20,'lsvm','')],
  #['ada-20-lsvm','adaboost',boostConfig % (20,'lsvm','')],
  #['adaprime-20-lsvm','adaboostprime',boostConfig % (20,'lsvm','')],
  #['twostagetrada-20-lsvm','twostagetradaboost',boostConfig % (20,'lsvm','')],
  #['twostagetrada-20-adaboostprime-20-lsvm','twostagetradaboost',twostagetradaConfig % (20,2,20,'lsvm','')],
  #['trbagg-200-lsvm','trbagg',trbaggConfig % (200,'lsvm','','nb','')],
  #['dt-inf','dt',''],
  #['dt-5','dt','"maxDepth": 5'],
  #['dt-10','dt','"maxDepth": 10'],
  #['weka-nb','weka','"options": "weka.classifiers.bayes.NaiveBayes"'],
  #['weka-j48','weka','"options": "weka.classifiers.trees.J48"'],
]

#for i in range(8):
  #tests.append(['lsvm-%i' % i,'lsvm','"solverType": %i' % i])

from weka.createDT import removeTrialStep

print '****************************************'
print '****************************************'
print '****************************************'
print 'RE-ENABLE REMOVING TRIAL STEP'
print '****************************************'
print '****************************************'
print '****************************************'
#print 'Removing Trial Step'
#removeTrialStep(inTrainData,trainData)
#removeTrialStep(inTestData,testData)

fracCorrect = {}
numCorrect = {}
counter = -1
for numTargetTrainingInstances in numTargetTrainingInstancesList:
  for useSourceData in [False,True]:
  #for useSourceData in [True]:
    for label,typeName,options in tests:
      counter += 1
      if (specifiedNum is not None) and (counter != specifiedNum):
        continue

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
      label = '%s-%s' % (numTargetTrainingInstances,label)
      config = baseConfig % (train,typeName,options)
      with open(configName,'w') as f:
        f.write(config)
      cmd = ['bin/%s/runClassifier' % getArch(),configName,testData,str(numTargetTrainingInstances)]
      p = subprocess.Popen(cmd,stdout=subprocess.PIPE)
      output = ''
      while p.poll() is None:
        temp = p.stdout.readline()
        output += temp
        print temp,
      if p.poll() != 0:
        print 'ERROR RUNNING: %s' % (' '.join(cmd))
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
