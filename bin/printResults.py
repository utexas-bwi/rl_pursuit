#!/usr/bin/env python

import numpy, csv, os

def loadResults(path):
  if os.path.isfile(path):
    res = loadResultsFromFile(path)
  else:
    res = loadResultsFromDirectory(path)
    if res is None:
      res = loadResultsFromDirectory(os.path.join(path,'results'))
  return res

def loadResultsFromFile(filename):
  numSteps = []
  with open(filename,'r') as f:
    reader = csv.reader(f)
    for row in reader:
      iteration = int(row[0])
      stepsPerTrial = map(int,row[1:])
      numSteps.append([iteration] + stepsPerTrial)
  numSteps = numpy.array(numSteps)
  return numSteps

def validResultFile(filename):
  _,ext =  os.path.splitext(filename)
  return ext == '.csv'

def loadResultsFromDirectory(directory):
  if not(os.path.isdir(directory)):
    return []
  tempFilenames = [os.path.join(directory,x) for x in os.listdir(directory)]
  filenames = filter(validResultFile,tempFilenames)
  if len(filenames) == 0:
    return None
  #filenames.sort(key=lambda x:int(os.path.basename(x).replace('.csv','')))
  return loadResultsFromFileSet(filenames)

def loadResultsFromFileSet(filenames):
  numSteps = None
  for filename in filenames:
    res = loadResults(filename)
    if numSteps is None:
      numSteps = res
    else:
      try:
        numSteps = numpy.vstack((numSteps,res))
      except:
        pass
  return numSteps

def printResults(episodeLengths,label,outputCsv,outputHeader):
  if outputHeader:
    if outputCsv:
      print 'label, Num episodes, mean, median, std, min, max'
    return

  if outputCsv:
    vals = [label]
    if (episodeLengths is None) or (len(episodeLengths) == 0):
      vals.append(0)
    else:
      vals.append(len(episodeLengths))
      vals.append(numpy.mean(episodeLengths))
      #vals.append(numpy.mean(episodeLengths,0))
      vals.append(numpy.median(episodeLengths))
      vals.append(numpy.std(episodeLengths))
      vals.append(numpy.min(episodeLengths))
      vals.append(numpy.max(episodeLengths))
    print ','.join(map(str,vals))
  else:
    print '-----------------------------------'
    print label
    if episodeLengths is None:
      print 'Num episodes = ',0
      return
    print 'Num episodes = ',len(episodeLengths)
    print 'mean=',numpy.mean(episodeLengths)
    #print 'means=',numpy.mean(episodeLengths,0)
    print 'median=',numpy.median(episodeLengths)
    print 'std=',numpy.std(episodeLengths)
    print 'min,max=',numpy.min(episodeLengths),numpy.max(episodeLengths)
    #print episodeLengths[:10]
    #print numpy.median(episodeLengths)
    #from scipy import stats
    #print [stats.scoreatpercentile(episodeLengths,x)[0] for x in [10,90]]
    #episodeLengths.sort(axis=0)
    #for amount in [10,25,50,100]:
      #print amount,numpy.mean(episodeLengths[amount:len(episodeLengths)-amount])

def getStudentInds(path,includeStudents,excludeStudents):
  with open(path,'r') as f:
    students = [x.strip() for x in f.readlines()]
  # sanity check some stuff
  for student in includeStudents:
    if student not in students:
      print >>sys.stderr,'ERROR: Unknown include student: %s' % student
      sys.exit(1)
  for student in excludeStudents:
    if student not in students:
      print >>sys.stderr,'ERROR: Unknown exclude student: %s' % student
      sys.exit(1)
  # get the indices
  inds = []
  for i,student in enumerate(students):
    if (len(includeStudents) > 0) and (student not in includeStudents):
      continue
    if student in excludeStudents:
      continue
    inds.append(i)
  return inds

