#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: syncDir path"
  exit 1
fi

dir=$1

rsync -avz ~/pursuit2/$dir school:pursuit2/$dir/*
