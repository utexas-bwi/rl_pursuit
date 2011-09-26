#!/bin/bash

#rm -r condor/output/* condor/results/* condor/configs/*
ls -1d condor/* | grep -v "base.condor" | xargs rm -r
