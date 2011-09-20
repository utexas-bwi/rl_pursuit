#!/usr/bin/env python
  
import os, shutil, re

def createCondorConfig(name,numJobs,arguments,outFilename):
  inFilename = 'condor/base.condor'

  with open(inFilename,'r') as f:
    contents = f.read()
  contents = contents.replace('$(NAME)',name)
  contents = contents.replace('$(NUM_JOBS)',str(numJobs))
  contents = contents.replace('$(ARGUMENTS)',arguments)
  with open(outFilename,'w') as f:
    f.write(contents)

def createPursuitConfig(name,numTrials,numTrialsPerJob,configs,outFilename):
  contents = '''
{
  "trials": %s,
  "trialsPerJob": %s,
  "save": {"results":"%s", "config":"%s"}
}
'''
  results = os.path.join('condor','results',name,'$(JOBNUM).csv')
  configSave = os.path.join('condor','results',name,'config.json')
  contents = contents % (numTrials,numTrialsPerJob,results,configSave)
  oldContents = ''
  for config in configs:
    with open(config,'r') as f:
      oldContents += f.read()
  contents = oldContents + contents
  contents = re.sub('\n}\s*{',',\n',contents,re.MULTILINE)
  with open(outFilename,'w') as f:
    f.write(contents)

def makeDir(name):
  try:
    shutil.rmtree(name)
  except:
    pass
  os.mkdir(name)

def makeCondorDirs(name):
  makeDir(os.path.join('condor','output',name))
  makeDir(os.path.join('condor','results',name))

def run(name,numTrials,numTrialsPerJob,configs):
  numJobs = numTrials / numTrialsPerJob
  jsonFilename = os.path.join('condor','configs','%s.json'%name)
  condorFilename = os.path.join('condor','configs','%s.condor' % name)
  if os.path.exists(jsonFilename):
    print 'ERROR: %s already exists, exiting' % jsonFilename
    return 2
  if os.path.exists(condorFilename):
    print 'ERROR: %s already exists, exiting' % condorFilename
    return 2
  
  createPursuitConfig(name,numTrials,numTrialsPerJob,configs,jsonFilename)
  createCondorConfig(name,numJobs,'$(Process) %s' % jsonFilename,condorFilename)
  makeCondorDirs(name)
  return 0

def main(args):
  if len(args) < 4:
    print 'Invalid number of arguments'
    print 'Usage: setupCondor.py name numTrials numTrialsPerJob config1 [config2 ...]'
    return 1
  name = args[0]
  numTrials = int(args[1])
  numTrialsPerJob = int(args[2])
  configs = args[3:]
  return run(name,numTrials,numTrialsPerJob,configs)

if __name__ == '__main__':
  import sys
  sys.exit(main(sys.argv[1:]))
