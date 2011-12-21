#!/usr/bin/env python
  
import os, re, sys

def createCondorConfig(baseDir,numJobs,arguments,outFilename):
  inFilename = 'condor/base.condor'

  with open(inFilename,'r') as f:
    contents = f.read()
  contents = contents.replace('$(BASE_DIR)',baseDir)
  contents = contents.replace('$(NUM_JOBS)',str(numJobs))
  contents = contents.replace('$(ARGUMENTS)',arguments)
  with open(outFilename,'w') as f:
    f.write(contents)

def createPursuitConfig(directory,numTrials,numTrialsPerJob,configs,outFilename):
  contents = '''
{
  "trials": %s,
  "trialsPerJob": %s,
  "save": {"results":"$(DIR)/results/$(JOBNUM).csv", "config":"$(DIR)/results/config.json"},
  "dir": "%s"
}
'''
  contents = contents % (numTrials,numTrialsPerJob,directory)
  oldContents = ''
  for config in configs:
    with open(config,'r') as f:
      oldContents += f.read()
  contents = oldContents + contents
  contents = re.sub('\n}\s*{',',\n',contents,re.MULTILINE)
  with open(outFilename,'w') as f:
    f.write(contents)

def makeDir(name):
  #try:
    #shutil.rmtree(name)
  #except:
    #pass
  os.mkdir(name)

def makeCondorDirs(baseDir):
  makeDir(os.path.join(baseDir))
  makeDir(os.path.join(baseDir,'output'))
  makeDir(os.path.join(baseDir,'results'))
  makeDir(os.path.join(baseDir,'models'))

def main(baseDir,numTrials,numTrialsPerJob,configs):
  if os.path.exists(baseDir):
    print 'ERROR: %s already exists, exiting' % baseDir
    return 2
  print 'Using baseDir: %s' % baseDir
  makeCondorDirs(baseDir)
  numJobs = numTrials / numTrialsPerJob
  jsonFilename = os.path.join(baseDir,'config.json')
  condorFilename = os.path.join(baseDir,'job.condor')
  if os.path.exists(jsonFilename):
    print 'ERROR: %s already exists, exiting' % jsonFilename
    return 2
  if os.path.exists(condorFilename):
    print 'ERROR: %s already exists, exiting' % condorFilename
    return 2
  
  createPursuitConfig(baseDir,numTrials,numTrialsPerJob,configs,jsonFilename)
  createCondorConfig(baseDir,numJobs,'$(Process) %s' % jsonFilename,condorFilename)
  return 0

def selectBaseDir(configName,suffix,name=None):
  DIR = 'condor'
  if name is not None:
    return os.path.join(DIR,name)
  if len(suffix) > 0:
    if suffix[0] != '-':
      suffix = '-' + suffix
  baseName,_ = os.path.splitext(os.path.basename(configName))
  baseDir = os.path.join(DIR,'%s%s' % (baseName,suffix))
  while os.path.exists(baseDir):
    print '%s already exists' % baseDir
    print 'Current suffix: %s' % suffix
    suffix = raw_input('Input new suffix (empty to abort): ')
    if len(suffix) == 0:
      sys.exit(1)
    if suffix[0] != '-':
      suffix = '-' + suffix
    baseDir = os.path.join(DIR,'%s%s' % (baseName,suffix))
  return baseDir

def mainArgs(args=None):
  if args is None:
    args = sys.argv[1:]
  from optparse import OptionParser
  parser = OptionParser('%prog [options] config [config ...]')
  parser.add_option('-t','--trials',dest='numTrials',action='store',type='int',default=1000,help='number of trials to run',metavar='NUM')
  parser.add_option('-j','--trialsPerJob',dest='numTrialsPerJob',action='store',type='int',default=1,help='number of trials per job to run',metavar='NUM')
  parser.add_option('-s','--suffix',dest='suffix',action='store',type='str',default='',help='suffix to job name',metavar='STR')
  parser.add_option('-n','--name',dest='name',action='store',type='str',default=None,help='name of job',metavar='STR')
  options,args = parser.parse_args()
  if len(args) < 1:
    parser.parse_args(['--help'])
    sys.exit(1)
  baseDir = selectBaseDir(args[0],options.suffix,options.name)
  sys.exit(main(baseDir,options.numTrials,options.numTrialsPerJob,args))

if __name__ == '__main__':
  mainArgs()
