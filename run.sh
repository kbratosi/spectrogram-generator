#!/bin/bash
rm output/*
mkdir -p output > /dev/null   # clears previous output!
export PYTHONPATH=$PWD
./bin/spectrograms $1