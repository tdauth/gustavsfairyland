#!/bin/bash
# The Jenkins build script for Linux.
# Copy clips file which are not in the GIT repository
cp -f "$WORKSPACE/fairytale/clips/gustav.xml" "$WORKSPACE/gustav.xml.tmp"
cp -rf "/home/tamino/Dokumente/Projekte/fairytale/clips/"* "$WORKSPACE/fairytale/clips/"
cp -f "$WORKSPACE/gustav.xml.tmp" "$WORKSPACE/fairytale/clips/gustav.xml"
cd "$WORKSPACE/build"
make -j4
ctest -T "Test" || true