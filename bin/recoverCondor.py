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

def main(directory,numJobs,simulate, cluster):
  recoveryDir = os.path.join(directory,'recovery')
  if cluster is not None:
    runningJobs = getRunningJobs(cluster)
  if not(os.path.exists(recoveryDir)):
    if simulate:
      print 'Would make dir: %s' % recoveryDir
    else:
      os.mkdir(recoveryDir)
  with open(os.path.join(directory,'job.condor')) as f:
    contents = f.read()
  if simulate:
    print 'Would run jobs:',
  for i in range(numJobs):
    if isJobComplete(directory,i):
      continue
    if i in runningJobs:
      continue
    if simulate:
      print i,
    else:
      runJob(directory,i,recoveryDir,contents)
  print ''

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  usage = 'recoverCondor.py [options] directory numJobs'
  usage += '\nOptions:'
  usage += '\n  -s --simulate - only simulate, don\'t start the jobs or create the files'
  usage += '\n  -r CLUSTER - checks for currently running jobs'
  simulate = False
  cluster = None
  if '-s' in args:
    args.remove('-s')
    simulate = True
  if '--simulate' in args:
    args.remove('--simulate')
    simulate = True
  try:
    ind = args.index('-r')
    args.pop(ind)
    cluster = int(args.pop(ind))
  except:
    pass

  if args[0] in ['-h','--help']:
    print usage
    sys.exit(0)
  if len(args) != 2:
    print usage
    sys.exit(1)
  print 'CLUSTER: %s' % cluster
  directory = args[0]
  numJobs = int(args[1])
  main(directory,numJobs,simulate,cluster)
