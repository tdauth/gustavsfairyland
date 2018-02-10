#!/bin/bash
# The Jenkins build script for Linux.
bash -x "$WORKSPACE/fairytale/scripts/copyclipsforbuild.sh"
mkdir "$WORKSPACE/build"
cd "$WORKSPACE/build"
cmake "$WORKSPACE/fairytale/" -DUSE_COMPRESSED_FILES=${USE_COMPRESSED_FILES} -DCMAKE_BUILD_TYPE="Release"
make -j4
cpack .