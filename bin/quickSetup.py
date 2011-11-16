#!/usr/bin/env python

import sys
from optparse import OptionParser, OptionValueError, OptionGroup, BadOptionError

def addOption(parser,name,typeStr,default=None,**kwargs):
  parser.add_option('--%s'%name,action='store',dest=name,type=typeStr,default=default,**kwargs)

def addBoolOption(parser,name,default):
  parser.add_option('--%s' % name,action='store_true',dest=name,default=default)
  parser.add_option('--no-%s' % name,action='store_false',dest=name,default=default)

def setupVerbosity(parser):
  group = OptionGroup(parser,'verbosity')
  addBoolOption(group,'description',True)
  addBoolOption(group,'observation',False)
  addBoolOption(group,'stepsPerEpisode',False)
  addBoolOption(group,'stepsPerTrial',False)
  addBoolOption(group,'summary',True)
  parser.add_option_group(group)

def setupWorld(parser):
  addBoolOption(parser,'centerPrey',True)
  addOption(parser,'width','int',20)
  addOption(parser,'height','int',20)
  addOption(parser,'prey','str','random')
  addOption(parser,'adhoc','str')
  addOption(parser,'predator','str')
  # TODO - agent options
  setupVerbosity(parser)

def setupWorldCond(parser,values):
  baseList = ['prey','adhoc','predator']
  for base in baseList:
    group = OptionGroup(parser,'%sOptions' % base)
    if values[base] == 'student':
      addOption(group,'%sOptions-student'%base,'str','data/students.txt')
    elif values[base] == 'dt':
      addOption(group,'%sOptions-filename'%base,'str')
    elif values[base] == 'dummy':
      addOption(group,'%sOptions-action'%base,'int')
    parser.add_option_group(group)

def setupPlanner(parser,values):
  if values['adhoc'] != 'uct':
    return
  group = OptionGroup(parser,'planner')
  addBoolOption(group,'silver',False)
  addBoolOption(group,'weighted',True)
  addOption(group,'update','choice','polynomial',choices=['bayesian','polynomial'])
  addOption(group,'lambda','float',0.8)
  addOption(group,'gamma','float',0.95)
  addOption(group,'unseenValue','float',999999)
  addOption(group,'initialValue','float',0)
  addOption(group,'initialStateVisits','float',0)
  addOption(group,'initialStateActionVisits','float',0)
  addOption(group,'time','float',0)
  addOption(group,'rewardBound','float',0.5)
  addOption(group,'playouts','int',1000)
  addOption(group,'depth','int',100)
  addBoolOption(group,'theoreticallyCorrectLambda',False)
  addOption(group,'models','str')
  parser.add_option_group(group)

def handleModels(values):
  if 'planner' not in values:
    return
  modelFilename = values['planner']['models']
  with open(modelFilename,'r') as f:
    values['planner']['models'] = eval(f.read())

def checkRemainingArgs(args,minExpected,maxExpected):
  if (len(args) < minExpected) or (len(args) > maxExpected):
    print >>sys.stderr,'Expected %i-%i argument(s), got %s' % (minExpected,maxExpected,args)
    print >>sys.stderr,'Exitting'
    sys.exit(1)
  return args
  
def getUnassignedArgs(options,values):
  optionDests = [x.dest for x in options]
  for k,v in values.iteritems():
    if v != None:
      continue
    newValue = None
    while newValue is None:
      userInput = raw_input('Value for %s: ' % k)
      option = options[optionDests.index(k)]
      try:
        newValue = option.convert_value(option.dest,userInput)
      except OptionValueError:
        pass
    values[k] = newValue
  return values

def processGroups(parser,values):
  optionGroups = [g.title for g in parser.option_groups]
  optionGroupDests = [[x.dest for x in g.option_list] for g in parser.option_groups]
  newValues = {}
  for optionGroup,dests in zip(optionGroups,optionGroupDests):
    if len(dests) > 0:
      newValues[optionGroup] = {}

  for k,v in values.iteritems():
    found = False
    for optionGroup,dests in zip(optionGroups,optionGroupDests):
      if k in dests:
        found = True
        if k.startswith(optionGroup):
          k = k[len(optionGroup)+1:]
        newValues[optionGroup][k] = v
        break
    if not(found):
      newValues[k] = v
  return newValues

def _process_args(self, largs, rargs, values):
  """_process_args(largs : [string],
                     rargs : [string],
                     values : Values)

    Process command-line arguments and populate 'values', consuming
    options and arguments from 'rargs'.  If 'allow_interspersed_args' is
    false, stop at the first non-option argument.  If true, accumulate any
    interspersed non-option arguments in 'largs'.
  """
  while rargs:
    arg = rargs[0]
    # We handle bare "--" explicitly, and bare "-" is handled by the
    # standard arg handler since the short arg case ensures that the
    # len of the opt string is greater than 1.
    try:
      if arg == "--":
        del rargs[0]
        return
      elif arg[0:2] == "--":
        # process a single long option (possibly with value(s))
        self._process_long_opt(rargs, values)
      elif arg[:1] == "-" and len(arg) > 1:
        # process a cluster of short options (possibly with
          # value(s) for the last one only)
          self._process_short_opts(rargs, values)
      elif self.allow_interspersed_args:
        largs.append(arg)
        del rargs[0]
      else:
        return                  # stop now, leave this arg in rargs
    except BadOptionError:
      largs.append(arg)

def main(args):
  from copy import copy
  import json

  parser = OptionParser('%prog [options] outFilename\n  (Add -- before --help to get options of conditional parser)')
  parser._process_args = lambda x,y,z: _process_args(parser,x,y,z)
  #parser.error = test
  setupWorld(parser)
  tempValues,args = parser.parse_args(args)
  values = tempValues.__dict__
  completeOptions = copy(parser.option_list)
  for g in parser.option_groups:
    completeOptions += g.option_list
  values = getUnassignedArgs(completeOptions,values)
  values = processGroups(parser,values)

  condParser = OptionParser()
  setupWorldCond(condParser,values)
  setupPlanner(condParser,values)
  tempValues,args = condParser.parse_args(args)
  condValues = tempValues.__dict__
  completeOptions = copy(condParser.option_list)
  for g in condParser.option_groups:
    completeOptions += g.option_list
  condValues = getUnassignedArgs(completeOptions,condValues)
  condValues = processGroups(condParser,condValues)

  values = dict(values.items() + condValues.items())
  handleModels(values)

  args = checkRemainingArgs(args,0,1)
  res = json.dumps(values,sort_keys=True,indent=2)
  if len(args) > 0:
    filename = args[0]
    with open(filename,'w') as f:
      f.write(res)
  else:
    print res

if __name__ == '__main__':
  main(sys.argv[1:])
