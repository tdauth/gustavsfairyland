#!/bin/bash
# The Jenkins build script for Linux.
bash copyclipsforbuild.sh
cd "$WORKSPACE/build"
make -j4
ctest -T "Test" || true