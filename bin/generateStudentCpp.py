#!/usr/bin/env python

import subprocess, os, re

def getStudents():
  proc = subprocess.Popen('ls src/studentAgents/agents/*/Predator.h -1',shell=True,stdout = subprocess.PIPE)
  output = proc.communicate()[0].strip()
  files = output.split('\n')
  students = [os.path.basename(os.path.dirname(f)) for f in files]
  return students

def main(dest):
  students = getStudents()
  with open(dest,'w') as out:
    for student in students:
      out.write('#include <studentAgents/agents/%s/Predator.h>\n' % student)
    out.write('\n\n')
    out.write('boost::shared_ptr<AbstractCppPredator> PredatorStudentCpp::createPredator(const std::string &name, unsigned int predatorInd) {\n')
    out.write('  int dims[2];\n')
    out.write('  int moves[5][2];\n')
    out.write('  dims[0] = this->dims.x;\n')
    out.write('  dims[1] = this->dims.y;\n')
    for i,student in enumerate(students):
      with open('src/studentAgents/agents/%s/config.py'%student,'r') as f:
        contents = f.read()
      predators = re.findall('Predator\.([^,\]]*)',contents)
      out.write('  ')
      if i != 0:
        out.write('else ')
      out.write('if (name == %s) {\n' % student)
      for predInd,predator in enumerate(predators):
        out.write('    ')
        if predInd != 0:
          out.write('else ')
        out.write('if (predatorInd == %i) {\n' % predInd)
        out.write('      return boost::shared_ptr<AbstractCppPredator>(%s::%s())\n' % (student,predator))
        out.write('    }\n')
      #out.write('    return new
      out.write('  }\n')
    out.write('}\n')

if __name__ == '__main__':
  main('src/controller/PredatorStudentCpp_gen.cpp')
