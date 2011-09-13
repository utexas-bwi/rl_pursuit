#!/bin/bash

while true; do
  PID=`pgrep main*`
  if [ $? -eq 0 ]
  then
    pmap $PID | grep total
  fi
  sleep 0.1
done
