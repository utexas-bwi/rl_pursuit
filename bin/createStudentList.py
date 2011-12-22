#!/usr/bin/env python

import random
from weka.common import parseArgs, getSelectedStudents

def main(students,filename,num=1000):
  with open(filename,'w') as f:
    for i in range(num):
      s = random.choice(students)
      f.write('%s\n' % s)

def mainArgs(args=None):
  from optparse import make_option
  commandLineOptions = []
  commandLineOptions.append(make_option('-s','--students',dest='studentFilename',action='store',default='data/newStudentsComplete.txt',help='filename of original student list (default is data/newStudentsComplete.txt)',metavar='FILE'))
  options,args,unprocessedArgs = parseArgs(usage='%prog [options] outputFilename',args=args,numArgs=1,unprocessedArgsAllowed=False,wekaFlag=False,options=commandLineOptions)
  filename = args[0]
  students = getSelectedStudents(options.studentFilename,options.includeStudents,options.excludeStudents)
  main(students,filename)

if __name__ == '__main__':
  import sys
  mainArgs(sys.argv[1:])
