#!/usr/bin/env python

import subprocess

numTrials = 100

with open('data/aamas11students.txt','r') as f:
  students = set(f.read().split())

with open('configs/student_output.json','r') as f:
  config = f.read()

with open('configs/trials-test.json','w') as f:
  f.write('{\n')
  f.write('"trials": %i\n' % numTrials)
  f.write('}\n')

outputFilenames = []
for student in students:
  contents = config.replace('data/aamas11students.txt',student)
  outputFilename = 'results/dtfile/%s.arff' % student
  outputFilenames.append(outputFilename)
  contents = contents.replace('"dtfile":""','"dtfile":"%s"'%outputFilename)
  with open('configs/student-test.json','w') as f:
    f.write(contents)
  subprocess.check_call(['bin/main','configs/student-test.json','configs/trials-test.json'])

combinedFilename = 'results/dtfile/combined.arff'
out = open(combinedFilename,'w')
for i,outputFilename in enumerate(outputFilenames):
  with open(outputFilename,'r') as f:
    lines = f.readlines()
  startLine = 0
  if i != 0:
    startLine = lines.index('@data\n') + 1 # + 1 to skip the data line
  out.writelines(lines[startLine:])

out.close()
