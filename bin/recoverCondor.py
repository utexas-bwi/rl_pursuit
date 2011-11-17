#!/usr/bin/env python

import os, re, subprocess

def isJobComplete(directory,cluster):
  resFile = os.path.join(directory,'results','%i.csv'%cluster)
  return os.path.exists(resFile)

def runJob(directory,cluster,recoveryDir,contents):
  print cluster
  jobFile = os.path.join(recoveryDir,'%i.condor' % cluster)
  contents = contents.replace('$(Process)','%i' % cluster)
  contents = re.sub('Queue.*','Queue',contents)
  with open(jobFile,'w') as f:
    f.write(contents)
  subprocess.check_call(['condor_submit',jobFile])

def getRunningJobs(cluster):
  p = subprocess.Popen(['condor_q',str(cluster)],stdout=subprocess.PIPE)
  out = p.communicate()[0]
  res = map(int,re.findall('%i\.(\d+)' % cluster,out))
  return res

def main(directory,numJobs,cluster):
  recoveryDir = os.path.join(directory,'recovery')
  if cluster is not None:
    runningJobs = getRunningJobs(cluster)
  else:
    runningJobs = []

  if not(os.path.exists(recoveryDir)):
    print 'Making dir: %s' % recoveryDir
    os.mkdir(recoveryDir)
  with open(os.path.join(directory,'job.condor')) as f:
    contents = f.read()
  jobsToRun = []
  for i in range(numJobs):
    if isJobComplete(directory,i):
      continue
    if i in runningJobs:
      continue
    jobsToRun.append(i)
  
  print 'Want to run jobs:',
  for job in jobsToRun:
    print job,
  print ''
  cont = None
  while cont is None:
    val = raw_input('Continue (y/n): ')
    if val == 'y':
      cont = True
    elif val == 'n':
      cont = False
  
  if cont:
    for job in jobsToRun:
      runJob(directory,job,recoveryDir,contents)

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  usage = 'recoverCondor.py directory numJobs [cluster]'
  cluster = None
  helpStrings = ['-h','--help']
  for helpString in helpStrings:
    if helpString in args:
      print usage
      sys.exit(0)
  if (len(args) < 2) or (len(args) > 3):
    print usage
    sys.exit(1)
  directory = args[0]
  numJobs = int(args[1])
  if len(args) >= 3:
    cluster = int(args[2])
  print 'CLUSTER: %s' % cluster
  main(directory,numJobs,cluster)
