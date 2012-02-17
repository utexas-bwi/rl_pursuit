#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: syncDir path"
  exit 1
fi

dir=$1

rsync -avz school:pursuit2/$dir/* ~/pursuit2/$dir
