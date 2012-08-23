#!/usr/bin/env python

import os, sys, shutil

def getFilenames(sourceDir,expectedNumEpisodes,allowIncomplete=False):
  filenames = []
  incomplete = False
  for i in range(expectedNumEpisodes):
    filename = os.path.join(sourceDir,'results','%i.csv' % i)
    if not(os.path.isfile(filename)):
      if allowIncomplete:
        incomplete = True
        continue
      # try to recover using the length of the models file
      modelFile = os.path.join(sourceDir,'models','%i.txt' % i)
      if os.path.isfile(modelFile):
        filenames.append([modelFile,True])
        incomplete = True
        print 'model file'
      else:
        print >>sys.stderr,'Missing episode: %i' % i
        return None, True
    else:
      filenames.append([filename,False])
      
  return filenames, incomplete

def run(targetBase,sourceDir,expectedNumEpisodes,options):
  sourceJSON = os.path.join(sourceDir,'config.json')
  if not(os.path.isfile(sourceJSON)):
    print 'Source json not found:',sourceJSON
    return 3
  dirPath,targetName = os.path.split(sourceDir)
  if targetName == '':
    _,targetName = os.path.split(dirPath)
  filenames,incomplete = getFilenames(sourceDir,expectedNumEpisodes,options.allowIncomplete)
  if filenames is None:
    return 3
  if options.allowIncomplete and incomplete:
    pass
  else:
    assert(len(filenames) == expectedNumEpisodes)
  if incomplete:
    targetName += '-incomplete'
    print '  Incomplete',
    if options.allowIncomplete:
      print ''
    else:
      print ', using model output'
  targetCSV = os.path.join(targetBase,'%s.csv'%targetName)
  targetJSON = os.path.join(targetBase,'configs','%s.json'%targetName)
  if os.path.exists(targetCSV):
    print 'Target csv already exists:',targetCSV
    return 2
  if os.path.exists(targetJSON):
    print 'Target json already exists:',targetJSON
    return 2
  contents = ''
  for i,(filename,isModelFile) in enumerate(filenames):
    if isModelFile:
      with open(filename,'r') as f:
        steps = len(f.readlines())
        contents += '%s,%s\n' % (i,steps)
    else:
      with open(filename,'r') as f:
        contents += f.read()
  with open(targetCSV,'w') as f:
    f.write(contents)
  shutil.copy(sourceJSON,targetJSON)
  return 0

def main(args):
  from optparse import OptionParser
  usage = 'Usage: combineResults.py sourceDirectory [sourceDirectory ...]'
  parser = OptionParser(usage)
  parser.add_option('-t','--target',dest='target',action='store',type='str',default='results',help='Target directory for the results')
  parser.add_option('--allowIncomplete',dest='allowIncomplete',action='store_true',default=False,help='Allow incomplete runs to be summarized')
  retCode = 0
  options,args = parser.parse_args(args)
  if len(args) < 1:
    print 'Invalid number of arguments'
    print usage
    return 1
  sourceDirs = args[0:]
  expectedNumEpisodes = 1000
  for sourceDir in sourceDirs:
    print 'Combining',sourceDir
    res = run(options.target,sourceDir,expectedNumEpisodes,options)
    if res != 0:
      print 'Skipping %s' % sourceDir
      retCode = res
    #if res != 0:
      #return res
  return retCode

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
