#!/usr/bin/env python

import os, re, subprocess

def isJobComplete(directory,jobNum):
  resFile = os.path.join(directory,'results','%i.csv'%jobNum)
  return os.path.exists(resFile)

def runJob(directory,jobNum,recoveryDir,contents):
  print jobNum
  jobFile = os.path.join(recoveryDir,'%i.condor' % jobNum)
  contents = contents.replace('$(Process)','%i' % jobNum)
  contents = re.sub('Queue.*','Queue',contents)
  with open(jobFile,'w') as f:
    f.write(contents)
  subprocess.check_call(['condor_submit',jobFile])

def main(directory,numJobs):
  recoveryDir = os.path.join(directory,'recovery')
  if not(os.path.exists(recoveryDir)):
    os.mkdir(recoveryDir)
  with open(os.path.join(directory,'job.condor')) as f:
    contents = f.read()
  for i in range(numJobs):
    if isJobComplete(directory,i):
      continue
    runJob(directory,i,recoveryDir,contents)

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  usage = 'recoverCondor.py directory numJobs'
  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  if len(args) != 2:
    print usage
    sys.exit(1)
  directory = args[0]
  numJobs = int(args[1])
  main(directory,numJobs)
