#!/usr/bin/env python

configBase = '''{
  "planner": {
    "modelOutputFile": "$(DIR)/models/$(TRIALNUM).txt",
    "weighted": true, 
    "theoreticallyCorrectLambda": false, 
    "models": [
      %s
    ], 
    "playouts": 1000, 
    "initialStateVisits": 0, 
    "rewardBound": 0.5, 
    "update": "polynomial", 
    "initialStateActionVisits": 0, 
    "unseenValue": 999999, 
    "pruningMemory": -1, 
    "depth": 100, 
    "initialValue": 0, 
    "students": "data/students.txt", 
    "student": "$(STUDENT)", 
    "time": 0, 
    "silver": false, 
    "gamma": 0.95, 
    "lambda": 0.8
  }, 
  "verbosity": {
    "stepsPerEpisode": false, 
    "stepsPerTrial": false, 
    "summary": true, 
    "description": true, 
    "observation": false
  }, 
  "predator": "student", 
  "adhoc": "uct", 
  "height": 20, 
  "width": 20, 
  "prey": "random", 
  "predatorOptions": {
    "student": "data/students.txt"
  }, 
  "centerPrey": true
}
'''

modelBase = '''
      {
        "desc": "%s", 
        "predator": "%s", 
        "prey": "random", 
        "prob": 1.0
      }'''

models = ['gr','ta','gp','pd']

nameBase = 'ra-student-uct-%s-29-20x20-centerPrey-myActionHistory'

import itertools, subprocess

for n in range(1,len(models)):
  for selectedModels in itertools.combinations(models,n):
    modelStr = ','.join([modelBase % (x,x) for x in selectedModels])
    config = configBase % modelStr
    name = nameBase % ('_'.join(selectedModels))
    configPath = 'configs/hc/%s.json' % name
    setupCondorName = 'hc/%s' % name
    condorPath = 'condor/%s/job.condor' % setupCondorName
    with open(configPath,'w') as f:
      f.write(config)

    subprocess.check_call(['bin/setupCondor.py',configPath,'--name',setupCondorName])
    subprocess.check_call(['condor_submit',condorPath])
