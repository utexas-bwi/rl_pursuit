#!/usr/bin/env python

import subprocess, os, shutil, re

SOURCE='src/studentAgents/origAgentsNew'

def getDest(path):
  return path.replace('origAgentsNew','agentsNew')

def runCmd(cmd,shell):
  p = subprocess.Popen(cmd,shell=shell,stdout=subprocess.PIPE)
  out = p.communicate()[0]
  ret = p.poll()
  if ret != 0:
    raise ValueError('Bad return code %i for command %s' % (ret,cmd))
  return out

def getStudentsForPattern(suffix):
  studentsPath = runCmd('ls -d -1 %s/*/%s' % (SOURCE,suffix),True).strip().split('\n')
  return studentsPath

def getStudents():
  pythonStudents = getStudentsForPattern('pythonPredator')
  cppStudents    = getStudentsForPattern('cppPredator')
  return pythonStudents,cppStudents

def setupPythonPredators(students):
  with open(getDest(os.path.join(SOURCE,'__init__.py')),'w') as f:
    pass
  for student in students:
    with open(getDest(os.path.join(os.path.dirname(student),'__init__.py')),'w') as f:
      pass
    with open(getDest(os.path.join(student,'__init__.py')),'w') as f:
      f.write('from Predator import generatePredators\n')
    mySrc = os.path.join(student,'MyPredator*.py')
    myDest = getDest(student)
    subprocess.check_call(' '.join(['cp',mySrc,myDest]),shell=True)

    src = os.path.join(student,'Predator.py')
    dest = getDest(src)
    with open(src,'r') as f:
      contents = f.read()
    
    contents = contents.replace('from ..util.common import abstract','def abstract():\n  raise NotImplementedError')

    with open(dest,'w') as f:
      f.write(contents)

def addNamespace(contents,namespaceStr):
  # find the last include
  ind = contents.rfind('#include')
  while contents[ind] != '\n':
    ind += 1
  ind += 1
  return contents[:ind] + namespaceStr + contents[ind:] + '\n}\n'

def setupCppPredators(students):
  for studentPath in students:
    student = os.path.basename(os.path.dirname(studentPath))
    srcH = os.path.join(studentPath,'MyPredator.h')
    destH = getDest(srcH)
    srcCxx = os.path.join(studentPath,'MyPredator.cxx')
    destCxx = getDest(srcCxx).replace('cxx','cpp')
    namespaceStr = '\nnamespace STUDENT_PREDATOR_%s{\n' % student
    
    # process the header
    with open(srcH,'r') as f:
      contents = f.read()
    contents = contents.replace('\r\n','\n')
    contents = re.sub('#ifndef.*\n','',contents)
    contents = re.sub('#define.*\n','',contents)
    contents = re.sub('#endif.*\n','',contents)
    contents = re.sub('#include\s*"Predator.h"\s*\n','',contents)
    contents = contents.replace('public Predator','public PredatorStudentCppAbstract')
    contents = addNamespace(contents,namespaceStr)
    contents = '''
#ifndef _STUDENT_PREDATOR_%s_
#define _STUDENT_PREDATOR_%s_

#include <controller/PredatorStudentCppAbstract.h>

%s

#endif
''' % (student,student,contents)
    
    with open(destH,'w') as f:
      f.write(contents)

    # process the cxx
    with open(srcCxx,'r') as f:
      contents = f.read()
    contents = contents.replace('\r\n','\n')
    contents = contents.replace('Predator(dims)','PredatorStudentCppAbstract(dims)')
    contents = addNamespace(contents,namespaceStr)
    with open(destCxx,'w') as f:
      f.write(contents)

def main():
  pythonStudents,cppStudents = getStudents()
  # make the destination if necessary
  dest = getDest(SOURCE)
  if not(os.path.exists(dest)):
    # make the destinations for each student
    for path in pythonStudents + cppStudents:
      os.makedirs(getDest(path))

  setupPythonPredators(pythonStudents)
  setupCppPredators(cppStudents)

if __name__ == '__main__':
  main()
