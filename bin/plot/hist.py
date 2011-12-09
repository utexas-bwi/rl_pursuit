#!/usr/bin/env python

import numpy
import matplotlib.pyplot as plt
from optparse import OptionParser

def createBins(results,numBins,exponent=1.0):
  minVal = results.min()
  maxVal = results.max()
  minVal = minVal ** (1.0 / exponent)
  maxVal = maxVal ** (1.0 / exponent)
  bins = [minVal]
  val = minVal
  for i in range(numBins):
    val += float(maxVal-minVal) / numBins
    bins.append(val)
  bins = [x ** exponent for x in bins]
  bins[-1] += 1
  return bins

def readFile(filename):
  data = numpy.loadtxt(filename,dtype=int,delimiter=',')
  # should be of form: jobNum, steps
  assert(data.shape[1] == 2),'Two columns expected in data'
  return data[:,1]

def getStudentInds(students,filename='data/students.txt'):
  studentList = numpy.loadtxt(filename,dtype=str)
  studentInds = []
  for student in students:
    studentInds.append((studentList == student).nonzero())
  return studentInds

def getLabels(filenames,students):
  import os
  labelMap = {}
  for filename in filenames:
    f = os.path.splitext(os.path.split(filename)[1])[0]
    labelMap[filename] = f
    for student in students:
      labelMap[filename,student] = '%s-%s' % (f,student)
    labelMap[filename,'exc'] = '%s-excluding' % f
  return labelMap

def plot(results,filenames,students):
  labelMap = getLabels(filenames,students)
  plt.figure()
  # create the bin sizes
  allResults = numpy.hstack(results)
  bins = createBins(allResults,10,3.0)
  # deal with the students
  studentIndList = getStudentInds(students)
  combinedStudentInds = numpy.hstack(studentIndList)[0,:]
  excludingStudentInds = numpy.array(list(set(range(len(results[0]))) - set(combinedStudentInds)))
  # plot the histograms
  p = lambda result,label: plt.hist(result,bins=bins,label=label,histtype='step',linewidth=3)#,log=True)
  for result,filename in zip(results,filenames):
    p(result,labelMap[filename])
    p(result[excludingStudentInds],labelMap[filename,'exc'])
    for studentInds,student in zip(studentIndList,students):
      p(result[studentInds],labelMap[filename,student])
  # plot the histogram
  #plt.hist(numpy.transpose(numpy.vstack(plotResults)),bins=bins,label=labels,histtype='step',linewidth=3)#,log=True)
  plt.legend()

def main(filenames,students):
  results = []
  for filename in filenames:
    results.append(readFile(filename))
  plot(results,filenames,students)
  plt.show()

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option('-s','--student',action='append',dest='students',help='plot a student\'s results separately from the general population',metavar='STUDENT',default=[])
  options,args = parser.parse_args()
  main(args,options.students)
