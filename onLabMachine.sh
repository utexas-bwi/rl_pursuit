#!/bin/bash

name=`hostname`
nonLabMachines="ubik scannerdarkly"
for machine in $nonLabMachines
do
  if [ $name == $machine ]
  then
    echo "no"
    exit 0
  fi
done

echo "yes"
exit 1