def loadAndProcessResults(paths,options,postProcessFunc=None):
  results = {}

  studentInds = getStudentInds('data/newStudents29.txt',options.includeStudents,options.excludeStudents)
  numEpisodes = len(studentInds)
  if options.matchNumEpisodes:
    print 'MATCHING number of episodes, via sorting first axis (MIGHT BE WRONG)'
  for i,path in enumerate(paths):
    res = loadResults(path)
    if (options.requireNumTrials is not None) and (res.shape[0] != options.requireNumTrials):
      continue
    if (res is None) or (len(res) == 0):
      continue
    else:
      trials = res[:,0]
      numSteps = res[:,1:]
      inds = []
      for j,trial in enumerate(trials):
        if trial in studentInds:
          inds.append(j)
      numSteps = numSteps[inds,:]
      trials = trials[inds]
    if options.matchNumEpisodes:
      numEpisodes = min(numEpisodes,len(numSteps))
      inds = numpy.argsort(numSteps,axis=0)
      numSteps = numSteps[inds,:]
      trials = trials[inds]

      numSteps = numSteps[:numEpisodes,:]
      trials = trials[:numEpisodes,:]
    else:
      num = numSteps.shape[0]
      if options.useQuantile:
        numToRemoveTop = numpy.floor((1.0 - options.quantile) * 0.5 * num + 0.5)
        numToRemoveBottom = numToRemoveTop
        if options.assumeMissingLong:
          numToRemoveTop = numpy.floor((1.0 - options.quantile) * 0.5 * 1000 + 0.5)
          numToRemoveBottom = numToRemoveTop - (1000 - num)
          if numToRemoveBottom < 0:
            print >>sys.stderr,'Missing too many episodes, wanted to remove %i' % (numToRemoveTop)
            sys.exit(2)
        print >>sys.stderr,'Removing bottom %i and top %i episodes for quantile %g' % (numToRemoveBottom,numToRemoveTop,options.quantile)

        inds = numpy.argsort(numSteps,axis=0)
        numSteps = numSteps[inds,:]
        trials = trials[inds]
        
        numSteps = numSteps[numToRemoveTop:-numToRemoveBottom]
        trials = trials[numToRemoveTop:-numToRemoveBottom]
    if options.maxLength is not None:
      if options.removeLongerThanMax:
        inds = (numSteps <= options.maxLength)
        numSteps = numSteps[inds]
        trials = trials[inds]
      else:
        numSteps[numSteps > options.maxLength] = options.maxLength
    if postProcessFunc is not None:
      postProcessFunc(trials,numSteps,path)
    results[path] = [trials,numSteps]
  return results

def printHelper(trials,numSteps,path,options):
  printResults(numSteps,path,options.outputCsv,False)
  if options.printLongest:
    print 'longest episodes: ',
    for i in range(5):
      ind = numpy.argmax(numSteps)
      print '%i(%i) ' % (numSteps[ind],trials[ind]),
      numSteps[ind] = -999999

def main(paths,options):
  printResults(None,None,options.outputCsv,True)
  loadAndProcessResults(paths,options,postProcessFunc=lambda x,y,z: printHelper(x,y,z,options))
  return 0

def parseArgs(args,parserOptions=[]):
  from optparse import OptionParser
  parser = OptionParser('printResults.py [options] result1.csv [result2.csv ...]\nNOTE: can take directories or files')
  parser.add_option('-c','--csv',action='store_true',dest='outputCsv',default=False,help='output in csv format')
  parser.add_option('-i','--include',action='append',dest='includeStudents',default=[],help='output only for specified students',metavar='STUDENT')
  parser.add_option('-x','--exclude',action='append',dest='excludeStudents',default=[],help='output excluding specified students',metavar='STUDENT')
  parser.add_option('-m','--match',action='store_true',dest='matchNumEpisodes',default=False,help='matches the number of episodes between the results')
  parser.add_option('-q','--quantile',action='store',dest='quantile',default=1.0,help='fraction of data to use, 0.9 removes the lowest and highest 0.05',type='float',metavar='NUM')
  parser.add_option('--assumeMissingLong',action='store_true',dest='assumeMissingLong',default=False,help='assumes that any missing episodes are long for quantiles, and that there are 1000 episodes')
  parser.add_option('--maxLength',action='store',dest='maxLength',default=None,type='int',help='Max length of episodes, longer ones get reduced to this value or removed if removeLongerThanMax is set',metavar='NUM')
  parser.add_option('--removeLongerThanMax',action='store_true',dest='removeLongerThanMax',default=False)
  parser.add_option('--requireNumTrials',action='store',type='int',default=None,dest='requireNumTrials',help='Ignore results without the proper number of trials',metavar='NUM')
  parser.add_option('--printLongest',action='store_true',dest='printLongest',default=False,help='prints a list of the 5 longest episodes')
  for o in parserOptions:
    parser.add_option(o)
  options,args = parser.parse_args(args)
  options.useQuantile = (options.quantile < 0.9999)

  if options.matchNumEpisodes and options.useQuantile:
    print >>sys.stderr,'Do not support matching with quantiles'
    sys.exit(2)
  return options,args

def mainArgs(args):
  options,args = parseArgs(args)
  return main(args,options)

if __name__ == '__main__':
  import sys
  sys.exit(mainArgs(sys.argv[1:]))
