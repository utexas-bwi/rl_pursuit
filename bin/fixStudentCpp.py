#!/usr/bin/env python

import subprocess, os, re

def getStudents():
  proc = subprocess.Popen('ls src/studentAgents/agents/*/Predator.h -1',shell=True,stdout = subprocess.PIPE)
  output = proc.communicate()[0].strip()
  files = output.split('\n')
  students = [os.path.basename(os.path.dirname(f)) for f in files]
  return students

def main():
  pattern = re.compile('(class Predator[^:{\n]*)({?)$',re.MULTILINE)
  replacement = '\\1: public AbstractCppPredator \\2'
  students = getStudents()
  for student in students:
    with open('src/studentAgents/origAgents/%s/Predator.h' % student,'r') as f:
      contents = f.read()
    contents = re.sub(pattern,replacement,contents)
    namespaceStr = '\nnamespace STUDENT_PREDATOR_%s {\n' % student
    ind = contents.rfind('#include')
    while contents[ind] != '\n':
      ind += 1
    ind += 1
    contents = contents[:ind] + namespaceStr + contents[ind:]
    #contents = contents.replace('class','nampespace STUDENT_PREDATOR_%s
    newContents = '''
#ifndef _STUDENT_PREDATOR_%s_
#define _STUDENT_PREDATOR_%s_

#include <controller/PredatorStudentCpp.h>

%s
}
#endif
''' % (student,student,contents)
    with open('src/studentAgents/agents/%s/Predator.h' % student,'w') as f:
      f.write(newContents)

    with open('src/studentAgents/origAgents/%s/Predator.cxx' % student,'r') as f:
      contents = f.read()
    ind = contents.rfind('#include')
    while contents[ind] != '\n':
      ind += 1
    ind += 1
    newContents = contents[:ind] + namespaceStr + contents[ind:] + '\n}\n'
    with open('src/studentAgents/agents/%s/Predator.cxx' % student,'w') as f:
      f.write(newContents)
    

if __name__ == '__main__':
  main()
