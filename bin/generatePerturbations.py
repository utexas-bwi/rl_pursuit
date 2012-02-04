#!/usr/bin/env python

import random

predsAreSame = True
numPerturbations = 1000
#types = ['noop','random']
types = ['random']
#types = ['noop']
amountMin = 0.0
amountMax = 0.2
numPreds = 4

for i in range(numPerturbations):
  for j in range(numPreds):
    if not(predsAreSame) or (j == 0):
      t = random.choice(types)
      amount = random.uniform(amountMin,amountMax)
    print t,amount,
  print ''
