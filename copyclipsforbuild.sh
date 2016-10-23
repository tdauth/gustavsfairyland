#!/bin/bash

if [ "$USER" != "jenkins" ]; then
	echo "User is not jenkins."
	exit 1
fi

if [ -z "$WORKSPACE" ]; then
	echo "WORKSPACE is not defined."
	exit 1
fi

# Copy clips file which are not in the GIT repository
cp -f "$WORKSPACE/fairytale/clips/gustav.xml" "$WORKSPACE/gustav.xml.tmp"
cp -rf "/home/tamino/Dokumente/Projekte/fairytale/clips/"* "$WORKSPACE/fairytale/clips/"
cp -f "$WORKSPACE/gustav.xml.tmp" "$WORKSPACE/fairytale/clips/gustav.xml"