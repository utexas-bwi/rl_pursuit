#!/usr/bin/env python

import sys, os, subprocess, re, time
from trainClassifier import mainOptions as trainClassifierMain
from trainClassifier import parseArgs
from common import getUniqueStudents

def addArgs(args,suffix='',studentInd = None):
  newArgs = ['twostagetransfer'+suffix]
  if studentInd is not None:
    newArgs += [str(studentInd)]
  newArgs += args + ['--no-source']
  return newArgs
 
def addSuffix(filename,suffix='-ordering'):
  base,ext = os.path.splitext(filename)
  return base + suffix + ext

def main(args=sys.argv[1:]):
  if '--suffix' in args:
    ind = args.index('--suffix')
    suffix = args[ind+1]
    del args[ind]
    del args[ind]
  else:
    suffix = ''

  if '--monitor' in args:
    args.remove('--monitor')
    return monitorCondor(args,suffix)

  optionsNoSuffix,_ = parseArgs(addArgs(args))

  args = addArgs(args,suffix)
  options,_ = parseArgs(args)

  if os.path.exists(options.saveFile):
    return

  if not(os.path.exists(addSuffix(options.saveFile))) and os.path.exists(addSuffix(optionsNoSuffix.saveFile)):
    import shutil
    shutil.copy(addSuffix(optionsNoSuffix.saveFile),addSuffix(options.saveFile))

  def repl(x):
    numStudentsToAdd = 1
    filename = ''
    saveFile = addSuffix(options.saveFile)
    if os.path.exists(saveFile):
      filename = options.saveFile

    x = x.replace('$(EVAL_PATH)','data/dt/studentsNew29-unperturbed-%i/weighted/only-$(EVAL_STUDENT).weka' % options.numSource)
    x = x.replace('$(SOURCE_DATA_PATH)','data/dt/studentsNew29-unperturbed-%i/train/$(DATA_STUDENT).arff' % options.numSource)
    x = x.replace('$(TARGET_STUDENT)',options.student)
    x = x.replace('$(NUM_STUDENTS_TO_ADD)',str(numStudentsToAdd))
    x = x.replace('$(FILENAME)',filename)
    return x

  trainClassifierMain(options,repl)

def monitorCondor(args,suffix):
  numStudents = len(getUniqueStudents())
  jobs = [-1 for i in range(numStudents)]
  unfinished = range(numStudents)
  saveFiles = []
  for studentInd in range(numStudents):
    options,_ = parseArgs(addArgs(args,suffix,studentInd))
    saveFiles.append(options.saveFile)
    
  while len(unfinished) > 0:
    needToRun = []
    # check what's running and still needs to run
    p = subprocess.Popen(['condor_q','sbarrett'],stdout=subprocess.PIPE)
    out,_ = p.communicate()
    for studentInd in list(unfinished):
      if out.find(str(jobs[studentInd])) < 0:
        if os.path.exists(saveFiles[studentInd]):
          unfinished.remove(studentInd)
        else:
          needToRun.append(studentInd)
    # submit new jobs as needed
    for studentInd in needToRun:
      jobs[studentInd] = submit([str(studentInd)] + args,suffix)
    # good-night sweet prince
    time.sleep(20)

def submit(args,suffix):
  options,_ = parseArgs(addArgs(args,suffix))
  base = 'condor/createTwoStageTransfer'
  orig = os.path.join(base,'base.condor')
  path = os.path.join(base,'jobs/%i%s.condor' % (options.studentInd,suffix))
  with open(orig,'r') as f:
    contents = f.read()
  contents = contents.replace('$(Process)','%i%s' % (options.studentInd,suffix))
  contents = contents.replace('$(ARGS)',' '.join(args))
  with open(path,'w') as f:
    f.write(contents)

  print 'SUBMITTING ',options.studentInd
  p = subprocess.Popen(['condor_submit',path],stdout=subprocess.PIPE)
  out,_ = p.communicate()
  jobNum = int(re.findall('cluster (\d+)',out)[0])
  return jobNum

if __name__ == '__main__':
  main()
