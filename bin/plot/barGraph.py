#!/usr/bin/env python

# NOTE: for better hatching, hack /usr/lib/pymodules/python2.7/matplotlib/backends/backend_ps.py
#  change line: "0 setlinewidth" to "1 setlinewidth"

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

def getAxisBounds(values,errors,yMinFixed):
  if errors is None:
    vals = values
  else:
    vals =  [v+e for v,e in zip(values,errors)]
    vals += [v-e for v,e in zip(values,errors)]
  yMin = min(vals)
  yMax = max(vals)
  yFudge = (yMax - yMin) * 0.1
  yMin -= yFudge
  yMax += yFudge
  xFudge = 0.5
  xMin = -xFudge
  xMax = len(values) - xFudge
  if yMinFixed is not None:
    yMin = yMinFixed
  return [xMin,xMax,yMin,yMax]


def makeBarGraph(values,options):

  fontSize = options.fontSize
  tickSize = fontSize

  params = {'backend': 'PS','axes.labelsize': fontSize,'text.fontsize': fontSize,'legend.fontsize': fontSize,'xtick.labelsize': tickSize,'ytick.labelsize': tickSize,'text.usetex': True, 'ps.usedistiller': 'xpdf'}
  plt.rcParams.update(params)

  plt.figure()
  bounds = getAxisBounds(values,options.errors,options.yMinFixed)
  plt.axis(bounds)
  for x,v in enumerate(values):
    plt.bar(x-0.4,v,label=options.labels[x],color=options.colors[x],hatch=options.styles[x],yerr=options.errors[x],ecolor='black')
  plt.xticks([])
  plt.legend(loc=options.legendLoc)
  
  plt.ylabel(options.ylabel)
  plt.xlabel(options.xlabel)
  plt.title(options.title)
  
  if options.filename is None:
    plt.show()
  else:
    plt.savefig(options.filename,format='eps',bbox_inches='tight',pad_inches=0.1)

def readFile(filename):
  data = numpy.loadtxt(filename,dtype=int,delimiter=',')
  # should be of form: jobNum, steps
  assert(data.shape[1] == 2),'Two columns expected in data'
  return data[:,1]

def main(filenames,options):
  bars = []
  for filename in filenames:
    vals = readFile(filename)
    bars.append(vals.mean())
    if options.errors is not None:
      options.errors.append(numpy.std(vals) / numpy.sqrt(len(vals)))
  makeBarGraph(bars,options)

def getMainOpts(**kwargs):
  defaults = {
    'filename': None,
    'yMinFixed': None,
    'labels': None,
    'errors': [],
    'colors': ['b','r','g','c','m','y','k'],
    'styles': ['.','|','-','/','\\','+','x','o','O','*'],
    'legendLoc': 'best',
    'ylabel': '',
    'xlabel': '',
    'title': '',
    'fontSize': 24
  }
  options = Options()
  parseOptions(options,defaults,kwargs,'Main')
  return options

if __name__ == '__main__':
  args = sys.argv[1:]
  assert(len(args) > 0)
  options = getMainOpts(labels=args)
  main(args,options)
