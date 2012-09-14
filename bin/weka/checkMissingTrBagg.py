#!/usr/bin/env python

import sys, os
from common import getUniqueStudents

directory = sys.argv[1]
if not os.path.exists(directory):
  print 'Directory does not exist'
  sys.exit(2)
prefix = sys.argv[2]
numJobs = 1000
students = getUniqueStudents()
for student in students:
  missing = []
  for job in range(numJobs):
    path = os.path.join(directory, '%s-%s-%i.weka' % (prefix,student,job))
    if not os.path.exists(path):
      missing.append(job)
  if len(missing) == numJobs:
    print 'Probably a problem with your directory or prefix'
    sys.exit(3)
  if len(missing) != 0:
    print student,' '.join(map(str,missing))
