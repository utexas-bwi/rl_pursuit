#!/usr/bin/env python

import subprocess, sys, numpy

p = subprocess.Popen('bin/recoverCondor.py %s 1000 | grep -o -E "[0-9]* "' % (' '.join(map(str,sys.argv[1:]))),shell=True,stdout=subprocess.PIPE,stdin=subprocess.PIPE)
output = p.communicate('n')[0].strip()
lines = [int(o.strip()) for o in output.split('\n')]

with open('data/newStudents29.txt','r') as f:
  students = numpy.array([s.strip() for s in f.readlines()])

incompleteStudents = list(students[lines])
uniqueStudents = list(set(incompleteStudents))
for student in uniqueStudents:
  print student,incompleteStudents.count(student)
print lines
