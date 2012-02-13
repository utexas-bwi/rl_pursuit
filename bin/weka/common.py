#!/usr/bin/env python

import numpy, os

BASE_DIR = os.path.join('data','dt')
BIN_PATH = os.path.join('bin','weka')
#DEFAULT_STUDENT_FILENAME = os.path.join('data','newStudentsComplete.txt')
DEFAULT_STUDENT_FILENAME = os.path.join('data','students.txt')

# filetypes
TRAIN      = 0
DESC       = 1
UNWEIGHTED = 2
WEIGHTED   = 3
NUM_TYPES  = 4

FILETYPE_DIRS = ['train','desc','unweighted','weighted']
FILETYPE_EXTS = ['.arff','.txt','.weka',     '.weka']

def getFilename(base,name,filetype):
  assert(0 <= filetype < NUM_TYPES)
  d = FILETYPE_DIRS[filetype]
  ext = FILETYPE_EXTS[filetype]
  return os.path.join(BASE_DIR,base,d,'%s%s' % (name,ext))

def getStudents(filename=DEFAULT_STUDENT_FILENAME):
  return numpy.loadtxt(filename,dtype=str)

def getUniqueStudents(filename=DEFAULT_STUDENT_FILENAME):
  return sorted(list(set(getStudents(filename))))

def getSelectedStudents(filename=DEFAULT_STUDENT_FILENAME,includeStudents=[],excludeStudents=[]):
  students = getUniqueStudents(filename)
  if len(includeStudents) > 0:
    for student in includeStudents:
      assert(student in students)
    students = includeStudents

  if len(excludeStudents) > 0:
    for student in excludeStudents:
      students.remove(student)

  assert(len(students) > 0)
  return students

def makeDirs(base,printWarning=True):
  for d in FILETYPE_DIRS:
    path = os.path.join(BASE_DIR,base,d)
    if os.path.exists(path):
      if printWarning:
        print '%s already exists, skipping' % path
    else:
      os.mkdir(path)

def readArff(filename):
  with open(filename,'r') as f:
    temp = f.readlines()
  ind = temp.index('@data\n') + 1 # get the line where the data starts
  return temp[:ind],temp[ind:]

def readStudents(base,students):
  header = None
  studentData = []
  for student in students:
    filename = getFilename(base,student,TRAIN)
    header,lines = readArff(filename)
    studentData.append(lines)
  return header,studentData

def writeData(header,studentData,outputFilename,excludeInd=None):
  if excludeInd is None:
    data = studentData
  else:
    data = studentData[:excludeInd] + studentData[excludeInd+1:]
  contents = header + sum(data,[])
  with open(outputFilename,'w') as f:
    f.writelines(contents)

def makeTemp(*args,**kwargs):
  import tempfile
  fd,temp = tempfile.mkstemp(*args,**kwargs)
  os.close(fd)
  return temp

def parseArgs(usage=None,options=[],args=None, minArgs=None, maxArgs=None, numArgs=0, studentOptions=True,unprocessedArgsAllowed=True,wekaFlag=True):
  import sys
  from optparse import OptionParser
  parser = OptionParser(usage)
  if studentOptions:
    parser.add_option('-i','--include',action='append',dest='includeStudents',default=[],help='output only for specified students',metavar='STUDENT')
    parser.add_option('-x','--exclude',action='append',dest='excludeStudents',default=[],help='output excluding specified students',metavar='STUDENT')
  if wekaFlag:
    parser.add_option('--weka',action='store_true',dest='useWeka',default=False,help='use weka instead of Sam\'s dts')
  for option in options:
    parser.add_option(option)
  if args is None:
    args = sys.argv[1:]
  
  unprocessedArgs = []
  if '--' in args:
    ind = args.index('--')
    unprocessedArgs = args[ind+1:]
    args = args[:ind]

  resOptions,args = parser.parse_args(args)

  if minArgs is None:
    minArgs = numArgs
  if maxArgs is None:
    maxArgs = numArgs
  if not(minArgs <= len(args) <= maxArgs):
    if minArgs == maxArgs:
      exp = '%i' % minArgs
    else:
      exp = '%i-%i' % (minArgs,maxArgs)
    print 'Expected %s arguments, received %i' % (exp,len(args))
    parser.parse_args(['--help'])
    sys.exit(1)
  if not(unprocessedArgsAllowed) and (len(unprocessedArgs) > 0):
    print 'Unprocessed args not allowed, get rid of the --'
    parser.parse_args(['--help'])
    sys.exit(1)
  return resOptions,args,unprocessedArgs

def getArch():
  import os
  if os.uname()[4] == 'x86_64':
    return '64'
  else:
    return '32'
