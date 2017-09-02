#!/bin/bash
cd "$WORKSPACE/fairytale"
bash -x buildlinux_release.sh
cd "$WORKSPACE/fairytale/build"
cpack -R "$VERSION" -B "$WORKSPACE/build/pkg" -G TBZ2
cpack -R "$VERSION" -B "$WORKSPACE/build/pkg" -G RPM
cpack -R "$VERSION" -B "$WORKSPACE/build/pkg" -G DEB