#!/usr/bin/env python

import matplotlib.pyplot as plt

def loadResults(filename):
  import cPickle as pickle
  with open(filename,'r') as f:
    vals = pickle.load(f)
  return vals

def plot(vals):
  for key in vals.iterkeys():
    ys = [v[0] for v in vals[key]]
    plt.plot(ys,label=key)
  plt.legend()
  plt.show()

def main(filename):
  vals = loadResults(filename)
  plot(vals)

if __name__ == '__main__':
  import sys
  main(sys.args[1])
