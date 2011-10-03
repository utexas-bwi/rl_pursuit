#!/usr/bin/env python

import subprocess

with open('data/aamas11students.txt','r') as f:
  students = set(f.read().split())

with open('configs/student.json','r') as f:
  config = f.read()

for student in students:
  contents = config.replace('data/aamas11students.txt',student)
  contents = contents.replace('"dtcsv":""','"dtcsv":"results/dtcsv/%s.csv"'%student)
  with open('configs/student-test.json','w') as f:
    f.write(contents)
  subprocess.check_call(['bin/main','configs/student-test.json','configs/trials.json'])
