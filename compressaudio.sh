#!/bin/bash

find ./clips -type f \( \( -iname "*.wav" -o -iname "*.mp3" \) -and -not -iname "*_compressed*"  \) -print0 | while IFS= read -r -d $'\0' line; do
	echo "$line"
	filename=$(basename "$line")
	extension="${filename##*.}"
	filename="${filename%.*}"
	compressedName=$(dirname "$line")/"$filename""_compressed.$extension"
	echo "$compressedName"
	if [ -e "$compressedName" ] ; then
		rm "$compressedName"
	fi
	# Make sure the codec is supported on Android:
	# https://developer.android.com/guide/appendix/media-formats.html
	ffmpeg -nostdin -i "$line" -acodec mp3 -f mp3 "$compressedName"
done