#!/bin/bash

line=""
while true; do
  newLine=`condor_q sbarrett | grep running`
  if [ "$line" != "$newLine" ]
  then
    line=$newLine
    echo $line
    if [ ${line:0:1} -eq 0 ] 
    then
      break
    fi
  fi
  sleep 10
done
