#!/usr/bin/env python

import re, subprocess
from weka.common import getUniqueStudents

fracPattern = re.compile('Frac\s*Correct: \d+.?\d+\((0.\d+)\)')
numPattern = re.compile('Num\s*Correct: \d+.?\d+\((0.\d+)\)')
configFile = 'classifierTest.json'
studentFile = 'data/newStudents29.txt'

students = getUniqueStudents(studentFile)

configContents = ''' {
  "initialTrain": false,
  "type": "dt",
  "shared": true,
  "filename": "data/dt/perturbed-noop0.1-%i/weighted/%s.weka",
  "caching": false
}
'''

configs = [
  [(1000,'only-%s'),'Small Target'],
  [(1000,'transfer50000-%s'),'Transfer'],
  [(50000,'only-%s'),'Full Target']
]

avgFracDiff = 0
avgNumDiff = 0

print 'student',
for _,label in configs:
  print ',',label + ' frac',',',label + ' num',
print ''
for student in students:
  fracs = []
  nums = []
  for config,label in configs:
    contents = (configContents % config) % student
    with open(configFile,'w') as f:
      f.write(contents)
    cmd = ['bin/32/runClassifier',configFile,'data/dt/perturbed-noop0.1-50000/train/%s.arff' % student,'0','--notrain']
    p = subprocess.Popen(cmd,stdout=subprocess.PIPE)
    output = p.communicate()[0]
    fracs.append(float(fracPattern.findall(output)[0]))
    nums.append(float(numPattern.findall(output)[0]))
  print ','.join([student] + ['%g,%g' % (f,n) for f,n in zip(fracs,nums)])
  avgFracDiff += fracs[1] - fracs[0]
  avgNumDiff += nums[1] - nums[0]
avgFracDiff /= len(students)
avgNumDiff /= len(students)
print 'AVG FRAC DIFF: ',avgFracDiff
print 'AVG NUM DIFF: ',avgNumDiff
