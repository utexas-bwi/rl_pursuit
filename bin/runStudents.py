#!/usr/bin/env python

import subprocess, time, multiprocessing, os

def waitForProcesses(processes,numToStop):
  while True:
    for p in processes:
      if p.poll() != None:
        processes.remove(p)
    if len(processes) <= numToStop:
      return
    time.sleep(1)

def combineARFFs(inFiles,outFile):
  out = open(outFile,'w')
  for i,inFile in enumerate(inFiles):
    with open(inFile,'r') as f:
      lines = f.readlines()
    startLine = 0
    if i != 0:
      startLine = lines.index('@data\n') + 1 # + 1 to skip the data line
    out.writelines(lines[startLine:])
  out.close()

def main(desc,numTrials,numTrainingSamples):
  dtTrainDir = 'data/dt/%s-%i/train' % (desc,numTrainingSamples)
  if not(os.path.exists(dtTrainDir)):
    os.makedirs(dtTrainDir)

  with open('data/newStudentsComplete.txt','r') as f:
    students = set(f.read().split())

  with open('configs/student_output.json','r') as f:
    config = f.read()

  trialConfig = 'configs/runStudents/trials.json'
  with open(trialConfig,'w') as f:
    f.write('{\n')
    f.write('"trials": %i\n' % numTrials)
    f.write('}\n')

  outputFilenames = []
  processes = []
  for student in students:
    waitForProcesses(processes,multiprocessing.cpu_count()-1)
    contents = config.replace('$(STUDENT)',student)
    outputFilename = os.path.join(dtTrainDir,'%s.arff' % student)
    outputFilenames.append(outputFilename)
    contents = contents.replace('$(DTFILE)',outputFilename)
    contents = contents.replace('$(DTSTEPS)',str(numTrainingSamples))
    studentConfig = 'configs/runStudents/%s.json' % student
    with open(studentConfig,'w') as f:
      f.write(contents)
    p = subprocess.Popen(['bin/main',studentConfig,trialConfig])
    processes.append(p)
  waitForProcesses(processes,0)

  #combinedFilename = os.path.join(dtTrainDir,'combined.arff')
  #combineARFFs(outputFilenames,combinedFilename)

if __name__ == '__main__':
  import sys
  if (len(sys.argv) != 3) or (sys.argv[1] in ['-h','--help']):
    print 'Usage: runStudents.py desc numTrainingSamplesPerAgent'
    sys.exit(1)
  #numTrials = int(sys.argv[1])
  desc = sys.argv[1]
  numTrainingSamples = int(sys.argv[2])
  numTrials = numTrainingSamples
  main(desc,numTrials,numTrainingSamples)
