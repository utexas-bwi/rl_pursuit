#!/usr/bin/env python

import subprocess, os
from setupCondor import run as setupCondor

def main(numTestingTrials,numTrainingTrials,runTrueModel):
  with open('data/students.txt','r') as f:
    students = set(f.read().split())

  with open('configs/student_compare.json','r') as f:
    config = f.read()

  with open('configs/student_compare_planner.json','r') as f:
    plannerConfig = f.read()
  
  for student in students:
    studentConfig = 'configs/runStudents/%s_compare.json' % student
    plannerConfigFile = 'configs/runStudents/%s_compare_planner.json' % student

    contents = config.replace('$(STUDENT)',student)
    with open(studentConfig,'w') as f:
      f.write(contents)
  
    if runTrueModel:
      # plan with the TRUE model
      plannerContents = plannerConfig.replace('$(PREDATOR)','student').replace('$(OPTIONS)','"student":"%s"' % student)
      with open(plannerConfigFile,'w') as f:
        f.write(plannerContents)
      basename = '20x20-%s-true' % student
      setupCondor(basename,numTestingTrials,1,[studentConfig,plannerConfigFile])
      subprocess.check_call(['condor_submit',os.path.join('condor',basename,'job.condor')])

    # plan with the DT model
    plannerContents = plannerConfig.replace('$(PREDATOR)','dt').replace('$(OPTIONS)','"filename":"data/dt/weighted/%s-%i.weka"' % (student,numTrainingTrials))
    with open(plannerConfigFile,'w') as f:
      f.write(plannerContents)
    basename = '20x20-%s-dt-%i' % (student,numTrainingTrials)
    setupCondor(basename,numTestingTrials,1,[studentConfig,plannerConfigFile])
    subprocess.check_call(['condor_submit',os.path.join('condor',basename,'job.condor')])

if __name__ == '__main__':
  import sys
  usage = 'Usage: compareStudentsToDTs.py [options] numTestingTrials numTrainingTrials \nOptions:\n  -t do not run the true model'
  args = sys.argv[1:]

  runTrueModel = True
  if '-t' in args:
    runTrueModel = False
    args.remove('-t')
  
  if len(args) != 2:
    print usage
    sys.exit(1)
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)

  numTestingTrials = int(args[0])
  numTrainingTrials = int(args[1])
  main(numTestingTrials,numTrainingTrials,runTrueModel)
