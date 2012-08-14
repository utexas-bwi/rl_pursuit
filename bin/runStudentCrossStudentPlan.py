#!/usr/bin/env python

import sys, os, subprocess
from weka.common import getUniqueStudents, makeTemp

trialsPerStudent = 100
trialsPerJob = 10
dirName = 'condor/matrix'

def getRunInfo(jobNum,trialsPerJob):
  students = getUniqueStudents('data/newStudents29.txt')
  jobsPerStudent = trialsPerStudent / trialsPerJob
  temp = jobNum
  numStudents = len(students)
  trialNumber = temp % jobsPerStudent
  temp /= jobsPerStudent
  studentModel = students[temp % numStudents]
  temp /= numStudents
  if temp > numStudents:
    sys.exit(3)
  student = students[temp]
  return trialNumber,student,studentModel

jobNum = int(sys.argv[1])

baseConfig = '''
{
  "planner": {
    "weighted": true, 
    "theoreticallyCorrectLambda": false, 
    "models": [
      {
        "prey": "random", 
        "predator": "classifier",
        "predatorOptions": {
          "type": "dt", 
          "minGain": 0.0001, 
          "filename": "data/dt/studentsNew29-unperturbed-50000/weighted/only-$(MODEL_STUDENT).weka", 
          "minInstances": 2, 
          "trainIncremental": true, 
          "trainingPeriod": -1, 
          "maxDepth": -1, 
          "caching": false, 
          "shared": true, 
          "data": ""
        },
        "dataPerStudent": false, 
        "modelPerStudent": true, 
        "includeCurrentStudent": false, 
        "randomForest": false,
        "desc": "Classifier-$(MODEL_STUDENT)"
      }

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
    "students": "data/newStudents29.txt", 
    "student": "$(MODEL_STUDENT)", 
    "time": 0, 
    "silver": false, 
    "gamma": 0.95, 
    "lambda": 0.8
  }, 
  "verbosity": {
    "stepsPerEpisode": false, 
    "stepsPerTrial": false, 
    "summary": true, 
    "description": false, 
    "observation": false
  }, 
  "predator": "student", 
  "adhoc": "mcts", 
  "height": 20, 
  "width": 20, 
  "prey": "random", 
  "predatorOptions": {
    "student": "$(STUDENT)"
  }, 
  "centerPrey": true,
  "maxNumStepsPerEpisode": 500,
  "trials": $(TRIALS_PER_STUDENT),
  "trialsPerJob": $(TRIALS_PER_JOB),
  "save": {"results":"$(RESULT_DIR)/$(JOBNUM).csv"},
  "dir": "$(DIR)"
}

'''

trialNumber,student,studentModel = getRunInfo(jobNum,trialsPerJob)

resultDir = '%s/results/%s/%s' % (dirName,student,studentModel)
if not(os.path.exists(resultDir)):
  os.makedirs(resultDir)
# make the config
config = baseConfig
config = config.replace('$(DIR)',dirName)
config = config.replace('$(TRIALS_PER_STUDENT)',str(trialsPerStudent))
config = config.replace('$(TRIALS_PER_JOB)',str(trialsPerJob))
config = config.replace('$(STUDENT)',student)
config = config.replace('$(MODEL_STUDENT)',studentModel)
config = config.replace('$(RESULT_DIR)',resultDir)
configFilename = makeTemp()
try:
  with open(configFilename,'w') as f:
    f.write(config)
  # run it
  cmd = ['bin/main',str(trialNumber),configFilename]
  subprocess.check_call(cmd)
finally:
  os.remove(configFilename)
