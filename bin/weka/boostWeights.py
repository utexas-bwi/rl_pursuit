#!/usr/bin/env python

import re, os, subprocess, sys
from common import getArch, getUniqueStudents
from createTransferDTs import extractTree

def parseDesc(filename):
  with open(filename,'r') as f:
    contents = f.read()
  basePattern = '([.0-9]+) (\w+)'
  pattern = '\s*'.join(28 * [basePattern]) + '\s*TwoStageTrAdaBoost'
  #pattern = '%s\s*TwoStageTrAdaBoost' % basePattern
  match = re.search(pattern,contents)
  if match is None:
    return None
  groups = match.groups()
  vals = {}
  for i in range(0,len(groups),2):
    amount = float(groups[i])
    student = groups[i+1]
    if amount > 1e-10:
      vals[student] = amount
  return vals

def main(targetDir,sourceDir,inPrefix,outPrefix,studentInd):
  #sourceDir = 'data/dt/perturbed-noop0.1-50000'
  #targetDir = 'data/dt/perturbed-noop0.1-1000'
  inFilename = inPrefix + '-%s.desc'
  outDesc = outPrefix + '-%s.desc'
  outTree = outPrefix + '-%s.weka'
  studentFile = 'data/newStudents29.txt'
 
  students = getUniqueStudents(studentFile)
  #maxLength = 0
  for i,student in enumerate(students):
    if (studentInd is not None) and (i != studentInd):
      continue
    filename = os.path.join(targetDir,'desc',inFilename % student)
    #print student
    res = parseDesc(filename)
    #print res
    if res is None:
      print >>sys.stderr,'SKIPPING',student
      continue
    #maxLength = max(maxLength,len(res))

    cmd = ['bin/%s/boostWeights' % getArch(),student,targetDir,sourceDir]
    for k,v in res.iteritems():
      cmd += [k,str(v)]
    #print ' '.join(cmd)
    outDescPath = os.path.join(targetDir,'desc',outDesc % student)
    outTreePath = os.path.join(targetDir,'weighted',outTree % student)
    subprocess.check_call(cmd,stdout=open(outDescPath,'w'))
    extractTree(outDescPath,outTreePath)
  
  #print 'MAX LENGTH: ',maxLength

if __name__ == '__main__':
  args = sys.argv[1:]
  studentInd = None
  usage = 'Usage: boostWeights.py targetDir sourceDir inPrefix outPrefix [--only NUM]'
  if ('-h' in args) or ('--help' in args):
    print usage
    sys.exit(0)
  if '--only' in args:
    ind = args.index('--only')
    studentInd = int(args[ind+1])
    args = args[:ind] + args[ind+2:]
  if len(args) != 4:
    print usage
    sys.exit(1)
  targetDir = args[0]
  sourceDir = args[1]
  inPrefix = args[2]
  outPrefix = args[3]
  main(targetDir,sourceDir,inPrefix,outPrefix,studentInd)
