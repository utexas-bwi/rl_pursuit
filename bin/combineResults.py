#!/usr/bin/env python

import os
import shutil

def getFilenames(sourceDir):
  filenames = []
  i = 0
  while True:
    filename = os.path.join(sourceDir,'%i.csv' % i)
    if not(os.path.isfile(filename)):
      return filenames
    filenames.append(filename)
    i += 1

def run(targetBase,sourceDir):
  sourceJSON = os.path.join(sourceDir,'config.json')
  if not(os.path.isfile(sourceJSON)):
    print 'Source json not found:',sourceJSON
    return 3
  dirPath,targetName = os.path.split(sourceDir)
  if targetName == '':
    _,targetName = os.path.split(dirPath)
  targetCSV = os.path.join(targetBase,'%s.csv'%targetName)
  targetJSON = os.path.join(targetBase,'%s.json'%targetName)
  if os.path.exists(targetCSV):
    print 'Target csv already exists:',targetCSV
    return 2
  if os.path.exists(targetJSON):
    print 'Target json already exists:',targetJSON
    return 2
  filenames = getFilenames(sourceDir)
  contents = ''
  for filename in filenames:
    with open(filename,'r') as f:
      contents += f.read()
  with open(targetCSV,'w') as f:
    f.write(contents)
  shutil.copy(sourceJSON,targetJSON)
  return 0

def main(args):
  usage = 'Usage: combineResults.py targetBase sourceDirectory'
  if (len(args) == 1) and (args[0] in ('-h','--help')):
    print usage
    return 0
  if len(args) < 2:
    print 'Invalid number of arguments'
    print usage
    return 1
  target = args[0]
  sourceDir = args[1]
  return run(target,sourceDir)

if __name__ == '__main__':
  import sys
  sys.exit(main(sys.argv[1:]))
