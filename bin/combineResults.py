#!/usr/bin/env python

import os, sys, shutil

def getFilenames(sourceDir,expectedNumEpisodes):
  filenames = []
  usedModelFiles = False
  for i in range(expectedNumEpisodes):
    filename = os.path.join(sourceDir,'results','%i.csv' % i)
    if not(os.path.isfile(filename)):
      # try to recover using the length of the models file
      modelFile = os.path.join(sourceDir,'models','%i.txt' % i)
      if os.path.isfile(modelFile):
        filenames.append([modelFile,True])
        usedModelFiles = True
      else:
         print >>sys.stderr,'Missing episode: %i' % i
         sys.exit(3)
    else:
      filenames.append([filename,False])
      
  return filenames, usedModelFiles

def run(targetBase,sourceDir,expectedNumEpisodes):
  sourceJSON = os.path.join(sourceDir,'config.json')
  if not(os.path.isfile(sourceJSON)):
    print 'Source json not found:',sourceJSON
    return 3
  dirPath,targetName = os.path.split(sourceDir)
  if targetName == '':
    _,targetName = os.path.split(dirPath)
  filenames,usedModelFiles = getFilenames(sourceDir,expectedNumEpisodes)
  assert(len(filenames) == expectedNumEpisodes)
  if usedModelFiles:
    targetName += '-incomplete'
    print '  Incomplete, using model output'
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
  options,args = parser.parse_args(args)
  if len(args) < 1:
    print 'Invalid number of arguments'
    print usage
    return 1
  sourceDirs = args[0:]
  expectedNumEpisodes = 1000
  for sourceDir in sourceDirs:
    print 'Combining',sourceDir
    res = run(options.target,sourceDir,expectedNumEpisodes)
    if res != 0:
      return res
  return res

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
