#!/usr/bin/env python
  
import os, shutil

def createCondorConfig(name,numJobs,arguments):
  inFilename = 'condor/base.condor'
  outFilename = 'condor/configs/%s.condor' % name

  with open(inFilename,'r') as f:
    contents = f.read()
  contents = contents.replace('$(NAME)',name)
  contents = contents.replace('$(NUM_JOBS)',str(numJobs))
  contents = contents.replace('$(ARGUMENTS)',arguments)
  with open(outFilename,'w') as f:
    f.write(contents)

def createPursuitConfig(name,numTrials,numTrialsPerJob):
  outFilename = 'condor/configs/%s.json' % name
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
  with open(outFilename,'w') as f:
    f.write(contents)

def makeDir(name):
  shutil.rmtree(name)
  os.mkdir(name)

def makeCondorDirs(name):
  makeDir(os.path.join('condor','output',name))
  makeDir(os.path.join('condor','results',name))

def run(name,numTrials,numTrialsPerJob,configs):
  numJobs = numTrials / numTrialsPerJob
  createCondorConfig(name,numJobs,'$(Process) %s condor/configs/%s.json' % (' '.join(configs),name))
  createPursuitConfig(name,numTrials,numTrialsPerJob)
  makeCondorDirs(name)

def main(args):
  if len(args) < 4:
    print 'Invalid number of arguments'
    print 'Usage: setupCondor.py name numTrials numTrialsPerJob config1 [config2 ...]'
    return 1
  name = args[0]
  numTrials = int(args[1])
  numTrialsPerJob = int(args[2])
  configs = args[3:]
  run(name,numTrials,numTrialsPerJob,configs)

if __name__ == '__main__':
  import sys
  sys.exit(main(sys.argv[1:]))
