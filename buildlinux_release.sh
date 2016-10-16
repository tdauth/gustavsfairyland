#!/bin/bash
# The Jenkins build script for Linux.
bash -x copyclipsforbuild.sh
cd "$WORKSPACE/fairytale"
mkdir "$WORKSPACE/build"
cd "$WORKSPACE/build"
cmake ../ -DUSE_COMPRESSED_FILES=${USE_COMPRESSED_FILES} -DCMAKE_BUILD_TYPE="Release"
make -j4