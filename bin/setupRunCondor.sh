#!/bin/bash

USAGE="Usage: $0 config [opts for setupCondor]"

if [ "$#" == "0" ]; then
  echo "$USAGE"
  exit 1
fi

name=$1
shift
args=$@
base=$(basename "$name")
noext="${base%.*}"

#echo "name: $name"
#echo "base: $base"
#echo "args: $args"

config=configs/$base
condor=condor/$noext/job.condor
echo "Assuming that config is at $config and condor file will be at $condor"

bin/setupCondor.py $config $args
condor_submit $condor
