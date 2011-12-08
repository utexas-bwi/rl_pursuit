#!/usr/bin/env python

from PyQt4 import QtGui, QtCore
import json

class DependentItem(QtGui.QTreeWidgetItem):
  def __init__(self,parent,dependency,depVals,vals):
    super(DependentItem, self).__init__(parent,vals)
    self.dependency = dependency
    if type(depVals) is list:
      self.depVals = depVals
    else:
      self.depVals = [depVals]
    if vals[1] != '':
      self.setFlags(QtCore.Qt.ItemIsEditable | QtCore.Qt.ItemIsEnabled)
    self.setExpanded(True)
    self.setView()

  def setView(self):
    if not(self.dependency.isHidden()) and (self.dependency.text(1) in self.depVals):
      self.setHidden(False)
    else:
      self.setHidden(True)

  def getVal(self):
    return self.text(1)

class Tree(object):
  def __init__(self):
    self.tree = QtGui.QTreeWidget()
    self.tree.setWindowTitle('Config Editor')
    self.tree.setColumnCount(2)
    self.tree.setHeaderLabels(["Name","Val"])
    self.tree.header().resizeSection(0,300)
    self.tree.show()
    self.depOptions = []
    self.models = {}
    self.tree.itemChanged.connect(self.setViews)
    self.tree.itemClicked.connect(self.click)

  def click(self,x,col):
    if x in self.models:
      self.models[x] = not(self.models[x])
      disable = not(self.models[x])
      if disable:
        bg = QtGui.QColor('black')
        fg = QtGui.QColor('white')
      else:
        bg = QtGui.QColor('white')
        fg = QtGui.QColor('black')
      x.setBackground(0,bg)
      x.setBackground(1,bg)
      x.setForeground(0,fg)
      x.setForeground(1,fg)
      x.setSelected(False)

  def setViews(self,x):
    for opt in self.depOptions:
      opt.setView()
 
  def addOption(self,vals,parent=None,parent2=None):
    if parent2 is not None:
      vals = [vals,parent]
      parent = parent2
    elif type(parent) not in [QtGui.QTreeWidget,QtGui.QTreeWidgetItem,None]:
      vals = [vals,parent]
      parent = None
    if parent is None:
      parent = self.tree
    vals = map(str,vals)
    temp = QtGui.QTreeWidgetItem(parent,vals)
    if vals[1] != '':
      temp.setFlags(QtCore.Qt.ItemIsEditable | QtCore.Qt.ItemIsEnabled)
    temp.setExpanded(True)
    return temp
  
  def addDependentOption(self,dependency,depVal,vals,parent=None):
    if parent is None:
      parent = self.tree
    vals = map(str,vals)
    temp = DependentItem(parent,dependency,depVal,vals)
    self.depOptions.append(temp)
    return temp

  def populateJson(self,val,item):
    if item.isHidden():
      return
    if (item in self.models) and not(self.models[item]):
      return
    key = str(item.text(0))
    if item.text(1) == '':
      val[key] = {}
      for i in range(item.childCount()):
        self.populateJson(val[key],item.child(i))
      if item.text(0) == 'models':
        val[key] = val[key].values()
    else:
      val[key] = str(item.text(1))

  def output(self,item=None):
    self.json = {}
    for i in range(self.tree.topLevelItemCount()):
      self.populateJson(self.json,self.tree.topLevelItem(i))
    print json.dumps(self.json,indent=2)

class Options(object):
  def __init__(self,tree):
    self.tree = tree
    self.setupWorld()
    self.setupPlanner()

  def addOption(self,*args,**kwargs):
    return self.tree.addOption(*args,**kwargs)
  
  def addDependentOption(self,*args,**kwargs):
    return self.tree.addDependentOption(*args,**kwargs)

  def setupWorld(self):
    self.addOption('centerPrey',True)
    self.addOption('width',20)
    self.addOption('height',20)
    self.setupVerbosity()

    self.setupAgent('prey','random')
    self.adhoc = self.setupAgent('adhoc','mcts')
    self.setupAgent('predator','student')

  def setupVerbosity(self):
    x = self.addOption('verbosity','')
    self.addOption('description',True,x)
    self.addOption('observation',False,x)
    self.addOption('stepsPerEpisode',False,x)
    self.addOption('stepsPerTrial',False,x)
    self.addOption('summary',True,x)
    x.setExpanded(False)

  def setupAgent(self,agent,agentType,parent=None):
    a = self.addOption(agent,agentType,parent)
    ao = self.addDependentOption(a,['student','dt','dummy'],[agent + 'Options',''],parent)
    self.addDependentOption(a,'student',['student','data/students.txt'],ao)
    self.addDependentOption(a,'dt',['filename','data/dt/weighted/studentNewComplete.weka'],ao)
    self.addDependentOption(a,'dummy',['action',0],ao)
    return a

  def setupPlanner(self):
    p = self.addDependentOption(self.adhoc,'mcts',['planner',''])
    self.addOption('silver',False,p)
    self.addOption('weighted',True,p)
    self.addOption('update','polynomial',p)
    self.addOption('lambda',0.8,p)
    self.addOption('gamma',0.95,p)
    self.addOption('unseenValue',999999,p)
    self.addOption('initialValue',0,p)
    self.addOption('initialStateVisits',0,p)
    self.addOption('initialStateActionVisits',0,p)
    self.addOption('time',0,p)
    self.addOption('rewardBound',0.5,p)
    self.addOption('playouts',1000,p)
    self.addOption('depth',100,p)
    self.addOption('theoreticallyCorrectLambda',False,p)
    self.models = self.addOption('models','',p)
    self.setupModels()

  def setupModels(self):
    self.addModel('GR','random','gr')
    self.addModel('TA','random','ta')
    self.addModel('GP','random','gp')
    self.addModel('PD','random','pd')
    self.addModel('DT','random','dt')

  def addModel(self,desc,prey,pred):
    x = self.addOption(desc,'',self.models)
    self.addOption('prob',1.0,x)
    self.addOption('desc',desc,x)
    self.setupAgent('prey',prey,x)
    self.setupAgent('predator',pred,x)
    x.setExpanded(False)
    self.tree.models[x] = True

def main(args):
  app = QtGui.QApplication(args)

  tree = Tree()
  Options(tree)

  tree.setViews(None)
  app.exec_()
  tree.output()

if __name__ == '__main__':
  import sys
  main(sys.argv)
