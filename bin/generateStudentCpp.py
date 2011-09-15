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
    out.write('bool PredatorStudentCpp::handlesStudent(const std::string &name) {\n')
    for student in students:
      out.write('  if (name == "%s")\n' % student)
      out.write('    return true;\n')
    out.write('  return false;\n')
    out.write('}\n')

    code = '''
boost::shared_ptr<AbstractCppPredator> PredatorStudentCpp::createPredator(const std::string &name, unsigned int predatorInd) {
  int dims[2];
  bool toroidalWorld = true;
  int moves[Action::NUM_MOVES][2];
  
  dims[0] = this->dims.x;
  dims[1] = this->dims.y;
  
  for (int i = 0; i < 5; i++) {
    moves[i][0] = this->moves[i].x;
    moves[i][1] = this->moves[i].y;
  }
'''
    out.write(code)
    for i,student in enumerate(students):
      with open('src/studentAgents/agents/%s/config.py'%student,'r') as f:
        contents = f.read()
      predators = re.findall('Predator\.([^,\]]*)',contents)
      out.write('  ')
      if i != 0:
        out.write('else ')
      out.write('if (name == "%s") {\n' % student)
      for predInd,predator in enumerate(predators):
        out.write('    ')
        if predInd != 0:
          out.write('else ')
        out.write('if (predatorInd == %i) {\n' % predInd)
        out.write('      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_%s::%s(dims,toroidalWorld,moves));\n' % (student,predator))
        out.write('    }\n')
      #out.write('    return new
      out.write('  }\n')
    out.write('  std::cerr << "PredatorStudentCpp::createPredator: ERROR unknown student name: " << name << std::endl;\n')
    out.write('  exit(6);\n')
    out.write('}\n')

if __name__ == '__main__':
  main('src/controller/PredatorStudentCpp_gen.cpp')
