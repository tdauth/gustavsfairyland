#!/bin/bash

if [ "$USER" != "jenkins" ]; then
	echo "User is not jenkins."
	exit 1
fi

if [ -z "$WORKSPACE" ]; then
	echo "WORKSPACE is not defined."
	exit 1
fi

SOURCE_CLIPS_DIR="/home/tamino/Dokumente/Projekte/fairytale/clips/"

# Copy clips file which are not in the GIT repository
cp -f "$WORKSPACE/fairytale/clips/gustav.xml" "$WORKSPACE/gustav.xml.tmp"
cp -rf "$SOURCE_CLIPS_DIR"* "$WORKSPACE/fairytale/clips/"
cp -f "$WORKSPACE/gustav.xml.tmp" "$WORKSPACE/fairytale/clips/gustav.xml"