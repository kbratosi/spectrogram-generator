#!/bin/bash
rm output/*
mkdir -p output > /dev/null   # clears previous output!
export PYTHONPATH=$PWD
valgrind ./bin/spectrograms $1