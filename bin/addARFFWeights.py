#!/usr/bin/env python


def main(args):
  assert(len(args)==3),'Expected 3 arguments, got %i' % len(args)
  inFilename = args[0]
  outFilename = args[1]
  stayWeight = float(args[2])
  with open(inFilename,'r') as f:
    contents = f.readlines()
  dataStart = contents.index('@data\n') + 1
  for i,line in enumerate(contents[dataStart:]):
    vals = line.split(',')
    c = int(vals[-1])
    weight = 1.0
    if c == 4:
      weight = stayWeight
    contents[dataStart+i] = '%s,{%g}\n' % (line.strip(),weight)

  with open(outFilename,'w') as f:
    f.writelines(contents)

if __name__ == '__main__':
  import sys
  main(sys.argv[1:])
