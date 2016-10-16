#!/bin/bash
# The Jenkins build script for Linux.
bash -x copyclipsforbuild.sh
cd "$WORKSPACE/build"
make -j4