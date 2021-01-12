#!/bin/bash
rm output/* > /dev/null
mkdir -p output > /dev/null
export PYTHONPATH=$PWD
./bin/spectrograms $1