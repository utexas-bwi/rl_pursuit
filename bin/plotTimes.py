#!/usr/bin/env python

import sys
import matplotlib.pyplot as plt

args = sys.argv[1:]
assert(len(args) == 2)
filename = args[0]
selStudent = args[1]

students = []
steps = []
times = []
selSteps = []
selTimes = []

with open(filename,'r') as f:
  for line in f:
    student,step,time = line.split(',')
    students.append(student)
    steps.append(int(step))
    times.append(float(time))
    if student == selStudent:
      selSteps.append(steps[-1])
      selTimes.append(times[-1])
minTime = min(times)
times = [t - minTime for t in times]
selTimes = [t - minTime for t in selTimes]

plt.scatter(steps,times)
plt.scatter(selSteps,selTimes,c='r')
plt.show()
