#!/usr/bin/env python

import numpy

def readFile(filename):
  numSteps = []
  with open(filename,'r') as f:
    for line in f:
      job,steps = map(int,line.split(','))
      numSteps.append(steps)
  return numpy.array(numSteps)

def main(filenames):
  results = []
  for filename in filenames:
    results.append(readFile(filename))
  with open('data/students.txt','r') as f:
    students = numpy.array([s.strip() for s in f.readlines()])
  uniqueStudents = list(set(students))
  
  print 'student,',
  for filename in filenames:
    print filename,',',
  print ''
  for student in uniqueStudents:
    print student,',',
    filt = (students == student)
    for res in results:
      print '%2.1f,' % res[filt].mean(),
    print ''
 
  for filename,numSteps in zip(filenames,results):
    print '------------------'
    print filename
    for steps,student in zip(numSteps,students):
      if steps > 1000:
        print student,steps

if __name__ == '__main__':
  import sys
  args = sys.argv[1:]
  main(args)
