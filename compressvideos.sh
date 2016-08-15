#!/bin/bash

find ./clips -type f \( -iname "*.avi" -o -iname "*.mkv" \) -print0 | while IFS= read -r -d $'\0' line; do
	echo "$line"
	filename=$(basename "$line")
	extension="${filename##*.}"
	filename="${filename%.*}"
	compressedName=$(dirname "$line")/"$filename""_compressed.mkv"
	echo "$compressedName"
	if [ -e "$compressedName" ] ; then
		rm "$compressedName"
	fi
	ffmpeg -nostdin -i "$line" -s 640x480 -b 512k -vcodec theora -acodec copy "$compressedName"
done