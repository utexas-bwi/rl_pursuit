#!/usr/bin/env python

import sys, numpy
import matplotlib.pyplot as plt

class Options:
  pass

def parseOptions(dest,defaults,options,labelForErrors = None):
  if labelForErrors is None:
    outputLabel = ''
  else:
    outputLabel = '(%s)' % labelForErrors
  for key in options.iterkeys():
    if key not in defaults:
      print >>sys.stderr,'Unknown option given to parseOptions%s: %s' % (outputLabel,key)
      sys.exit(11)
  for key in defaults.iterkeys():
    if key in options:
      val = options[key]
    else:
      val = defaults[key]
    dest.__dict__[key] = val

class Bar(object):
  defaults = {
    'label': 'Unknown'
  }
  def __init__(self,value,**kwargs):
    self.value = value
    parseOptions(self,self.defaults,kwargs,'Bar')

def makeBarGraph(bars,options):
  vals = [x.value for x in bars]
  yMin = min(vals)
  yMax = max(vals)
  yMin -= (yMax - yMin) * 0.1
  yMax += (yMax - yMin) * 0.1
  xMin = 0 - 0.1
  xMax = len(bars) - 0.1

  fontSize = 12
  tickSize = 12

  params = {'backend': 'agg','axes.labelsize': fontSize,'text.fontsize': fontSize,'legend.fontsize': fontSize,'xtick.labelsize': tickSize,'ytick.labelsize': tickSize,'text.usetex': True, 'ps.usedistiller': 'xpdf'}
  #if options.filename is not None:
  params['backend'] = 'PS'
  plt.rcParams.update(params)

  plt.figure()
  plt.axis([xMin,xMax,yMin,yMax])
  for x,bar in enumerate(bars):
    plt.bar(x,bar.value,label=bar.label)
  plt.legend()
  
  if options.filename is None:
    plt.show()
  else:
    plt.savefig(options.filename)

def readFile(filename):
  data = numpy.loadtxt(filename,dtype=int,delimiter=',')
  # should be of form: jobNum, steps
  assert(data.shape[1] == 2),'Two columns expected in data'
  return data[:,1]

def main(filenames,options):
  bars = []
  for filename in filenames:
    vals = readFile(filename)
    bars.append(Bar(vals.mean(),label=filename))
  makeBarGraph(bars,options)

if __name__ == '__main__':
  args = sys.argv[1:]
  clo = {}
  defaults = {
    'filename': None
  }
  options = Options()
  parseOptions(options,defaults,clo,'Main')
  assert(len(args) > 0)
  main(args,options)
