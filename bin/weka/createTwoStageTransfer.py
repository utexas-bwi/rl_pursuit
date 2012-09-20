#!/usr/bin/env python

import sys, os, subprocess, re, time
from trainClassifier import mainOptions as trainClassifierMain
from trainClassifier import parseArgs

def addArgs(args,studentInd = None):
  newArgs = ['twostagetransfer']
  if studentInd is not None:
    newArgs += [str(studentInd)]
  newArgs += args + ['--no-source']
  return newArgs
  

def main(args=sys.argv[1:]):
  if '--monitor' in args:
    args.remove('--monitor')
    return monitorCondor(args)

  args = addArgs(args)
  options,_ = parseArgs(args)

  if os.path.exists(options.saveFile):
    return

  def repl(x):
    numStudentsToAdd = 1
    filename = ''
    base,ext = os.path.splitext(options.saveFile)
    saveFile = base + '-ordering' + ext
    if os.path.exists(saveFile):
      filename = options.saveFile

    x = x.replace('$(EVAL_PATH)','data/dt/studentsNew29-unperturbed-%i/weighted/only-$(EVAL_STUDENT).weka' % options.numSource)
    x = x.replace('$(SOURCE_DATA_PATH)','data/dt/studentsNew29-unperturbed-%i/train/$(DATA_STUDENT).arff' % options.numSource)
    x = x.replace('$(TARGET_STUDENT)',options.student)
    x = x.replace('$(NUM_STUDENTS_TO_ADD)',str(numStudentsToAdd))
    x = x.replace('$(FILENAME)',filename)
    return x

  trainClassifierMain(options,repl)

def monitorCondor(args):
  numStudents = 29
  jobs = [-1 for i in range(numStudents)]
  unfinished = range(numStudents)
  needToRun = range(numStudents)
  saveFiles = []
  for studentInd in range(numStudents):
    options,_ = parseArgs(addArgs(args,studentInd))
    saveFiles.append(options.saveFile)
    
  while len(unfinished) > 0:
    for studentInd in needToRun:
      jobs[studentInd] = submit([str(studentInd)] + args)
    needToRun = []
    # wait on jobs
    time.sleep(20)
    p = subprocess.Popen(['condor_q','sbarrett'],stdout=subprocess.PIPE)
    out,_ = p.communicate()
    for studentInd in unfinished:
      if out.find(str(jobs[studentInd])) < 0:
        if os.path.exists(saveFiles[studentInd]):
          unfinished.remove(studentInd)
        else:
          needToRun.append(studentInd)

def submit(args):
  options,_ = parseArgs(addArgs(args))
  base = 'condor/createTwoStageTransfer'
  orig = os.path.join(base,'base.condor')
  path = os.path.join(base,'jobs/%i.condor' % options.studentInd)
  with open(orig,'r') as f:
    contents = f.read()
  contents = contents.replace('$(Process)',str(options.studentInd))
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
